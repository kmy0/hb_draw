#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "reframework/API.hpp"
#include "rendering/d3d12.hpp"
#include <sol/sol.hpp>

#include "draw.h"
#include "plugin.h"
#include "scene.h"

#include <mutex>

using API = reframework::API;

hbdraw g_hbdraw{};

bool initialize_imgui() {
    if (g_hbdraw.imgui.initialized) {
        return true;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    const auto renderer = API::get()->param()->renderer_data;

    DXGI_SWAP_CHAIN_DESC desc{};
    static_cast<IDXGISwapChain *>(renderer->swapchain)->GetDesc(&desc);

    if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
        return false;
    }

    if (renderer->renderer_type == REFRAMEWORK_RENDERER_D3D12) {
        if (!g_d3d12.initialize()) {
            return false;
        }
    } else {
        return false;
    }

    ImGui::GetStyle().AntiAliasedFill = false;
    g_hbdraw.imgui.initialized = true;
    return true;
}

bool imgui_ok() {
    return initialize_imgui() &&
           (ID3D12CommandQueue *)API::get()
                   ->param()
                   ->renderer_data->command_queue != nullptr;
}

template <typename R, typename... Args>
auto new_frame_wrapper(R (*func)(Args...)) {
    return [func](Args... args) {
        std::lock_guard _{g_hbdraw.mutex};
        if (g_hbdraw.camera.is_frame_gen || !g_hbdraw.imgui.initialized) {
            return;
        }

        if (g_hbdraw.do_new_frame) {
            if (!scene::update_camera()) {
                return;
            }
            g_hbdraw.do_new_frame = false;
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
        }
        func(args...);
    };
}

void do_render() {
    std::lock_guard _{g_hbdraw.mutex};
    if (!imgui_ok() || g_hbdraw.do_new_frame) {
        return;
    }

    ImGui::Render();
    g_d3d12.render_imgui();
    g_hbdraw.do_new_frame = true;
}

void on_lua_state_created(lua_State *l) {
    API::LuaLock _{};
    g_hbdraw.lua = l;
    sol::state_view lua{g_hbdraw.lua};

    auto hb_draw = lua.create_table();
    hb_draw["cylinder"] = new_frame_wrapper(draw::draw_cylinder);
    hb_draw["ring"] = new_frame_wrapper(draw::draw_ring);
    hb_draw["box"] = new_frame_wrapper(draw::draw_box);
    hb_draw["triangle"] = new_frame_wrapper(draw::draw_triangle);
    hb_draw["capsule"] = new_frame_wrapper(draw::draw_capsule);
    hb_draw["sphere"] = new_frame_wrapper(draw::draw_sphere);
    hb_draw["set_num_segments"] = [&](unsigned num) {
        g_hbdraw.imgui.num_segments = num;
    };
    hb_draw["set_outline_tickness"] = [&](unsigned num) {
        g_hbdraw.imgui.outline_tickness = num;
    };
    hb_draw["set_w2s"] = [&](bool b) { g_hbdraw.w2s = b; };
    lua["hb_draw"] = hb_draw;
}

void on_device_reset() {
    ImGui_ImplDX12_Shutdown();
    g_d3d12 = {};
    g_hbdraw.imgui.initialized = false;
    g_hbdraw.camera = {};
    g_hbdraw.do_new_frame = true;
}

void on_lua_state_destroyed(lua_State *l) {
    API::LuaLock _{};
    g_hbdraw.lua = nullptr;
}

extern "C" __declspec(dllexport) bool
reframework_plugin_initialize(const REFrameworkPluginInitializeParam *param) {
    API::initialize(param);

    const auto functions = param->functions;
    functions->on_lua_state_created(on_lua_state_created);
    functions->on_lua_state_destroyed(on_lua_state_destroyed);
    functions->on_present(do_render);
    functions->on_device_reset(on_device_reset);

    if (strcmp(param->version->game_name, "MHWILDS") == 0) {
        g_hbdraw.w2s = false;
    }

    return true;
}

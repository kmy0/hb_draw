#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "reframework/API.hpp"
#include "rendering/d3d12.hpp"
#include <sol/sol.hpp>

#include "draw.h"
#include "plugin.h"
#include "scene.h"

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
    ImGui::GetStyle().AntiAliasedLines = false;
    g_hbdraw.imgui.initialized = true;
    return true;
}

bool imgui_ok() {
    return initialize_imgui() &&
           (ID3D12CommandQueue *)API::get()
                   ->param()
                   ->renderer_data->command_queue != nullptr;
}

void do_draw() {
    std::lock_guard m{g_hbdraw.mutex};
    if (!g_hbdraw.imgui.initialized || !scene::update_camera()) {
        return;
    }

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    g_hbdraw.is_frame = true;

    API::LuaLock _{};
    for (const auto &fn : g_hbdraw.draw_fns) {
        fn();
    }
}

void do_render() {
    std::lock_guard m{g_hbdraw.mutex};
    if (!imgui_ok() || g_hbdraw.camera.is_frame_gen || !g_hbdraw.is_frame) {
        return;
    }

    ImGui::Render();
    g_d3d12.render_imgui();
}

void on_lua_state_created(lua_State *l) {
    API::LuaLock _{};
    g_hbdraw.lua = l;
    sol::state_view lua{g_hbdraw.lua};

    auto hb_draw = lua.create_table();
    hb_draw["cylinder"] = draw::draw_cylinder;
    hb_draw["box"] = draw::draw_box;
    hb_draw["triangle"] = draw::draw_triangle;
    hb_draw["capsule"] = draw::draw_capsule;
    hb_draw["sliced_cylinder"] = draw::draw_sliced_cylinder;
    hb_draw["sphere"] = draw::draw_sphere;
    hb_draw["set_num_segments"] = [](unsigned num) {
        g_hbdraw.imgui.num_segments = num;
    };
    hb_draw["set_outline_tickness"] = [](unsigned num) {
        g_hbdraw.imgui.outline_tickness = num;
    };
    hb_draw["register"] = [](sol::protected_function fn) {
        g_hbdraw.draw_fns.push_back(fn);
    };
    lua["hb_draw"] = hb_draw;
}

void on_device_reset() {
    ImGui_ImplDX12_Shutdown();
    g_d3d12 = {};
    g_hbdraw.imgui.initialized = false;
    g_hbdraw.camera = {};
    g_hbdraw.is_frame = false;
}

void on_lua_state_destroyed(lua_State *l) {
    API::LuaLock _{};
    g_hbdraw.lua = nullptr;
    g_hbdraw.draw_fns.clear();
}

extern "C" __declspec(dllexport) bool
reframework_plugin_initialize(const REFrameworkPluginInitializeParam *param) {
    API::initialize(param);

    const auto functions = param->functions;
    functions->on_lua_state_created(on_lua_state_created);
    functions->on_lua_state_destroyed(on_lua_state_destroyed);
    functions->on_present(do_render);
    functions->on_post_application_entry("EndRendering", do_draw);
    functions->on_device_reset(on_device_reset);

    return true;
}

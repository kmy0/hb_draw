#include "reframework/Math.hpp"

#include "plugin.h"
#include "scene.h"

#include <optional>

reframework::API::ManagedObject *scene::get_main_view() {
    const auto &api = reframework::API::get();
    static auto scene_manager_type = api->tdb()->find_type("via.SceneManager");
    static auto get_main_view_method =
        scene_manager_type->find_method("get_MainView");
    static auto main_view =
        get_main_view_method->call<reframework::API::ManagedObject *>(
            api->sdk()->functions->get_vm_context(),
            api->get_native_singleton("via.SceneManager"));

    return main_view;
}

reframework::API::ManagedObject *scene::get_primary_camera() {
    const auto &api = reframework::API::get();
    const auto main_view = get_main_view();

    if (main_view == nullptr) {
        return nullptr;
    }

    static auto scene_view_type = api->tdb()->find_type("via.SceneView");
    static auto get_primary_camera_method =
        scene_view_type->find_method("get_PrimaryCamera");

    return get_primary_camera_method->call<reframework::API::ManagedObject *>(
        api->sdk()->functions->get_vm_context(), main_view);
}

reframework::API::ManagedObject *scene::get_current_scene() {
    const auto &api = reframework::API::get();

    static auto scene_manager_type = api->tdb()->find_type("via.SceneManager");
    static auto get_current_scene_method =
        scene_manager_type->find_method("get_CurrentScene");
    static auto scene =
        get_current_scene_method->call<reframework::API::ManagedObject *>(
            api->sdk()->functions->get_vm_context(),
            api->get_native_singleton("via.SceneManager"));

    return scene;
}

std::optional<Vector2f> scene::world_to_screen(const Vector3f &world_pos) {
    auto &api = reframework::API::get();
    auto context = api->sdk()->functions->get_vm_context();
    const auto tdb = api->tdb();

    static auto math_t = tdb->find_type("via.math");
    static auto world_to_screen = math_t->find_method(
        "worldPos2ScreenPos(via.vec3, via.mat4, via.mat4, via.Size)");

    const Vector4f pos = Vector4f{world_pos, 1.0f};
    // behind camera
    if (glm::dot(pos - g_hbdraw.camera.origin, -g_hbdraw.camera.forward) <=
        0.0f) {
        return std::nullopt;
    }

    Vector2f screen_pos{};
    world_to_screen->call(&screen_pos, context, &pos, &g_hbdraw.camera.view,
                          &g_hbdraw.camera.proj, &g_hbdraw.camera.screen_size);
    return screen_pos;
}

bool scene::setup_camera() {
    if (g_hbdraw.camera.is_setup &&
        g_hbdraw.camera.camera->is_managed_object()) {
        return true;
    }

    g_hbdraw.camera.camera = scene::get_primary_camera();
    if (g_hbdraw.camera.camera == nullptr) {
        return false;
    }

    auto main_view = scene::get_main_view();
    if (main_view == nullptr) {
        return false;
    }

    auto &api = reframework::API::get();
    auto context = api->sdk()->functions->get_vm_context();
    const auto tdb = api->tdb();

    static auto transform_def = tdb->find_type("via.Transform");
    static auto scene_def = tdb->find_type("via.SceneView");
    static auto gameobject_def = tdb->find_type("via.GameObject");

    static auto get_gameobject_method =
        transform_def->find_method("get_GameObject");
    static auto get_transform_method =
        gameobject_def->find_method("get_Transform");
    static auto get_window_size_method =
        scene_def->find_method("get_WindowSize");

    auto camera_gameobject =
        get_gameobject_method->call<reframework::API::ManagedObject *>(
            context, g_hbdraw.camera.camera);
    g_hbdraw.camera.camera_transform =
        get_transform_method->call<reframework::API::ManagedObject *>(
            context, camera_gameobject);
    get_window_size_method->call(&g_hbdraw.camera.screen_size, context,
                                 main_view);
    g_hbdraw.camera.is_setup = true;
    return true;
}

bool scene::update_camera() {
    if (!setup_camera()) {
        return false;
    }

    auto &api = reframework::API::get();
    auto context = api->sdk()->functions->get_vm_context();
    const auto tdb = api->tdb();

    static auto transform_def = tdb->find_type("via.Transform");
    static auto get_axisz_method = transform_def->find_method("get_AxisZ");
    static auto get_axisy_method = transform_def->find_method("get_AxisY");
    static auto get_position_method =
        transform_def->find_method("get_Position");

    g_hbdraw.camera.origin = get_position_method->call<Vector4f>(
        context, g_hbdraw.camera.camera_transform);
    g_hbdraw.camera.forward = get_axisz_method->call<Vector4f>(
        context, g_hbdraw.camera.camera_transform);
    g_hbdraw.camera.up = get_axisy_method->call<Vector4f>(
        context, g_hbdraw.camera.camera_transform);
    g_hbdraw.camera.camera->call("get_ProjectionMatrix", &g_hbdraw.camera.proj,
                                 context, g_hbdraw.camera.camera);
    g_hbdraw.camera.camera->call("get_ViewMatrix", &g_hbdraw.camera.view,
                                 context, g_hbdraw.camera.camera);
    return true;
}

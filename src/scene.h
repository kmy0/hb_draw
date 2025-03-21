#pragma once

#include "reframework/API.hpp"
#include "reframework/Math.hpp"
#include "reframework/sdk.h"

#include <memory>
#include <optional>

namespace scene {
reframework::API::ManagedObject *get_primary_camera();
reframework::API::ManagedObject *get_main_view();
reframework::API::ManagedObject *get_current_scene();
std::optional<Vector2f> world_to_screen(const Vector3f &world_pos);
std::optional<Vector2f> world_to_screen_generic(const Vector3f &world_pos);
std::optional<Vector2f> world_to_screen_wilds(const Vector3f &world_pos);
bool update_camera();
bool setup_camera();
bool is_frame_gen();
} // namespace scene

struct camera {
    Vector4f up{};
    Vector4f origin{};
    Vector4f forward{};
    Matrix4x4f proj{};
    Matrix4x4f view{};
    float screen_size[2];
    reframework::API::ManagedObject *via_size;
    std::unique_ptr<ValueType> nullable_via_size;
    reframework::API::ManagedObject *camera;
    reframework::API::ManagedObject *camera_transform;
    bool is_setup{false};
    bool is_frame_gen{false};
};

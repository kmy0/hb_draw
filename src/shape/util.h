#pragma once

#include "imgui.h"
#include "reframework/Math.hpp"

#include "plugin.h"
#include "scene.h"

#include <optional>
#include <vector>

inline bool is_frontface(const Vector2f &a, const Vector2f &b,
                         const Vector2f &c) {
    const auto d1 = c - a;
    const auto d2 = c - b;
    return (d1.x * d2.y) - (d1.y * d2.x) > 0;
}

inline std::optional<std::pair<float, Vector2f>>
get_screen_radius(const Vector3f &pos, float radius) {
    const auto screen_pos_center = scene::world_to_screen(pos);
    if (screen_pos_center) {
        const auto pos_top =
            pos + (glm::normalize(Vector3f(g_hbdraw.camera.up)) * radius);
        const auto screen_pos_top = scene::world_to_screen(pos_top);

        if (screen_pos_top) {
            const auto radius2d =
                glm::length(*screen_pos_top - *screen_pos_center);

            return std::pair(radius2d, *screen_pos_center);
        }
    }

    return std::nullopt;
}

template <size_t S>
std::optional<std::array<Vector2f, S>>
get_screen_corners(const std::array<Vector4f, S> &points,
                   const Matrix4x4f &transform, const Vector3f &pos) {
    std::array<Vector2f, S> ret;
    for (size_t i = 0; i < points.size(); i++) {
        auto opt =
            scene::world_to_screen(Vector3f(points[i] * transform) + pos);

        if (!opt) {
            return std::nullopt;
        }
        ret[i] = std::move(*opt);
    }

    return ret;
}

inline bool is_point_ok(const Vector2f p) {
    const auto screen_size = g_hbdraw.camera.screen_size;
    return std::abs(p.x - screen_size[0]) < screen_size[0] * 2 &&
           std::abs(p.y - screen_size[1]) < screen_size[1] * 2;
}

inline float get_angle(const Vector2f &point, const Vector2f &center) {
    const auto delta = glm::normalize(point - center);
    return glm::atan(delta.y, delta.x);
}

inline bool path_ellipse(const Vector2f &center, float radius_x, float radius_y,
                         float rot, float a_min, float a_max, int num_segments,
                         std::vector<Vector2f> &out) {
    const auto drawlist = ImGui::GetBackgroundDrawList();
    drawlist->PathEllipticalArcTo(*(ImVec2 *)&center,
                                  ImVec2(radius_x, radius_y), rot, a_min, a_max,
                                  num_segments);
    std::vector<Vector2f> ret(drawlist->_Path.Size);

    for (size_t i = 0; i < drawlist->_Path.Size; i++) {
        const auto im_p = drawlist->_Path.Data[i];
        const auto p = Vector2f(im_p.x, im_p.y);
        if (!is_point_ok(p)) {
            return false;
        }
        out.push_back(p);
    }
    drawlist->PathClear();
    return true;
}

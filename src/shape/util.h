#pragma once

#include "reframework/Math.hpp"

#include "plugin.h"
#include "scene.h"

#include <optional>

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

inline bool intersect(const Vector2f &p1, const Vector2f &p2,
                      const Vector2f &q1, const Vector2f &q2) {
    return (((q1.x - p1.x) * (p2.y - p1.y) - (q1.y - p1.y) * (p2.x - p1.x)) *
                ((q2.x - p1.x) * (p2.y - p1.y) -
                 (q2.y - p1.y) * (p2.x - p1.x)) <
            0) &&
           (((p1.x - q1.x) * (q2.y - q1.y) - (p1.y - q1.y) * (q2.x - q1.x)) *
                ((p2.x - q1.x) * (q2.y - q1.y) -
                 (p2.y - q1.y) * (q2.x - q1.x)) <
            0);
}

inline bool is_point_ok(const Vector2f p) {
    return std::abs(p.x) < 10000 && std::abs(p.y) < 10000;
}

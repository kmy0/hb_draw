#include "reframework/Math.hpp"

#include "plugin.h"
#include "shapes.h"
#include "util.h"

#include <array>
#include <utility>

CylinderBase::CylinderBase(const Vector3f &start, const Vector3f &end,
                           float radius, float max_distortion)
    : m_max_distortion(max_distortion) {
    const auto dir = glm::normalize(end - start);

    if (!get_base(start, dir, radius, m_top) ||
        !get_base(end, dir, radius, m_bottom)) {
        return;
    }

    m_angle = get_angle(m_top.center, m_bottom.center) + glm::radians(90.0f);
    m_is_ok = true;
}

bool CylinderBase::get_base(const Vector3f &center, const Vector3f &dir,
                            float radius, Base &out) {
    const auto screen_radius = get_screen_radius(center, radius);

    if (!screen_radius) {
        return false;
    }

    out.major_radius = screen_radius->first;
    out.center = screen_radius->second;

    const auto origin = Vector3f(g_hbdraw.camera.origin);
    const auto view = glm::normalize(center - origin);
    const auto perp = glm::normalize(glm::cross(dir, view));
    const auto view_plane = glm::normalize(glm::cross(perp, dir));
    const std::array<Vector3f, 4> points3f{
        center + radius * perp, center - radius * perp,
        center + radius * view_plane, center - radius * view_plane};
    std::array<Vector2f, 4> points2f;

    for (size_t i = 0; i < points3f.size(); i++) {
        const auto p2f = scene::world_to_screen(points3f[i]);
        if (!p2f) {
            return false;
        }
        points2f[i] = *p2f;
    }

    if (is_distorted(out.center, points2f[0], points2f[1]) ||
        is_distorted(out.center, points2f[2], points2f[3])) {
        return false;
    }

    out.minor_radius = out.major_radius * glm::dot(dir, view);
    return true;
}

bool CylinderBase::is_distorted(const Vector2f &center, const Vector2f &p1,
                                const Vector2f &p2) {
    return std::abs(glm::length(p1 - center) - glm::length(p2 - center)) >=
           m_max_distortion;
}
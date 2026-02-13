#include "reframework/Math.hpp"

#include "plugin.h"
#include "shapes.h"
#include "util.h"

#include <array>
#include <utility>

CylinderBase::CylinderBase(const Vector3f &start, const Vector3f &end,
                           float radius, bool force_projected) {
    const auto dir = glm::normalize(end - start);
    const auto length = glm::length(start - end);

    if (!get_cap(start, dir, radius, force_projected, m_top) ||
        !get_cap(end, dir, radius, force_projected, m_bottom)) {
        return;
    }

    m_angle = m_top.angle;
    m_is_ok = true;
}

bool CylinderBase::get_cap(const Vector3f &center, const Vector3f &dir,
                           float radius, bool force_projected,
                           EllipseStruct &out) {
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

    const Vector2f screen_center = (points2f[0] + points2f[1]) * 0.5f;
    const Vector2f screen_center2 = (points2f[2] + points2f[3]) * 0.5f;

    const float center_deviation = glm::length(screen_center - screen_center2);
    const float avg_radius = (glm::length(points2f[0] - screen_center) +
                              glm::length(points2f[2] - screen_center2)) *
                             0.5f;

    if (center_deviation > avg_radius * 0.5f) {
        return false;
    }

    out.major_axis.start = points3f[0];
    out.major_axis.end = points3f[1];
    out.minor_axis.start = points3f[2];
    out.minor_axis.end = points3f[3];

    const auto cos_theta = glm::dot(dir, view);
    if (radius > 5.0f) {
        out.center = (points2f[2] + points2f[3]) * 0.5f;
        out.minor_radius = glm::length(points2f[2] - points2f[3]) * 0.5f;
        out.minor_radius =
            (cos_theta < 0) ? -out.minor_radius : out.minor_radius;
    } else if (force_projected) {
        out.minor_radius = glm::length(points2f[2] - points2f[3]) * 0.5f;
        out.major_radius = glm::length(points2f[0] - points2f[1]) * 0.5f;
    } else {
        out.minor_radius = out.major_radius * cos_theta;
    }

    out.angle = get_angle(points2f[0], points2f[1]);
    return true;
}

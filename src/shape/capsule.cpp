#include "reframework/Math.hpp"

#include "shapes.h"
#include "util.h"

Capsule::Capsule(const Vector3f &start, const Vector3f &end, float radius) {
    const auto top_screen_radius = get_screen_radius(start, radius);
    const auto bottom_screen_radius = get_screen_radius(end, radius);

    if (!top_screen_radius || !bottom_screen_radius) {
        return;
    }

    m_top.radius = top_screen_radius->first;
    m_bottom.radius = bottom_screen_radius->first;
    m_top.center = top_screen_radius->second;
    m_bottom.center = bottom_screen_radius->second;

    const auto ctcb = m_top.center - m_bottom.center;
    m_distance = glm::length(ctcb);
    if ((m_distance + m_bottom.radius) * 0.99 <= m_top.radius ||
        (m_distance + m_top.radius) * 0.99 <= m_bottom.radius) {
        m_is_ok = true;
        m_is_sphere = true;
        return;
    }

    const auto r_diff = m_bottom.radius - m_top.radius;
    const auto t_length_bottom = std::sqrt(
        std::max(0.0f, m_bottom.radius * m_bottom.radius -
                           (r_diff * r_diff) / (m_distance * m_distance) *
                               m_bottom.radius * m_bottom.radius));
    const auto t_length_top = std::sqrt(
        std::max(0.0f, m_top.radius * m_top.radius -
                           (r_diff * r_diff) / (m_distance * m_distance) *
                               m_top.radius * m_top.radius));
    const auto dir = ctcb / m_distance;
    const auto perp = Vector2f(-dir.y, dir.x);
    const auto h = r_diff / m_distance;

    m_quad[0] = m_top.center + dir * (h * m_top.radius) + perp * t_length_top;
    m_quad[1] = m_top.center + dir * (h * m_top.radius) - perp * t_length_top;
    m_quad[2] =
        m_bottom.center + dir * (h * m_bottom.radius) - perp * t_length_bottom;
    m_quad[3] =
        m_bottom.center + dir * (h * m_bottom.radius) + perp * t_length_bottom;

    for (const auto &p : m_quad) {
        if (!is_point_ok(p)) {
            return;
        }
    }

    auto get_angle = [](const Vector2f &point, const Vector2f &center) {
        const auto delta = glm::normalize(point - center);
        return glm::atan(delta.y, delta.x);
    };

    m_top.a_min = get_angle(m_quad[0], m_top.center);
    m_top.a_max = get_angle(m_quad[1], m_top.center);
    m_bottom.a_min = get_angle(m_quad[2], m_bottom.center);
    m_bottom.a_max = get_angle(m_quad[3], m_bottom.center);

    m_top.a_min = m_top.a_min < m_top.a_max ? m_top.a_min + glm::radians(360.0f)
                                            : m_top.a_min;
    m_bottom.a_min = m_bottom.a_min < m_bottom.a_max
                         ? m_bottom.a_min + glm::radians(360.0f)
                         : m_bottom.a_min;
    m_is_ok = true;
}
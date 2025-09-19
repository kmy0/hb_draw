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

    const float r_diff = m_bottom.radius - m_top.radius;
    const float inv_distance = 1.0f / m_distance;
    const Vector2f dir = ctcb * inv_distance;
    const Vector2f perp(-dir.y, dir.x);
    const float h = r_diff * inv_distance;
    const float sqrt_factor = std::sqrt(std::max(
        0.0f, 1.0f - (r_diff * r_diff) * (inv_distance * inv_distance)));

    const float t_length_bottom = m_bottom.radius * sqrt_factor;
    const float t_length_top = m_top.radius * sqrt_factor;

    const Vector2f top_base = m_top.center + dir * (h * m_top.radius);
    const Vector2f bottom_base = m_bottom.center + dir * (h * m_bottom.radius);
    const Vector2f perp_top = perp * t_length_top;
    const Vector2f perp_bottom = perp * t_length_bottom;

    m_quad[0] = top_base + perp_top;
    m_quad[1] = top_base - perp_top;
    m_quad[2] = bottom_base - perp_bottom;
    m_quad[3] = bottom_base + perp_bottom;

    for (const auto &p : m_quad) {
        if (!is_point_ok(p)) {
            return;
        }
    }

    m_top.a_min = get_angle(m_quad[0], m_top.center);
    m_top.a_max = get_angle(m_quad[1], m_top.center);
    m_bottom.a_min = get_angle(m_quad[2], m_bottom.center);
    m_bottom.a_max = get_angle(m_quad[3], m_bottom.center);

    constexpr float TWO_PI = glm::radians(360.0f);
    if (m_top.a_min < m_top.a_max) {
        m_top.a_min += TWO_PI;
    }
    if (m_bottom.a_min < m_bottom.a_max) {
        m_bottom.a_min += TWO_PI;
    }
    m_is_ok = true;
}
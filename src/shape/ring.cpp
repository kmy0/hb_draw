
#include "reframework/Math.hpp"

#include "shapes.h"
#include "util.h"

Ring::Ring(const Vector3f &start, const Vector3f &end, float radius_a,
           float radius_b, unsigned num_segments)
    : m_num_segments(num_segments) {
    radius_b = radius_b - radius_a;

    auto base_inner = CylinderBase(start, end, radius_a);
    if (!base_inner.m_is_ok) {
        return;
    }

    auto base_outer = CylinderBase(start, end, radius_b);
    if (!base_outer.m_is_ok) {
        return;
    }

    if (!path_ellipse(base_inner.m_top.center, base_inner.m_top.major_radius,
                      base_inner.m_top.minor_radius, base_inner.m_angle, 0,
                      glm::radians(360.0f), m_num_segments, m_inner_top_el) ||
        !path_ellipse(
            base_inner.m_bottom.center, base_inner.m_bottom.major_radius,
            base_inner.m_bottom.minor_radius, base_inner.m_angle, 0,
            glm::radians(360.0f), m_num_segments, m_inner_bottom_el) ||
        !path_ellipse(base_outer.m_top.center, base_outer.m_top.major_radius,
                      base_outer.m_top.minor_radius, base_outer.m_angle, 0,
                      glm::radians(360.0f), m_num_segments, m_outer_top_el) ||
        !path_ellipse(
            base_outer.m_bottom.center, base_outer.m_bottom.major_radius,
            base_outer.m_bottom.minor_radius, base_outer.m_angle, 0,
            glm::radians(360.0f), m_num_segments, m_outer_bottom_el)) {
        return;
    }

    m_is_ok = true;
}

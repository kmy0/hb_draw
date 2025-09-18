#include "reframework/Math.hpp"

#include "shapes.h"
#include "util.h"

Cylinder::Cylinder(const Vector3f &start, const Vector3f &end, float radius,
                   unsigned num_segments)
    : CylinderBase(start, end, radius), m_num_segments(num_segments) {
    if (!m_is_ok) {
        return;
    }

    m_is_ok = false;
    if (!path_ellipse(m_top.center, m_top.major_radius, m_top.minor_radius,
                      m_angle, 0, glm::radians(360.0f), m_num_segments,
                      m_el1) ||
        !path_ellipse(m_bottom.center, m_bottom.major_radius,
                      m_bottom.minor_radius, m_angle, 0, glm::radians(360.0f),
                      m_num_segments, m_el2)) {
        return;
    }

    const size_t cap_max = m_num_segments / 2;
    m_cap = nullptr;
    for (size_t i = 0; i <= m_num_segments; i += 2) {
        i = i == m_num_segments ? i - 1 : i;
        size_t j = i <= cap_max ? i + cap_max : i - cap_max;
        if (is_frontface(m_el1[i], m_el1[i + 1], m_el1[j])) {
            m_cap = &m_el1;
            break;
        }

        if (is_frontface(m_el2[j], m_el2[i + 1], m_el2[i])) {
            m_cap = &m_el2;
            break;
        }
    }

    unsigned begin = 0;
    std::vector<Vector2f *> rev;
    for (size_t i = 0; i <= m_num_segments; i += 2) {
        i = i == m_num_segments ? i - 1 : i;
        if (is_frontface(m_el2[i], m_el1[i + 1], m_el1[i])) {
            m_top_el.insert(m_top_el.begin() + begin,
                            {&m_el1[i], &m_el1[i + 1]});
            m_bottom_el.insert(m_bottom_el.begin() + begin,
                               {&m_el2[i], &m_el2[i + 1]});
            begin += 2;
        } else {
            begin = 0;
        }
    }

    m_is_ok = true;
}

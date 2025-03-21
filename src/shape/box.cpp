#include "reframework/Math.hpp"

#include "shapes.h"
#include "util.h"

#include <array>
#include <vector>

Box::Box(const Vector3f &pos, const Vector3f &extent, const Matrix4x4f &rot) {
    const std::array<Vector4f, 8> corners4f = {
        Vector4f(extent, 0) * -1.0f,
        Vector4f(extent.x, -extent.y, -extent.z, 0),
        Vector4f(extent.x, extent.y, -extent.z, 0),
        Vector4f(-extent.x, extent.y, -extent.z, 0),
        Vector4f(-extent.x, extent.y, extent.z, 0),
        Vector4f(-extent.x, -extent.y, extent.z, 0),
        Vector4f(extent.x, -extent.y, extent.z, 0),
        Vector4f(extent, 0)};

    auto opt = get_screen_corners(corners4f, rot, pos);
    if (!opt) {
        return;
    }

    m_points = std::move(*opt);
    m_quads_p = {{
        {&m_points[0], &m_points[1], &m_points[2], &m_points[3]},
        {&m_points[0], &m_points[3], &m_points[4], &m_points[5]},
        {&m_points[5], &m_points[6], &m_points[1], &m_points[0]},
        {&m_points[6], &m_points[7], &m_points[2], &m_points[1]},
        {&m_points[7], &m_points[4], &m_points[3], &m_points[2]},
        {&m_points[7], &m_points[6], &m_points[5], &m_points[4]},
    }};

    for (auto &quad : m_quads_p) {
        if (is_frontface(*quad[0], *quad[1], *quad[2])) {
            m_quads.push_back(&quad);
        }
    }

    m_is_ok = !m_quads.empty();
}

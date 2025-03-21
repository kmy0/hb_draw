#include "reframework/Math.hpp"

#include "shapes.h"
#include "util.h"

#include <array>
#include <vector>

Triangle::Triangle(const Vector3f &pos, const Vector3f &extent,
                   const Matrix4x4f &rot) {
    const std::array<Vector4f, 3> top_tri = {
        Vector4f(extent, 0),
        Vector4f(-extent.x, extent.y, extent.z, 0),
        Vector4f(0, extent.y, -extent.z, 0),
    };
    const std::array<Vector4f, 3> bottom_tri = {
        Vector4f(0, -extent.y, -extent.z, 0),
        Vector4f(-extent.x, -extent.y, extent.z, 0),
        Vector4f(extent.x, -extent.y, extent.z, 0),
    };

    if (!get_triangle(pos, rot, top_tri, m_top_points, m_top_triangle) ||
        !get_triangle(pos, rot, bottom_tri, m_bottom_points,
                      m_bottom_triangle)) {
        return;
    };

    m_quads_p = {
        {
            {
                &m_bottom_points[2],
                &m_bottom_points[1],
                &m_top_points[1],
                &m_top_points[0],

            },
            {
                &m_bottom_points[1],
                &m_bottom_points[0],
                &m_top_points[2],
                &m_top_points[1],

            },
            {
                &m_bottom_points[0],
                &m_bottom_points[2],
                &m_top_points[0],
                &m_top_points[2],
            },
        },
    };

    for (auto &quad : m_quads_p) {
        if (is_frontface(*quad[0], *quad[1], *quad[2])) {
            m_quads.push_back(&quad);
        }
    }
    m_is_ok = (!m_top_triangle && !m_top_triangle) || !m_quads.empty();
}

bool Triangle::get_triangle(const Vector3f &pos, const Matrix4x4f &rot,
                            const std::array<Vector4f, 3> &triangle4f,
                            std::array<Vector2f, 3> &corners2f,
                            std::array<Vector2f, 3> *&culled) {
    auto opt = get_screen_corners(triangle4f, rot, pos);
    if (!opt) {
        return false;
    }
    corners2f = std::move(*opt);
    if (is_frontface(corners2f[2], corners2f[0], corners2f[1])) {
        culled = &corners2f;
    }
    return true;
}
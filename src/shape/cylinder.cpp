#include "reframework/Math.hpp"

#include "plugin.h"
#include "shapes.h"
#include "util.h"

#include <array>

Cylinder::Cylinder(const Vector3f &start, const Vector3f &end, float radius,
                   float rot, bool is_hollow)
    : m_rot(rot), m_is_hollow(is_hollow) {
    m_angle_increment = glm::radians(360.0f) / g_hbdraw.imgui.num_segments;
    m_top_points.resize(g_hbdraw.imgui.num_segments);
    m_bottom_points.resize(g_hbdraw.imgui.num_segments);

    const auto dir = glm::normalize(end - start);
    m_up = glm::cross(dir, Vector3f(0, 1, 0));

    if (glm::length(m_up) < 0.0001f) {
        m_up = glm::cross(dir, Vector3f(1, 0, 0));
    }

    m_right = glm::cross(m_up, dir);
    m_up = glm::normalize(m_up) * radius;
    m_right = glm::normalize(m_right) * radius;

    const size_t base_max_i = g_hbdraw.imgui.num_segments / 2;
    size_t face_begin = 0;
    size_t base_begin = 0;
    std::array<Vector2f *, 4> out;
    size_t j = 0;
    size_t i = 0;

    auto insert_base = [&](std::vector<Vector2f *> &partial_base,
                           std::vector<Vector2f *> &full_base) {
        partial_base.insert(partial_base.begin() + base_begin,
                            {out[0], out[1]});
        full_base.insert(full_base.end(), {out[0], out[1]});
        base_begin += 2;
        face_begin = 0;
    };

    auto insert_face = [&]() {
        m_top_ellipse_face.insert(m_top_ellipse_face.begin() + face_begin,
                                  {out[0], out[1]});
        m_bottom_ellipse_face.insert(m_bottom_ellipse_face.begin() + face_begin,
                                     {out[2], out[3]});
        m_top_base.insert(m_top_base.end(), {out[0], out[1]});
        m_bottom_base.insert(m_bottom_base.end(), {out[2], out[3]});
        face_begin += 2;
        base_begin = 0;
    };

    for (i = 0; i <= g_hbdraw.imgui.num_segments; i += 2) {
        i = i == g_hbdraw.imgui.num_segments ? i - 1 : i;
        j = i <= base_max_i ? i + base_max_i : i - base_max_i;

        result top_res, bottom_res = result::miss;
        // get_face has to be always called before get_base!
        if (m_bottom_ellipse_base.empty()) {
            top_res = get_top_face(start, end, out, i);
            switch (top_res) {
            case result::miss:
                top_res = get_top_base(start, end, out, j);
                switch (top_res) {
                case result::miss:
                    break;
                case result::none:
                    return;
                case result::hit:
                    insert_base(m_top_ellipse_base, m_top_base);
                    break;
                }
                break;
            case result::none:
                return;
            case result::hit:
                insert_face();
                break;
            }
        }

        if (m_top_ellipse_base.empty() && top_res == result::miss) {
            bottom_res = get_bottom_face(start, end, out, i);
            switch (bottom_res) {
            case result::miss:
                bottom_res = get_bottom_base(start, end, out, j);
                switch (bottom_res) {
                case result::miss:
                    break;
                case result::none:
                    return;
                case result::hit:
                    insert_base(m_bottom_ellipse_base, m_bottom_base);
                    break;
                }
                break;
            case result::none:
                return;
            case result::hit:
                insert_face();
                break;
            }
        }

        // this entire insert in front is necessary to keep points in clockwise
        // order, calculating angles and sorting might be faster?
        if (top_res == result::miss && bottom_res == result::miss) {
            base_begin = 0;
            face_begin = 0;
        }
    }
    m_is_ok = true;
}

Cylinder::result Cylinder::get_top_face(const Vector3f &start,
                                        const Vector3f &end,
                                        std::array<Vector2f *, 4> &out,
                                        size_t segment) {
    if (!(m_points[0] = get_point(end, m_bottom_points, segment)) ||
        !(m_points[1] = get_point(start, m_top_points, segment + 1)) ||
        !(m_points[2] = get_point(start, m_top_points, segment))) {
        return result::none;
    }
    if ((!m_is_hollow &&
         is_frontface(*m_points[0], *m_points[1], *m_points[2])) ||
        (m_is_hollow &&
         is_frontface(*m_points[2], *m_points[1], *m_points[0]))) {
        if (!(m_points[3] = get_point(end, m_bottom_points, segment + 1))) {
            return result::none;
        }
        out[0] = m_points[2];
        out[1] = m_points[1];
        out[2] = m_points[0];
        out[3] = m_points[3];
        return result::hit;
    }
    return result::miss;
}
Cylinder::result Cylinder::get_top_base(const Vector3f &start,
                                        const Vector3f &end,
                                        std::array<Vector2f *, 4> &out,
                                        size_t segment) {
    if (!(m_points[4] = get_point(start, m_top_points, segment))) {
        return result::none;
    }
    if (is_frontface(*m_points[2], *m_points[1], *m_points[4])) {
        out[0] = m_points[2];
        out[1] = m_points[1];
        return result::hit;
    }
    return result::miss;
}

Cylinder::result Cylinder::get_bottom_face(const Vector3f &start,
                                           const Vector3f &end,
                                           std::array<Vector2f *, 4> &out,
                                           size_t segment) {
    if (!(m_points[0] = get_point(end, m_bottom_points, segment)) ||
        !(m_points[1] = get_point(end, m_bottom_points, segment + 1)) ||
        !(m_points[2] = get_point(start, m_top_points, segment))) {
        return result::none;
    }
    if (((!m_is_hollow &&
          is_frontface(*m_points[0], *m_points[1], *m_points[2])) ||
         (m_is_hollow &&
          is_frontface(*m_points[2], *m_points[1], *m_points[0])))) {
        if (!(m_points[3] = get_point(start, m_top_points, segment + 1))) {
            return result::none;
        }
        out[0] = m_points[2];
        out[1] = m_points[3];
        out[2] = m_points[0];
        out[3] = m_points[1];
        return result::hit;
    }
    return result::miss;
}

Cylinder::result Cylinder::get_bottom_base(const Vector3f &start,
                                           const Vector3f &end,
                                           std::array<Vector2f *, 4> &out,
                                           size_t segment) {
    if (!(m_points[4] = get_point(end, m_bottom_points, segment))) {
        return result::none;
    }
    if (is_frontface(*m_points[4], *m_points[1], *m_points[0])) {
        out[0] = m_points[0];
        out[1] = m_points[1];
        return result::hit;
    }
    return result::miss;
}

Vector2f *Cylinder::get_point(const Vector3f &pos,
                              std::vector<std::optional<Vector2f>> &cache,
                              size_t segment) {
    if (segment == g_hbdraw.imgui.num_segments) {
        segment = 0;
    }

    if (cache[segment]) {
        return &*cache[segment];
    }

    const float angle = m_rot + m_angle_increment * segment;
    cache[segment] = scene::world_to_screen(pos + m_right * std::cos(angle) +
                                            m_up * std::sin(angle));

    if (!cache[segment]) {
        return nullptr;
    }
    return &*cache[segment];
}

#include "reframework/Math.hpp"

#include "shapes.h"
#include "util.h"

#include <vector>

SlicedCylinder::SlicedCylinder(const Vector3f &start, const Vector3f &end,
                               float radius, const Vector3f &direction,
                               float degrees, unsigned num_segments)
    : CylinderBase(start, end, radius, 150.0f),
      m_slice_angle(glm::radians(degrees) / 2), m_num_segments(num_segments) {
    if (!m_is_ok) {
        return;
    }

    m_is_ok = false;
    const auto dir = glm::normalize(end - start);
    const auto eye_proj =
        glm::normalize(direction - glm::dot(direction, dir) * dir);
    auto s_top = SlicedEllipseStruct(m_top);
    auto s_bottom = SlicedEllipseStruct(m_bottom);

    get_eye(start, dir, eye_proj, radius, s_top, m_eye1);
    get_eye(end, dir, eye_proj, radius, s_bottom, m_eye2);

    if (!path_ellipse(s_top.center, s_top.major_radius, s_top.minor_radius,
                      m_angle, s_top.a_min, s_top.a_max, m_num_segments,
                      m_top_el) ||
        !path_ellipse(s_bottom.center, s_bottom.major_radius,
                      s_bottom.minor_radius, m_angle, s_bottom.a_min,
                      s_bottom.a_max, m_num_segments, m_bottom_el)) {
        return;
    }

    auto a_max = clamp_angle(s_top.a_max);
    auto a_min = clamp_angle(s_top.a_min);
    const auto rad = glm::radians(180.0f);

    if (((a_max >= rad && a_min <= rad) || m_slice_angle >= rad) &&
        !get_side_outline(1, m_right_outline)) {
        return;
    }

    if (((a_max <= rad && a_min >= rad) || m_slice_angle >= rad) &&
        !get_side_outline(-1, m_left_outline)) {
        return;
    }

    m_is_ok = true;
}

void SlicedCylinder::get_eye(const Vector3f &center, const Vector3f &dir,
                             const Vector3f &eye_proj, float radius,
                             SlicedEllipseStruct &ellipse, Vector3f &out) {
    out = center + radius * eye_proj;
    const auto major_dir = glm::normalize(ellipse.major_axis.start - center);
    const auto major_proj =
        glm::normalize(major_dir - glm::dot(major_dir, dir) * dir);
    const auto cross = glm::cross(eye_proj, major_proj);
    const auto sign = glm::dot(cross, dir) < 0 ? 1.0f : -1.0f;
    auto angle = glm::angle(eye_proj, major_proj) * sign;
    ellipse.a_min = angle - m_slice_angle;
    ellipse.a_max = angle + m_slice_angle;
}

bool SlicedCylinder::get_side_outline(
    int side, std::optional<std::pair<Vector2f, Vector2f>> &out) {
    std::vector<Vector2f> el1;
    std::vector<Vector2f> el2;
    if (!path_ellipse(m_top.center, m_top.major_radius, m_top.minor_radius,
                      m_angle, 0, glm::radians(180.0f), 4, el1) ||
        !path_ellipse(m_bottom.center, m_bottom.major_radius,
                      m_bottom.minor_radius, m_angle, 0, glm::radians(180.0f),
                      4, el2)) {
        return false;
    }

    if (side < 0) {
        out = std::make_pair(el1[0], el2[0]);
    } else {
        out = std::make_pair(el1.back(), el2.back());
    }
    return true;
}

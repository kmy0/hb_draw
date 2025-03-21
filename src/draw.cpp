#include "imgui.h"
#include "reframework/Math.hpp"

#include "draw.h"
#include "plugin.h"

#include <vector>

void draw::util::paint(ImU32 color, bool outline, ImU32 color_outline,
                       ImDrawFlags stroke_flags, fill_type fill_type) {
    const auto drawlist = ImGui::GetBackgroundDrawList();
    switch (fill_type) {
    case fill_type::convex:
        drawlist->AddConvexPolyFilled(drawlist->_Path.Data,
                                      drawlist->_Path.Size, color);
        break;
    case fill_type::concave:
        drawlist->AddConcavePolyFilled(drawlist->_Path.Data,
                                       drawlist->_Path.Size, color);
        break;
    }
    if (outline) {
        drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size,
                              color_outline, stroke_flags,
                              g_hbdraw.imgui.outline_tickness);
    }
    drawlist->PathClear();
}

void draw::util::draw_ellipse(const ImVec2 &center, float radius_x,
                              float radius_y, float rot, float a_min,
                              float a_max, ImU32 color, int num_segments,
                              float thickness, ImDrawFlags flags) {
    const auto drawlist = ImGui::GetBackgroundDrawList();
    drawlist->PathEllipticalArcTo(center, ImVec2(radius_x, radius_y), rot,
                                  a_min, a_max, num_segments);
    drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size, color,
                          flags, thickness);
    drawlist->PathClear();
}

void draw::util::path_points(const std::vector<Vector2f *> *points,
                             bool reverse) {
    const auto drawlist = ImGui::GetBackgroundDrawList();
    const auto size = points->size();
    if (points->empty()) {
        return;
    }

    const auto vec = *points;
    if (reverse) {
        for (int i = size - 1; i >= 0; i--) {
            drawlist->PathLineToMergeDuplicate(*(ImVec2 *)&*vec[i]);
        }
    } else {
        for (size_t i = 0; i < size; i++) {
            drawlist->PathLineToMergeDuplicate(*(ImVec2 *)&*vec[i]);
        }
    }
}

void draw::util::path_points_duplicate(const std::vector<Vector2f *> *points,
                                       bool reverse) {
    const auto drawlist = ImGui::GetBackgroundDrawList();
    const auto size = points->size();
    if (points->empty()) {
        return;
    }

    const auto vec = *points;
    if (reverse) {
        for (int i = size - 1; i > 0; i--) {
            drawlist->PathLineTo(*(ImVec2 *)&*vec[i]);
            drawlist->PathLineTo(*(ImVec2 *)&*vec[i - 1]);
        }
    } else {
        for (size_t i = 0; i < size - 1; i++) {
            drawlist->PathLineTo(*(ImVec2 *)&*vec[i]);
            drawlist->PathLineTo(*(ImVec2 *)&*vec[i + 1]);
        }
    }
}

void draw::draw_sphere(const Vector3f &center, float radius, ImU32 color,
                       bool outline, ImU32 color_outline) {
    const auto sphere = Sphere(center, radius);
    if (!sphere.m_is_ok) {
        return;
    }
    draw(sphere, color, outline, color_outline);
}

void draw::draw_box(const Vector3f &pos, const Vector3f &extent,
                    const Matrix4x4f &rot, ImU32 color, bool outline,
                    ImU32 color_outline) {
    const auto box = Box(pos, extent, glm::inverse(rot));
    if (!box.m_is_ok) {
        return;
    }
    draw(box, color, outline, color_outline);
}

void draw::draw_triangle(const Vector3f &pos, const Vector3f &extent,
                         const Matrix4x4f &rot, ImU32 color, bool outline,
                         ImU32 color_outline) {
    const auto triangle = Triangle(pos, extent, glm::inverse(rot));
    if (!triangle.m_is_ok) {
        return;
    }
    draw(triangle, color, outline, color_outline);
}

void draw::draw_cylinder(const Vector3f &start, const Vector3f &end,
                         float radius, ImU32 color, bool outline,
                         ImU32 color_outline) {
    if (glm::length(end - start) <= 0.0f) {
        draw_sphere(start, radius, color, outline, color_outline);
        return;
    }
    const auto cylinder = Cylinder(start, end, radius);
    if (!cylinder.m_is_ok) {
        return;
    }
    draw(cylinder, color, outline, color_outline);
}

void draw::draw_ring(const Vector3f &start, const Vector3f &end, float radius_a,
                     float radius_b, ImU32 color, bool outline,
                     ImU32 color_outline) {
    const auto ring = Ring(start, end, radius_a, radius_b);
    if (!ring.m_is_ok) {
        return;
    }
    draw(ring, color, outline, color_outline);
}

void draw::draw_capsule(const Vector3f &start, const Vector3f &end,
                        float radius, ImU32 color, bool outline,
                        ImU32 color_outline) {
    if (glm::length(end - start) <= 0.0f) {
        draw_sphere(start, radius, color, outline, color_outline);
        return;
    }

    const auto capsule = Capsule(start, end, radius);
    if (!capsule.m_is_ok) {
        return;
    }
    draw(capsule, color, outline, color_outline);
};

void draw::draw(const Box &shape, ImU32 color, bool outline,
                ImU32 color_outline) {
    if (!shape.m_is_ok) {
        return;
    }
    const auto drawlist = ImGui::GetBackgroundDrawList();
    for (auto &quad : shape.m_quads) {
        const auto arr = *quad;
        drawlist->PathLineTo(*(ImVec2 *)&*arr[0]);
        drawlist->PathLineTo(*(ImVec2 *)&*arr[1]);
        drawlist->PathLineTo(*(ImVec2 *)&*arr[2]);
        drawlist->PathLineTo(*(ImVec2 *)&*arr[3]);

        util::paint(color, outline, color_outline);
    }
}

void draw::draw(const Sphere &shape, ImU32 color, bool outline,
                ImU32 color_outline) {
    if (!shape.m_is_ok) {
        return;
    }
    const auto drawlist = ImGui::GetBackgroundDrawList();
    const auto center = *(ImVec2 *)&shape.m_center;
    drawlist->AddCircleFilled(center, shape.m_radius, color,
                              g_hbdraw.imgui.num_segments);

    if (outline) {
        const float minor_radius = shape.m_radius * std::cos(45);
        const auto rad = glm::radians(180.0f);
        drawlist->AddCircle(center, shape.m_radius, color_outline,
                            g_hbdraw.imgui.num_segments);
        util::draw_ellipse(center, shape.m_radius, minor_radius, 0, 0, rad,
                           color_outline, g_hbdraw.imgui.num_segments,
                           g_hbdraw.imgui.outline_tickness);
        util::draw_ellipse(center, shape.m_radius, minor_radius, 90, 0, rad,
                           color_outline, g_hbdraw.imgui.num_segments,
                           g_hbdraw.imgui.outline_tickness);
        util::draw_ellipse(center, shape.m_radius, minor_radius, 180, 0, rad,
                           color_outline, g_hbdraw.imgui.num_segments,
                           g_hbdraw.imgui.outline_tickness);
    }
}

void draw::draw(const Triangle &shape, ImU32 color, bool outline,
                ImU32 color_outline) {
    if (!shape.m_is_ok) {
        return;
    }

    const auto drawlist = ImGui::GetBackgroundDrawList();
    for (const auto &tri : {shape.m_top_triangle, shape.m_bottom_triangle}) {
        if (!tri) {
            continue;
        }
        const auto arr = *tri;
        drawlist->PathLineTo(*(ImVec2 *)&arr[0]);
        drawlist->PathLineTo(*(ImVec2 *)&arr[1]);
        drawlist->PathLineTo(*(ImVec2 *)&arr[2]);

        util::paint(color, outline, color_outline);
    }

    for (auto &quad : shape.m_quads) {
        const auto arr = *quad;
        drawlist->PathLineTo(*(ImVec2 *)&*arr[0]);
        drawlist->PathLineTo(*(ImVec2 *)&*arr[1]);
        drawlist->PathLineTo(*(ImVec2 *)&*arr[2]);
        drawlist->PathLineTo(*(ImVec2 *)&*arr[3]);

        util::paint(color, outline, color_outline);
    }
}

void draw::draw(const Cylinder &shape, ImU32 color, bool outline,
                ImU32 color_outline) {
    if (!shape.m_is_ok) {
        return;
    }

    const auto drawlist = ImGui::GetBackgroundDrawList();
    const auto base_ellipse = shape.m_top_ellipse_base.empty()
                                  ? &shape.m_bottom_ellipse_base
                                  : &shape.m_top_ellipse_base;
    const std::vector<Vector2f *> *face_ellipse1, *face_ellipse2;
    if (!shape.m_top_ellipse_face.empty()) {
        face_ellipse1 = &shape.m_top_ellipse_face;
        face_ellipse2 = &shape.m_bottom_ellipse_face;
        if (shape.m_top_ellipse_base.empty()) {
            face_ellipse1 = &shape.m_bottom_ellipse_face;
            face_ellipse2 = &shape.m_top_ellipse_face;
        }

        auto size = face_ellipse1->size();
        for (size_t i = 0; i < size - 1; i++) {
            drawlist->AddQuadFilled(*(ImVec2 *)&*(*face_ellipse1)[i],
                                    *(ImVec2 *)&*(*face_ellipse1)[i + 1],
                                    *(ImVec2 *)&*(*face_ellipse2)[i + 1],
                                    *(ImVec2 *)&*(*face_ellipse2)[i], color);
        }

        if (outline) {
            util::path_points(face_ellipse1);
            util::path_points(face_ellipse2, true);
            drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size,
                                  color_outline, 1,
                                  g_hbdraw.imgui.outline_tickness);
            drawlist->PathClear();
        }

        if (!base_ellipse->empty()) {
            util::path_points(face_ellipse1);
            util::path_points(base_ellipse);
            util::paint(color, outline, color_outline, 1,
                        util::fill_type::convex);
        }
    } else {
        util::path_points(base_ellipse);
        util::paint(color, outline, color_outline, 0, util::fill_type::convex);
    }
}

void draw::draw(const Ring &shape, ImU32 color, bool outline,
                ImU32 color_outline) {
    if (!shape.m_is_ok) {
        return;
    }
    const auto drawlist = ImGui::GetBackgroundDrawList();
    const std::vector<Vector2f *> *base_ellipse_outer, *base_outer,
        *base_ellipse_inner, *base_inner, *face_ellipse_outer1,
        *face_ellipse_outer2, *face_ellipse_inner1, *face_ellipse_inner2;
    const Vector2f *center;

    if (shape.m_outer_cylinder->m_top_ellipse_base.empty()) {
        base_ellipse_outer = &shape.m_outer_cylinder->m_bottom_ellipse_base;
        base_outer = &shape.m_outer_cylinder->m_bottom_base;
        base_ellipse_inner = &shape.m_inner_cylinder->m_bottom_ellipse_base;
        base_inner = &shape.m_inner_cylinder->m_bottom_base;
        center = &shape.m_end2f;
    } else {
        base_ellipse_outer = &shape.m_outer_cylinder->m_top_ellipse_base;
        base_outer = &shape.m_outer_cylinder->m_top_base;
        base_ellipse_inner = &shape.m_inner_cylinder->m_top_ellipse_base;
        base_inner = &shape.m_inner_cylinder->m_top_base;
        center = &shape.m_start2f;
    }

    if (shape.m_outer_cylinder->m_top_ellipse_base.empty()) {
        face_ellipse_outer1 = &shape.m_outer_cylinder->m_bottom_ellipse_face;
        face_ellipse_outer2 = &shape.m_outer_cylinder->m_top_ellipse_face;
        face_ellipse_inner1 = &shape.m_inner_cylinder->m_bottom_ellipse_face;
        face_ellipse_inner2 = &shape.m_inner_cylinder->m_top_ellipse_face;
    } else {
        face_ellipse_outer1 = &shape.m_outer_cylinder->m_top_ellipse_face;
        face_ellipse_outer2 = &shape.m_outer_cylinder->m_bottom_ellipse_face;
        face_ellipse_inner1 = &shape.m_inner_cylinder->m_top_ellipse_face;
        face_ellipse_inner2 = &shape.m_inner_cylinder->m_bottom_ellipse_face;
    }

    // inner not visible
    if (base_ellipse_outer->empty() && base_ellipse_inner->empty()) {
        draw(*shape.m_outer_cylinder, color, outline, color_outline);
        return;
    }

    // fill between inner and outer
    {
        const auto size = base_outer->size();
        for (int i = 0; i <= size - 1; i++) {
            auto j = i == size - 1 ? 0 : i + 1;
            drawlist->AddQuadFilled(*(ImVec2 *)&*(*base_outer)[i],
                                    *(ImVec2 *)&*(*base_outer)[j],
                                    *(ImVec2 *)&*(*base_inner)[j],
                                    *(ImVec2 *)&*(*base_inner)[i], color);
        }
    }

    if (outline) {
        util::path_points(base_inner);
        drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size,
                              color_outline, 0,
                              g_hbdraw.imgui.outline_tickness);
        drawlist->PathClear();
        util::path_points(base_outer);
        drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size,
                              color_outline, 0,
                              g_hbdraw.imgui.outline_tickness);
        drawlist->PathClear();
    }

    // fully see through
    if ((base_inner->size() == face_ellipse_inner1->size()) &&
        (base_inner->size() == face_ellipse_inner2->size()) &&
        face_ellipse_outer1->empty()) {

        const auto size = base_inner->size();
        for (int i = 0; i <= size - 1; i++) {
            auto j = i == size - 1 ? 0 : i + 1;
            drawlist->AddQuadFilled(
                *(ImVec2 *)&*(*base_inner)[i], *(ImVec2 *)&*(*base_inner)[j],
                *(ImVec2 *)&*(*face_ellipse_inner2)[j],
                *(ImVec2 *)&*(*face_ellipse_inner2)[i], color);
        }

        if (outline) {
            util::path_points(face_ellipse_inner2);
            drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size,
                                  color_outline, 0,
                                  g_hbdraw.imgui.outline_tickness);
            drawlist->PathClear();
        }
    }
    // inner + outer
    else {
        // outer
        if (!face_ellipse_outer1->empty()) {
            auto size = face_ellipse_outer1->size();
            for (size_t i = 0; i < size - 1; i++) {
                drawlist->AddQuadFilled(
                    *(ImVec2 *)&*(*face_ellipse_outer1)[i],
                    *(ImVec2 *)&*(*face_ellipse_outer1)[i + 1],
                    *(ImVec2 *)&*(*face_ellipse_outer2)[i + 1],
                    *(ImVec2 *)&*(*face_ellipse_outer2)[i], color);
            }

            if (outline) {
                util::path_points(face_ellipse_outer1);
                drawlist->PathLineTo(*(ImVec2 *)&*face_ellipse_outer2->back());
                util::path_points(face_ellipse_outer2, true);
                drawlist->PathLineTo(*(ImVec2 *)&*(*face_ellipse_outer1)[0]);
                drawlist->AddPolyline(drawlist->_Path.Data,
                                      drawlist->_Path.Size, color_outline, 0,
                                      g_hbdraw.imgui.outline_tickness);
                drawlist->PathClear();
            }
        }

        // inner
        std::vector<Vector2f *> face_ellipse_inner2_trim;
        shape.remove_intersections(*center, *base_inner, *face_ellipse_inner2,
                                   face_ellipse_inner2_trim);

        if (face_ellipse_inner2_trim.empty()) {
            util::path_points(base_inner);
            drawlist->AddConvexPolyFilled(drawlist->_Path.Data,
                                          drawlist->_Path.Size, color);
            drawlist->PathClear();
            return;
        }

        const size_t idx1 = shape.get_intersection(*face_ellipse_inner2_trim[0],
                                                   *base_ellipse_inner, true);
        const size_t idx2 = shape.get_intersection(
            *face_ellipse_inner2_trim.back(), *base_ellipse_inner);

        util::path_points_duplicate(&face_ellipse_inner2_trim);
        drawlist->PathLineToMergeDuplicate(
            *(ImVec2 *)&*face_ellipse_inner2_trim.back());
        drawlist->PathLineTo(*(ImVec2 *)&*(*base_ellipse_inner)[0]);
        util::path_points_duplicate(face_ellipse_inner1, true);
        drawlist->PathLineTo(*(ImVec2 *)&*base_ellipse_inner->back());
        drawlist->AddConcavePolyFilled(drawlist->_Path.Data,
                                       drawlist->_Path.Size, color);
        drawlist->PathClear();

        if (outline) {
            drawlist->AddLine(*(ImVec2 *)&*(*base_ellipse_inner)[idx1],
                              *(ImVec2 *)&*face_ellipse_inner2_trim[0],
                              color_outline);
            drawlist->AddLine(*(ImVec2 *)&*(*base_ellipse_inner)[idx2],
                              *(ImVec2 *)&*face_ellipse_inner2_trim.back(),
                              color_outline);
            drawlist->PathClear();
        }

        drawlist->PathLineTo(*(ImVec2 *)&*face_ellipse_inner2_trim[0]);
        for (size_t i = idx1; i < base_ellipse_inner->size(); i++) {
            drawlist->PathLineTo(*(ImVec2 *)&*(*base_ellipse_inner)[i]);
        }
        drawlist->PathLineTo(*(ImVec2 *)&*face_ellipse_inner2_trim[0]);
        drawlist->AddConvexPolyFilled(drawlist->_Path.Data,
                                      drawlist->_Path.Size, color);
        drawlist->PathClear();

        drawlist->PathLineToMergeDuplicate(
            *(ImVec2 *)&*face_ellipse_inner2_trim.back());
        for (size_t i = 0; i <= idx2; i++) {
            drawlist->PathLineTo(*(ImVec2 *)&*(*base_ellipse_inner)[i]);
        }
        drawlist->PathLineToMergeDuplicate(
            *(ImVec2 *)&*face_ellipse_inner2_trim.back());
        drawlist->AddConvexPolyFilled(drawlist->_Path.Data,
                                      drawlist->_Path.Size, color);
        drawlist->PathClear();

        if (outline) {
            util::path_points(&face_ellipse_inner2_trim);
            drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size,
                                  color_outline, 0,
                                  g_hbdraw.imgui.outline_tickness);
            drawlist->PathClear();
        }
    }
}

void draw::draw(const Capsule &shape, ImU32 color, bool outline,
                ImU32 color_outline) {
    if (!shape.m_is_ok) {
        return;
    }

    const auto drawlist = ImGui::GetBackgroundDrawList();

    if (shape.m_is_sphere) {
        const auto cap = shape.m_bottom.radius > shape.m_top.radius
                             ? shape.m_bottom
                             : shape.m_top;
        drawlist->AddCircleFilled(*(ImVec2 *)&cap.center, cap.radius, color);
        if (outline) {
            drawlist->AddCircle(*(ImVec2 *)&cap.center, cap.radius,
                                color_outline);
        }
    } else {
        drawlist->PathArcTo(*(ImVec2 *)&shape.m_top.center, shape.m_top.radius,
                            shape.m_top.a_min, shape.m_top.a_max,
                            g_hbdraw.imgui.num_segments);
        drawlist->PathArcTo(*(ImVec2 *)&shape.m_bottom.center,
                            shape.m_bottom.radius, shape.m_bottom.a_min,
                            shape.m_bottom.a_max, g_hbdraw.imgui.num_segments);
        util::paint(color, outline, color_outline, 1, util::fill_type::convex);
    }
}

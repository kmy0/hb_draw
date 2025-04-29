#include "imgui.h"
#include "reframework/Math.hpp"

#include "draw.h"
#include "plugin.h"

#include <vector>

void draw::util::outline(ImU32 color, ImDrawFlags stroke_flags) {
    const auto drawlist = ImGui::GetBackgroundDrawList();
    drawlist->AddPolyline(drawlist->_Path.Data, drawlist->_Path.Size, color,
                          stroke_flags, g_hbdraw.imgui.outline_tickness);
    drawlist->PathClear();
}

void draw::util::fill(ImU32 color, fill_type fill_type) {
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
    drawlist->PathClear();
}

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
}

void draw::draw_cylinder(const Vector3f &start, const Vector3f &end,
                         float radius, ImU32 color, bool outline,
                         ImU32 color_outline) {
    if (glm::length(end - start) <= 0.0f) {
        return;
    }
    const auto cylinder = Cylinder(start, end, radius);
    if (!cylinder.m_is_ok) {
        return;
    }
    draw(cylinder, color, outline, color_outline);
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

    if (!shape.m_cap.empty()) {
        util::path_points(&shape.m_cap);
        util::paint(color, outline, color_outline);
    }

    if (!shape.m_body.empty()) {
        const auto drawlist = ImGui::GetBackgroundDrawList();
        const auto size = shape.m_body.size();
        for (size_t i = 0; i < size / 2; i++) {
            auto j = size - 1 - i;
            drawlist->AddQuadFilled(*(ImVec2 *)&*shape.m_body[i],
                                    *(ImVec2 *)&*shape.m_body[i + 1],
                                    *(ImVec2 *)&*shape.m_body[j - 1],
                                    *(ImVec2 *)&*shape.m_body[j], color);
        }

        if (outline) {
            util::path_points(&shape.m_body);
            util::outline(color_outline);
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

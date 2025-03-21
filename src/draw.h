#pragma once

#include "imgui.h"
#include "reframework/Math.hpp"

#include "shape/shapes.h"

#include <vector>

namespace draw {
void draw_sphere(const Vector3f &center, float radius, ImU32 color,
                 bool outline, ImU32 color_outline);
void draw_box(const Vector3f &pos, const Vector3f &extent,
              const Matrix4x4f &rot, ImU32 color, bool outline,
              ImU32 color_outline);
void draw_triangle(const Vector3f &pos, const Vector3f &extent,
                   const Matrix4x4f &rot, ImU32 color, bool outline,
                   ImU32 color_outline);
void draw_cylinder(const Vector3f &start, const Vector3f &end, float radius,
                   ImU32 color, bool outline, ImU32 color_outline);
void draw_ring(const Vector3f &start, const Vector3f &end, float radius_a,
               float radius_b, ImU32 color, bool outline, ImU32 color_outline);
void draw_capsule(const Vector3f &start, const Vector3f &end, float radius,
                  ImU32 color, bool outline, ImU32 color_outline);

void draw(const Box &shape, ImU32 color, bool outline, ImU32 color_outline);
void draw(const Sphere &shape, ImU32 color, bool outline, ImU32 color_outline);
void draw(const Triangle &shape, ImU32 color, bool outline,
          ImU32 color_outline);
void draw(const Cylinder &shape, ImU32 color, bool outline,
          ImU32 color_outline);
void draw(const Ring &shape, ImU32 color, bool outline, ImU32 color_outline);
void draw(const Capsule &shape, ImU32 color, bool outline, ImU32 color_outline);
} // namespace draw

namespace draw::util {
enum class fill_type { convex, concave };
void path_points(const std::vector<Vector2f *> *points, bool reverse = false);
void path_points_duplicate(const std::vector<Vector2f *> *points,
                           bool reverse = false);
void draw_ellipse(const ImVec2 &center, float radius_x, float radius_y,
                  float rot, float a_min, float a_max, ImU32 color,
                  int num_segments, float thickness, ImDrawFlags flags = 0);
void paint(ImU32 color, bool outline, ImU32 color_outline,
           ImDrawFlags stroke_flags = 1,
           fill_type fill_type = fill_type::convex);
} // namespace draw::util

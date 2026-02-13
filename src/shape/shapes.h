#include "reframework/Math.hpp"

#include <array>
#include <optional>
#include <vector>

struct EllipseAxis {
    Vector3f start;
    Vector3f end;
};

struct EllipseStruct {
    Vector2f center;
    float minor_radius;
    float major_radius;
    EllipseAxis major_axis;
    EllipseAxis minor_axis;
    float angle;
};

struct Shape {
    bool m_is_ok = false;
};

struct Sphere : Shape {
    Sphere(const Vector3f &center, float radius);
    float m_radius;
    Vector2f m_center;
};

struct Box : Shape {
    Box(const Vector3f &pos, const Vector3f &extent, const Matrix4x4f &rot);
    std::vector<std::array<Vector2f *, 4> *> m_quads;

  private:
    std::array<Vector2f, 8> m_points;
    std::array<std::array<Vector2f *, 4>, 6> m_quads_p;
};

struct Triangle : Shape {
    Triangle(const Vector3f &pos, const Vector3f &extent,
             const Matrix4x4f &rot);
    std::array<Vector2f, 3> *m_top_triangle = nullptr;
    std::array<Vector2f, 3> *m_bottom_triangle = nullptr;
    std::vector<std::array<Vector2f *, 4> *> m_quads;

  private:
    bool get_triangle(const Vector3f &pos, const Matrix4x4f &rot,
                      const std::array<Vector4f, 3> &triangle3f,
                      std::array<Vector2f, 3> &corners2f,
                      std::array<Vector2f, 3> *&culled);

    std::array<Vector2f, 3> m_top_points;
    std::array<Vector2f, 3> m_bottom_points;
    std::array<std::array<Vector2f *, 4>, 3> m_quads_p;
};

struct CylinderBase : Shape {
    CylinderBase(const Vector3f &start, const Vector3f &end, float radius,
                 bool force_projected = false);

    EllipseStruct m_top;
    EllipseStruct m_bottom;
    float m_angle;

  protected:
    bool get_cap(const Vector3f &center, const Vector3f &dir, float radius,
                 bool force_projected, EllipseStruct &out);
};

struct Cylinder : CylinderBase {
    Cylinder(const Vector3f &start, const Vector3f &end, float radius,
             unsigned num_segments = 64);

    bool m_is_sphere = false;
    std::vector<Vector2f> *m_cap;
    std::vector<Vector2f *> m_top_el;
    std::vector<Vector2f *> m_bottom_el;
    unsigned m_num_segments;

  protected:
    std::vector<Vector2f> m_el1;
    std::vector<Vector2f> m_el2;
};

struct Capsule : Shape {
    Capsule(const Vector3f &start, const Vector3f &end, float radius);

    struct Cap {
        Vector2f center;
        float radius;
        float a_min;
        float a_max;
    };

    Cap m_top;
    Cap m_bottom;
    float m_distance;
    std::array<Vector2f, 4> m_quad;
    bool m_is_sphere = false;
};

struct SlicedCylinder : CylinderBase {
    SlicedCylinder(const Vector3f &start, const Vector3f &end, float radius,
                   const Vector3f &direction, float degrees,
                   unsigned num_segments = 64);

    struct SlicedEllipseStruct : EllipseStruct {
        float a_min;
        float a_max;
    };

    std::vector<Vector2f> m_top_el;
    std::vector<Vector2f> m_bottom_el;
    std::optional<std::pair<Vector2f, Vector2f>> m_right_outline;
    std::optional<std::pair<Vector2f, Vector2f>> m_left_outline;
    unsigned m_num_segments;
    float m_slice_angle;

  protected:
    void get_eye(const Vector3f &center, const Vector3f &dir,
                 const Vector3f &eye_dir, float radius,
                 SlicedEllipseStruct &ellipse, Vector3f &out);
    bool get_side_outline(int side,
                          std::optional<std::pair<Vector2f, Vector2f>> &out);

    Vector3f m_eye1;
    Vector3f m_eye2;
};

struct Ring : Shape {
    Ring(const Vector3f &start, const Vector3f &end, float radius_a,
         float radius_b, unsigned num_segments);

    std::vector<Vector2f> m_inner_top_el;
    std::vector<Vector2f> m_inner_bottom_el;
    std::vector<Vector2f> m_outer_top_el;
    std::vector<Vector2f> m_outer_bottom_el;
    unsigned m_num_segments;
};

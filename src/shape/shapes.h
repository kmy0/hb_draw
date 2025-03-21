#include "reframework/Math.hpp"

#include <array>
#include <memory>
#include <optional>
#include <vector>

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

struct Cylinder : Shape {
    Cylinder(const Vector3f &start, const Vector3f &end, float radius,
             float rot = 0.0f, bool is_hollow = false);

    std::vector<Vector2f *> m_top_ellipse_base;
    std::vector<Vector2f *> m_bottom_ellipse_base;
    std::vector<Vector2f *> m_top_ellipse_face;
    std::vector<Vector2f *> m_bottom_ellipse_face;
    std::vector<Vector2f *> m_top_base;
    std::vector<Vector2f *> m_bottom_base;

  private:
    enum result { none = 0, hit = 1, miss = 2 };
    Vector2f *get_point(const Vector3f &pos,
                        std::vector<std::optional<Vector2f>> &cache,
                        size_t segment);
    result get_top_face(const Vector3f &start, const Vector3f &end,
                        std::array<Vector2f *, 4> &out, size_t segment);
    result get_top_base(const Vector3f &start, const Vector3f &end,
                        std::array<Vector2f *, 4> &out, size_t segment);
    result get_bottom_face(const Vector3f &start, const Vector3f &end,
                           std::array<Vector2f *, 4> &out, size_t segment);
    result get_bottom_base(const Vector3f &start, const Vector3f &end,
                           std::array<Vector2f *, 4> &out, size_t segment);

    float m_rot;
    Vector3f m_up;
    Vector3f m_right;
    float m_angle_increment;
    bool m_is_hollow;
    std::array<Vector2f *, 5> m_points;
    std::vector<std::optional<Vector2f>> m_top_points;
    std::vector<std::optional<Vector2f>> m_bottom_points;
};

struct Ring : Shape {
    Ring(const Vector3f &start, const Vector3f &end, float radius_a,
         float radius_b);
    size_t get_intersection(const Vector2f &point1,
                            const std::vector<Vector2f *> &target,
                            bool reverse = false) const;

    void remove_intersections(const Vector2f &center,
                              const std::vector<Vector2f *> &test,
                              const std::vector<Vector2f *> &target,
                              std::vector<Vector2f *> &out) const;

    std::unique_ptr<Cylinder> m_inner_cylinder;
    std::unique_ptr<Cylinder> m_outer_cylinder;
    Vector2f m_start2f;
    Vector2f m_end2f;
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

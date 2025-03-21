
#include "reframework/Math.hpp"

#include "scene.h"
#include "shapes.h"
#include "util.h"

Ring::Ring(const Vector3f &start, const Vector3f &end, float radius_a,
           float radius_b) {
    radius_b = radius_b - radius_a;

    auto center2f = scene::world_to_screen(start);
    if (!center2f) {
        return;
    }
    m_start2f = *center2f;

    center2f = scene::world_to_screen(end);
    if (!center2f) {
        return;
    }
    m_end2f = *center2f;

    m_outer_cylinder =
        std::make_unique<Cylinder>(Cylinder(start, end, radius_a));
    if (!m_outer_cylinder->m_is_ok) {
        return;
    }

    m_inner_cylinder = std::make_unique<Cylinder>(
        Cylinder(start, end, radius_b, glm::radians(180.0f), true));
    if (!m_inner_cylinder->m_is_ok) {
        return;
    }

    m_is_ok = true;
}

void Ring::remove_intersections(const Vector2f &center,
                                const std::vector<Vector2f *> &test,
                                const std::vector<Vector2f *> &target,
                                std::vector<Vector2f *> &out) const {
    const auto i_size = target.size();
    const auto b_size = test.size();

    auto is_intersecting = [&](Vector2f &point) {
        for (size_t j = 0; j <= b_size - 1; j++) {
            auto k = j == b_size - 1 ? 0 : j + 1;
            if (intersect(point, center, *test[k], *test[j])) {
                return true;
            }
        }
        return false;
    };
    for (size_t i = 0; i < i_size; i++) {
        if (!is_intersecting(*target[i])) {
            out.push_back(target[i]);
        }
    }
}

size_t Ring::get_intersection(const Vector2f &point1,
                              const std::vector<Vector2f *> &target,
                              bool reverse) const {
    size_t ret = 0;
    auto smallest_dist = 0.0f;
    const auto size = target.size();
    for (size_t i = 0; i < size; i++) {
        auto dist = glm::length(point1 - *target[i]);
        if (!smallest_dist || dist < smallest_dist) {
            ret = i;
            smallest_dist = dist;
        }
    }
    if (reverse) {
        ret = ret == 0 ? size - 1 : ret - 1;
    } else {
        ret = ret == size - 1 ? 0 : ret + 1;
    }
    return ret;
}

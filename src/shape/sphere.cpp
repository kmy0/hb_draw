#include "reframework/Math.hpp"

#include "shapes.h"
#include "util.h"

Sphere::Sphere(const Vector3f &center, float radius) {
    auto opt = get_screen_radius(center, radius);
    if (opt) {
        m_radius = opt->first;
        m_center = opt->second;
        m_is_ok = true;
    }
}
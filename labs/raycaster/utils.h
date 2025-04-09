#ifndef UTILS_H
#define UTILS_H

// #include "ray.h"

#include <QPointF>
#include <optional>
#include <vector>

using Intersection = std::optional<std::pair<QPointF, double>>;

constexpr auto kEpsilon = 1e-9;

inline double Distance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

void FindNearestPoint(std::vector<QPointF>* points, const QPointF& pos, NearestPoint& nearest_point) {
    for (auto it = points->begin(); it != points->end(); ++it) {
        if (const auto distance = Distance(pos, *it); distance < kCaptureRadius && (!nearest_point || nearest_point->second > distance)) {
            nearest_point = std::make_pair(it, distance);
        }
    }
}

Intersection IntersectRay(
    const QPointF& vertex1, const QPointF& vertex2, const Ray& ray) {
    if (Ray::AreParallel(ray, {vertex2, vertex1})) {
        return std::nullopt;
    }
    const auto [r_px, r_py] = ray.GetBegin();
    const auto r_dx = std::cos(ray.GetAngle());
    const auto r_dy = std::sin(ray.GetAngle());
    const auto [s_px, s_py] = vertex1;
    const auto [s_dx, s_dy] = vertex2 - vertex1;
    const auto t2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx * r_dy - s_dy * r_dx);
    auto t1 = (s_px + s_dx * t2 - r_px) / r_dx;
    if (std::abs(r_dx) < kEpsilon) [[unlikely]] {
        t1 = (s_py + s_dy * t2 - r_py) / r_dy;
    }
    if (t1 < -kEpsilon || t2 < -kEpsilon || t2 > 1 + kEpsilon) {
        return std::nullopt;
    }
    return std::make_pair(ray.GetBegin() + QPointF(r_dx, r_dy) * t1, t1);
}

double NormalizeAngle(double angle) {
    while (angle < -kEpsilon) {
        angle += 2 * std::numbers::pi;
    }
    angle = std::fmod(angle, 2 * std::numbers::pi);
    return angle;
}

#endif
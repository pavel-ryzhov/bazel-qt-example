#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"

#include <QPointF>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <optional>
#include <utility>
#include <vector>

using Intersection = std::optional<std::pair<QPointF, double>>;

class Polygon {
   public:
    explicit Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
    }

    [[nodiscard]] const std::vector<QPointF>& GetVertecis() const {
        return vertices_;
    }

    [[nodiscard]] const QPointF& operator[](size_t index) const {
        return vertices_[index];
    }

    QPointF& operator[](size_t index) {
        return vertices_[index];
    }

    void AddVertex(const QPointF& vertex) {
        vertices_.push_back(vertex);
    }

    void UpdateLastVertex(const QPointF& new_vertex) {
        if (!vertices_.empty()) [[likely]] {
            vertices_.back() = new_vertex;
        }
    }

    [[nodiscard]] Intersection IntersectRay(const Ray& ray) const {
        Intersection result;
        for (size_t i = 1; i < vertices_.size(); ++i) {
            IntersectRay(vertices_[i - 1], vertices_[i], ray, result);
        }
        IntersectRay(vertices_.back(), vertices_.front(), ray, result);
        return result;
    }

   private:
    std::vector<QPointF> vertices_;

    static void IntersectRay(
        const QPointF& vertex1, const QPointF& vertex2, const Ray& ray, Intersection& result) {
        if (const auto intersection = IntersectRay(vertex1, vertex2, ray);
            intersection && (!result || intersection->second < result->second)) {
            result = intersection;
        }
    }

    static Intersection IntersectRay(
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
};

#endif
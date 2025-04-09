#ifndef POLYGON_H
#define POLYGON_H

#include "ray.h"
#include "utils.h"

#include <QPointF>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <optional>
#include <utility>
#include <vector>

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
};

#endif
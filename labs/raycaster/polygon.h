#pragma once

#include "ray.h"
#include "utils.h"

#include <QPointF>
#include <vector>

class Polygon {
   public:
    explicit Polygon(const std::vector<QPointF>& vertices);

    [[nodiscard]] const std::vector<QPointF>& GetVertices() const;

    [[nodiscard]] const QPointF& operator[](size_t index) const;

    QPointF& operator[](size_t index);

    void AddVertex(const QPointF& vertex);

    void UpdateLastVertex(const QPointF& new_vertex);

    [[nodiscard]] Intersection IntersectRay(const Ray& ray) const;

   private:
    std::vector<QPointF> vertices_;

    static Intersection IntersectRay(
        const QPointF& vertex1, const QPointF& vertex2, const Ray& ray);

    static void IntersectRay(
        const QPointF& vertex1, const QPointF& vertex2, const Ray& ray, Intersection& result);

    friend class Controller;
};

#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

constexpr auto kEpsilon = 1e-9;

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle)
        : begin_(begin), end_(end), angle_(angle) {
    }

    Ray(const QPointF& begin, const QPointF& end)
        : begin_(begin), end_(end), angle_(atan2(end.x() - begin.x(), end.y() - begin.y())) {
    }

    Ray(const QPointF& begin, double angle, double length)
        : begin_(begin)
        , end_(QPointF{begin.x() + cos(angle) * length, begin.y() + sin(angle) * length})
        , angle_(angle) {
    }

    Ray(const QPointF& begin, double angle) : Ray{begin, angle, 1} {
    }

    [[nodiscard]] const QPointF& GetBegin() const {
        return begin_;
    }

    [[nodiscard]] const QPointF& GetEnd() const {
        return end_;
    }

    [[nodiscard]] double GetAngle() const {
        return angle_;
    }

    void SetBegin(const QPointF& begin) {
        begin_ = begin;
    }

    void SetEnd(const QPointF& end) {
        end_ = end;
    }

    void SetAngle(double angle) {
        angle_ = angle;
    }

    Ray Rotate(double angle) {
        return {begin_, angle_ + angle, Length()};
    }

    [[nodiscard]] double Length() const {
        return hypot(begin_.x() - end_.x(), begin_.y() - end_.y());
    }

    static bool AreParallel(const Ray& ray1, const Ray& ray2) {
        return (ray1.GetEnd().x() - ray1.GetBegin().x()) /
                       (ray2.GetEnd().x() - ray2.GetBegin().x()) -
                   (ray1.GetEnd().y() - ray1.GetBegin().y()) /
                       (ray2.GetEnd().y() - ray2.GetBegin().y()) <
               kEpsilon;
    }

   private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

#endif
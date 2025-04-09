#include "ray.h"
#include "utils.h"

#include <QPointF>
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(NormalizeAngle(angle)) {
}

Ray::Ray(const QPointF& begin, const QPointF& end)
    : begin_(begin)
    , end_(end)
    , angle_(NormalizeAngle(std::atan2(end.y() - begin.y(), end.x() - begin.x()))) {
}

Ray::Ray(const QPointF& begin, double angle, double length)
    : begin_(begin)
    , end_(QPointF{begin.x() + std::cos(angle) * length, begin.y() + std::sin(angle) * length})
    , angle_(NormalizeAngle(angle)) {
}

Ray::Ray(const QPointF& begin, double angle) : Ray{begin, angle, 1} {
}

const QPointF& Ray::GetBegin() const {
    return begin_;
}

const QPointF& Ray::GetEnd() const {
    return end_;
}

double Ray::GetAngle() const {
    return angle_;
}

void Ray::SetBegin(const QPointF& begin) {
    begin_ = begin;
}

void Ray::SetEnd(const QPointF& end) {
    end_ = end;
}

void Ray::SetAngle(double angle) {
    angle_ = NormalizeAngle(angle);
}

Ray Ray::Rotate(double angle) const {
    return {begin_, angle_ + angle, Length()};
}

double Ray::Length() const {
    return Distance(begin_, end_);
}

bool Ray::AreParallel(const Ray& ray1, const Ray& ray2) {
    const auto v1 = ray1.end_ - ray1.begin_;
    const auto v2 = ray2.end_ - ray2.begin_;
    return std::abs(v1.x() * v2.y() - v1.y() * v2.x()) < kEpsilon;
}

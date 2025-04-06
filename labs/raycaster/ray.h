#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>
#include <cstdlib>
#include <numbers>

constexpr auto kEpsilon = 1e-9;

inline double Distance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle)
        : begin_(begin), end_(end), angle_(NormalizeAngle(angle)) {
    }

    // Ray(const QPointF& begin, const QPointF& end)
    //     : begin_(begin)
    //     , end_(end)
    //     // , angle_(std::atan((end.y() - begin.y()) / (end.x() - begin.x())))
    //     {
    //         const auto dx = end.x() - begin.x();
    //         const auto dy = end.y() - begin.y();
    //         bool change_sign = false;
    //         if (std::abs(dx) > kEpsilon) [[likely]] {
    //             angle_ = NormalizeAngle(std::atan(dy / dx));
    //             if (dx < -kEpsilon) {
    //                 change_sign = true;
    //             }
    //         } else {
    //             angle_ = .5 * std::numbers::pi;
    //             if (dy < -kEpsilon) {
    //                 change_sign = true;
    //             }
    //         }
    //         if (change_sign) {
    //             angle_ = NormalizeAngle(angle_ + std::numbers::pi);
    //         }

    // }

    Ray(const QPointF& begin, const QPointF& end)
        : begin_(begin)
        , end_(end)
        , angle_(std::atan((end.y() - begin.y()) / (end.x() - begin.x()))) {
        if (end.x() - begin.x() < -kEpsilon) {
            angle_ = NormalizeAngle(angle_ + std::numbers::pi);
        }
    }

    Ray(const QPointF& begin, double angle, double length)
        : begin_(begin)
        , end_(QPointF{begin.x() + std::cos(angle) * length, begin.y() + std::sin(angle) * length})
        , angle_(NormalizeAngle(angle)) {
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
        angle_ = NormalizeAngle(angle);
    }

    [[nodiscard]] Ray Rotate(double angle) const {
        return {begin_, angle_ + angle, Length()};
    }

    [[nodiscard]] double Length() const {
        return Distance(begin_, end_);
    }

    static bool AreParallel(const Ray& ray1, const Ray& ray2) {
        const auto v1 = ray1.end_ - ray1.begin_;
        const auto v2 = ray2.end_ - ray2.begin_;
        return std::abs(v1.x() * v2.y() - v1.y() * v2.x()) < kEpsilon;
    }

    // bool operator==(const Ray& other) const {
    //     return std::abs(angle_ - other.angle_) < kEpsilon;
    // }

    // std::partial_ordering operator<=>(const Ray& other) const {
    //     return angle_ <=> other.angle_;
    // }

   private:
    QPointF begin_;
    QPointF end_;
    double angle_;

    static double NormalizeAngle(double angle) {
        // while (angle < -kEpsilon) {
        //     angle += 2 * std::numbers::pi;
        // }
        // angle = std::fmod(angle, 2 * std::numbers::pi);
        return angle;
    }
};

#endif
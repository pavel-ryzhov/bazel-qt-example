#pragma once

#include <QPointF>

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    Ray(const QPointF& begin, const QPointF& end);

    Ray(const QPointF& begin, double angle, double length);

    Ray(const QPointF& begin, double angle);

    [[nodiscard]] const QPointF& GetBegin() const;

    [[nodiscard]] const QPointF& GetEnd() const;

    [[nodiscard]] double GetAngle() const;

    void SetBegin(const QPointF& begin);

    void SetEnd(const QPointF& end);

    void SetAngle(double angle);

    [[nodiscard]] Ray Rotate(double angle) const;

    [[nodiscard]] double Length() const;

    static bool AreParallel(const Ray& ray1, const Ray& ray2);

   private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

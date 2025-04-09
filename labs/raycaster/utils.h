#pragma once

#include <QColor>
#include <QPointF>
#include <optional>
#include <vector>

using Intersection = std::optional<std::pair<QPointF, double>>;
using NearestPoint = std::optional<std::pair<std::vector<QPointF>::iterator, double>>;
using CapturedPoint = std::optional<std::vector<QPointF>::iterator>;

constexpr auto kEpsilon = 1e-9;
constexpr auto kRotateAngle = 1e-4;
constexpr auto kNearDiffrence = 1e-3;
constexpr auto kAdditionalLightSourcesCount = 8;
constexpr auto kAdditionalLightSourceRadius = 0.02;
constexpr auto kBorderOffset = 0.05;
constexpr auto kCaptureRadius = 0.02;
constexpr auto kShadowAlpha = 0.125;
constexpr auto kPolygonColor = Qt::green;

inline double Distance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

inline void FindNearestPoint(
    std::vector<QPointF>* points, const QPointF& pos, NearestPoint& nearest_point) {
    for (auto it = points->begin(); it != points->end(); ++it) {
        if (const auto distance = Distance(pos, *it);
            distance < kCaptureRadius && (!nearest_point || nearest_point->second > distance)) {
            nearest_point = std::make_pair(it, distance);
        }
    }
}

inline double NormalizeAngle(double angle) {
    while (angle < -kEpsilon) {
        angle += 2 * std::numbers::pi;
    }
    angle = std::fmod(angle, 2 * std::numbers::pi);
    return angle;
}
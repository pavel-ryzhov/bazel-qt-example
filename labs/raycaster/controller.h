#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include "ray.h"

#include <QObject>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

constexpr auto kRotateAngle = 1e-4;
constexpr auto kNearDiffrence = 1e-3;
constexpr auto kAdditionalLightSourcesCount = 6;
constexpr auto kAdditionalLightSourceRadius = 0.02;
constexpr auto kBorderOffset = 0.05;
constexpr auto kCaptureRadius = 0.05;

using NearestPoint = std::optional<std::pair<std::vector<QPointF>::iterator, double>>;

class Controller : public QObject {
    Q_OBJECT

   public:
    enum Mode : uint8_t { Light, Polygons, StaticLights };

    Controller() {
        Init();
    }

    void Init() {
        polygons_.push_back(Polygon{
          {QPointF{-kBorderOffset, -kBorderOffset}, QPointF{1. + kBorderOffset, -kBorderOffset},
           QPointF{1. + kBorderOffset, 1. + kBorderOffset},
           QPointF{-kBorderOffset, 1. + kBorderOffset}}});
    }

    void AddVertex(const QPointF& vertex) {
        if (drawing_polygon_) {
            AddVertexToLastPolygon(vertex);
        } else {
            drawing_polygon_ = true;
            const Polygon p({vertex});
            AddPolygon(p);
        }
    }

    void FinishPolygon() {
        drawing_polygon_ = false;
    }

    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const {
        return polygons_;
    }

    void AddPolygon(const Polygon& polygon) {
        polygons_.push_back(polygon);
        emit RepaintStatic();
    }

    void AddVertexToLastPolygon(const QPointF& new_vertex) {
        if (!polygons_.empty()) [[likely]] {
            polygons_.back().AddVertex(new_vertex);
        }
        emit RepaintStatic();
    }

    void UpdateLastPolygon(const QPointF& new_vertex) {
        if (!polygons_.empty() && !polygons_.back().GetVertecis().empty()) {
            polygons_.back().UpdateLastVertex(new_vertex);
        }
        emit RepaintStatic();
    }

    [[nodiscard]] const QPointF& GetLightSource() const {
        return light_source_;
    }

    void SetLightSource(const QPointF& point) {
        light_source_ = point;
        emit Repaint();
    }

    [[nodiscard]] bool HasLightSource() const {
        return light_source_.toPoint() != QPoint{-1, -1};
    }

    void SetMode(Mode mode) {
        mode_ = mode;
        emit Repaint();
    }

    [[nodiscard]] Mode GetMode() const {
        return mode_;
    }

    [[nodiscard]] std::vector<Ray> CastRays(const QPointF& light_source) const {
        std::vector<Ray> result;
        for (const auto& polygon : polygons_) {
            for (const auto& vertex : polygon.GetVertecis()) {
                const Ray ray{light_source, vertex};
                result.push_back(ray);
                result.push_back(ray.Rotate(-kRotateAngle));
                result.push_back(ray.Rotate(kRotateAngle));
            }
        }
        SortRaysByAngle(&result);
        return result;
    }

    [[nodiscard]] std::vector<Ray> CastRays() const {
        return CastRays(light_source_);
    }

    void IntersectRays(std::vector<Ray>* rays) const {
        for (auto& ray : *rays) {
            Intersection result;
            for (const auto& polygon : polygons_) {
                if (const auto intersection = polygon.IntersectRay(ray);
                    intersection && (!result || intersection->second < result->second)) {
                    result = intersection;
                }
            }
            ray.SetEnd(result->first);
        }
    }

    static void RemoveAdjacentRays(std::vector<Ray>* rays) {
        if (rays->size() <= 1) [[unlikely]] {
            return;
        }
        std::vector<Ray> unique_rays{rays->front()};
        Ray current_unique_ray{rays->front()};
        for (auto it = rays->begin() + 1; it != rays->end(); ++it) {
            if (Distance(it->GetEnd(), current_unique_ray.GetEnd()) > kNearDiffrence) {
                current_unique_ray = *it;
                unique_rays.push_back(*it);
            }
        }
        if (Distance(unique_rays.front().GetEnd(), unique_rays.back().GetEnd()) < kNearDiffrence) {
            unique_rays.pop_back();
        }
        *rays = std::move(unique_rays);
    }

    [[nodiscard]] Polygon CreateLightArea() const {
        auto rays = CastRays();
        IntersectRays(&rays);
        RemoveAdjacentRays(&rays);
        return CreateLightArea(rays);
    }

    [[nodiscard]] std::vector<Polygon> CreateAdditionalLightAreas() const {
        constexpr auto kAngleStep = 2 * std::numbers::pi / kAdditionalLightSourcesCount;
        std::vector<Polygon> result;
        result.reserve(kAdditionalLightSourcesCount);
        for (size_t i = 0; i < kAdditionalLightSourcesCount; ++i) {
            const auto angle = kAngleStep * static_cast<double>(i);
            auto rays = CastRays(
                light_source_ +
                kAdditionalLightSourceRadius * QPointF{std::cos(angle), std::sin(angle)});
            IntersectRays(&rays);
            RemoveAdjacentRays(&rays);
            result.push_back(CreateLightArea(rays));
        }
        return result;
    }

    void AddStaticLightSource(const QPointF& point) {
        static_lights_.push_back(point);
        emit RepaintStatic();
    }

    // void UpdateStaticLightSource(const QPointF& point) {

    // }

    void Refresh() {
        polygons_.clear();
        static_lights_.clear();
        light_source_ = {-1, -1};
        mode_ = Mode::Polygons;
        drawing_polygon_ = false;
        Init();
        emit RepaintStatic();
    }

   signals:
    void Repaint();
    void RepaintStatic();

   private:
    std::vector<Polygon> polygons_;
    QPointF light_source_{-1, -1};
    Mode mode_ = Mode::Polygons;
    std::vector<QPointF> static_lights_;
    bool drawing_polygon_ = false;

    std::optional<std::vector<QPointF>::iterator> CaptureVertex(const QPointF& pos) const {
        NearestPoint result;
        for (auto& polygon : polygons_) {
            FindNearestPoint(, const QPointF &pos, NearestPoint &nearest_point)
        }
    }

    static void SortRaysByAngle(std::vector<Ray>* rays) {
        std::ranges::sort(
            *rays, [](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });
    }

    [[nodiscard]] static Polygon CreateLightArea(const std::vector<Ray>& rays) {
        std::vector<QPointF> result;
        result.reserve(rays.size());
        std::ranges::copy(
            (rays | std::ranges::views::transform(&Ray::GetEnd)), std::back_inserter(result));
        return Polygon(result);
    }
};

#endif
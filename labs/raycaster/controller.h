#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ray.h"
#include "polygon.h"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <QObject>
#include <algorithm>
#include <iterator>
#include <ranges>
#include <utility>
#include <vector>

constexpr auto kRotateAngle = 1e-4;
constexpr auto kNearDiffrence = 1e-3;
constexpr auto kAdditionalLightSourcesCount = 8;
constexpr auto kAdditionalLightSourceRadius = 10;

class Controller : public QObject {
    Q_OBJECT

   public:
    enum Mode : uint8_t { Light, Polygons, StaticLights };

    Controller(int width, int height) {
        // Resize(width, height, width, height);
        Init(width, height);
    }

    void Init(int width, int height) {
        polygons_.push_back(Polygon{{QPoint{-10, -10}, QPoint{width + 10, -10}, QPoint{width + 10, height + 10}, QPoint{-10, height + 10}}});
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

    void Resize(int width, int height, int old_width, int old_height) {
        const auto x_ratio = static_cast<double>(width) / old_width;
        const auto y_ratio = static_cast<double>(height) / old_height;
        const Polygon p{{QPoint{-10, -10}, QPoint{width + 10, -10}, QPoint{width + 10, height + 10}, QPoint{-10, height + 10}}};
        if (polygons_.empty()) {
            polygons_.push_back(p);
        } else {
            polygons_.front() = p;
        }
        if (HasLightSource()) {
            light_source_ = {light_source_.rx() * x_ratio, light_source_.ry() * y_ratio};
        }
        for (size_t i = 1; i < polygons_.size(); ++i) {
            polygons_[i].Resize(x_ratio, y_ratio);
        }
        emit RepaintStatic();
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
                if (const auto intersection = polygon.IntersectRay(ray); intersection && (!result || intersection->second < result->second)) {
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
        // RemoveAdjacentRays(&rays);
        return CreateLightArea(rays);
    }

    [[nodiscard]] std::vector<Polygon> CreateAdditionalLightAreas() const {
        constexpr auto kAngleStep = 2 * std::numbers::pi / kAdditionalLightSourcesCount;
        std::vector<Polygon> result;
        result.reserve(kAdditionalLightSourcesCount);
        for (size_t i = 0; i < kAdditionalLightSourcesCount; ++i) {
            const auto angle = kAngleStep * static_cast<double>(i);
            auto rays = CastRays(light_source_ + kAdditionalLightSourceRadius * QPointF{std::cos(angle), std::sin(angle)});
            IntersectRays(&rays);
            // RemoveAdjacentRays(&rays);
            result.push_back(CreateLightArea(rays));
        }
        return result;
    }

    void Refresh(int width, int height) {
        polygons_.clear();
        static_lights_.clear();
        light_source_ = {-1, -1};
        mode_ = Mode::Polygons;
        drawing_polygon_ = false;
        // Resize(width, height, width, height);
        Init(width, height);
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

    static void SortRaysByAngle(std::vector<Ray>* rays) {
        std::ranges::sort(*rays, [](const Ray& a, const Ray& b){
            return a.GetAngle() < b.GetAngle();
        });
    }

    [[nodiscard]] static Polygon CreateLightArea(const std::vector<Ray>& rays) {
        std::vector<QPointF> result;
        result.reserve(rays.size());
        std::ranges::copy((rays | std::ranges::views::transform(&Ray::GetEnd)), std::back_inserter(result));
        return Polygon(result);
    }
};

#endif
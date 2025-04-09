#include "controller.h"

#include "polygon.h"
#include "ray.h"
#include "utils.h"

#include <QObject>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

Controller::Controller() {
    Init();
}

void Controller::Init() {
    polygons_.push_back(Polygon{
      {QPointF{-kBorderOffset, -kBorderOffset}, QPointF{1. + kBorderOffset, -kBorderOffset},
       QPointF{1. + kBorderOffset, 1. + kBorderOffset},
       QPointF{-kBorderOffset, 1. + kBorderOffset}}});
}

void Controller::AddVertex(const QPointF& vertex) {
    if (drawing_polygon_) {
        AddVertexToLastPolygon(vertex);
    } else {
        drawing_polygon_ = true;
        const Polygon p({vertex});
        AddPolygon(p);
    }
}

void Controller::FinishPolygon() {
    drawing_polygon_ = false;
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
    emit RepaintStatic();
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) [[likely]] {
        polygons_.back().AddVertex(new_vertex);
    }
    emit RepaintStatic();
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty() && !polygons_.back().GetVertices().empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
    emit RepaintStatic();
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& point) {
    light_source_ = point;
    emit Repaint();
}

bool Controller::HasLightSource() const {
    return light_source_.toPoint() != QPoint{-1, -1};
}

void Controller::SetMode(Mode mode) {
    mode_ = mode;
    emit Repaint();
}

Controller::Mode Controller::GetMode() const {
    return mode_;
}

std::vector<Ray> Controller::CastRays(const QPointF& light_source) const {
    std::vector<Ray> result;
    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            const Ray ray{light_source, vertex};
            result.push_back(ray);
            result.push_back(ray.Rotate(-kRotateAngle));
            result.push_back(ray.Rotate(kRotateAngle));
        }
    }
    SortRaysByAngle(&result);
    return result;
}

std::vector<Ray> Controller::CastRays() const {
    return CastRays(light_source_);
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
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

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
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

Polygon Controller::CreateLightArea() const {
    auto rays = CastRays();
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);
    return CreateLightArea(rays);
}

std::vector<Polygon> Controller::CreateAdditionalLightAreas() const {
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

void Controller::AddStaticLightSource(const QPointF& point) {
    static_lights_.push_back(point);
    emit RepaintStatic();
}

// void UpdateStaticLightSource(const QPointF& point) {

// }

void Controller::Refresh() {
    polygons_.clear();
    static_lights_.clear();
    light_source_ = {-1, -1};
    mode_ = Mode::Polygons;
    drawing_polygon_ = false;
    captured_vertex_ = std::nullopt;
    Init();
    emit RepaintStatic();
}

std::optional<std::vector<QPointF>::iterator> Controller::CaptureVertex(const QPointF& pos) {
    NearestPoint result;
    for (auto& polygon : polygons_) {
        FindNearestPoint(&polygon.vertices_, pos, result);
    }
    return result ? std::make_optional(result->first) : std::nullopt;
}

std::optional<std::vector<QPointF>::iterator> Controller::CaptureStaticLightSource(const QPointF& pos) {
    NearestPoint result;
    FindNearestPoint(&static_lights_, pos, result);
    return result ? std::make_optional(result->first) : std::nullopt;
}

void Controller::SortRaysByAngle(std::vector<Ray>* rays) {
    std::ranges::sort(
        *rays, [](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });
}

Polygon Controller::CreateLightArea(const std::vector<Ray>& rays) {
    std::vector<QPointF> result;
    result.reserve(rays.size());
    std::ranges::copy(
        (rays | std::ranges::views::transform(&Ray::GetEnd)), std::back_inserter(result));
    return Polygon(result);
}

CapturedPoint Controller::GetCapturedVertex() const {
    return captured_vertex_;
}

void Controller::StartVertexDrag(const QPointF& pos) {
    captured_vertex_ = CaptureVertex(pos);
}

void Controller::FinishVertexDrag() {
    captured_vertex_ = std::nullopt;
}


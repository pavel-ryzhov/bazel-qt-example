#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include "ray.h"

#include <QObject>
#include <vector>

class Controller : public QObject {
    Q_OBJECT

   public:
    enum Mode : uint8_t { Light, Polygons, StaticLights };

    Controller();

    void Init();

    void AddVertex(const QPointF& vertex);

    void FinishPolygon();

    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;

    void AddPolygon(const Polygon& polygon);

    void AddVertexToLastPolygon(const QPointF& new_vertex);

    void UpdateLastPolygon(const QPointF& new_vertex);

    [[nodiscard]] const QPointF& GetLightSource() const;

    void SetLightSource(const QPointF& point);

    [[nodiscard]] bool HasLightSource() const;

    void SetMode(Mode mode);

    [[nodiscard]] Mode GetMode() const;

    [[nodiscard]] std::vector<Ray> CastRays(const QPointF& light_source) const;

    [[nodiscard]] std::vector<Ray> CastRays() const;

    void IntersectRays(std::vector<Ray>* rays) const;

    static void RemoveAdjacentRays(std::vector<Ray>* rays);

    [[nodiscard]] Polygon CreateLightArea() const;

    [[nodiscard]] std::vector<Polygon> CreateAdditionalLightAreas() const;

    [[nodiscard]] std::vector<Polygon> CreateStaticLightArea() const;

    [[nodiscard]] std::vector<std::vector<Polygon>> CreateAdditionalStaticLightAreas() const;

    void AddStaticLightSource(const QPointF& point);

    void RemoveStaticLightSource(const QPointF& point);

    [[nodiscard]] const std::vector<QPointF>& GetStaticLights() const;

    void Refresh();

    void StartVertexDrag(const QPointF& pos);

    void FinishVertexDrag();

    [[nodiscard]] CapturedPoint GetCapturedVertex() const;

   signals:
    void Repaint();
    void RepaintStatic();

   private:
    std::vector<Polygon> polygons_;
    QPointF light_source_{-1, -1};
    Mode mode_ = Mode::Polygons;
    std::vector<QPointF> static_lights_;
    bool drawing_polygon_ = false;
    CapturedPoint captured_vertex_;

    [[nodiscard]] std::vector<Polygon> CreateAdditionalLightAreas(
        const QPointF& light_source) const;

    [[nodiscard]] Polygon CreateLightArea(const QPointF& light_source) const;

    [[nodiscard]] CapturedPoint CaptureVertex(const QPointF& pos);

    [[nodiscard]] CapturedPoint CaptureStaticLightSource(const QPointF& pos);

    [[nodiscard]] static Polygon CreateLightArea(const std::vector<Ray>& rays);
};

#endif
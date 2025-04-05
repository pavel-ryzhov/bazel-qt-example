#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

#include <QObject>
#include <vector>

class Controller : public QObject {
    Q_OBJECT

   public:
    enum Mode : uint8_t { Light, Polygons };

    Controller(int width, int height) {
        Resize(width, height);
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

    void Resize(int width, int height) {
        const Polygon p{{QPoint{0, 0}, QPoint{width - 1, 0}, QPoint{width - 1, height - 1}, QPoint{0, height - 1}}};
        if (polygons_.empty()) {
            polygons_.push_back(p);
        } else {
            polygons_.front() = p;
        }
        emit Repaint();
    }

    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const {
        return polygons_;
    }

    void AddPolygon(const Polygon& polygon) {
        polygons_.push_back(polygon);
        emit Repaint();
    }

    void AddVertexToLastPolygon(const QPointF& new_vertex) {
        if (!polygons_.empty()) [[likely]] {
            polygons_.back().AddVertex(new_vertex);
        }
        emit Repaint();
    }

    void UpdateLastPolygon(const QPointF& new_vertex) {
        if (!polygons_.empty() && !polygons_.back().GetVertecis().empty()) {
            polygons_.back().UpdateLastVertex(new_vertex);
        }
        emit Repaint();
    }

    [[nodiscard]] const QPointF& GetLightSource() const {
        return light_source_;
    }

    void SetLightSource(const QPointF& point) {
        light_source_ = point;
        emit Repaint();
    }

    void SetMode(Mode mode) {
        mode_ = mode;
    }

    [[nodiscard]] Mode GetMode() const {
        return mode_;
    }

   signals:
    void Repaint();

   private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    Mode mode_ = Mode::Polygons;
    bool drawing_polygon_ = false;
};

#endif
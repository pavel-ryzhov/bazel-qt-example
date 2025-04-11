#include "main_widget.h"

#include "controller.h"
#include "polygon.h"
#include "utils.h"

#include <vector>

// NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)

MainWidget::MainWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    connect(&controller_, &Controller::RepaintStatic, this, &MainWidget::RepaintStatic);
    connect(&controller_, &Controller::Repaint, this, &MainWidget::Repaint);
    connect(&timer_, &QTimer::timeout, this, &MainWidget::RepaintByTimer);
    if constexpr (kTimerRepaint) {
        timer_.setInterval(kTimerDelay);
        timer_.start();
    }
}

void MainWidget::SetMode(Controller::Mode mode) {
    controller_.SetMode(mode);
}

void MainWidget::Resize() {
    RepaintStatic();
}

static void DrawPoints(QPainter* painter, const std::vector<QPointF>& points, const QColor& color) {
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    for (const auto& point : points) {
        painter->drawEllipse(point, .25 * kCaptureRadius, .25 * kCaptureRadius);
    }
    painter->restore();
}

void MainWidget::paintEvent(QPaintEvent* /*event*/) {
    QImage image{width(), height(), QImage::Format_ARGB32_Premultiplied};
    image.fill(Qt::transparent);
    QPainter painter{&image};
    QPainter result_painter{this};
    painter.scale(width(), height());
    Paint(&painter);
    result_painter.drawPixmap(0, 0, static_background_);
    result_painter.drawImage(0, 0, image);
}

void MainWidget::mouseMoveEvent(QMouseEvent* event) {
    const auto pos = ToRelative(event->pos());
    switch (controller_.GetMode()) {
        case Controller::Mode::Light: {
            controller_.SetLightSource(pos);
            break;
        }
        case Controller::Mode::Polygons: {
            auto vertex = controller_.GetCapturedVertex();
            if (vertex) {
                **vertex = pos;
                RepaintStatic();
            }
            break;
        }
        default: {
        }
    }
}

void MainWidget::mousePressEvent(QMouseEvent* event) {
    const auto pos = ToRelative(event->pos());
    switch (controller_.GetMode()) {
        case Controller::Mode::Polygons: {
            switch (event->button()) {
                case Qt::LeftButton: {
                    if (ctrl_pressed_) {
                        controller_.StartVertexDrag(pos);
                    } else {
                        controller_.AddVertex(pos);
                    }
                    break;
                }
                case Qt::RightButton: {
                    controller_.FinishPolygon();
                    break;
                }
                default: {
                }
            }
            break;
        }
        case Controller::Mode::StaticLights: {
            if (event->button() == Qt::LeftButton) {
                if (ctrl_pressed_) {
                    controller_.RemoveStaticLightSource(pos);
                } else {
                    controller_.AddStaticLightSource(pos);
                }
            }
            break;
        }
        default: {
        }
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (controller_.GetMode() == Controller::Mode::Polygons) {
        switch (event->button()) {
            case Qt::LeftButton: {
                if (ctrl_pressed_) {
                    controller_.FinishVertexDrag();
                }
                break;
            }
            default: {
            }
        }
    }
}

void MainWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        ctrl_pressed_ = true;
    }
}

void MainWidget::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        ctrl_pressed_ = false;
        controller_.FinishVertexDrag();
    }
}

void MainWidget::Refresh() {
    controller_.Refresh();
}

void MainWidget::Repaint() {
    if constexpr (!kTimerRepaint) {
        repaint();
    }
}

void MainWidget::RepaintByTimer() {
    repaint();
}

void MainWidget::RepaintStatic() {
    static_background_ = QPixmap{width(), height()};
    static_background_.fill(Qt::black);
    QPainter painter{&static_background_};
    painter.scale(width(), height());
    PaintStatic(&painter);
    Repaint();
}

void MainWidget::PaintStatic(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    QPen pen{Qt::green, 1};
    pen.setWidthF(.002);
    painter->setPen(pen);
    for (const auto& polygon : controller_.GetPolygons()) {
        painter->drawPolygon(
            polygon.GetVertices().data(), static_cast<int>(polygon.GetVertices().size()));
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(245, 233, 66));
    const auto shadow_alpha = static_cast<int>(255 * kShadowAlpha);
    const auto light_areas = controller_.CreateStaticLightArea();
    const auto additional_light_areas = controller_.CreateAdditionalStaticLightAreas();
    for (const auto& light_area : light_areas) {
        painter->drawPolygon(
            light_area.GetVertices().data(), static_cast<int>(light_area.GetVertices().size()));
    }
    painter->setBrush(QColor{245, 233, 66, shadow_alpha});
    for (const auto& light_source : additional_light_areas) {
        for (const auto& polygon : light_source) {
            painter->drawPolygon(
                polygon.GetVertices().data(), static_cast<int>(polygon.GetVertices().size()));
        }
    }
}

void MainWidget::Paint(QPainter* painter) {
    switch (controller_.GetMode()) {
        case Controller::Mode::Light: {
            if (controller_.HasLightSource()) {
                const auto shadow_alpha = static_cast<int>(255 * kShadowAlpha);
                const auto light_area = controller_.CreateLightArea();
                const auto additional_polygons = controller_.CreateAdditionalLightAreas();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setBrush(QColor(255, 255, 255, 200));
                painter->setPen(Qt::NoPen);
                painter->drawPolygon(
                    light_area.GetVertices().data(),
                    static_cast<int>(light_area.GetVertices().size()));
                painter->setBrush(QColor(255, 255, 255, shadow_alpha));
                for (const auto& polygon : additional_polygons) {
                    painter->drawPolygon(
                        polygon.GetVertices().data(),
                        static_cast<int>(polygon.GetVertices().size()));
                }
            }
        }
        case Controller::Mode::StaticLights: {
            DrawPoints(painter, controller_.GetStaticLights(), {255, 0, 0});
            break;
        }
        case Controller::Mode::Polygons: {
            for (const auto& polygon : controller_.GetPolygons()) {
                DrawPoints(painter, polygon.GetVertices(), Qt::green);
            }
            break;
        }
    }
}

void MainWidget::showEvent(QShowEvent* /*event*/) {
    RepaintStatic();
}

QPointF MainWidget::ToRelative(const QPoint& absolute) const {
    return {
      static_cast<double>(absolute.x()) / width(), static_cast<double>(absolute.y()) / height()};
}

QPoint MainWidget::ToAbsolute(const QPointF& relative) const {
    return {static_cast<int>(relative.x() * width()), static_cast<int>(relative.y() * height())};
}

// NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
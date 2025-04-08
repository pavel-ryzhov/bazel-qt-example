#include "main_widget.h"

#include "controller.h"
#include "polygon.h"

// NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)

MainWidget::MainWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setMouseTracking(true);
    connect(&controller_, &Controller::RepaintStatic, this, &MainWidget::RepaintStatic);
    connect(&controller_, &Controller::Repaint, this, &MainWidget::Repaint);
}

void MainWidget::SetMode(Controller::Mode mode) {
    controller_.SetMode(mode);
}

void MainWidget::Resize() {
    RepaintStatic();
}

void MainWidget::paintEvent(QPaintEvent* /*event*/) {
    QPixmap pixmap{width(), height()};
    pixmap.fill(Qt::transparent);
    QPainter painter{&pixmap};
    QPainter result_painter{this};
    painter.scale(width(), height());
    Paint(&painter);
    result_painter.drawPixmap(0, 0, static_background_);
    result_painter.drawPixmap(0, 0, pixmap);
}

void MainWidget::mouseMoveEvent(QMouseEvent* event) {
    if (controller_.GetMode() == Controller::Mode::Light) {
        controller_.SetLightSource(ToRelative(event->pos()));
    }
}

void MainWidget::mousePressEvent(QMouseEvent* event) {
    if (controller_.GetMode() == Controller::Mode::Polygons) {
        switch (event->button()) {
            case Qt::LeftButton: {
                controller_.AddVertex(ToRelative(event->pos()));
                break;
            }
            case Qt::RightButton: {
                controller_.FinishPolygon();
                break;
            }
            default: {
            }
        }
    }
}

void MainWidget::Refresh() {
    controller_.Refresh();
}

void MainWidget::Repaint() {
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
            polygon.GetVertecis().data(), static_cast<int>(polygon.GetVertecis().size()));
    }
}

void MainWidget::Paint(QPainter* painter) {
    if (controller_.HasLightSource() && controller_.GetMode() == Controller::Mode::Light) {
        const auto light_area = controller_.CreateLightArea();
        const auto additional_polygons = controller_.CreateAdditionalLightAreas();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(Qt::white);
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(
            light_area.GetVertecis().data(), static_cast<int>(light_area.GetVertecis().size()));
        painter->setBrush(QColor(255, 255, 255, 32));
        for (const auto& polygon : additional_polygons) {
            painter->drawPolygon(
                polygon.GetVertecis().data(), static_cast<int>(polygon.GetVertecis().size()));
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
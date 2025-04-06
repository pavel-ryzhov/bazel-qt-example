#include "main_widget.h"
#include "controller.h"

//NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)

MainWidget::MainWidget(QWidget* parent): QWidget(parent), controller_(width(), height()) {
    setMouseTracking(true);
    connect(&controller_, &Controller::Repaint, this, &MainWidget::Repaint);
}

void MainWidget::SetMode(Controller::Mode mode) {
    controller_.SetMode(mode);
}

static void DrawPoint(QPainter* painter, const QPointF& point) {
    const auto pen = painter->pen();
    const auto brush = painter->brush();
    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);
    painter->drawEllipse(point, 3, 3);
    painter->setPen(pen);
    painter->setBrush(brush);
}

void MainWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::white);
    painter.setPen(QPen(Qt::black, 2));
    for (const auto& polygon : controller_.GetPolygons()) {
        painter.drawPolygon(polygon.GetVertecis().data(), static_cast<int>(polygon.GetVertecis().size()));
    }
    

    if (controller_.HasLightSource()) {
        auto rays = controller_.CastRays();
        controller_.IntersectRays(&rays);
        const auto light_area = Controller::CreateLightArea(rays);
        // painter.setBrush(Qt::yellow);
        painter.setPen(Qt::gray);
        for (const auto& vertex : light_area.GetVertecis()) {
            painter.drawLine(controller_.GetLightSource(), vertex);
            DrawPoint(&painter, vertex);
        }
        DrawPoint(&painter, controller_.GetLightSource());
    }
}



void MainWidget::mouseMoveEvent(QMouseEvent* event) {
    if (controller_.GetMode() == Controller::Mode::Light) {
        controller_.SetLightSource(event->pos());
    }
}

void MainWidget::mousePressEvent(QMouseEvent* event) {
    if (controller_.GetMode() == Controller::Mode::Polygons) {
        switch (event->button()) {
            case Qt::LeftButton: {
                controller_.AddVertex({event->pos()});
                break;
            }
            case Qt::RightButton: {
                controller_.FinishPolygon();
                break;
            }
            default: {}
        }
    }
}

void MainWidget::resizeEvent(QResizeEvent* event) {
    controller_.Resize(event->size().width(), event->size().height());
    QWidget::resizeEvent(event);
}

void MainWidget::Refresh() {
    controller_.Refresh(width(), height());
}

void MainWidget::Repaint() {
    repaint();
}

//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
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

void MainWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    for (const auto& polygon : controller_.GetPolygons()) {
        // for (size_t i = 1; i < polygon.GetVertecis().size(); ++i) {
            painter.drawPolygon(polygon.GetVertecis().data(), static_cast<int>(polygon.GetVertecis().size()));
        // }
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent* event) {
    if (controller_.GetMode() == Controller::Mode::Light) {

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

void MainWidget::Repaint() {
    repaint();
}

//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
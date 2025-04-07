#include "main_widget.h"
#include "controller.h"
#include "labs/raycaster/polygon.h"
#include <chrono>

//NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)

MainWidget::MainWidget(QWidget* parent): QWidget(parent), controller_(width(), height()) {
    setMouseTracking(true);
    connect(&controller_, &Controller::RepaintStatic, this, &MainWidget::RepaintStatic);
    connect(&controller_, &Controller::Repaint, this, &MainWidget::Repaint);

    timer_.setInterval(16);
    // timer_.setSingleShot(true);
    connect(&timer_, &QTimer::timeout, this, [this](){
        Repaint();
    });
    timer_.start();
}

void MainWidget::SetMode(Controller::Mode mode) {
    controller_.SetMode(mode);
}

void MainWidget::paintEvent(QPaintEvent* /*event*/) {
    QPixmap pixmap{width(), height()};
    pixmap.fill(Qt::transparent);
    QPainter painter{&pixmap};
    QPainter result_painter{this};
    Paint(&painter);
    result_painter.drawPixmap(0, 0, static_background_);
    result_painter.drawPixmap(0, 0, pixmap);
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
}

void MainWidget::Refresh() {
    controller_.Refresh(width(), height());
}

void MainWidget::Repaint() {
    repaint();
}

void MainWidget::RepaintStatic() {
    static_background_ = QPixmap{width(), height()};
    static_background_.fill(Qt::black);
    // static_background_.fill(QColor(40, 40, 40));
    QPainter painter{&static_background_};
    PaintStatic(&painter);
    Repaint();
}

static void DrawPoint(QPainter* painter, const QPointF& point) {
    painter->save();
    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);
    painter->drawEllipse(point, 3, 3);
    painter->restore();
}

// static void DrawPolygon(QPainter* painter, const Polygon& polygon, const QColor& color) {

// }

void MainWidget::PaintStatic(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::green, 2));
    for (const auto& polygon : controller_.GetPolygons()) {
        painter->drawPolygon(polygon.GetVertecis().data(), static_cast<int>(polygon.GetVertecis().size()));
    }
}

void MainWidget::Paint(QPainter* painter) {

    if (controller_.HasLightSource()) {
        // const auto timestamp = std::chrono::system_clock::now();
        painter->setRenderHint(QPainter::Antialiasing);

        const auto light_area = controller_.CreateLightArea();
        const auto additional_polygons = controller_.CreateAdditionalLightAreas();

        // const auto timestamp1 = std::chrono::system_clock::now();

        painter->setBrush(Qt::white);
        // painter->setPen(QColor(128, 0, 0, 128));
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(light_area.GetVertecis().data(), static_cast<int>(light_area.GetVertecis().size()));
        // for (const auto& vertex : light_area.GetVertecis()) {
        //     painter->drawLine(controller_.GetLightSource(), vertex);
        //     DrawPoint(painter, vertex);
        // }
        // DrawPoint(painter, controller_.GetLightSource());

        painter->setBrush(QColor(255, 255, 255, 32));
        
        for (const auto& polygon : additional_polygons) {
            painter->drawPolygon(polygon.GetVertecis().data(), static_cast<int>(polygon.GetVertecis().size()));
        }

        // const auto timestamp2 = std::chrono::system_clock::now();
        // qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(timestamp1 - timestamp) << ' ' << std::chrono::duration_cast<std::chrono::milliseconds>(timestamp2 - timestamp);
        // constexpr auto kAngleStep = 2 * std::numbers::pi / kAdditionalLightSourcesCount;
        // for (size_t i = 0; i < kAdditionalLightSourcesCount; ++i) {
        //     const auto angle = kAngleStep * static_cast<double>(i);
        //     DrawPoint(painter, controller_.GetLightSource() + kAdditionalLightSourceRadius * QPointF{std::cos(angle), std::sin(angle)});
        // }
    }
}

//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
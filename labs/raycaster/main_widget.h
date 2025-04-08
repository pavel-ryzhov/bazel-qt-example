#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include "controller.h"

#include <QOpenGLWidget>
#include <QtWidgets>

class MainWidget : public QOpenGLWidget {
    Q_OBJECT
   public:
    explicit MainWidget(QWidget* parent = nullptr);

    void SetMode(Controller::Mode mode);
    void Refresh();
    void Resize();

   private slots:
    void Repaint();
    void RepaintStatic();

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void showEvent(QShowEvent* event) override;

   private:
    Controller controller_;
    QPixmap static_background_;

    void Paint(QPainter* painter);
    void PaintStatic(QPainter* painter);

    [[nodiscard]] QPointF ToRelative(const QPoint& absolute) const;
    [[nodiscard]] QPoint ToAbsolute(const QPointF& relative) const;
};

#endif
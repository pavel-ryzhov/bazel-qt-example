#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include "controller.h"

#include <QtWidgets>
#include <QOpenGLWidget>

class MainWidget : public QWidget {
    Q_OBJECT
   public:
    explicit MainWidget(QWidget* parent = nullptr);

    void SetMode(Controller::Mode mode);
    void Refresh();

   private slots:
    void Repaint();
    void RepaintStatic();

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    Controller controller_;
    QPixmap static_background_;
    QTimer timer_;

    void Paint(QPainter* painter);
    void PaintStatic(QPainter* painter);
};

#endif
#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include "controller.h"

#include <QtWidgets>

class MainWidget : public QWidget {
    Q_OBJECT
   public:
    explicit MainWidget(QWidget* parent = nullptr);

    void SetMode(Controller::Mode mode);
    void Refresh();

   private slots:
    void Repaint();

   protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    Controller controller_;
};

#endif
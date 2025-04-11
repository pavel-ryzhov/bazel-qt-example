#pragma once

#include "main_widget.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QComboBox;
class QPushButton;
class QCheckBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow();

   private slots:
    void ModeChanged(int mode);
    void RefreshClicked();

   protected:
    void resizeEvent(QResizeEvent* event) override;

   private:
    QComboBox* mode_combo_box_;
    QPushButton* refresh_button_;
    MainWidget* main_widget_;
};

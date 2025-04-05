#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "main_widget.h"
#include <QMainWindow>

constexpr auto kDefaultWidth = 1200;
constexpr auto kDefaultHeight = 600;

QT_BEGIN_NAMESPACE
class QComboBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void ModeChanged(int mode);

private: // NOLINT(readability-redundant-access-specifiers)
    QComboBox* mode_combo_box_;
    MainWidget* main_widget_;
};

#endif
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

constexpr auto kDefaultWidth = 1200;
constexpr auto kDefaultHeight = 600;

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    

private: // NOLINT(readability-redundant-access-specifiers)
    

    
};

#endif
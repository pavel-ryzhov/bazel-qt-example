#pragma once

#include "tasks_widget.h"

#include <QMainWindow>
#include <cstdint>

constexpr auto kDefaultWidth = 1200;
constexpr auto kDefaultHeight = 600;

QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow();

   private slots:
   void StartExercise();

   private:  // NOLINT(readability-redundant-access-specifiers)
    QStackedLayout* stacked_layout_;
    TasksWidget* tasks_widget_;
    QLabel* score_label_;
    QLabel* menu_score_label_;
    QLabel* difficulty_label_;
    QMenu* difficulty_menu_;

    enum LayoutState : uint8_t { Main, Exercise };
};
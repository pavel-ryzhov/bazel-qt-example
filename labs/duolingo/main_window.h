#pragma once

#include <QMainWindow>

constexpr auto kDefaultWidth = 1200;
constexpr auto kDefaultHeight = 600;

QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLabel;
class QMenu;
class TasksWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    // void TranslationButtonClicked();
    // void GrammarButtonClicked();
    // void MixedButtonClicked();
    // void MistakesButtonClicked();

private: // NOLINT(readability-redundant-access-specifiers)
    QStackedLayout* stacked_layout_;
    TasksWidget* tasks_widget_;
    QLabel* score_label_;
    QLabel* menu_score_label_;
    QLabel* difficulty_label_;
    QMenu* difficulty_menu_;
    // QPushButton* translation_button_;
    // QPushButton* grammar_button_;
    // QPushButton* mixed_button_;
    // QPushButton* mistakes_button_;    
};
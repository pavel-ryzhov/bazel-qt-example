#pragma once

#include "entities.h"
#include "audio_player.h"

#include <QWidget>
#include <cstdint>
#include <memory>
#include <vector>

constexpr auto kExerciseTime = 180;
constexpr auto kNTasks = 7;
constexpr auto kMTranslationMistakes = 3;
constexpr auto kMGrammarMistakes = 2;
constexpr auto kAllMistakes = 4;

class Database;
class Settings;
class AudioPlayer;

QT_BEGIN_NAMESPACE
class QTimer;
class QLabel;
class QLineEdit;
class QButtonGroup;
class QVBoxLayout;
class QStackedLayout;
class QProgressBar;
QT_END_NAMESPACE

class TasksWidget : public QWidget {
    Q_OBJECT
   public:
    enum TasksCategory : uint8_t { Translation, Grammar, Mixed, Mistakes };

    explicit TasksWidget(QWidget* parent = nullptr);

    bool InitExercise(TasksCategory category);

   signals:
    void ExerciseFinished() const;

   private slots:
    void UpdateTime();
    void CheckTask();
    void DisplayHint();

   protected:
    void keyPressEvent(QKeyEvent* event) override;

   private:
    QTimer* timer_;
    QLabel* time_label_;
    QLabel* mistakes_label_;
    QLabel* title_label_;
    QLabel* task_label_;
    QLineEdit* line_edit_;
    QButtonGroup* button_group_;
    QLabel* result_label_;
    QLabel* result_icon_;
    QVBoxLayout* grammar_layout_;
    QStackedLayout* stacked_layout_;
    QProgressBar* progress_bar_;

    TasksCategory category_ = Translation;
    Database& database_;
    Settings& settings_;
    AudioPlayer& audio_player_;
    std::vector<std::unique_ptr<Task>> tasks_;
    std::vector<std::unique_ptr<Task>>::const_iterator current_task_;
    int mistakes_ = 0;
    int time_ = kExerciseTime;
    int score_ = 0;
    int mistakes_in_current_task_ = 0;
    bool block_submit_ = false;

    [[nodiscard]] bool HasTask() const;
    void InitTask();
    void RemoveRadioButtons() const;
    void AddRadioButtons(const QStringList& options) const;
    void FinishExercise(const QString& title, const QString& message);
    void SetResult(bool value);
};

#include "tasks_widget.h"

#include "database.h"
#include "entities.h"
#include "settings.h"

#include <QtWidgets>

// NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)

TasksWidget::TasksWidget(QWidget* parent)
    : QWidget(parent)
    , timer_(new QTimer(this))
    , time_label_(new QLabel())
    , mistakes_label_(new QLabel())
    , title_label_(new QLabel())
    , task_label_(new QLabel())
    , line_edit_(new QLineEdit())
    , button_group_(new QButtonGroup())
    , result_label_(new QLabel())
    , grammar_layout_(new QVBoxLayout())
    , stacked_layout_(new QStackedLayout())
    , database_(Database::GetInstance())
    , settings_(Settings::GetInstance()) {
    timer_->setInterval(1000);

    auto* v_layout = new QVBoxLayout();
    auto* h_layout = new QHBoxLayout();
    auto* h_layout_buttons = new QHBoxLayout();
    auto* v_layout_translation = new QVBoxLayout();

    auto* submit_button = new QPushButton("Подтвердить");
    auto* finish_button = new QPushButton("Закончить");

    connect(timer_, &QTimer::timeout, this, &TasksWidget::UpdateTime);
    connect(submit_button, &QPushButton::pressed, this, [] {

    });
    connect(finish_button, &QPushButton::pressed, this, [this] {
        timer_->stop();
        emit ExerciseFinished();
    });

    h_layout->addWidget(new QLabel("Ошибки: "));
    h_layout->addWidget(mistakes_label_);
    h_layout->addStretch();
    h_layout->addWidget(new QLabel("Оставшееся время: "));
    h_layout->addWidget(time_label_);

    v_layout->addItem(h_layout);
    v_layout->addWidget(title_label_, 0, Qt::AlignHCenter);
    v_layout->addWidget(task_label_, 0, Qt::AlignHCenter);

    v_layout_translation->addWidget(line_edit_);

    auto* layout_container_translation = new QWidget();
    auto* layout_container_grammar = new QWidget();
    layout_container_translation->setLayout(v_layout_translation);
    layout_container_grammar->setLayout(grammar_layout_);

    stacked_layout_->addWidget(layout_container_translation);
    stacked_layout_->addWidget(layout_container_grammar);

    v_layout->addItem(stacked_layout_);

    h_layout_buttons->addStretch();
    h_layout_buttons->addWidget(finish_button);
    h_layout_buttons->addStretch();
    h_layout_buttons->addWidget(submit_button);
    h_layout_buttons->addStretch();

    v_layout->addItem(h_layout_buttons);

    setLayout(v_layout);
}

bool TasksWidget::InitExercise(TasksCategory category) {
    const auto difficulty = settings_.GetDifficulty();
    category_ = category;
    mistakes_ = 0;
    score_ = 0;
    time_ = kExerciseTime;
    UpdateTime();
    mistakes_label_->setText("0");
    switch (category) {
        case Translation: {
            tasks_ = database_.SelectRandomTasksByTypeAndDifficultyAndCompletion(
                Task::Translation, difficulty, Task::NotDone, kNTasks);
            break;
        }
        case Grammar: {
            tasks_ = database_.SelectRandomTasksByTypeAndDifficultyAndCompletion(
                Task::Grammar, difficulty, Task::NotDone, kNTasks);
            break;
        }
        case Mixed: {
            tasks_ = database_.SelectRandomTasksByDifficultyAndCompletion(
                difficulty, Task::NotDone, kNTasks);
            break;
        }
        case Mistakes: {
            tasks_ = database_.SelectRandomTasksByCompletion(Task::WithMistake, kNTasks);
            break;
        }
    }
    current_task_ = tasks_.cbegin();
    if (!HasTask()) {
        return false;
    }
    InitTask();
    return true;
}

void TasksWidget::InitTask() {
    mistakes_in_current_task_ = 0;
    mistake_in_current_task_ = false;
    int type = (**current_task_).GetType();
    switch (type) {
        case Task::Translation: {
            title_label_->setText("Переведите текст");
            const auto& task = dynamic_cast<const TranslationTask&>(**current_task_);
            task_label_->setText(task.GetTask());
            line_edit_->setText("");
            break;
        }
        case Task::Grammar: {
            title_label_->setText("Выберите верный вариант");
            const auto& task = dynamic_cast<const GrammarTask&>(**current_task_);
            task_label_->setText(task.GetTask());
            ReplaceRadioButtons(task.GetOptions());
            break;
        }
        default: {}
    }
    stacked_layout_->setCurrentIndex(type);
}

void TasksWidget::UpdateTime() {
    --time_;
    time_label_->setText(QString("%1:%2")
    .arg(time_ / 60, 2, 10, QLatin1Char('0'))
    .arg(time_ % 60, 2, 10, QLatin1Char('0')));
    if (time_ == 0) {
        timer_->stop();
        QMessageBox::information(this, "Упражнение не завершено", "Время истекло!");
        emit ExerciseFinished();
    }
}

bool TasksWidget::HasTask() const {
    return current_task_ != tasks_.cend();
}

void TasksWidget::ReplaceRadioButtons(const QStringList& options) const {
    for (auto* button : button_group_->buttons()) {
        button_group_->removeButton(button);
        grammar_layout_->removeWidget(button);
        button->deleteLater();
    }
    auto it = options.begin();
    for (int i = 0; i < options.size(); ++i, ++it) {
        auto* button = new QRadioButton(*it);
        button_group_->addButton(button, i);
        grammar_layout_->addWidget(button);
    }
}

void TasksWidget::MistakeMade() {

}

// NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
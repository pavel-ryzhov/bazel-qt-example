#include "tasks_widget.h"

#include "database.h"
#include "entities.h"
#include "settings.h"

#include <QtWidgets>
#include <string>

constexpr auto kMainStyle = R"(
    QTextEdit {
        background: #1e2127;
        color: #ffffff;
        border: none;
        padding: 8px;
    }
    QLabel {
        font-size: 16pt;
        font-weight: bold;
    }
)";

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
    , result_icon_(new QLabel())
    , result_layout_container_(new QWidget())
    , grammar_layout_(new QVBoxLayout())
    , stacked_layout_(new QStackedLayout())
    , database_(Database::GetInstance())
    , settings_(Settings::GetInstance()) {
    timer_->setInterval(1000);

    setStyleSheet(kMainStyle);
    result_label_->setStyleSheet("font-size: 11pt;");

    auto* v_layout = new QVBoxLayout();
    auto* h_layout = new QHBoxLayout();
    auto* h_layout_buttons = new QHBoxLayout();
    auto* v_layout_translation = new QVBoxLayout();

    auto* submit_button = new QPushButton("Ответить");
    auto* finish_button = new QPushButton("Закончить");

    submit_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    finish_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    const int max_width =
        std::max(submit_button->sizeHint().width(), finish_button->sizeHint().width()) + 50;

    submit_button->setFixedWidth(max_width);
    finish_button->setFixedWidth(max_width);

    connect(timer_, &QTimer::timeout, this, &TasksWidget::UpdateTime);
    connect(submit_button, &QPushButton::pressed, this, &TasksWidget::CheckTask);
    connect(finish_button, &QPushButton::pressed, this, [this] {
        timer_->stop();
        emit ExerciseFinished();
    });

    auto* result_layout = new QHBoxLayout();
    result_layout->addWidget(result_icon_);
    result_layout->addWidget(result_label_);
    result_layout->addStretch();
    result_layout_container_->setLayout(result_layout);

    h_layout->addWidget(new QLabel("Ошибки: "));
    h_layout->addWidget(mistakes_label_);
    h_layout->addStretch();
    h_layout->addWidget(new QLabel("Оставшееся время: "));
    h_layout->addWidget(time_label_);

    v_layout->addItem(h_layout);
    v_layout->addWidget(title_label_, 0, Qt::AlignHCenter);
    v_layout->addStretch();
    v_layout->addWidget(task_label_, 0, Qt::AlignHCenter);

    v_layout_translation->addWidget(line_edit_, 0, Qt::AlignTop);

    auto* layout_container_translation = new QWidget();
    auto* layout_container_grammar = new QWidget();
    layout_container_translation->setLayout(v_layout_translation);
    layout_container_grammar->setLayout(grammar_layout_);

    stacked_layout_->addWidget(layout_container_translation);
    stacked_layout_->addWidget(layout_container_grammar);

    auto* v1_layout = new QVBoxLayout();
    auto* h1_layout = new QHBoxLayout();
    v1_layout->addItem(stacked_layout_);
    v1_layout->addWidget(result_layout_container_);

    h1_layout->addStretch();
    h1_layout->addItem(v1_layout);
    h1_layout->addStretch();
    v_layout->addItem(h1_layout);

    v_layout->addWidget(result_layout_container_, 0, Qt::AlignTop);
    
    v_layout->addStretch();
    v_layout->addStretch();

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
    timer_->start();
    return true;
}

void TasksWidget::InitTask() {
    mistakes_in_current_task_ = 0;
    result_layout_container_->setVisible(false);
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
        default: {
        }
    }
    stacked_layout_->setCurrentIndex(type);
}

void TasksWidget::UpdateTime() {
    --time_;
    time_label_->setText(QString("%1:%2")
                             .arg(time_ / 60, 2, 10, QLatin1Char('0'))
                             .arg(time_ % 60, 2, 10, QLatin1Char('0')));
    if (time_ == 0) {
        FinishExercise("Упражнение не пройдено", "Время истекло!");
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
        button->setStyleSheet("font-size: 13pt;");
        button_group_->addButton(button, i);
        grammar_layout_->addWidget(button);
    }
}

void TasksWidget::CheckTask() {
    TaskResult result;
    int k_mistakes = -1;
    switch ((**current_task_).GetType()) {
        case Task::Translation: {
            const auto& task = dynamic_cast<const TranslationTask&>(**current_task_);
            result = task.GetScore(line_edit_->text());
            k_mistakes = kMTranslationMistakes;
            break;
        }
        case Task::Grammar: {
            const auto& task = dynamic_cast<const GrammarTask&>(**current_task_);
            const auto checked_id = button_group_->checkedId();
            if (checked_id == -1) {
                return;
            }
            result = task.GetScore(button_group_->checkedId());
            k_mistakes = kMGrammarMistakes;
            break;
        }
    }
    auto finish = false;
    SetResult(result.first);
    if (!result.first) {
        mistakes_label_->setText(std::to_string(++mistakes_).c_str());
        ++mistakes_in_current_task_;
        finish = mistakes_in_current_task_ == kAllMistakes || mistakes_ == k_mistakes;
        database_.UpdateTaskCompletion((**current_task_).GetId(), Task::WithMistake);
        if (!finish) {
            return;
        }
    }
    if (finish) {
        FinishExercise("Упражнение не пройдено", "Превышен лимит ошибок!");
    } else {
        if (mistakes_in_current_task_ == 0) {
            database_.UpdateTaskCompletion((**current_task_).GetId(), Task::Done);
        }
        ++current_task_;
        score_ += result.second;
        if (HasTask()) {
            InitTask();
        } else {
            if (mistakes_in_current_task_ == 0) {
                settings_.AddScore(score_);
                FinishExercise("Отлично! Упражнение пройдено!", ("Рейтинг: +" + std::to_string(score_)).c_str());
            } else {
                FinishExercise("Упражнение пройдено", "Вам есть над чем поработать");
            }
        }
    }
}

void TasksWidget::FinishExercise(const QString& title, const QString& message) {
    timer_->stop();
    QMessageBox::information(this, title, message);
    emit ExerciseFinished();
}

void TasksWidget::SetResult(bool value) const {
    result_icon_->setPixmap(QApplication::style()->standardPixmap(value ? QStyle::SP_DialogApplyButton : QStyle::SP_DialogCancelButton));
    result_label_->setText(value ? "Верно!" : "Ответ неверный!");
    result_layout_container_->setVisible(true);
}

// NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
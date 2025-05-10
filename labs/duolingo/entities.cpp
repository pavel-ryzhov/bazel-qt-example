#include "entities.h"

#include "database.h"

#include <QtSql/QSqlQuery>
#include <cstdint>
#include <memory>
#include <utility>

#define colon QString(":")

Task::Task(int id, Difficulty difficulty, Completion completion)
    : id_(id), difficulty_(difficulty), completion_(completion) {
}

void Task::BindToQuery(QSqlQuery* query) const {
    query->bindValue(colon + kDifficultyField, difficulty_);
    query->bindValue(colon + kCompletionField, completion_);
    query->bindValue(colon + kTypeField, GetType());
}

void Task::ReadQuery(const QSqlQuery& query) {
    id_ = query.value(kIdField).toInt();
    difficulty_ = static_cast<Difficulty>(query.value(kDifficultyField).toUInt());
    completion_ = static_cast<Completion>(query.value(kCompletionField).toUInt());
}

std::unique_ptr<Task> Task::FromQuery(const QSqlQuery& query) {
    return Task::FromQuery(query, query);
}

std::unique_ptr<Task> Task::FromQuery(const QSqlQuery& base_query, const QSqlQuery& query) {
    std::unique_ptr<Task> task;
    switch (base_query.value(kTypeField).toUInt()) {
        case Task::Translation: {
            task = std::make_unique<TranslationTask>();
            break;
        }
        case Task::Grammar: {
            task = std::make_unique<GrammarTask>();
            break;
        }
        default: {
        }
    }
    task->Task::ReadQuery(base_query);
    task->ReadQuery(query);
    return task;
}

TranslationTask::TranslationTask(
    int id, Difficulty difficulty, Completion completion, QString task, QString answer)
    : Task(id, difficulty, completion), task_(std::move(task)), answer_(std::move(answer)) {
}

TranslationTask::TranslationTask(
    int id, uint8_t difficulty, uint8_t completion, QString task, QString answer)
    : Task(id, difficulty, completion), task_(std::move(task)), answer_(std::move(answer)) {
}

TranslationTask::TranslationTask(
    Difficulty difficulty, Completion completion, QString task, QString answer)
    : Task(difficulty, completion), task_(std::move(task)), answer_(std::move(answer)) {
}

TranslationTask::TranslationTask(
    uint8_t difficulty, uint8_t completion, QString task, QString answer)
    : Task(difficulty, completion), task_(std::move(task)), answer_(std::move(answer)) {
}

void TranslationTask::BindToQuery(QSqlQuery* query) const {
    query->bindValue(colon + kTaskField, task_);
    query->bindValue(colon + kAnswerField, answer_);
}

void TranslationTask::ReadQuery(const QSqlQuery& query) {
    task_ = query.value(kTaskField).toString();
    answer_ = query.value(kAnswerField).toString();
}

TaskResult TranslationTask::GetScore(const QString& actual_answer) const {
    const double result = GradeAnswer(actual_answer);
    return std::make_pair(result > .9, static_cast<int>((GetDifficulty() + 1) * 2 * result * kScoreMultiplier));
}

double TranslationTask::GradeAnswer(const QString& actual_answer) const {
    return static_cast<double>(actual_answer == answer_);
}

GrammarTask::GrammarTask(
    int id, Difficulty difficulty, Completion completion, QString task, QStringList options,
    int answer)
    : Task(id, difficulty, completion)
    , task_(std::move(task))
    , options_(std::move(options))
    , answer_(answer) {
}

GrammarTask::GrammarTask(
    int id, uint8_t difficulty, uint8_t completion, QString task, QStringList options, int answer)
    : Task(id, difficulty, completion)
    , task_(std::move(task))
    , options_(std::move(options))
    , answer_(answer) {
}

GrammarTask::GrammarTask(
    Difficulty difficulty, Completion completion, QString task, QStringList options, int answer)
    : Task(difficulty, completion)
    , task_(std::move(task))
    , options_(std::move(options))
    , answer_(answer) {
}

GrammarTask::GrammarTask(
    uint8_t difficulty, uint8_t completion, QString task, QStringList options, int answer)
    : Task(difficulty, completion)
    , task_(std::move(task))
    , options_(std::move(options))
    , answer_(answer) {
}

TaskResult GrammarTask::GetScore(int actual_answer) const {
    const bool result = actual_answer == answer_;
    return std::make_pair(result, static_cast<int>(result) * (GetDifficulty() + 1) * kScoreMultiplier);
}

void GrammarTask::BindToQuery(QSqlQuery* query) const {
    query->bindValue(colon + kTaskField, task_);
    query->bindValue(colon + kOptionsField, options_.join('\x1E'));
    query->bindValue(colon + kAnswerField, answer_);
}

void GrammarTask::ReadQuery(const QSqlQuery& query) {
    task_ = query.value(kTaskField).toString();
    options_ = query.value(kOptionsField).toString().split('\x1E');
    answer_ = query.value(kAnswerField).toInt();
}

std::ostream& operator<<(std::ostream& os, const Task& task) {
    switch (task.GetType()) {
        case Task::Translation: {
            const auto& tt = dynamic_cast<const TranslationTask&>(task);
            return os << "TranslationTask{" << tt.GetId() << ", "
                      << static_cast<int>(tt.GetDifficulty()) << ", "
                      << static_cast<int>(tt.GetCompletion()) << ", "
                      << tt.GetTask().toStdString() << ", " << tt.GetAnswer().toStdString() << "}";
        }
        case Task::Grammar: {
            const auto& gt = dynamic_cast<const GrammarTask&>(task);
            return os << "TranslationTask{" << gt.GetId() << ", "
                      << static_cast<int>(gt.GetDifficulty()) << ", "
                      << static_cast<int>(gt.GetCompletion()) << ", "
                      << gt.GetTask().toStdString() << ", {"
                      << gt.GetOptions().join(", ").toStdString() << "}, " << gt.GetAnswer() << "}";
        }
    }
    return os;
}
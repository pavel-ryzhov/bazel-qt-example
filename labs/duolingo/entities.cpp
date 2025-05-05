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

std::unique_ptr<Task> FromQuery(const QSqlQuery& query) {
    return Task::FromQuery(query, query);
}

std::unique_ptr<Task> Task::FromQuery(const QSqlQuery& base_query, const QSqlQuery& query) {
    std::unique_ptr<Task> task;
    switch (query.value(kTypeField).toUInt()) {
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

void TranslationTask::BindToQuery(QSqlQuery* query) const {
    query->bindValue(colon + kTaskField, task_);
    query->bindValue(colon + kAnswerField, answer_);
}

void TranslationTask::ReadQuery(const QSqlQuery& query) {
    task_ = query.value(kTaskField).toString();
    answer_ = query.value(kAnswerField).toString();
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
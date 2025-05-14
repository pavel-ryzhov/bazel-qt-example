#pragma once

#include "entities.h"

#include <QtSql/QSqlDatabase>
#include <memory>
#include <vector>

constexpr auto kDatabaseName = "labs/duolingo/data/database.db";

constexpr auto kTasksTable = "tasks";
constexpr auto kTranslationTasksTable = "translation_tasks";
constexpr auto kGrammarTasksTable = "grammar_tasks";

constexpr auto kLimit = "limit";

constexpr auto kIdField = "id";
constexpr auto kTypeField = "type";
constexpr auto kDifficultyField = "difficulty";
constexpr auto kCompletionField = "completion";
constexpr auto kTaskField = "task";
constexpr auto kAnswerField = "answer";
constexpr auto kOptionsField = "options";
constexpr auto kHintField = "hint";

class Database {
   public:
    static Database& GetInstance() {
        static Database db;
        return db;
    }

    void InsertTask(const std::unique_ptr<Task>& task);
    [[nodiscard]] std::vector<std::unique_ptr<Task>> SelectRandomTasksByDifficultyAndCompletion(
        Task::Difficulty difficulty, Task::Completion completion = Task::Completion::NotDone,
        int limit = 1) const;
    [[nodiscard]] std::vector<std::unique_ptr<Task>>
    SelectRandomTasksByTypeAndDifficultyAndCompletion(
        Task::Type type, Task::Difficulty difficulty,
        Task::Completion completion = Task::Completion::NotDone, int limit = 1) const;
    void UpdateTaskCompletion(int id, Task::Completion completion) const;
    [[nodiscard]] std::vector<std::unique_ptr<Task>> SelectRandomTasksByCompletion(
        Task::Completion completion = Task::Completion::NotDone, int limit = 1) const;
    void ResetToNotDone() const;

    Database(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(const Database&) = delete;
    Database& operator=(Database&&) = delete;

    ~Database();

   private:
    QSqlDatabase database_;

    [[nodiscard]] std::vector<std::unique_ptr<Task>> SelectTasks(QSqlQuery* base_query) const;

    Database();
};
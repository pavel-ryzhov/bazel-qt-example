#include "database.h"

#include "entities.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <memory>
#include <vector>

// NOLINTBEGIN(fuchsia-statically-constructed-objects, cert-err58-cpp)

static const auto kColon = QStringLiteral(":");
static const QString kInsert3ArgTaskQueryTemplate = QStringLiteral(R"(
    INSERT INTO %1 (%2, %3, %4)
    VALUES (:%2, :%3, :%4)
)");
static const QString kSelectByIdQueryTemplate = QStringLiteral(R"(
    SELECT * FROM %1
    WHERE %2 = :%2
)");
static const QString kSelectRandomWithJoinQueryTemplate = QStringLiteral(R"(
    SELECT * FROM %1
    INNER JOIN %2 ON %1.%3 = %2.%3
    WHERE %4 = :%4 AND %5 = :%5
    ORDER BY RANDOM()
    LIMIT :%6
)");

static const QString kCreateTasksTableQuery =
    QString(R"(
    CREATE TABLE IF NOT EXISTS %1 (
        %2 INTEGER PRIMARY KEY AUTOINCREMENT,
        %3 INTEGER,
        %4 INTEGER,
        %5 INTEGER
    )
)")
        .arg(kTasksTable, kIdField, kTypeField, kDifficultyField, kCompletionField);
static const QString kCreateTranslationTasksTableQuery =
    QString(R"(
    CREATE TABLE IF NOT EXISTS %1 (
        %3 INTEGER PRIMARY KEY,
        %4 TEXT NOT NULL,
        %5 TEXT NOT NULL,
        FOREIGN KEY (%3) REFERENCES %2(%3) ON DELETE CASCADE
    )
)")
        .arg(kTranslationTasksTable, kTasksTable, kIdField, kTaskField, kAnswerField);
static const QString kCreateGrammarTasksTableQuery =
    QString(R"(
    CREATE TABLE IF NOT EXISTS %1 (
        %3 INTEGER PRIMARY KEY,
        %4 TEXT NOT NULL,
        %5 TEXT NOT NULL,
        %6 INTEGER,
        FOREIGN KEY (%3) REFERENCES %2(%3) ON DELETE CASCADE
    )
)")
        .arg(kGrammarTasksTable, kTasksTable, kIdField, kTaskField, kOptionsField, kAnswerField);
static const QString kInsertTaskQuery =
    kInsert3ArgTaskQueryTemplate.arg(kTasksTable, kTypeField, kDifficultyField, kCompletionField);
static const QString kInsertTranslationTaskQuery =
    kInsert3ArgTaskQueryTemplate.arg(kTranslationTasksTable, kIdField, kTaskField, kAnswerField);
static const QString kInsertGrammarTaskQuery =
    QString(R"(
    INSERT INTO %1 (%2, %3, %4, %5)
    VALUES (:%2, :%3, :%4, :%5)
)")
        .arg(kGrammarTasksTable, kIdField, kTaskField, kOptionsField, kAnswerField);
static const QString kSelectRandomTasksByDifficultyAndCompletionQuery =
    QString(R"(
    SELECT * FROM %1
    WHERE %2 = :%2 AND %3 = :%3
    ORDER BY RANDOM()
    LIMIT :%4
)")
        .arg(kTasksTable, kDifficultyField, kCompletionField, kLimit);
static const QString kSelectRandomTasksByCompletionQuery =
    QString(R"(
        SELECT * FROM %1
        WHERE %2 = :%2
        ORDER BY RANDOM()
        LIMIT :%3
    )")
        .arg(kTasksTable, kCompletionField, kLimit);
static const QString kSelectTranslatioTaskByIdQuery =
    kSelectByIdQueryTemplate.arg(kTranslationTasksTable, kIdField);
static const QString kSelectGrammarTaskByIdQuery =
    kSelectByIdQueryTemplate.arg(kGrammarTasksTable, kIdField);
static const QString kSelectRandomTranslationTasksByDifficultyAndCompletionQuery =
    kSelectRandomWithJoinQueryTemplate.arg(
        kTasksTable, kTranslationTasksTable, kIdField, kDifficultyField, kCompletionField, kLimit);
static const QString kSelectRandomGrammarTasksByDifficultyAndCompletionQuery =
    kSelectRandomWithJoinQueryTemplate.arg(
        kTasksTable, kGrammarTasksTable, kIdField, kDifficultyField, kCompletionField, kLimit);
static const QString kUpdateTaskCompletionQuery =
    QString(R"(
    UPDATE %1
    SET %2 = :%2
    WHERE %3 = :%3
)")
        .arg(kTasksTable, kCompletionField, kIdField);

Database::Database() : database_(QSqlDatabase::addDatabase("QSQLITE")) {
    database_.setDatabaseName(kDatabaseName);
    if (!database_.open()) {
        qDebug() << "Database connection error!" << database_.lastError().text();
        return;
    }
    QSqlQuery query(database_);
    if (!(query.exec(kCreateTasksTableQuery) && query.exec(kCreateTranslationTasksTableQuery) &&
          query.exec(kCreateGrammarTasksTableQuery))) {
        qDebug() << "Error when creaating tables!" << query.lastError().text();
    }
}

// NOLINTEND(fuchsia-statically-constructed-objects, cert-err58-cpp)

void Database::InsertTask(const std::unique_ptr<Task>& task) {
    database_.transaction();
    QSqlQuery query(database_);
    QSqlQuery base_query(database_);
    switch (task->GetType()) {
        case Task::Translation:
            query.prepare(kInsertTranslationTaskQuery);
            break;
        case Task::Grammar:
            query.prepare(kInsertGrammarTaskQuery);
            break;
    }
    base_query.prepare(kInsertTaskQuery);
    task->Task::BindToQuery(&base_query);
    task->BindToQuery(&query);
    bool fail = false;
    if (!base_query.exec()) {
        qDebug() << "Failed to insert a base task!" << query.lastError().text();
        fail = true;
    } else {
        query.bindValue(kColon + kIdField, base_query.lastInsertId().toInt());
    }
    if (fail || !query.exec()) {
        qDebug() << "Failed to insert a task!" << query.lastError().text();
        fail = true;
    }
    if (fail) {
        database_.rollback();
    }
    if (!database_.commit()) {
        qDebug() << "Failed to commit!" << query.lastError().text();
    }
}

// да-да, медленно при большом лимите
std::vector<std::unique_ptr<Task>> Database::SelectTasks(QSqlQuery* base_query) const {
    if (!base_query->exec()) {
        qDebug() << "Error when selecting base tasks!" << base_query->lastError().text();
        return {};
    }
    std::vector<std::unique_ptr<Task>> result;
    while (base_query->next()) {
        auto id = base_query->value(kIdField).toInt();
        QSqlQuery query(database_);
        switch (base_query->value(kTypeField).toUInt()) {
            case Task::Translation: {
                query.prepare(kSelectTranslatioTaskByIdQuery);
                break;
            }
            case Task::Grammar: {
                query.prepare(kSelectGrammarTaskByIdQuery);
                break;
            }
        }
        query.bindValue(kColon + kIdField, id);
        if (!(query.exec() && query.next())) {
            qDebug() << "Error when selecting task!" << query.lastError().text();
        } else {
            result.emplace_back(Task::FromQuery(*base_query, query));
        }
    }
    return result;
}

std::vector<std::unique_ptr<Task>> Database::SelectRandomTasksByDifficultyAndCompletion(
    Task::Difficulty difficulty, Task::Completion completion, int limit) const {
    QSqlQuery base_query(database_);
    base_query.prepare(kSelectRandomTasksByDifficultyAndCompletionQuery);
    base_query.bindValue(kColon + kDifficultyField, difficulty);
    base_query.bindValue(kColon + kCompletionField, completion);
    base_query.bindValue(kColon + kLimit, limit);
    return SelectTasks(&base_query);
}

std::vector<std::unique_ptr<Task>> Database::SelectRandomTasksByCompletion(
    Task::Completion completion, int limit) const {
    QSqlQuery base_query(database_);
    base_query.prepare(kSelectRandomTasksByCompletionQuery);
    base_query.bindValue(kColon + kCompletionField, completion);
    base_query.bindValue(kColon + kLimit, limit);
    return SelectTasks(&base_query);
}

std::vector<std::unique_ptr<Task>> Database::SelectRandomTasksByTypeAndDifficultyAndCompletion(
    Task::Type type, Task::Difficulty difficulty, Task::Completion completion, int limit) const {
    QSqlQuery query(database_);
    switch (type) {
        case Task::Type::Translation: {
            query.prepare(kSelectRandomTranslationTasksByDifficultyAndCompletionQuery);
            break;
        }
        case Task::Type::Grammar: {
            query.prepare(kSelectRandomGrammarTasksByDifficultyAndCompletionQuery);
            break;
        }
    }
    query.bindValue(kColon + kDifficultyField, difficulty);
    query.bindValue(kColon + kCompletionField, completion);
    query.bindValue(kColon + kLimit, limit);
    if (!query.exec()) {
        qDebug() << "Error when selecting tasks!" << query.lastError().text();
        return {};
    }
    std::vector<std::unique_ptr<Task>> result;
    while (query.next()) {
        result.emplace_back(Task::FromQuery(query));
    }
    return result;
}

void Database::UpdateTaskCompletion(int id, Task::Completion completion) const {
    QSqlQuery query(database_);
    query.prepare(kUpdateTaskCompletionQuery);
    query.bindValue(kColon + kCompletionField, completion);
    query.bindValue(kColon + kIdField, id);
    if (!query.exec()) {
        qDebug() << "Failed to update completion!" << query.lastError().text();
    }
}

Database::~Database() {
    database_.close();
}
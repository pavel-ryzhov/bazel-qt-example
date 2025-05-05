#pragma once

#include <QList>
#include <QString>
#include <cstdint>
#include <memory>

class QSqlQuery;

class Task {
   public:
    enum Difficulty : uint8_t { Low, Middle, High };

    enum Type : uint8_t { Translation, Grammar };

    enum Completion : uint8_t { Done, WithMistake, NotDone };

    Task() = default;
    Task(int id, Difficulty difficulty, Completion completion);

    Task(int id, uint8_t difficulty, uint8_t completion)
        : Task(id, static_cast<Difficulty>(difficulty), static_cast<Completion>(completion)) {
    }

    [[nodiscard]] int GetId() const {
        return id_;
    }

    [[nodiscard]] Difficulty GetDifficulty() const {
        return difficulty_;
    }

    [[nodiscard]] Completion GetCompletion() const {
        return completion_;
    }

    [[nodiscard]] virtual Type GetType() const = 0;

    virtual void BindToQuery(QSqlQuery* query) const;

    static std::unique_ptr<Task> FromQuery(const QSqlQuery& query);

    static std::unique_ptr<Task> FromQuery(const QSqlQuery& base_query, const QSqlQuery& query);

    virtual ~Task() = default;

    Task(const Task&) = default;
    Task(Task&&) = default;
    Task& operator=(const Task&) = default;
    Task& operator=(Task&&) = default;

   protected:
    virtual void ReadQuery(const QSqlQuery& query);

   private:
    int id_ = -1;
    Difficulty difficulty_ = Low;
    Completion completion_ = NotDone;
};

class TranslationTask : public Task {
   public:
    TranslationTask() = default;
    TranslationTask(
        int id, Difficulty difficulty, Completion completion, QString task, QString answer);
    TranslationTask(int id, uint8_t difficulty, uint8_t completion, QString task, QString answer);

    [[nodiscard]] const QString& GetTask() const {
        return task_;
    }

    [[nodiscard]] const QString& GetAnswer() const {
        return answer_;
    }

    [[nodiscard]] Type GetType() const override {
        return Translation;
    }

    void BindToQuery(QSqlQuery* query) const override;

   private:
    QString task_;
    QString answer_;

    void ReadQuery(const QSqlQuery& query) override;
};

class GrammarTask : public Task {
   public:
    GrammarTask() = default;
    GrammarTask(
        int id, Difficulty difficulty, Completion completion, QString task, QStringList options,
        int answer);
    GrammarTask(
        int id, uint8_t difficulty, uint8_t completion, QString task, QStringList options,
        int answer);

    [[nodiscard]] const QString& GetTask() const {
        return task_;
    }

    [[nodiscard]] int GetAnswer() const {
        return answer_;
    }

    [[nodiscard]] const QStringList& GetOptions() const {
        return options_;
    }

    [[nodiscard]] Type GetType() const override {
        return Grammar;
    }

    void BindToQuery(QSqlQuery* query) const override;

   private:
    QString task_;
    QStringList options_;
    int answer_ = -1;

    void ReadQuery(const QSqlQuery& query) override;
};
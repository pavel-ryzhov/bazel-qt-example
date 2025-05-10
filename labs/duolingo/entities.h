#pragma once

#include <QList>
#include <QString>
#include <cstdint>
#include <memory>
#include <ostream>
#include <utility>

constexpr auto kScoreMultiplier = 10;

class QSqlQuery;

class Task {
   public:
    enum Difficulty : uint8_t { Low, Medium, High };

    enum Type : uint8_t { Translation, Grammar };

    enum Completion : uint8_t { Done, WithMistake, NotDone };

    Task() = default;
    Task(int id, Difficulty difficulty, Completion completion);

    Task(int id, uint8_t difficulty, uint8_t completion)
        : Task(id, static_cast<Difficulty>(difficulty), static_cast<Completion>(completion)) {
    }

    Task(Difficulty difficulty, Completion completion) : Task(-1, difficulty, completion) {}

    Task(uint8_t difficulty, uint8_t completion) : Task(-1, difficulty, completion) {}

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

    static QString GetDifficultyText(Difficulty difficulty) {
        switch (difficulty) {
            case Low: return "Низкая";
            case Medium: return "Средняя";
            case High: return "Высокая";
        }
        return "";
    }

   protected:
    virtual void ReadQuery(const QSqlQuery& query);

   private:
    int id_ = -1;
    Difficulty difficulty_ = Low;
    Completion completion_ = NotDone;
};

using TaskResult = std::pair<bool, int>;

class TranslationTask : public Task {
   public:
    TranslationTask() = default;
    TranslationTask(
        int id, Difficulty difficulty, Completion completion, QString task, QString answer);
    TranslationTask(int id, uint8_t difficulty, uint8_t completion, QString task, QString answer);
    TranslationTask(Difficulty difficulty, Completion completion, QString task, QString answer);
    TranslationTask(uint8_t difficulty, uint8_t completion, QString task, QString answer);

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

    [[nodiscard]] TaskResult GetScore(const QString& actual_answer) const;

   private:
    QString task_;
    QString answer_;

    void ReadQuery(const QSqlQuery& query) override;

    [[nodiscard]] double GradeAnswer(const QString& actual_answer) const;
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
    GrammarTask(
        Difficulty difficulty, Completion completion, QString task, QStringList options,
        int answer);
    GrammarTask(
        uint8_t difficulty, uint8_t completion, QString task, QStringList options, int answer);

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

    [[nodiscard]] TaskResult GetScore(int actual_answer) const;

   private:
    QString task_;
    QStringList options_;
    int answer_ = -1;

    void ReadQuery(const QSqlQuery& query) override;
};

std::ostream& operator<<(std::ostream& os, const Task& task);
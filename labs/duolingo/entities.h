#pragma once

#include <QList>
#include <QString>
#include <cstdint>
#include <memory>
#include <optional>
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

    Task(
        int id, Difficulty difficulty, Completion completion, QString task,
        std::optional<QString> hint = {})
        : id_(id)
        , difficulty_(difficulty)
        , completion_(completion)
        , task_(std::move(task))
        , hint_(std::move(hint)) {
    }

    Task(
        int id, uint8_t difficulty, uint8_t completion, QString task,
        std::optional<QString> hint = {})
        : id_(id)
        , difficulty_(static_cast<Difficulty>(difficulty))
        , completion_(static_cast<Completion>(completion))
        , task_(std::move(task))
        , hint_(std::move(hint)) {
    }

    Task(
        Difficulty difficulty, Completion completion, QString task,
        std::optional<QString> hint = {})
        : difficulty_(difficulty)
        , completion_(completion)
        , task_(std::move(task))
        , hint_(std::move(hint)) {
    }

    Task(uint8_t difficulty, uint8_t completion, QString task, std::optional<QString> hint = {})
        : difficulty_(static_cast<Difficulty>(difficulty))
        , completion_(static_cast<Completion>(completion))
        , task_(std::move(task))
        , hint_(std::move(hint)) {
    }

    [[nodiscard]] int GetId() const {
        return id_;
    }

    [[nodiscard]] Difficulty GetDifficulty() const {
        return difficulty_;
    }

    [[nodiscard]] const QString& GetTask() const {
        return task_;
    }

    [[nodiscard]] Completion GetCompletion() const {
        return completion_;
    }

    [[nodiscard]] const QString& GetHint() const {
        return *hint_;
    }

    [[nodiscard]] bool HasHint() const {
        return hint_.has_value();
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
            case Low:
                return "Low";
            case Medium:
                return "Medium";
            case High:
                return "High";
        }
        return "";
    }

   protected:
    virtual void ReadQuery(const QSqlQuery& query);

   private:
    int id_ = -1;
    Difficulty difficulty_ = Low;
    Completion completion_ = NotDone;
    QString task_;
    std::optional<QString> hint_;
};

using TaskResult = std::pair<bool, int>;

class TranslationTask : public Task {
   public:
    TranslationTask() = default;
    TranslationTask(
        int id, Difficulty difficulty, Completion completion, const QString& task, QString answer,
        const std::optional<QString>& hint = {});
    TranslationTask(
        int id, uint8_t difficulty, uint8_t completion, const QString& task, QString answer,
        const std::optional<QString>& hint = {});
    TranslationTask(
        Difficulty difficulty, Completion completion, const QString& task, QString answer,
        const std::optional<QString>& hint = {});
    TranslationTask(
        uint8_t difficulty, uint8_t completion, const QString& task, QString answer,
        const std::optional<QString>& hint = {});

    [[nodiscard]] const QString& GetAnswer() const {
        return answer_;
    }

    [[nodiscard]] Type GetType() const override {
        return Translation;
    }

    void BindToQuery(QSqlQuery* query) const override;

    [[nodiscard]] TaskResult GetScore(const QString& actual_answer) const;

   private:
    QString answer_;

    void ReadQuery(const QSqlQuery& query) override;
};

class GrammarTask : public Task {
   public:
    GrammarTask() = default;
    GrammarTask(
        int id, Difficulty difficulty, Completion completion, const QString& task,
        QStringList options, int answer, const std::optional<QString>& hint = {});
    GrammarTask(
        int id, uint8_t difficulty, uint8_t completion, const QString& task, QStringList options,
        int answer, const std::optional<QString>& hint = {});
    GrammarTask(
        Difficulty difficulty, Completion completion, const QString& task, QStringList options,
        int answer, const std::optional<QString>& hint = {});
    GrammarTask(
        uint8_t difficulty, uint8_t completion, const QString& task, QStringList options,
        int answer, const std::optional<QString>& hint = {});

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
    QStringList options_;
    int answer_ = -1;

    void ReadQuery(const QSqlQuery& query) override;
};

std::ostream& operator<<(std::ostream& os, const Task& task);
#pragma once

#include "entities.h"

#include <QObject>
#include <QSettings>

constexpr auto kSettingsName = "settings.ini";

class Settings : public QObject {
    Q_OBJECT
   public:
    static Settings& GetInstance() {
        static Settings settings;
        return settings;
    }

    void SaveScore(int score);
    [[nodiscard]] int GetScore() const;
    void AddScore(int score);

    void SaveDifficulty(Task::Difficulty difficulty);
    [[nodiscard]] Task::Difficulty GetDifficulty() const;

    void InvokeSignals() const {
        emit ScoreChanged(GetScore());
        emit DifficultyChanged(GetDifficulty());
    }

   signals:
    void ScoreChanged(int score) const;
    void DifficultyChanged(Task::Difficulty difficulty) const;

   private:
    QSettings setttings_{kSettingsName, QSettings::IniFormat};

    Settings() = default;
};
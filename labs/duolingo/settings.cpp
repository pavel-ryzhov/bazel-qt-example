#include "settings.h"
#include "entities.h"

constexpr auto kScoreKey = "score";
constexpr auto kDifficultyKey = "difficulty";

void Settings::SaveScore(int score) {
    setttings_.setValue(kScoreKey, score);
    emit ScoreChanged(score);
}

int Settings::GetScore() const {
    return setttings_.value(kScoreKey, 0).toInt();
}

void Settings::AddScore(int score) {
    const auto value = setttings_.value(kScoreKey, 0).toInt() + score;
    setttings_.setValue(kScoreKey, value);
    emit ScoreChanged(value);
}

void Settings::SaveDifficulty(Task::Difficulty difficulty) {
    setttings_.setValue(kDifficultyKey, difficulty);
    emit DifficultyChanged(difficulty);
}

Task::Difficulty Settings::GetDifficulty() const {
    return static_cast<Task::Difficulty>(setttings_.value(kDifficultyKey, Task::Difficulty::Low).toUInt());
}
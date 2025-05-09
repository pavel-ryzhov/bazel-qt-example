#include "main_window.h"
// #include "database.h"
// #include "entities.h"

#include  <QApplication>
// #include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow main_window;
    main_window.show();
    // auto& db = Database::GetInstance();
    // db.InsertTask(std::make_unique<TranslationTask>(Task::Low, Task::NotDone, "task1", "aaaa"));
    // db.InsertTask(std::make_unique<TranslationTask>(Task::High, Task::NotDone, "task2", "aaaa"));
    // db.InsertTask(std::make_unique<GrammarTask>(Task::High, Task::NotDone, "task3", QStringList{"o1"}, 0));
    // db.InsertTask(std::make_unique<GrammarTask>(Task::High, Task::NotDone, "task4", QStringList{"o1", "o2"}, 1));
    // db.InsertTask(std::make_unique<TranslationTask>(Task::High, Task::WithMistake, "task5", "aaaa"));
    // db.InsertTask(std::make_unique<TranslationTask>(Task::High, Task::WithMistake, "task6", "aaaa"));
    // const auto& data = db.SelectRandomTasksByDifficultyAndCompletion(Task::High, Task::NotDone);
    // std::cout << *data.front() << std::endl;
    return QApplication::exec();
}
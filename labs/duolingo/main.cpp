#include "database.h"
#include "inserts.h"
#include "main_window.h"

#include <QApplication>
#include <iterator>
#include <string>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    if (argc > 1) {
        const std::string arg = *std::next(argv);
        if (arg == "init") {
            Insert();
        } else if (arg == "reset") {
            Database::GetInstance().ResetToNotDone();
        }
    }
    MainWindow main_window;
    main_window.show();
    return QApplication::exec();
}
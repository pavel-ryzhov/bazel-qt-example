#include "main_window.h"

#include <QtWidgets>

//NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)
MainWindow::MainWindow() {
    auto* widget = new QWidget();
    setCentralWidget(widget);
    setWindowTitle("BASICS");


    setMinimumSize(kDefaultWidth / 2, kDefaultHeight / 2);
    resize(kDefaultWidth, kDefaultHeight);
}


//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
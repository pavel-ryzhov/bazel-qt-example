#include "main_window.h"
#include "controller.h"
#include "main_widget.h"

#include <QtWidgets>

//NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)
MainWindow::MainWindow(): mode_combo_box_(new QComboBox()), main_widget_(new MainWidget()) {
    auto* widget = new QWidget();
    setCentralWidget(widget);
    setWindowTitle("RAYCASTER");

    mode_combo_box_->addItems({"Light", "Polygons"});
    mode_combo_box_->setCurrentIndex(Controller::Mode::Polygons);

    connect(mode_combo_box_, &QComboBox::currentIndexChanged, this, &MainWindow::ModeChanged);

    auto* v_layout = new QVBoxLayout();

    v_layout->addWidget(mode_combo_box_);
    v_layout->addWidget(main_widget_);

    widget->setLayout(v_layout);

    setMinimumSize(kDefaultWidth / 2, kDefaultHeight / 2);
    resize(kDefaultWidth, kDefaultHeight);
}

void MainWindow::ModeChanged(int mode) {
    main_widget_->SetMode(static_cast<Controller::Mode>(mode));
}
//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
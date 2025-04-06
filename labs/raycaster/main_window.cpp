#include "main_window.h"
#include "controller.h"
#include "main_widget.h"

#include <QtWidgets>

//NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)
MainWindow::MainWindow(): mode_combo_box_(new QComboBox()), refresh_button_(new QPushButton("Refresh")), main_widget_(new MainWidget()) {
    auto* widget = new QWidget();
    setCentralWidget(widget);
    setWindowTitle("RAYCASTER");

    mode_combo_box_->addItems({"Light", "Polygons"});
    mode_combo_box_->setCurrentIndex(Controller::Mode::Polygons);

    connect(mode_combo_box_, &QComboBox::currentIndexChanged, this, &MainWindow::ModeChanged);
    connect(refresh_button_, &QPushButton::clicked, main_widget_, &MainWidget::Refresh);
    connect(refresh_button_, &QPushButton::clicked, this, &MainWindow::RefreshClicked);

    auto* v_layout = new QVBoxLayout();
    auto* h_layout = new QHBoxLayout();

    h_layout->addWidget(mode_combo_box_, 1);
    h_layout->addWidget(refresh_button_);
    v_layout->addItem(h_layout);
    v_layout->addWidget(main_widget_);

    widget->setLayout(v_layout);

    setMinimumSize(kDefaultWidth / 2, kDefaultHeight / 2);
    resize(kDefaultWidth, kDefaultHeight);
}

void MainWindow::ModeChanged(int mode) {
    main_widget_->SetMode(static_cast<Controller::Mode>(mode));
}

void MainWindow::RefreshClicked() {
    mode_combo_box_->setCurrentIndex(Controller::Mode::Polygons);
}
//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
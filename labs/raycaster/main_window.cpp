#include "main_window.h"

#include "controller.h"
#include "main_widget.h"
#include "utils.h"

#include <QtWidgets>

// NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)
MainWindow::MainWindow()
    : mode_combo_box_(new QComboBox())
    , refresh_button_(new QPushButton("Refresh"))
    , main_widget_(new MainWidget()) {
    auto* widget = new QWidget();
    setCentralWidget(widget);
    setWindowTitle("RAYCASTER");

    mode_combo_box_->addItems({"Light", "Polygons", "Static Lights"});
    mode_combo_box_->setCurrentIndex(Controller::Mode::Polygons);
    mode_combo_box_->setFocusPolicy(Qt::NoFocus);

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

    setMinimumSize(kDefaultWindowWidth / 2, kDefaultWindowHeight / 2);
    setGeometry(
        (screen()->size().width() - kDefaultWindowWidth) / 2,
        (screen()->size().height() - kDefaultWindowHeight) / 2, kDefaultWindowWidth,
        kDefaultWindowHeight);
}

void MainWindow::resizeEvent(QResizeEvent* /*event*/) {
    main_widget_->Resize();
}

void MainWindow::ModeChanged(int mode) {
    main_widget_->SetMode(static_cast<Controller::Mode>(mode));
}

void MainWindow::RefreshClicked() {
    mode_combo_box_->setCurrentIndex(Controller::Mode::Polygons);
}

// NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
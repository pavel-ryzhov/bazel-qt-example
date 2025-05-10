#include "main_window.h"

#include "settings.h"
#include "tasks_widget.h"

#include <QtWidgets>

constexpr auto kMainStyle = R"(
        QWidget {
            background-color: #282c34;
            color: #ffffff;
        }
        QMenuBar {
            background-color: #21252b;
            color: #ffffff;
            padding: 4px 0px;
        }
        QMenuBar::item {
            background: transparent;
            padding: 5px 5px;
        }
        QMenuBar::item:selected {
            background: #3a3f4b;
        }
        QMenu {
            background-color: #282c34;
            color: #ffffff;
            border: 1px solid #3a3f4b;
        }
        QMenu::item:selected {
            background: #3a3f4b;
        }
        QTextEdit {
            background: #1e2127;
            color: #ffffff;
            border: none;
            padding: 8px;
        }
    )";
constexpr auto kLargeTextStyle = R"(
    font-size: 16pt;
    font-weight: bold;
)";

// NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)
MainWindow::MainWindow()
    : stacked_layout_(new QStackedLayout())
    , tasks_widget_(new TasksWidget())
    , score_label_(new QLabel())
    , menu_score_label_(new QLabel())
    , difficulty_label_(new QLabel()) {
    auto* widget = new QWidget();
    setCentralWidget(widget);
    setWindowTitle("DUOLINGO");

    setStyleSheet(kMainStyle);
    score_label_->setStyleSheet(kLargeTextStyle);
    difficulty_label_->setStyleSheet(kLargeTextStyle);

    menu_score_label_->setStyleSheet("background: #21252b; padding-right: 10px");
    auto* menu_bar = new QMenuBar();
    auto* menu_container = new QWidget();
    auto* menu_layout = new QHBoxLayout(menu_container);
    auto* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer->setStyleSheet("background-color: #21252b;");
    difficulty_menu_ = menu_bar->addMenu("");
    menu_layout->setContentsMargins(0, 0, 0, 0);
    menu_layout->setSpacing(0);
    menu_layout->addWidget(menu_bar);
    menu_layout->addWidget(spacer);
    menu_layout->addWidget(menu_score_label_);
    setMenuWidget(menu_container);

    // auto* corner_container = new QWidget();
    // auto* corner_layout = new QVBoxLayout();
    // corner_layout->setContentsMargins(0, 0, 0, 0);
    // corner_layout->addStretch();
    // corner_layout->addWidget(menu_score_label_);
    // corner_layout->addStretch();
    // corner_container->setLayout(corner_layout);

    // menu_score_label_->setContentsMargins(0, 0, 10, 0);
    // menu_bar->setCornerWidget(corner_container, Qt::TopRightCorner);
    // setMenuBar(menu_bar);

    auto* button_translation = new QPushButton("Перевод");
    auto* button_grammar = new QPushButton("Грамматика");
    auto* button_mixed = new QPushButton("Микс");
    auto* button_mistakes = new QPushButton("Ошибки");

    button_translation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button_grammar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button_mixed->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    button_mistakes->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    int max_width = std::max(
                        {button_translation->sizeHint().width(), button_grammar->sizeHint().width(),
                         button_mixed->sizeHint().width(), button_mistakes->sizeHint().width()}) +
                    50;

    button_translation->setFixedWidth(max_width);
    button_grammar->setFixedWidth(max_width);
    button_mixed->setFixedWidth(max_width);
    button_mistakes->setFixedWidth(max_width);

    connect(&Settings::GetInstance(), &Settings::ScoreChanged, this, [this](int score) {
        QString str{("Рейтинг: " + std::to_string(score)).c_str()};
        score_label_->setText(str);
        menu_score_label_->setText(str);
    });
    connect(
        &Settings::GetInstance(), &Settings::DifficultyChanged, this,
        [this](Task::Difficulty difficulty) {
            const auto str = "Сложность: " + Task::GetDifficultyText(difficulty);
            difficulty_menu_->setTitle(str);
            difficulty_label_->setText(str);
        });
    Settings::GetInstance().InvokeSignals();

    auto* v_layout = new QVBoxLayout();
    auto* h_layout = new QHBoxLayout();

    h_layout->addWidget(difficulty_label_);
    h_layout->addStretch();
    h_layout->addWidget(score_label_);

    v_layout->addItem(h_layout);
    v_layout->addStretch();

    v_layout->addWidget(button_translation, 0, Qt::AlignHCenter);
    v_layout->addWidget(button_grammar, 0, Qt::AlignHCenter);
    v_layout->addWidget(button_mixed, 0, Qt::AlignHCenter);
    v_layout->addWidget(button_mistakes, 0, Qt::AlignHCenter);
    v_layout->addStretch();

    auto* layout_container = new QWidget();
    layout_container->setLayout(v_layout);

    stacked_layout_->addWidget(layout_container);
    stacked_layout_->addWidget(tasks_widget_);

    widget->setLayout(stacked_layout_);

    setMinimumSize(kDefaultWidth / 2, kDefaultHeight / 2);
    resize(kDefaultWidth, kDefaultHeight);
}

// NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
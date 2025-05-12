#include "difficulty_dialog.h"

#include "settings.h"

#include <QtWidgets>

// NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)

constexpr auto kLargeTextStyle = "font-size: 16pt;";

DifficultyDialog::DifficultyDialog(QWidget* parent)
    : QDialog(parent), button_group_(new QButtonGroup()), settings_(Settings::GetInstance()) {
    auto* v_layout = new QVBoxLayout();

    auto* button_low = new QRadioButton(Task::GetDifficultyText(Task::Low));
    auto* button_medium = new QRadioButton(Task::GetDifficultyText(Task::Medium));
    auto* button_high = new QRadioButton(Task::GetDifficultyText(Task::High));

    auto* button_submit = new QPushButton("Подтвердить");

    button_low->setStyleSheet(kLargeTextStyle);
    button_medium->setStyleSheet(kLargeTextStyle);
    button_high->setStyleSheet(kLargeTextStyle);
    button_submit->setStyleSheet("font-size: " + QString::number(QApplication::font().pointSize()) + "pt;");

    connect(button_submit, &QPushButton::clicked, this, [this]{
        settings_.SaveDifficulty(static_cast<Task::Difficulty>(button_group_->checkedId()));
        accept();
    });

    button_group_->addButton(button_low, Task::Low);
    button_group_->addButton(button_medium, Task::Medium);
    button_group_->addButton(button_high, Task::High);

    button_group_->button(settings_.GetDifficulty())->setChecked(true);

    button_submit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    auto* v1_container = new QWidget();
    auto* v1_layout = new QVBoxLayout();

    v1_layout->addWidget(button_low);
    v1_layout->addWidget(button_medium);
    v1_layout->addWidget(button_high);
    v1_container->setLayout(v1_layout);

    v_layout->addWidget(v1_container, 0, Qt::AlignHCenter);
    v_layout->addStretch();
    v_layout->addWidget(button_submit, 0, Qt::AlignHCenter);

    setLayout(v_layout);
    setWindowTitle("Изменить сложность");
    resize(300, 200);
}

// NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)
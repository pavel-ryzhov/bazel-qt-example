#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QButtonGroup;
class Settings;
QT_END_NAMESPACE

class DifficultyDialog : public QDialog {
    Q_OBJECT
   public:
    explicit DifficultyDialog(QWidget* parent = nullptr);

   private:
    QButtonGroup* button_group_;

    Settings& settings_;
};
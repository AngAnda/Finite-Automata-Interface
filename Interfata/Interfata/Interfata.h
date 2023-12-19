#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Interfata.h"

class Interfata : public QMainWindow
{
    Q_OBJECT

public:
    Interfata(QWidget *parent = nullptr);
    ~Interfata();

private:
    Ui::InterfataClass ui;
};

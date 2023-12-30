#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Interfata.h"
#include "FiniteAutomaton.h"

class Interfata : public QMainWindow
{
    Q_OBJECT

public:
    Interfata(QWidget *parent = nullptr);
    ~Interfata();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override; 

private:
    Ui::InterfataClass ui;
    IAutomaton* m_automaton;
    float m_radius = 40.00;
};

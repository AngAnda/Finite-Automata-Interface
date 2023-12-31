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
    void mousePressEvent(QMouseEvent* event) override; 

private slots:
    void HandleStateManager1(bool checked);
    void HandleStateManager2(bool checked);
    void HandleStateManager3(bool checked);
    void HandleStateManager4(bool checked);


private:  
    void OpenInNotepad(const QString& filePath);
    void DrawArrow(QPainter& painter, const Transition* transition);
    enum ButtonRightAction{
        AddingState,
        AddingFinalState,
        AddingStartingState,
        DeleteState
    };

    std::optional<int> CheckUpdatePosition(QPoint position);

private:
    Ui::InterfataClass ui;
    IAutomaton* m_automaton;
    float m_radius = 40.00;
    const QString m_automatonFile = "./Automaton.txt";
    const QString m_wordsFile = "./Words.txt";
    ButtonRightAction m_currentAction;
    std::optional<int> m_stateMoving;
    std::pair<std::optional<State*>, std::optional<State*>> m_newTransitions;
};

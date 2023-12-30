#include "Interfata.h"
#include <QMouseEvent> 
#include <QPainter>
#include <qpoint.h>
#include <qprocess.h>


Interfata::Interfata(QWidget* parent)
    : QMainWindow(parent),
    m_currentAction(ButtonRightAction::AddingState)
{
    ui.setupUi(this);
    m_automaton = new FiniteAutomaton();
    QObject::connect(ui.stateManager1, &QRadioButton::toggled, this, &Interfata::HandleStateManager1);
    QObject::connect(ui.stateManager2, &QRadioButton::toggled, this, &Interfata::HandleStateManager2);
    QObject::connect(ui.stateManager3, &QRadioButton::toggled, this, &Interfata::HandleStateManager3);

}

Interfata::~Interfata()
{}

void Interfata::paintEvent(QPaintEvent* event)
{
    std::vector<State*> states = m_automaton->GetStates();
    QPainter p(this);

    p.setBrush(QBrush(Qt::white));
    for (auto& state : states) {
        auto [x, y] = state->GetCoordinate();
        

        if (state->GetStateType() == StateType::start || state->GetStateType() == StateType::start_finish) {
            p.drawLine(QPoint(x- 0.75 * m_radius, y- 0.75 * m_radius ), state->GetCoordinate());
            p.drawText(QPoint(x- 0.75 * m_radius + 5, y- 0.75 * m_radius), "Start");
        }
        
        if (state->GetStateType() == StateType::finish || state->GetStateType() == StateType::start_finish) {
            QRect r2(x - m_radius / 2 - m_radius / 8, y - m_radius / 2 - m_radius / 8, m_radius + m_radius / 4, m_radius + m_radius / 4);
            p.drawEllipse(r2);
        }

        QRect r(x - m_radius / 2, y - m_radius / 2, m_radius, m_radius);
        p.drawEllipse(r);
        p.drawText(r, Qt::AlignCenter, state->GetName());
        p.setPen(QPen(Qt::black));
    }

}

void Interfata::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) 
    {
        // adauga tranzitii
    }
    else 
    { 

        std::optional<int> indexExistingNode = CheckUpdatePosition(event->pos());

        switch (m_currentAction) {
        case ButtonRightAction::AddingState:
            if (m_stateMoving.has_value())
                m_automaton->UpdateCoordinate(event->pos(), m_stateMoving.value());
            else if(!indexExistingNode.has_value())
                m_automaton->AddState(event->pos());
            break;
        case ButtonRightAction::AddingStartingState:
            if (indexExistingNode.has_value())
                m_automaton->SetState(StateType::start, indexExistingNode.value());
            break;
        case ButtonRightAction::AddingFinalState:
            if (indexExistingNode.has_value())
                m_automaton->SetState(StateType::finish, indexExistingNode.value());
            break;
        case ButtonRightAction::DeleteState:
            // stergere de state
            break;
        }
    }

    update();
}

void Interfata::mousePressEvent(QMouseEvent* event)
{
    m_stateMoving = CheckUpdatePosition(event->pos());
}

void Interfata::HandleStateManager1(bool checked) 
{
    m_currentAction = ButtonRightAction::AddingState;
}

void Interfata::HandleStateManager2(bool checked)
{
    m_currentAction = ButtonRightAction::AddingFinalState;
}

void Interfata::HandleStateManager3(bool checked)
{
    m_currentAction = ButtonRightAction::AddingStartingState;
}

void Interfata::OpenInNotepad(const QString& filePath)
{
    QProcess::startDetached("notepad.exe", QStringList() << filePath);
}

std::optional<int> Interfata::CheckUpdatePosition(QPoint position)
{

    auto& [px, py] = position;
    // returns an index for an index of m_statesUi
    for (auto& state : m_automaton->GetStates()) {
        auto [sx, sy] = state->GetCoordinate();
        if (std::abs(sx - px) < m_radius && std::abs(sy - py) < m_radius)
            return state->GetIndex();
    }
    return std::nullopt;
}

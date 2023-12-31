#include "Interfata.h"
#include <QMouseEvent> 
#include <QPainter>
#include <qpoint.h>
#include <qprocess.h>
#include <QInputDialog>
#include <qdir.h>
#include <qpainterpath.h>

Interfata::Interfata(QWidget* parent)
    : QMainWindow(parent),
    m_currentAction(ButtonRightAction::AddingState),
    m_newTransitions({std::nullopt, std::nullopt})
{
    ui.setupUi(this);
    m_automaton = new FiniteAutomaton();
    QObject::connect(ui.stateManager1, &QRadioButton::toggled, this, &Interfata::HandleStateManager1);
    QObject::connect(ui.stateManager2, &QRadioButton::toggled, this, &Interfata::HandleStateManager2);
    QObject::connect(ui.stateManager3, &QRadioButton::toggled, this, &Interfata::HandleStateManager3);
    QObject::connect(ui.stateManager4, &QRadioButton::toggled, this, &Interfata::HandleStateManager4);

}

Interfata::~Interfata()
{}

void Interfata::paintEvent(QPaintEvent* event)
{
    std::vector<State*> states = m_automaton->GetStatesUi();
    std::vector<Transition*> transitions = m_automaton->GetTransitionsUi();

    if (states.empty() && transitions.empty())
        return;

    QPainter p(this);
    p.setBrush(QBrush(Qt::black));


    for (const auto& trans : transitions) {
       
        DrawArrow(p, trans);
    }
    

    p.setBrush(QBrush(Qt::white));

    for (const auto& state : states) {

        const auto& [x, y] = state->GetCoordinate();        

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
        if (m_newTransitions.first.has_value() && m_newTransitions.second.has_value()) {
           
            QString value = QInputDialog::getText(nullptr, "Add a transition", "Enter your transition:", QLineEdit::Normal,
                QDir::home().dirName());
            // de cautat daca putem folosi regex pentru a verifica expresii
            if(m_newTransitions.first.value() != m_newTransitions.second.value())
                m_automaton->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, TransitionType::base);
            else
                m_automaton->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, TransitionType::base);
        }
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
            if (indexExistingNode.has_value())
                m_automaton->DeleteState(indexExistingNode.value());
            break;
        }
    }

    update();
}

void Interfata::mousePressEvent(QMouseEvent* event)
{

    auto stateValue = CheckUpdatePosition(event->pos());
    if (!stateValue.has_value()) return;

    if (event->button() == Qt::RightButton) {
        if (!m_newTransitions.first.has_value() || m_newTransitions.second.has_value() ) {
            m_newTransitions.second= std::nullopt;
            m_newTransitions.first = m_automaton->GetStatesUi().at(stateValue.value());
        }
        else
        {
            m_newTransitions.second = m_automaton->GetStatesUi().at(stateValue.value());
        }
    }
    else {
        m_stateMoving = CheckUpdatePosition(event->pos());
    }

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

void Interfata::HandleStateManager4 (bool checked)
{
    m_currentAction = ButtonRightAction::DeleteState;
}

void Interfata::OpenInNotepad(const QString& filePath)
{
    QProcess::startDetached("notepad.exe", QStringList() << filePath);
}

std::optional<int> Interfata::CheckUpdatePosition(QPoint position)
{

    auto& [px, py] = position;
    // returns an index for an index of m_statesUi
    auto states = m_automaton->GetStatesUi();

    for (auto& state : states) {
        auto [sx, sy] = state->GetCoordinate();
        if (std::abs(sx - px) < m_radius && std::abs(sy - py) < m_radius) {
            return state->GetIndex();
        }
            
    }
    return std::nullopt;
}

void Interfata::DrawArrow(QPainter& painter, const Transition* transition) {
   
    const auto& [startCenter, endCenter] = transition->GetCoord();
    QString label = transition->GetValue();
    
    if (transition->GetType() == TransitionType::base) {
        double angle = std::atan2((endCenter.y() - startCenter.y()), (endCenter.x() - startCenter.x()));

        const double arrowSize = 10;

        QPoint startBorder = startCenter + QPoint(cos(angle) * m_radius/2, sin(angle) * m_radius/2);
        QPoint endBorder = endCenter - QPoint(cos(angle) * (m_radius/2 + arrowSize), sin(angle) * (m_radius/2 + arrowSize));

        painter.drawLine(startBorder, endBorder);

        QPolygon arrowHead;
        arrowHead << endCenter - QPoint(cos(angle) * m_radius/2, sin(angle) * m_radius/2)
            << endCenter - QPoint(cos(angle + M_PI / 6) * arrowSize + cos(angle) * m_radius/2,
                sin(angle + M_PI / 6) * arrowSize + sin(angle) * m_radius/2)
            << endCenter - QPoint(cos(angle - M_PI / 6) * arrowSize + cos(angle) * m_radius/2,
                sin(angle - M_PI / 6) * arrowSize + sin(angle) * m_radius/2);
        painter.drawPolygon(arrowHead);


        QFont originalFont = painter.font();
        QFont font = painter.font();
        font.setPointSize(12); // Set the point size to 12 (or any other value you prefer)
        font.setBold(true);
        painter.setFont(font);
        QRect textRect((startBorder + endBorder) / 2 - QPoint(50, 25), QSize(100, 50));
        painter.drawText(textRect, Qt::AlignCenter, label);
        painter.setFont(originalFont);
    }
    else if(transition->GetType() == TransitionType::self_pointing)
    {
        painter.setRenderHint(QPainter::Antialiasing);

        // Calculate the center of the circle
        QPointF center = startCenter + QPointF(m_radius, m_radius);

        // Define the loop's start and end angles and radius
        qreal startAngle = 60; // Starting angle in degrees
        qreal spanAngle = -120; // Span angle in degrees, negative for clockwise
        qreal loopRadius = m_radius + 10; // Loop radius (should be greater than the circle's radius)

        // Draw the loop as an arc
        QRectF arcRect(center.x() - loopRadius, center.y() - loopRadius - m_radius,
            loopRadius * 2, loopRadius * 2);
        painter.drawArc(arcRect, startAngle * 16, spanAngle * 16); // Qt expects angles in 1/16th of a degree

        // Draw the arrowhead at the end of the arc
        // Calculate the end position and angle for the arrowhead
        qreal endAngleRad = qDegreesToRadians(static_cast<double>(startAngle + spanAngle));
        QPointF arrowTip = center + QPointF(loopRadius * cos(endAngleRad), -loopRadius * sin(endAngleRad));
        QPointF arrowP1 = arrowTip + QPointF(sin(endAngleRad) * 5, cos(endAngleRad) * 5);
        QPointF arrowP2 = arrowTip - QPointF(sin(endAngleRad) * 5, -cos(endAngleRad) * 5);

        QPainterPath arrowHead;
        arrowHead.moveTo(arrowTip);
        arrowHead.lineTo(arrowP1);
        arrowHead.lineTo(arrowP2);
        arrowHead.closeSubpath();
        painter.fillPath(arrowHead, Qt::black);

        // Draw the label above the loop
        QFont font = painter.font();
        font.setPointSize(10); // Set the font size
        painter.setFont(font);
        QFontMetricsF metrics(font);
        QRectF textRect = metrics.boundingRect(label);
        textRect.moveCenter(QPointF(center.x(), center.y() - loopRadius - m_radius - 10)); // Adjust the 10 if needed
        painter.drawText(textRect, Qt::AlignCenter, label);
    }


}
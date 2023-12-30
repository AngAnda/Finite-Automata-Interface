#include "Interfata.h"
#include <QMouseEvent> 
#include <QPainter>
#include <qpoint.h>


Interfata::Interfata(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_automaton = new FiniteAutomaton();
}

Interfata::~Interfata()
{}

void Interfata::paintEvent(QPaintEvent * event)
{
    std::vector<State*> states = m_automaton->GetStates();
    QPainter p(this);

    p.setBrush(QBrush(Qt::white));
    /*for (auto& a : arches) {
        if (Graph::s_oriented)
            DrawArrow(p, nodes[a.getFirst()].getCoord(), nodes[a.getSecond()].getCoord());
        else {
            p.drawLine(nodes[a.getFirst()].getCoord(), nodes[a.getSecond()].getCoord());
            QPointF middle((nodes[a.getFirst()].getCoord().x() + nodes[a.getSecond()].getCoord().x()) / 2,
                (nodes[a.getFirst()].getCoord().y() + nodes[a.getSecond()].getCoord().y()) / 2);

            QRect r(middle.x(), middle.y(), Node::k_radius, Node::k_radius);
            p.drawText(r, Qt::AlignCenter, QString::number(a.getWeight()));
        }
    }*/

    p.setBrush(QBrush(Qt::white));
    for (auto& state : states) {
        //if (firstNode.getValue() == n.getValue())
         //   p.setPen(QPen(Qt::red));
        auto [x, y] = state->GetCoordinate();
        QRect r(x -m_radius / 2, y - m_radius / 2, m_radius, m_radius);
        p.drawEllipse(r);
        p.drawText(r, Qt::AlignCenter, state->GetName());
        p.setPen(QPen(Qt::black));
    }

}

void Interfata::mouseReleaseEvent(QMouseEvent* event)
{
    m_automaton->AddState(event->pos());
    update();
}

#include "Transition.h"

Transition::Transition(State* stateFrom, State* stateTo, QString value, TransitionType type):
	m_stateFrom(stateFrom),
	m_stateTo(stateTo),
	m_value(value),
	m_type(type)
{
}

QString Transition::GetValue() const
{
	return m_value;
}

TransitionType Transition::GetType() const
{
	return m_type;
}

std::pair<QPoint, QPoint> Transition::GetCoord() const
{
	return { m_stateFrom->GetCoordinate(), m_stateTo->GetCoordinate() };
}

Transition::~Transition()
{
	delete m_stateFrom;
	delete m_stateTo;
}

#include "Transition.h"

Transition::Transition(State* stateFrom, State* stateTo, QString value, TransitionType type):
	m_stateFrom(stateFrom),
	m_stateTo(stateTo),
	m_value({ value }),
	m_type(type)
{
}

QString Transition::GetValue() const
{
	if (m_value.size() == 1)
		return m_value[0];
	QString result;
	for (int i = 0; i < m_value.size(); i++) {
		result.append(m_value[i]);
		if (i != m_value.size() - 1)
			result.append(", ");
	}
	return result;
}

void Transition::Update(QString value)
{
	if(std::find(m_value.begin(), m_value.end(), value) == m_value.end())
		m_value.emplace_back(value);
}

TransitionType Transition::GetType() const
{
	return m_type;
}

std::pair<QPoint, QPoint> Transition::GetCoord() const
{
	return { m_stateFrom->GetCoordinate(), m_stateTo->GetCoordinate() };
}

bool Transition::existingTransition(State* stateFrom, State* stateTo)
{
	return (this->m_stateTo->GetIndex() == stateTo->GetIndex() && this->m_stateFrom->GetIndex() == stateFrom->GetIndex());
}

bool Transition::HasStateOfValue(int value)
{
	return (m_stateFrom->GetIndex() == value || m_stateTo->GetIndex() == value);
}

Transition::~Transition()
{
	delete m_stateFrom;
	delete m_stateTo;
}

#pragma once
#include "State.h"
#include "TransitionType.h"

class Transition
{
public:
	Transition() = default;

	Transition(State* stateFrom, State* stateTo, QString value, TransitionType type);
	
	QString GetValue() const;

	TransitionType GetType() const;

	std::pair<QPoint, QPoint> GetCoord() const;

	~Transition();

private:
	State* m_stateFrom, *m_stateTo;
	QString m_value;
	TransitionType m_type;
	bool m_isLambda;
};


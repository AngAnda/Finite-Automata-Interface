#pragma once
#include "State.h"
#include "TransitionType.h"

class Transition
{
public:
	Transition() = default;

	Transition(State* stateFrom, State* stateTo, QString value, TransitionType type);
	
	QString GetValue() const;

	void Update(QString);

	TransitionType GetType() const;

	std::pair<QPoint, QPoint> GetCoord() const;

	bool existingTransition(State* stateFrom, State* stateTo);

	bool HasStateOfValue(int value);

	~Transition();

private:
	State* m_stateFrom, *m_stateTo;
	std::vector<QString> m_value;
	TransitionType m_type;
	bool m_isLambda;
};


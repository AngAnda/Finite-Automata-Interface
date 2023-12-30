#pragma once
#include <qpoint.h>
#include <qstring.h>
#include "StateType.h"

class State
{

public:
	State() = default;
	State(QPoint p, int value);
	QPoint GetCoordinate() ;
	QString GetName() const;
	int GetIndex() const;
	StateType GetStateType() const;
	void SetState(StateType state);
	void SetCoordinate(QPoint p);

private:
	QPoint m_coord;
	int m_value;
	StateType m_type;

};


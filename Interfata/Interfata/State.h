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
	StateType GetStateType() const;

private:
	QPoint m_coord;
	int m_value;
	StateType m_type;

};


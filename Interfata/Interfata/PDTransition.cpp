#include "PDTransition.h"

PDTransition::PDTransition(State* stateFrom, State* stateTo, QString value, TransitionType type, QString stackHead, QString nextStateStackHead)

	: Transition(stateFrom, stateTo, value, type)
	, m_stackHead(stackHead)
	, m_nextStateStackHead(nextStateStackHead)
{
}

QString PDTransition::GetStackHead() const
{
	return m_stackHead;
}

QString PDTransition::GetNextStateStackHead() const
{
	return m_nextStateStackHead;
}

void PDTransition::SetStackHead(QString stackHead)
{
	m_stackHead = stackHead;
}

void PDTransition::SetNextStateStackHead(QString nextStateStackHead)
{
	m_nextStateStackHead = nextStateStackHead;
}

void PDTransition::Update(QString value, QString stackHead, QString nextStateStackHead)
{
	Transition::Update(value);
	m_stackHead = stackHead;
	m_nextStateStackHead = nextStateStackHead;
}

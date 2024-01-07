#pragma once
#include "Transition.h"
class PDTransition :
	public Transition
{
public:
	PDTransition() = default;
	PDTransition(State* stateFrom, State* stateTo, QString value, TransitionType type, QString stackHead, QString nextStateStackHead);

	~PDTransition() = default;

	QString GetStackHead() const;
	QString GetNextStateStackHead() const;

	void SetStackHead(QString stackHead);
	void SetNextStateStackHead(QString nextStateStackHead);

	void Update(QString value, QString stackHead, QString nextStateStackHead);


private:
	QString m_stackHead;
	QString m_nextStateStackHead;
};


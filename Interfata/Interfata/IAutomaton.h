#pragma once
#include <vector>
#include <iostream>
#include <QTextStream>
#include "State.h"
#include "Transition.h"

class IAutomaton
{

public:

	virtual std::vector<State*> GetStatesUi() = 0 ;

	virtual void AddState(QPoint) = 0;

	virtual void SetState(StateType state, int index) = 0;

	virtual void DeleteState(int index) = 0;

	virtual void UpdateCoordinate(QPoint, int) = 0;

	virtual bool CheckWord(const std::string& word) = 0;

	virtual void PrintAutomaton(std::ostream& out) = 0;

	virtual void ReadAutomaton(std::istream& is) = 0;

	virtual bool IsDeterministic()=0;

	virtual bool IsValid() const = 0;

	virtual State* getStateByKey(int index) = 0;
	
	virtual std::string GetLambda() const = 0;

	virtual void reset() = 0;

	virtual std::vector<std::vector<std::pair<char, int>>> GetTransitionForWord() = 0;

};


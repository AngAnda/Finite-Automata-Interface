#pragma once
#include <vector>
#include <iostream>
#include "State.h"

class IAutomaton
{

public:

	virtual std::vector<State*> GetStates()const = 0 ;

	virtual void AddState(QPoint) = 0;

	virtual void AddTransition(QPoint) = 0;

	virtual bool CheckWord(const std::string& word) = 0;

	virtual void PrintAutomaton(std::ostream os) = 0;

	virtual void ReadAutomaton(std::istream is) = 0;

	virtual bool IsDeterministic()=0;

};


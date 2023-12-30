#pragma once
#include <vector>
#include <string>
#include <map>
#include <format>
#include <iostream>
#include "IAutomaton.h"
#include "State.h"
#include "StateType.h"

class FiniteAutomaton : public IAutomaton
{
public:
	FiniteAutomaton();
	
	FiniteAutomaton(const std::vector<char>& Q, const std::vector<char>& sigma, const std::map<std::pair<char, char>, std::vector<char>>& delta, const char& q0, const std::vector<char>& F);

	void PrintAutomaton(std::ostream os) override;

	void ReadAutomaton(std::istream is) override;

	bool CheckWord(const std::string& word) override;

	bool IsDeterministic() override;

	virtual void AddState(QPoint p) override;

	virtual std::vector<State*> GetStates() const override;

	virtual void AddTransition(QPoint p) override;

	friend std::ostream& operator<<(std::ostream& os, FiniteAutomaton& fa);

private:

	bool VerifyAutomaton(); // mai avem nevoie de ea oare?

	std::map<std::pair<char, char>, std::vector<char>> m_transitions; 
	std::vector<char> m_alphabet;
	std::vector<char> m_states; // schimbam in int
	std::vector<char> m_finalStates; 
	char m_startState; 
	char m_lambda = '*';

	// pentru afisare
	std::vector<State*> m_statesUi;

};

std::ostream& operator<<(std::ostream& os, std::vector<char> vec);

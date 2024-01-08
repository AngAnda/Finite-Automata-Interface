#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <format>
#include <iostream>
#include <sstream>
#include "IAutomaton.h"
#include "State.h"
#include "StateType.h"
#include "Transition.h"

class FiniteAutomaton : public IAutomaton
{
public:
	FiniteAutomaton();
	
	FiniteAutomaton(const std::vector<char>& Q, const std::vector<char>& sigma, const std::map<std::pair<char, char>, std::vector<char>>& delta, const char& q0, const std::vector<char>& F);

	void PrintAutomaton(std::ostream& out) override;

	void ReadAutomaton(std::istream& is) override;

	bool CheckWord(const std::string& word) override;

	bool IsDeterministic() override;
	
	friend std::ostream& operator<<(std::ostream& os, FiniteAutomaton& fa);

	virtual bool IsValid() const override;

	std::string GetLambda() const override;

	virtual void AddState(QPoint p) override;

	void DeleteState(int value) override;

	void UpdateCoordinate(QPoint p, int index) override;

	virtual std::vector<State*> GetStatesUi() override;

	virtual void SetState(StateType state, int index) override;

	std::vector<Transition*> GetTransitionsUi();

	void AddTransition(State*, State*, QString, TransitionType transition);

	std::vector<std::vector<std::pair<char, int>>> GetTransitionForWord();

	State* getStateByKey(int index);
	
	virtual void reset() override;

	void setAlphabet(std::map<std::pair<char, char>, std::vector<char>> transitions);

private:

	bool VerifyAutomaton(); 

	std::map<std::pair<char, char>, std::vector<char>> m_transitions; 
	std::vector<char> m_alphabet;
	std::vector<char> m_states; 
	std::vector<char> m_finalStates; 
	std::optional<char> m_startState; 
	char m_lambda;

	std::vector<Transition*> m_transitionsUi;
	std::map<int, State*> m_statesUi;
	std::vector<std::vector<std::pair<char, int>>> m_transitionsAnimation;
};

std::ostream& operator<<(std::ostream& os, std::vector<char> vec);

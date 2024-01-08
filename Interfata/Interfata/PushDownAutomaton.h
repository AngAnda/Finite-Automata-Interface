#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include "IAutomaton.h"
#include "State.h"
#include "StateType.h"
#include "PDTransition.h"
#include<stack>
#include<unordered_set>
#include<unordered_map>

namespace std {
	template <>
	struct hash<std::tuple<char, char, char>> {
		size_t operator()(const std::tuple<char, char, char>& tup) const {
			auto hash1 = std::hash<char>{}(std::get<0>(tup));
			auto hash2 = std::hash<char>{}(std::get<1>(tup));
			auto hash3 = std::hash<char>{}(std::get<2>(tup));
			return hash1 ^ (hash2 << 1) ^ (hash3 << 2);
		}
	};
}
using TransitionMap = std::unordered_map<std::tuple<char, char, char>, std::pair<char, std::string>>;


class PushDownAutomaton : public IAutomaton
{
public:
public:
	PushDownAutomaton() = default;
	PushDownAutomaton(const std::vector<char>& states,
		const std::vector<char>& alphabet,
		const std::stack<char>& PDMemory,
		const std::unordered_set<char>& PDMemoryAlphabet,
		const std::vector<char>& finalStates,
		char startState,
		char startPDMemory,
		const TransitionMap& transitionFunction);
	~PushDownAutomaton() = default;

	std::string GetLambda() const override;
	std::stack<char> GetPDMemory() const;

	bool IsDeterministic() override;
	virtual bool IsValid() const override;
	void ReadAutomaton(std::istream& is) override;
	friend std::ostream& operator<<(std::ostream& os, PushDownAutomaton& fa);

	void AddTransition(State* stateFrom, State* stateTo, QString value, char memoryFrom, std::string memoryTo, TransitionType transition);
	virtual void AddState(QPoint p) override;
	virtual void SetState(StateType state, int index) override;
	void DeleteState(int value) override;


	virtual std::vector<State*> GetStatesUi() override;
	std::vector<PDTransition*> GetTransitionsUi();
	std::vector<std::vector<std::pair<char, int>>> GetTransitionForWord(); 
	std::vector<std::vector<std::vector<char>>> GetStackForWord();

	State* getStateByKey(int index);

	virtual void PrintAutomaton(std::ostream& out);
	virtual bool CheckWord(const std::string& word);
	void UpdateCoordinate(QPoint p, int index) override;
	virtual void reset() override;

	void setAlphabet(TransitionMap transitions);

private:

	bool CheckWordRecursive(std::string word, int index, char currentState, std::stack<char> PDMemory);

private:

	bool VerifyAutomaton();

	std::vector<char> m_states;
	std::vector<char> m_alphabet;
	std::stack<char> m_PDMemory;
	std::unordered_set<char> m_PDMemoryAlphabet;
	std::vector<char> m_finalStates;
	std::optional<char> m_startState; 
	char m_startPDMemory ='Z';
	QPoint m_stackPosition;
	TransitionMap m_transitions;
	char m_lambda;

	std::vector<PDTransition*> m_transitionsUi;
	std::map<int, State*> m_statesUi;

	std::unordered_map<int, std::vector<std::pair<char, int>>> m_transitionsAnimation; 
	std::unordered_map<int, std::vector<std::stack<int>>> m_stackAnimation;

};

std::ostream& operator<<(std::ostream& os, std::vector<char> vec);



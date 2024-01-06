#pragma once
#include <vector>
#include <string>
#include <map>
#include <format>
#include <iostream>
#include "IAutomaton.h"
#include "State.h"
#include "StateType.h"
#include "PDTransition.h"
#include<stack>
#include<unordered_set>

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

	// Functiile specifice pentru interfata

	void AddTransition(State* stateFrom, State* stateTo, QString value, char memoryFrom, std::string memoryTo, TransitionType transition);
	virtual void AddState(QPoint p) override;
	virtual void SetState(StateType state, int index) override;
	void DeleteState(int value) override;


	virtual std::vector<State*> GetStatesUi() override;
	std::vector<PDTransition*> GetTransitionsUi();
	std::vector<std::vector<std::pair<char, int>>> GetTransitionForWord(); // de vazut ce facem la apd
	State* getStateByKey(int index);

	virtual void PrintAutomaton(std::ostream& out);
	virtual bool CheckWord(const std::string& word);
	void UpdateCoordinate(QPoint p, int index) override;
	virtual void reset() override;

	//set stack position

private:

	bool VerifyAutomaton();

	std::vector<char> m_states;
	std::vector<char> m_alphabet;
	std::stack<char> m_PDMemory;
	std::unordered_set<char> m_PDMemoryAlphabet;
	std::vector<char> m_finalStates;
	std::optional<char> m_startState; // de lucrat cu el putin
	char m_startPDMemory;
	QPoint m_stackPosition;
	TransitionMap m_transitions;
	char m_lambda;

	// pentru afisare
	std::vector<PDTransition*> m_transitionsUi;
	std::map<int, State*> m_statesUi;

	std::vector<std::vector<std::pair<char, int>>> m_transitionsAnimation; // pentru animatie de tranzitii
};

std::ostream& operator<<(std::ostream& os, std::vector<char> vec);



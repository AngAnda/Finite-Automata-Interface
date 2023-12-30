#include "FiniteAutomaton.h"
#include <algorithm>

FiniteAutomaton::FiniteAutomaton(): 
	m_startState(char(0)),
	m_lambda('*')
{
	// empty
}

FiniteAutomaton::FiniteAutomaton(const std::vector<char>& Q, const std::vector<char>& sigma, const std::map<std::pair<char, char>, std::vector<char>>& delta, const char& q0, const std::vector<char>& F) : 
	m_states(Q), 
	m_alphabet(sigma), 
	m_transitions(delta), 
	m_startState(q0), 
	m_finalStates(F),
	m_lambda('*') // de vazut partea cu lambda
{
	// empty
}

void FiniteAutomaton::PrintAutomaton(std::ostream os)
{
	// to do
}

void FiniteAutomaton::ReadAutomaton(std::istream is)
{
	// to do
}

bool FiniteAutomaton::VerifyAutomaton()
{
	for (auto& el1 : m_states) {
		if (std::find(m_alphabet.begin(), m_alphabet.end(), el1) != m_alphabet.end())
			return false;
	}

	for (auto& el1 : m_alphabet) {
		if (std::find(m_states.begin(), m_states.end(), el1) != m_states.end())
			return false;
	}

	if (std::find(m_states.begin(), m_states.end(), m_startState) == m_states.end())
		return false;

	for (auto& el : m_finalStates) {
		if (std::find(m_states.begin(), m_states.end(), el) == m_states.end())
			return false;
	}

	for (auto& el : m_transitions) {
		// de facut poate
	}

	return true;
}

bool FiniteAutomaton::CheckWord(const std::string& word)
{
	// se verifica daca automatul este sau nu valid

	std::vector<std::pair<char, int>> crtState = { { m_startState.value(), 0}}; // state and index
	std::vector<std::pair<char, int>> toCheckState;

	while (!crtState.empty()) {
		toCheckState.clear();
		for (auto& crt : crtState) {
			auto& [state, index] = crt;
			if (index < word.length()) {
				std::pair<char, char> key = { state, word[index] };


				if (m_transitions.find(key) != m_transitions.end()) { // Check if the key exists in the map
					for (auto& transitionResult : m_transitions[key]) {
						if (std::find(toCheckState.begin(), toCheckState.end(), std::make_pair(transitionResult, index + 1)) == toCheckState.end()) {
							toCheckState.emplace_back(transitionResult, index + 1);
							if (std::find(m_finalStates.begin(), m_finalStates.end(), transitionResult) != m_finalStates.end() && word.size() - 1 == index + 1)
								return true;
						}
					}
				}

				key = { state, m_lambda };
				if (m_transitions.find(key) != m_transitions.end()) { // Check if the key exists in the map
					for (auto& transitionResult : m_transitions[key]) {
						if (std::find(toCheckState.begin(), toCheckState.end(), std::make_pair(transitionResult, index)) == toCheckState.end()) {
							toCheckState.emplace_back(transitionResult, index);
							if (std::find(m_finalStates.begin(), m_finalStates.end(), transitionResult) != m_finalStates.end() && word.size() - 1 == index)
								return true;
						}
					}
				}

			}
		}

		if (toCheckState.empty())
			return false;

		crtState = std::move(toCheckState);

	}


	return false;
}

bool FiniteAutomaton::IsDeterministic()
{

	for (auto& el : m_transitions) {
		if (m_transitions.count(el.first) != 1)
			return false;
	}

	return true;
}

void FiniteAutomaton::AddState(QPoint p)
{
	m_statesUi.insert({ m_states.size(), new State(p, m_states.size()) });
	m_states.emplace_back(char(m_states.size()));
	if (m_statesUi.size() == 1) {
		m_statesUi[0]->SetState(StateType::start);
		m_startState = char(0);
	}
}

void FiniteAutomaton::DeleteState(int value)
{
	// ce se intampla cu simbolul de start
	m_statesUi.erase(m_statesUi.find(value)); // se sterge din states
	// aici este problema
	m_states.erase(std::find(m_states.begin(), m_states.end(), value));
	if(std::find(m_finalStates.begin(), m_finalStates.end(), value) != m_finalStates.end())
		m_finalStates.erase(std::find(m_finalStates.begin(), m_finalStates.end(), value));
	if (m_startState == char(value) && !m_states.empty())
		m_startState = m_states.front();
	// ce se intampla cu m_startState? 
}

void FiniteAutomaton::UpdateCoordinate(QPoint p, int index)
{
	m_statesUi[index]->SetCoordinate(p);
}

std::vector<State*> FiniteAutomaton::GetStatesUi()
{
	std::vector<State*> states;
	for (auto& val : m_statesUi) {
		states.emplace_back(val.second);
	}
	return states;
}

void FiniteAutomaton::SetState(StateType state, int index)
{

	if (state == StateType::finish) {
		if (std::find(m_finalStates.begin(), m_finalStates.end(), char(index)) == m_finalStates.end()) {
			m_finalStates.emplace_back(char(index));
			if(m_statesUi[index]->GetStateType() == StateType::start)
				m_statesUi[index]->SetState(StateType::start_finish);
			else
				m_statesUi[index]->SetState(state);
		}
		else {
			m_finalStates.erase(std::find(m_finalStates.begin(), m_finalStates.end(), index));
			if (m_statesUi[index]->GetStateType() == StateType::start_finish)
				m_statesUi[index]->SetState(StateType::start);
			else
				m_statesUi[index]->SetState(StateType::normal);
		}
		return;
	}

	if (state == StateType::start)
	{
		if(m_statesUi[m_startState.value()]->GetStateType() == StateType::start_finish)
			m_statesUi[m_startState.value()]->SetState(StateType::finish);
		else
			m_statesUi[m_startState.value()]->SetState(StateType::normal);
		
		m_startState = char(index);

		if (m_statesUi[m_startState.value()]->GetStateType() == StateType::finish)
			m_statesUi[index]->SetState(StateType::start_finish);
		else
			m_statesUi[index]->SetState(state);
	}
}

void FiniteAutomaton::AddTransition(QPoint p)
{
}


std::ostream& operator<<(std::ostream& os, FiniteAutomaton& fa)
{
	os << fa.m_states << "\n" << fa.m_alphabet << "\n" << fa.m_transitions.size() << "\n" << fa.m_startState.value() << "\n" << fa.m_finalStates << "\n";

	for (auto& el : fa.m_transitions) {
		auto& [key, value] = el;
		for (auto el2 : value) {
			std::cout << std::format("{},{}={}\n", key.first, key.second, el2);
		}
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, std::vector<char> vec)
{
	for (auto& el : vec) {
		os << el << " ";
	}
	return os;
}

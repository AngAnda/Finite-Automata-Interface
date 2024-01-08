#include "FiniteAutomaton.h"
#include <algorithm>
#include "qmessagebox.h"

FiniteAutomaton::FiniteAutomaton() :
	m_startState(char(0)),
	m_lambda('*'),
	m_statesUi({}),
	m_transitionsUi({}),
	m_transitionsAnimation({})
{
	// empty
}

FiniteAutomaton::FiniteAutomaton(const std::vector<char>& Q, const std::vector<char>& sigma, const std::map<std::pair<char, char>, std::vector<char>>& delta, const char& q0, const std::vector<char>& F) :
	m_states(Q),
	m_alphabet(sigma),
	m_transitions(delta),
	m_startState(q0),
	m_finalStates(F),
	m_lambda('*'),
	m_statesUi({})
{
	// empty
}

void FiniteAutomaton::PrintAutomaton(std::ostream& out)
{
	out << "States: ";
	for (int i = 0; i < m_states.size(); i++)
	{
		out << 'q' << int(m_states[i]) << " ";
	}
	out << "\n";

	out << "Alphabet: ";
	setAlphabet(m_transitions);
	for (auto& symbol : m_alphabet) {
		out << symbol << " ";
	}
	out << "\n";

	out << "Transitions:\n";
	for (const auto& transition : m_transitions)
	{
		auto key = transition.first;
		out << "[q" << int(key.first) << " , " << key.second << "] = ";
		for (auto& nextState : transition.second)
		{
			out << 'q' << int(nextState) << " ";
		}
		out<<"\n";
	}

	if (m_startState.has_value()) {
		out << "Start state: q" << int(m_startState.value()) << "\n";
	}

	out << "Final states: ";
	for (auto& finalState : m_finalStates) {
		out << 'q' << int(finalState) << " ";
	}
	out << "\n";

	out.flush();
}


void FiniteAutomaton::ReadAutomaton(std::istream& is) {
	std::string line;
	char state;
	char input;
	char targetState;
	std::string startState;
	std::string finalStates;

	reset();

	while (std::getline(is, line)) 
	{
		std::istringstream iss(line);
		std::string word;
		iss >> word;

		if (word == "States:") {
			while (iss >> word && word != "Alphabet:") {
				if (word[0] == 'q') 
				{
					state = word[1] - '0';
					QPoint defaultPosition(200 + 50 * int(state), 200);
					AddState(defaultPosition);
				}
			}
		}
		else if (word == "Alphabet:") {
			while (iss >> input && input != 'T') {
				m_alphabet.push_back(input);
			}
		}
		else if (word == "Transitions:") {
			continue;
		}
		else if (word[0] == '[') {
			state = word[2] - '0';
			iss >> input;
			iss >> input;
			iss >> word;
			iss >> word;
			iss >> word;
			targetState = word[1] - '0';
			State* fromState = m_statesUi[int(state)];
			State* toState = m_statesUi[int(targetState)];
			QString transitionLabel = QString(input);
			TransitionType transitionType = base;

			if (input == '*')
				transitionLabel = QString::fromUtf8("\xce\xbb");
			if (int(state) == int(targetState))
				transitionType = self_pointing;
			else transitionType = base;
			AddTransition(fromState, toState, transitionLabel, transitionType);
		}
		else if (word == "Start") {
			iss >> word;
			iss >> startState;
			m_startState = startState[1] - '0';
			SetState(StateType::start, int(m_startState.value()));
		}
		else if (word == "Final") {
			iss >> word; 
			while (iss >> word) {
				finalStates.push_back(word[1] - '0');
			}
			for (char fs : finalStates) {
				SetState(StateType::finish, int(fs));
				m_finalStates.push_back(fs);
			}
		}
	}
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

	
	return true;
}

bool FiniteAutomaton::CheckWord(const std::string& word)
{
	m_transitionsAnimation.clear();
	if (word.size() == 0)
		return (std::find(m_finalStates.begin(), m_finalStates.end(), m_startState) != m_finalStates.end());


	std::vector<std::pair<char, int>> crtState = { { m_startState.value(), 0} };
	std::vector<std::pair<char, int>> toCheckState;
	m_transitionsAnimation.emplace_back(crtState);

	bool found = false;

	while (!crtState.empty() && !found) {
		toCheckState.clear();
		for (auto& crt : crtState) {
			auto& [state, index] = crt;
			if (index < word.length()) {
				std::pair<char, char> key = { state, word[index] };


				if (m_transitions.find(key) != m_transitions.end()) {
					for (auto& transitionResult : m_transitions[key]) {
						if (std::find(toCheckState.begin(), toCheckState.end(), std::make_pair(transitionResult, index)) == toCheckState.end()) {
							toCheckState.emplace_back(transitionResult, index + 1 );
							if (std::find(m_finalStates.begin(), m_finalStates.end(), transitionResult) != m_finalStates.end() && word.size() - 1 == index) {
								m_transitionsAnimation.push_back(toCheckState);
								found = true;
								//return true;
							}
						}
					}
				}

				key = { state, m_lambda };
				if (m_transitions.find(key) != m_transitions.end()) { 
					for (auto& transitionResult : m_transitions[key]) {
						if (std::find(toCheckState.begin(), toCheckState.end(), std::make_pair(transitionResult, index)) == toCheckState.end()) {
							toCheckState.emplace_back(transitionResult, index);
							if (std::find(m_finalStates.begin(), m_finalStates.end(), transitionResult) != m_finalStates.end() && word.size() - 1 == index) {
								m_transitionsAnimation.push_back(toCheckState);
								found = true;
								//return true;
							}
						}
					}
				}

			}
		}

		if (found)
			return true;

		if (toCheckState.empty())
			return false;

		crtState = std::move(toCheckState);
		m_transitionsAnimation.push_back(crtState);
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
	m_transitionsUi.erase(std::remove_if(m_transitionsUi.begin(), m_transitionsUi.end(),
		[value](Transition* transition) {
			return (transition->HasStateOfValue(value));
		}),
		m_transitionsUi.end()
	);

	m_statesUi.erase(m_statesUi.find(value)); 
	m_states.erase(std::find(m_states.begin(), m_states.end(), value));
	if (std::find(m_finalStates.begin(), m_finalStates.end(), value) != m_finalStates.end())
		m_finalStates.erase(std::find(m_finalStates.begin(), m_finalStates.end(), value));
	if (m_startState == char(value) && !m_states.empty())
		m_startState = m_states.front();

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
			if (m_statesUi[index]->GetStateType() == StateType::start)
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
		if (m_statesUi[m_startState.value()]->GetStateType() == StateType::start_finish)
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

std::vector<Transition*> FiniteAutomaton::GetTransitionsUi()
{
	return m_transitionsUi;
}

void FiniteAutomaton::AddTransition(State* stateFrom, State* stateTo, QString value, TransitionType transition)
{

	char transitionValue = (value == QString::fromUtf8("\xce\xbb")) ? m_lambda : value.at(0).toLatin1();

	for (auto& transition : m_transitionsUi) {
		if (transition->existingTransition(stateFrom, stateTo)) {
			transition->Update(value);
			m_transitions[{ char((stateFrom->GetIndex())), transitionValue }].emplace_back(char(stateTo->GetIndex()));
			return;
		}
	}
	m_transitionsUi.emplace_back(new Transition({ stateFrom, stateTo, value, transition }));

	auto it = m_transitions.find({ char((stateFrom->GetIndex())), transitionValue });
	if (it == m_transitions.end())
		m_transitions[{ char((stateFrom->GetIndex())), transitionValue }].push_back(char(stateTo->GetIndex()));
	else
		//it->second.emplace_back(char(stateTo->GetIndex()));
		it->second.emplace_back(char(stateTo->GetIndex()));
}

std::vector<std::vector<std::pair<char, int>>> FiniteAutomaton::GetTransitionForWord()
{
	return m_transitionsAnimation;
}

State* FiniteAutomaton::getStateByKey(int index)
{
	return m_statesUi[index];
}

void FiniteAutomaton::reset()
{
	m_statesUi.clear();
	m_transitionsUi.clear();
	m_transitions.clear();
	m_states.clear();
	m_finalStates.clear();
	m_startState = std::nullopt;
	m_transitionsAnimation.clear();
	m_alphabet.clear();
}

void FiniteAutomaton::setAlphabet(std::map<std::pair<char, char>, std::vector<char>> transitions)
{
	std::set<char> alfabet;
	for (const auto& transition : transitions)
	{
		auto key = transition.first;
		alfabet.insert(key.second);
	}
	std::vector<char> alphabet(alfabet.begin(), alfabet.end());
	m_alphabet = alphabet;
}

bool FiniteAutomaton::IsValid() const
{
	QMessageBox messageBox;
	messageBox.setFixedSize(500, 200);
	if (m_startState == std::nullopt)
	{
		messageBox.critical(0, "Invalid automaton", "No start state !");
		messageBox.show();
		return false;
	}
	if (m_finalStates.size() == 0)
	{
		messageBox.critical(0, "Invalid automaton", "No final state !");
		messageBox.show();
		return false;
	}
	if (m_states.size() == 0)
	{
		messageBox.critical(0, "Invalid automaton", "No states !");
		messageBox.show();
		return false;
	}
	if (m_transitions.size() == 0)
	{
		messageBox.critical(0, "Invalid automaton", "No transitions!");
		messageBox.show();
		return false;
	}

	return true;
}

std::string FiniteAutomaton::GetLambda() const
{
	return std::string(1, m_lambda);
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

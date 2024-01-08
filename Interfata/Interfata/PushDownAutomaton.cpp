#include "PushDownAutomaton.h"
#include <algorithm>
#include "qmessagebox.h"

PushDownAutomaton::PushDownAutomaton(const std::vector<char>& states, const std::vector<char>& alphabet, const std::stack<char>& PDMemory, const std::unordered_set<char>& PDMemoryAlphabet, const std::vector<char>& finalStates, char startState, char startPDMemory, const TransitionMap& transitionFunction)
	:m_states{ states },
	m_alphabet{ alphabet },
	m_PDMemory{ PDMemory },
	m_PDMemoryAlphabet{ PDMemoryAlphabet },
	m_finalStates{ finalStates },
	m_startState{ startState },
	m_startPDMemory{ startPDMemory },
	m_transitions{ transitionFunction }
{
}

bool PushDownAutomaton::CheckWord(const std::string& word) {
	std::cout << "Checking word: " << word << std::endl;
	std::cout << "Checking word: " << word << std::endl;
	m_stackAnimation.clear();
	m_transitionsAnimation.clear();
	m_PDMemory.push(m_startPDMemory);
	return CheckWordRecursive(word, 0, m_startState.value(), m_PDMemory);
}


bool PushDownAutomaton::CheckWordRecursive(std::string word, int index, char currentState, std::stack<char> PDMemory) {
	if (index == word.size()) {
		return (std::find(m_finalStates.begin(), m_finalStates.end(), currentState) != m_finalStates.end() && PDMemory.empty());
	}

	char currentLetter = word[index];

	std::vector<std::pair<char, std::string>> transitions;
	std::tuple<char, char, char> key = std::make_tuple(currentState, currentLetter, PDMemory.empty() ? '\0' : PDMemory.top());
	if (m_transitions.find(key) != m_transitions.end())
		transitions.emplace_back(m_transitions[key]);


	for (const auto& transition : transitions) {

		
		m_transitionsAnimation[index].emplace_back(std::make_pair(transition.first, index)); 

		std::stack<char> newPDMemory = PDMemory;
		if (!newPDMemory.empty()) newPDMemory.pop();

		if (transition.second != "") {
			for (int i = transition.second.size() - 1; i >= 0; --i) {
				newPDMemory.push(transition.second[i]);
			}
		}

		if (CheckWordRecursive(word, index + 1, transition.first, newPDMemory)) {
			return true; 
		}
	}

	return false; 
}


bool PushDownAutomaton::IsDeterministic()
{
	//for (auto transition : m_transitions)
	//{
	//	if (transition.second.size() > 1)
	//	{
	//		return false;
	//	}
	//}
	return true;
}

bool PushDownAutomaton::IsValid() const
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

void PushDownAutomaton::ReadAutomaton(std::istream& is) 
{
	reset();
	std::string line;
	while (std::getline(is, line)) {
		std::istringstream iss(line);
		std::string word;
		iss >> word;

		if (word == "States:") {
			char state;
			while (iss >> word && word != "Alphabet:") {
				if (word[0] == 'q') {
					state = word[1] - '0';
					QPoint defaultPosition(200 + 50 * int(state), 200);
					AddState(defaultPosition);
				}
			}
		}
		else if (word == "Alphabet:") {
			char input;
			while (iss >> input && input != 'S') {
				m_alphabet.push_back(input);
			}
		}
		else if (word == "Stack") {
			iss >> word;
			char stackSymbol;
			while (iss >> stackSymbol) {
				m_PDMemoryAlphabet.insert(stackSymbol);
			}
		}
		else if (word == "Transitions:") 
		{
			std::string transitionLine;
			while (std::getline(is, transitionLine) && transitionLine != "Start state:") {
				std::istringstream transitionStream(transitionLine);
				transitionStream >> word;

				if (word[0] == '[') 
				{
					char stateFrom = word[2] - '0';

					transitionStream >> word;
					transitionStream >> word;

					char inputSymbol = word[0];

					transitionStream >> word;
					transitionStream >> word;

					char stackTop = word[0];

					transitionStream >> word;
					transitionStream >> word;
					char stateTo = word[2] - '0';

					transitionStream >> word;
					transitionStream >> word;

					std::string stackPush(word.begin(), word.end() - 1);
					QString transitionValue = QString(inputSymbol);
					QString stackHead = QString(stackTop);
					QString nextStateStackHead = QString::fromStdString(stackPush);
					AddTransition(m_statesUi[stateFrom], m_statesUi[stateTo], transitionValue, stackTop, stackPush, TransitionType::base);
				}
			}
		}
		else if (word == "Start") {
			iss >> word;
			iss >> word;
			m_startState = word[1];
			SetState(StateType::start, int(word[1] - '0'));
		}
		else if (word == "Final") {
			iss >> word;
			while (iss >> word) {
				SetState(StateType::finish, int(word[1] - '0'));
				m_finalStates.push_back(word[1] - '0');
			}
		}
	}
}


std::string PushDownAutomaton::GetLambda() const
{
	return std::string(1, m_lambda);
}

std::stack<char> PushDownAutomaton::GetPDMemory() const
{
	return m_PDMemory;
}

void PushDownAutomaton::AddState(QPoint p)
{
	m_statesUi.insert({ m_states.size(), new State(p, m_states.size()) });
	m_states.emplace_back(char(m_states.size()));
	if (m_statesUi.size() == 1) {
		m_statesUi[0]->SetState(StateType::start);
		m_startState = char(0);
	}
}


void PushDownAutomaton::PrintAutomaton(std::ostream& out)
{
	out << "States: ";
	for (const auto& state : m_states) {
		out << 'q' << int(state) << " ";
	}
	out << "\n";

	out << "Alphabet: ";
	setAlphabet(m_transitions);
	for (const auto& symbol : m_alphabet) {
		out << symbol << " ";
	}
	out << "\n";

	out << "Stack Alphabet: ";
	for (const auto& symbol : m_PDMemoryAlphabet) {
		out << symbol << " ";
	}
	out << "\n";

	out << "Transitions:\n";
	for (const auto& transition : m_transitions) {
		auto key = transition.first;
		auto value = transition.second;
		out << "[q" << int(std::get<0>(key)) << " , " << std::get<1>(key) << " , " << std::get<2>(key) << "] -> ";
		out << "[q" << int(value.first) << " , " << value.second << "]\n";
	}

	if (m_startState.has_value()) {
		out << "Start state:\nq" << int(m_startState.value()) << "\n";
	}

	out << "Final states: ";
	for (const auto& finalState : m_finalStates) {
		out << 'q' << int(finalState) << " ";
	}
	out << "\n";

	if (!m_PDMemory.empty()) {
		out << "Initial Stack Memory: " << m_startPDMemory << "\n";
	}

	out.flush();
}

void PushDownAutomaton::DeleteState(int value){

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

void PushDownAutomaton::UpdateCoordinate(QPoint p, int index)
{
	m_statesUi[index]->SetCoordinate(p);
}

std::vector<State*> PushDownAutomaton::GetStatesUi()
{
	std::vector<State*> states;
	for (auto& val : m_statesUi) {
		states.emplace_back(val.second);
	}
	return states;
}

void PushDownAutomaton::SetState(StateType state, int index)
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

std::vector<PDTransition*> PushDownAutomaton::GetTransitionsUi()
{
	return m_transitionsUi;
}

std::vector<std::vector<std::pair<char, int>>> PushDownAutomaton::GetTransitionForWord()
{

	std::vector<std::vector<std::pair<char, int>>> transitions;
	
	for (const auto& step : m_transitionsAnimation) {
		transitions.emplace_back(step.second);
	}

	return transitions;
}

std::vector<std::vector<std::vector<char>>> PushDownAutomaton::GetStackForWord()
{
	std::vector<std::vector<char>> oneStepStacks;
	std::vector<std::vector<std::vector<char>>> allStacks;

	for (const auto& step : m_stackAnimation) {
		for (auto stiva : step.second) {
			std::vector<char> oneStack;
			while (!stiva.empty()) {
				oneStack.emplace_back(stiva.top());
				stiva.pop();
			}
			oneStepStacks.emplace_back(oneStack);
		}

		allStacks.emplace_back(oneStepStacks);
	}

	return allStacks;
}

void PushDownAutomaton::AddTransition(State* stateFrom, State* stateTo, QString value, char memoryFrom, std::string memoryTo, TransitionType transition)
{
	char transitionValue = (value == QString::fromUtf8("\xce\xbb")) ? m_lambda : value.at(0).toLatin1();

	
	QString stackHead = QString::fromStdString(std::string(1, memoryFrom));
	QString nextStateStackHead = QString::fromStdString(memoryTo);
	
	for (auto& transition : m_transitionsUi) {
		if (transition->existingTransition(stateFrom, stateTo)) {
			transition->Update(value,stackHead, nextStateStackHead);
			m_transitions[{ char((stateFrom->GetIndex())), transitionValue, memoryFrom }] = { char(stateTo->GetIndex()), memoryTo };
			return;
		}
	}
	m_transitionsUi.emplace_back(new PDTransition({ stateFrom, stateTo, value, transition,stackHead, nextStateStackHead }));

	auto it = m_transitions.find({ char((stateFrom->GetIndex())), transitionValue, memoryFrom });
	if (it == m_transitions.end())
		m_transitions[{char((stateFrom->GetIndex())), transitionValue, memoryFrom}] = { char(stateTo->GetIndex()), memoryTo };

}


State* PushDownAutomaton::getStateByKey(int index)
{
	return m_statesUi[index];
}

void PushDownAutomaton::reset()
{
	m_states.clear();
	m_alphabet.clear();
	m_PDMemory = std::stack<char>();
	m_PDMemoryAlphabet.clear();
	m_finalStates.clear();
	m_startState = ' ';
	m_startPDMemory = ' ';
	m_transitions.clear();
	m_transitionsUi.clear();
	m_statesUi.clear();
	m_transitionsAnimation.clear();
}

void PushDownAutomaton::setAlphabet(TransitionMap transitions)
{
	std::set<char> alfabet;
	std::unordered_set<char> stackAlphabet;
	std::unordered_map<std::tuple<char, char, char>, std::pair<char, std::string>>;
	for (const auto& transition : transitions)
	{
		auto key = transition.first;
		alfabet.insert(std::get<1>(key));
		stackAlphabet.insert(std::get<2>(key));
	}
	std::vector<char> alphabet(alfabet.begin(), alfabet.end());
	m_alphabet = alphabet;
	m_PDMemoryAlphabet = stackAlphabet;
}

bool PushDownAutomaton::VerifyAutomaton()
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

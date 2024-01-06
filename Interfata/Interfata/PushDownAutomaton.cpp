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
	char currentState = m_startState.has_value();
	int index = 0;
	while (index <= word.size())
	{
		char currentLetter;
		if (index == word.size())
			currentLetter = '*';
		else
			currentLetter = word[index];
		//char nextLeter = word[index + 1];
		if (std::find(m_alphabet.begin(), m_alphabet.end(), currentLetter) == m_alphabet.end() and currentLetter != '*')
		{
			return false;
		}
		std::vector<std::pair<char, std::string>> transitions{ m_transitions[std::make_tuple(currentState, currentLetter, m_PDMemory.top())] };
		if (transitions.size() == 0)
			return false;

		for (const auto& transition : transitions)
		{
			currentState = transition.first;
			m_PDMemory.pop();
			if (transition.second != "*")
			{
				for (int i = transition.second.size() - 1; i >= 0; --i)
				{
					m_PDMemory.push(transition.second[i]);
				}
			}
		}
		index++;
	}
	if (std::find(m_finalStates.begin(), m_finalStates.end(), currentState) != m_finalStates.end())
		return true;
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
	// de cautat probleme in continuare
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

	// DE FACUT : de verificat daca toate starile sunt conectate prin cel putin o singura tranzitie

	return true;
}

void PushDownAutomaton::ReadAutomaton(std::istream& is)
{
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

}

void PushDownAutomaton::DeleteState(int value)
{	// ce se intampla cu simbolul de start

	m_transitionsUi.erase(std::remove_if(m_transitionsUi.begin(), m_transitionsUi.end(),
		[value](Transition* transition) {
			return (transition->HasStateOfValue(value));
		}),
		m_transitionsUi.end()
	);

	m_statesUi.erase(m_statesUi.find(value)); // se sterge din states
	// aici este problema
	m_states.erase(std::find(m_states.begin(), m_states.end(), value));
	if (std::find(m_finalStates.begin(), m_finalStates.end(), value) != m_finalStates.end())
		m_finalStates.erase(std::find(m_finalStates.begin(), m_finalStates.end(), value));
	if (m_startState == char(value) && !m_states.empty())
		m_startState = m_states.front();

	// ce se intampla cu m_startState? 
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

void PushDownAutomaton::AddTransition(State* stateFrom, State* stateTo, QString value, char memoryFrom, std::string memoryTo, TransitionType transition)
{
	char transitionValue = (value == QString::fromUtf8("\xce\xbb")) ? m_lambda : value.at(0).toLatin1();

	
	//transform memoryFrom in QString
	QString stackHead = QString::fromStdString(std::string(1, memoryFrom));
	QString nextStateStackHead = QString::fromStdString(memoryTo);
	
	// adaugare tranzitie pentru ui 
	for (auto& transition : m_transitionsUi) {
		if (transition->existingTransition(stateFrom, stateTo)) {
			transition->Update(value,stackHead, nextStateStackHead);
			m_transitions[{ char((stateFrom->GetIndex())), transitionValue, memoryFrom }] = { char(stateTo->GetIndex()), memoryTo };
			return;
		}
	}
	m_transitionsUi.emplace_back(new PDTransition({ stateFrom, stateTo, value, transition,stackHead, nextStateStackHead }));

	// adaugare tranzitie 
	auto it = m_transitions.find({ char((stateFrom->GetIndex())), transitionValue, memoryFrom });
	if (it == m_transitions.end())
		m_transitions[{char((stateFrom->GetIndex())), transitionValue, memoryFrom}] = { char(stateTo->GetIndex()), memoryTo };

}

std::vector<std::vector<std::pair<char, int>>> PushDownAutomaton::GetTransitionForWord()
{
	return m_transitionsAnimation;
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

	for (auto& el : m_transitions) {
		// de facut poate
	}

	return true;
}

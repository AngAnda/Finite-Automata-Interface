#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Interfata.h"
#include "FiniteAutomaton.h"

class Interfata : public QMainWindow
{
	Q_OBJECT

public:
	Interfata(QWidget* parent = nullptr);
	~Interfata();

protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

private slots:
	void HandleStateManager1(bool checked);
	void HandleStateManager2(bool checked);
	void HandleStateManager3(bool checked);
	void HandleStateManager4(bool checked);
	void CheckOneWord();
	void CheckWordsFromFile();
	void OnComboBoxSelectionChanged(int index); // nu functioneaza

private:
	void OpenInNotepad(const QString& filePath);
	void DrawArrow(QPainter& painter, const Transition* transition);
	void DrawPreviousState(QPainter& painter, State* state, QString word);
	void DrawCurrentState(QPainter& painter, State* state, QString word);
	enum ButtonRightAction {
		AddingState,
		AddingFinalState,
		AddingStartingState,
		DeleteState
	};
	enum ApplicationState {
		Animating,
		Non_Animating
	};
	enum AutomatonType {
		Finite,
		Push_Down
	};

	std::optional<int> CheckUpdatePosition(QPoint position);

private:
	Ui::InterfataClass ui;
	IAutomaton* m_automaton;
	float m_radius = 40.00;
	const QString m_automatonFile = "./Automaton.txt";
	const QString m_wordsFile = "./Words.txt";
	ButtonRightAction m_currentAction;
	ApplicationState m_applicationState;
	AutomatonType m_automatonType;
	std::optional<int> m_stateMoving;
	std::pair<std::optional<State*>, std::optional<State*>> m_newTransitions;
	
	// variabile globale, sunt cam urate
	int m_AnimationStep;
	QString m_currentWord;

	std::vector<std::vector<std::pair<char, int>>> m_transitionsHistory;
};

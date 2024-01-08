#pragma once

#include <QtWidgets/QMainWindow>
#include <QListWidget>
#include <fstream>
#include "ui_Interfata.h"
#include "FiniteAutomaton.h"
#include "PushDownAutomaton.h"
#include "CheckWords.h"
#include "StivaScene.cpp"

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
	void mouseMoveEvent(QMouseEvent* event) override;

private slots:
	void HandleStateManager1(bool checked);
	void HandleStateManager2(bool checked);
	void HandleStateManager3(bool checked);
	void HandleStateManager4(bool checked);
	void CheckOneWord();
	void CheckWordsFromFile();
	void OnComboBoxSelectionChanged(int index);
	void SaveAutomatonToFile();
	void LoadAutomatonFromFile();

private:
	void DrawStack(QPainter& painter);
	void OpenInNotepad(const QString& filePath);
	void DrawArrow(QPainter& painter, const Transition* transition);
	void DrawArrowPDA(QPainter& painter, const PDTransition* transition);
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
	CheckWords* m_acceptedWordsWidget;

	QPoint m_stackPosition;
	bool m_stackMoving;
	size_t stackItemWidth;
	size_t stackItemHeight;
	size_t stackSize = 3;
	int m_AnimationStep;
	QString m_currentWord;

	std::vector<std::vector<std::pair<char, int>>> m_transitionsHistory;
};

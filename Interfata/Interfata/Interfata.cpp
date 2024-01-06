#include "Interfata.h"
#include <QMouseEvent> 
#include <QPainter>
#include <qpoint.h>
#include <qprocess.h>
#include <QInputDialog>
#include <qdir.h>
#include <qpainterpath.h>
#include <QVector2D>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QThread>

Interfata::Interfata(QWidget* parent)
	: QMainWindow(parent),
	m_currentAction(ButtonRightAction::AddingState),
	m_applicationState(ApplicationState::Non_Animating),
	m_automatonType(AutomatonType::Finite),
	m_newTransitions({ std::nullopt, std::nullopt }),
	m_currentWord(""),
	m_acceptedWordsWidget(new CheckWords()),
	m_stackMoving(false),
	m_stackPosition(200, 200)

{

	ui.setupUi(this);
	m_automaton = new FiniteAutomaton();
	QObject::connect(ui.stateManager1, &QRadioButton::toggled, this, &Interfata::HandleStateManager1);
	QObject::connect(ui.stateManager2, &QRadioButton::toggled, this, &Interfata::HandleStateManager2);
	QObject::connect(ui.stateManager3, &QRadioButton::toggled, this, &Interfata::HandleStateManager3);
	QObject::connect(ui.stateManager4, &QRadioButton::toggled, this, &Interfata::HandleStateManager4);
	QObject::connect(ui.readWordButton, &QRadioButton::clicked, this, &Interfata::CheckOneWord);
	QObject::connect(ui.wordsFromFile, &QRadioButton::clicked, this, &Interfata::CheckWordsFromFile);
	QObject::connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxSelectionChanged(int)));
	QObject::connect(ui.pushButton_3, &QPushButton::clicked, this, &Interfata::SaveAutomatonToFile);
	QObject::connect(ui.pushButton_4, &QPushButton::clicked, this, &Interfata::LoadAutomatonFromFile);
}

Interfata::~Interfata()
{}

void Interfata::paintEvent(QPaintEvent* event)
{
	std::vector<State*> states = m_automaton->GetStatesUi();
	PushDownAutomaton* pda = dynamic_cast<PushDownAutomaton*>(m_automaton);
	FiniteAutomaton* fa = dynamic_cast<FiniteAutomaton*>(m_automaton);

	QPainter p(this);

	if (fa)
	{
		std::vector<Transition*> transitions = fa->GetTransitionsUi();

		if (states.empty() && transitions.empty())
			return;


		for (const auto& trans : transitions) {

			DrawArrow(p, trans);
		}

		p.setBrush(QBrush(Qt::white));
	}
	if (pda)
	{
		std::vector<PDTransition*> transitions = pda->GetTransitionsUi();

		if (states.empty() && transitions.empty())
			return;

		for (const auto& trans : transitions) {

			DrawArrowPDA(p, trans);
		}

		p.setBrush(QBrush(Qt::white));
	}

	for (const auto& state : states) {

		const auto& [x, y] = state->GetCoordinate();

		if (state->GetStateType() == StateType::start || state->GetStateType() == StateType::start_finish) {
			p.drawLine(QPoint(x - 0.75 * m_radius, y - 0.75 * m_radius), state->GetCoordinate());
			p.drawText(QPoint(x - 0.75 * m_radius + 5, y - 0.75 * m_radius), "Start");
		}

		if (state->GetStateType() == StateType::finish || state->GetStateType() == StateType::start_finish) {
			QRect r2(x - m_radius / 2 - m_radius / 8, y - m_radius / 2 - m_radius / 8, m_radius + m_radius / 4, m_radius + m_radius / 4);
			p.drawEllipse(r2);
		}

		QRect r(x - m_radius / 2, y - m_radius / 2, m_radius, m_radius);
		p.drawEllipse(r);
		p.drawText(r, Qt::AlignCenter, state->GetName());
		p.setPen(QPen(Qt::black));
	}

	if (m_applicationState == ApplicationState::Animating) {
		for (int i = 0; i < m_AnimationStep; i++) {
			for (auto& transition : m_transitionsHistory[i]) {
				auto& [stateKey, wordSpan] = transition;
				DrawPreviousState(p, m_automaton->getStateByKey(stateKey), m_currentWord.left(wordSpan));
			}
		}

		for (auto& transition : m_transitionsHistory[m_AnimationStep]) {
			auto& [stateKey, wordSpan] = transition;
			DrawCurrentState(p, m_automaton->getStateByKey(stateKey), m_currentWord.left(wordSpan));
		}

	}

	if (m_automatonType == AutomatonType::Push_Down) {
		// Desenează stiva pentru PDA
		DrawStack(p);
	}
}

void Interfata::DrawStack(QPainter& painter)
{
	stackItemWidth = 30; // Width of a stack element
	stackItemHeight = 20; // Height of a stack element

	PushDownAutomaton* pda = dynamic_cast<PushDownAutomaton*>(m_automaton);
	if (pda) {
		std::stack<char> stack = pda->GetPDMemory();
		std::vector<char> stackVector(stack.size());

		// Convert the stack to a vector
		int i = stack.size() - 1;
		while (!stack.empty()) {
			stackVector[i--] = stack.top();
			stack.pop();
		}

		stackVector = { 'a', 'b', 'c' };
		// Draw each element of the stack
		for (size_t i = 0; i < stackVector.size(); ++i) {
			QRect stackItemRect(m_stackPosition.x(), m_stackPosition.y() + i * stackItemHeight, stackItemWidth, stackItemHeight);
			painter.drawRect(stackItemRect);

			// Convert the character to QString
			QString character = QString(stackVector[i]);
			painter.drawText(stackItemRect, Qt::AlignCenter, character);
		}
	}
}


void Interfata::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_stackMoving)
	{
		m_stackMoving = false;
		m_stackPosition = event->pos();
		update();
		return;
	}
	PushDownAutomaton* pda = dynamic_cast<PushDownAutomaton*>(m_automaton);
	m_applicationState = ApplicationState::Non_Animating;
	if (event->button() == Qt::RightButton)
	{
		FiniteAutomaton* fa = dynamic_cast<FiniteAutomaton*>(m_automaton);
		if (fa)
		{
			if (m_newTransitions.first.has_value() && m_newTransitions.second.has_value()) {
				QString value = (ui.addLambda->isChecked()) ? QString::fromUtf8("\xce\xbb") : QInputDialog::getText(nullptr, "Add a transition", "Enter your transition:", QLineEdit::Normal, QString('a'));
				if (value.isEmpty()) {
					m_newTransitions = { std::nullopt, std::nullopt };
					return;
				}

				if (m_newTransitions.first.value() != m_newTransitions.second.value())
					fa->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, TransitionType::base);
				else
					fa->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, TransitionType::self_pointing);
			}
		}
		if (pda)
		{
			if (m_newTransitions.first.has_value() && m_newTransitions.second.has_value()) {

				QString value = (ui.addLambda->isChecked()) ? QString::fromUtf8("\xce\xbb") : QInputDialog::getText(nullptr, "Add the simbol to transition", "Enter your simbol:", QLineEdit::Normal, QString('a'));
				QString stackHead = (ui.addLambda->isChecked()) ? QString::fromUtf8("\xce\xbb") : QInputDialog::getText(nullptr, "Add the head of the stack of transition", "Enter the head of the stack:", QLineEdit::Normal, QString('Z'));
				QString nextStackHead = (ui.addLambda->isChecked()) ? QString::fromUtf8("\xce\xbb") : QInputDialog::getText(nullptr, "Add the head of the stack for the next transition", "Enter the head of the stack for the next transition:", QLineEdit::Normal, QString("AZ"));

				if (value.isEmpty()) {
					m_newTransitions = { std::nullopt, std::nullopt };
					return;
				}
				
				char stackFrom = stackHead.toUtf8().constData()[0];
				std::string stackTo = nextStackHead.toUtf8().constData();

				if (m_newTransitions.first.value() != m_newTransitions.second.value())
					pda->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, stackFrom, stackTo, TransitionType::base);
				else
					pda->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, stackFrom, stackTo, TransitionType::self_pointing);
			}
		}

	}
	else
	{

		std::optional<int> indexExistingNode = CheckUpdatePosition(event->pos());

		switch (m_currentAction) {
		case ButtonRightAction::AddingState:
			if (m_stateMoving != std::nullopt) {
				m_automaton->UpdateCoordinate(event->pos(), m_stateMoving.value());
				m_stateMoving = std::nullopt;
			}
			else if (!indexExistingNode.has_value())
				m_automaton->AddState(event->pos());
			break;
		case ButtonRightAction::AddingStartingState:
			if (indexExistingNode.has_value())
				m_automaton->SetState(StateType::start, indexExistingNode.value());
			break;
		case ButtonRightAction::AddingFinalState:
			if (indexExistingNode.has_value())
				m_automaton->SetState(StateType::finish, indexExistingNode.value());
			break;
		case ButtonRightAction::DeleteState:
			if (indexExistingNode.has_value())
				m_automaton->DeleteState(indexExistingNode.value());
			break;
		}
	}
	update();
}

void Interfata::mousePressEvent(QMouseEvent* event)
{
	if (event->pos().x() >= m_stackPosition.x() && event->pos().x() < m_stackPosition.x() + stackItemWidth &&
		event->pos().y() >= m_stackPosition.y() && event->pos().y() < m_stackPosition.y() + stackItemHeight * stackSize) {
		m_stackMoving = true;
		return;
	}

	auto stateValue = CheckUpdatePosition(event->pos());
	if (!stateValue.has_value()) return;

	if (event->button() == Qt::RightButton) {
		if (!m_newTransitions.first.has_value() || m_newTransitions.second.has_value()) {
			m_newTransitions.second = std::nullopt;
			m_newTransitions.first = m_automaton->GetStatesUi().at(stateValue.value());
		}
		else
		{
			m_newTransitions.second = m_automaton->GetStatesUi().at(stateValue.value());
		}
	}
	else {
		m_stateMoving = CheckUpdatePosition(event->pos());
	}
}

void Interfata::mouseMoveEvent(QMouseEvent* event)
{
	PushDownAutomaton* pda = dynamic_cast<PushDownAutomaton*>(m_automaton);

	if (pda and m_stackMoving) {
		// Update the stack position as the mouse moves
		m_stackPosition = event->pos();
		update(); // Trigger a repaint
	}
	else if (m_stateMoving.has_value()) {
		// Update the position of the moving state
		m_automaton->UpdateCoordinate(event->pos(), m_stateMoving.value());
		update(); // Trigger a repaint
	}
}

void Interfata::HandleStateManager1(bool checked)
{
	m_currentAction = ButtonRightAction::AddingState;
}

void Interfata::HandleStateManager2(bool checked)
{
	m_currentAction = ButtonRightAction::AddingFinalState;
}

void Interfata::HandleStateManager3(bool checked)
{
	m_currentAction = ButtonRightAction::AddingStartingState;
}

void Interfata::HandleStateManager4(bool checked)
{
	m_currentAction = ButtonRightAction::DeleteState;
}

void Interfata::CheckOneWord()
{
	if (!m_automaton->IsValid()) {
		return;
	}

	QString value = QInputDialog::getText(nullptr, "Check word in automaton", "Enter your word:", QLineEdit::Normal, "");

	QMessageBox messageBox;
	messageBox.setFixedSize(500, 200);
	messageBox.setWindowTitle("Info");

	m_currentWord = value;
	m_applicationState = ApplicationState::Animating;


	if (m_automaton->CheckWord((value.isEmpty()) ? m_automaton->GetLambda() : std::string(value.toUtf8().constData())))
		messageBox.setText("Word has been accepted");
	else
		messageBox.setText("Word has not been accepted");


	if (m_automatonType == AutomatonType::Finite) {
		FiniteAutomaton* automaton = dynamic_cast<FiniteAutomaton*>(m_automaton);

		m_transitionsHistory = automaton->GetTransitionForWord();
		for (uint8_t i = 0; i < m_transitionsHistory.size(); i++) {
			m_AnimationStep = i;
			repaint();
			QThread::sleep(1);
		}

	}
	else {
		// structura de date folosita pentru APD
	}
	//m_applicationState = ApplicationState::Non_Animating;
	messageBox.exec();
}

void Interfata::CheckWordsFromFile()
{
	if (!m_automaton->IsValid())
		return;

	QString filter = "Text files (*.txt);;All files (*.*)"; // File filter for dialog
	QString filePath = QFileDialog::getOpenFileName(this, "Open Text File", QDir::homePath(), filter);

	if (!filePath.isEmpty()) {
		QFile file(filePath);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QMessageBox::warning(this, "Read Error", "Could not open the file for reading.");
			return;
		}

		QTextStream in(&file);
		QString word;

		m_acceptedWordsWidget->Clear();

		while (!in.atEnd()) {
			in >> word; // Read a word

			// Assuming you have a function to check if the word is accepted
			if (m_automaton->CheckWord(word.toStdString())) {
				m_acceptedWordsWidget->AddAcceptedWords(word); // Add to accepted list
			}
			else {
				m_acceptedWordsWidget->AddRejectedWords(word); // Add to rejected list
			}
		}

		m_acceptedWordsWidget->show();

		file.close();
	}
	else {
		// No file was selected
	}

	m_acceptedWordsWidget->show();
}


void Interfata::OnComboBoxSelectionChanged(int index)
{
	m_automaton->reset();
	update();

	if (index == 0)
	{
		m_automatonType = AutomatonType::Finite;
		m_automaton = new FiniteAutomaton();
	}
	else
	{
		m_automatonType = AutomatonType::Push_Down;
		delete m_automaton;
		m_automaton = new PushDownAutomaton();
	}
}

void Interfata::SaveAutomatonToFile()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Automaton"), "",
		tr("Text Files (*.txt);;All Files (*)"));

	if (fileName.isEmpty())
		return;

	std::ofstream file(fileName.toStdString());
	if (!file.is_open())
	{
		QMessageBox::information(this, tr("Unable to open file"),
			tr("Could not open the file for writing."));
		return;
	}

	m_automaton->PrintAutomaton(file);
	file.close();
}

void Interfata::LoadAutomatonFromFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
				tr("Open Automaton"), "",
				tr("Text Files (*.txt);;All Files (*)"));

	if (fileName.isEmpty())
		return;

	std::ifstream file(fileName.toStdString());
	if (!file.is_open())
	{
		QMessageBox::information(this, tr("Unable to open file"),
						tr("Could not open the file for reading."));
		return;
	}

	m_automaton->ReadAutomaton(file);
	
	update();
	
	file.close();
}

void Interfata::OpenInNotepad(const QString& filePath)
{
	QProcess::startDetached("notepad.exe", QStringList() << filePath);
}

std::optional<int> Interfata::CheckUpdatePosition(QPoint position)
{

	auto& [px, py] = position;
	// returns an index for an index of m_statesUi
	auto states = m_automaton->GetStatesUi();

	for (auto& state : states) {
		auto [sx, sy] = state->GetCoordinate();
		if (std::abs(sx - px) < m_radius && std::abs(sy - py) < m_radius) {
			return state->GetIndex();
		}

	}
	return std::nullopt;
}

void Interfata::DrawArrow(QPainter& painter, const Transition* transition) {

	const auto& [startCenter, endCenter] = transition->GetCoord();
	QString label = transition->GetValue();

	if (transition->GetType() == TransitionType::base) {
		double angle = std::atan2((endCenter.y() - startCenter.y()), (endCenter.x() - startCenter.x()));

		const double arrowSize = 10;

		QPoint startBorder = startCenter + QPoint(cos(angle) * m_radius / 2, sin(angle) * m_radius / 2);
		QPoint endBorder = endCenter - QPoint(cos(angle) * (m_radius / 2 + arrowSize), sin(angle) * (m_radius / 2 + arrowSize));

		painter.drawLine(startBorder, endBorder);

		QPolygon arrowHead;
		arrowHead << endCenter - QPoint(cos(angle) * m_radius / 2, sin(angle) * m_radius / 2)
			<< endCenter - QPoint(cos(angle + M_PI / 6) * arrowSize + cos(angle) * m_radius / 2,
				sin(angle + M_PI / 6) * arrowSize + sin(angle) * m_radius / 2)
			<< endCenter - QPoint(cos(angle - M_PI / 6) * arrowSize + cos(angle) * m_radius / 2,
				sin(angle - M_PI / 6) * arrowSize + sin(angle) * m_radius / 2);
		painter.drawPolygon(arrowHead);

		// Calculate the midpoint of the line
		QPointF midPoint = (startBorder + endBorder) / 2.0;

		// Calculate a perpendicular offset
		double textOffset = 10; // This is the padding amount
		QPointF perpOffset(-sin(angle) * textOffset, cos(angle) * textOffset);

		// Offset the midpoint by the perpendicular offset
		midPoint += perpOffset;

		// Draw the text with a background
		QFont font = painter.font();
		QFontMetrics metrics(font);
		QRect textRect = metrics.boundingRect(label);
		textRect.moveCenter(midPoint.toPoint());

		// Optionally, draw a background rectangle behind the text
		QRect backgroundRect = textRect.adjusted(-5, -2, 5, 2); // Add some padding around the text
		painter.setBrush(QColor(255, 255, 255, 127)); // Semi-transparent white background
		painter.setPen(Qt::NoPen); // No border for the background
		painter.drawRect(backgroundRect);

		// Set the pen back to black for the text and draw the text
		painter.setPen(Qt::black);
		painter.drawText(textRect, Qt::AlignCenter, label);

	}
	else if (transition->GetType() == TransitionType::self_pointing)
	{
		painter.setBrush(QBrush(Qt::transparent));
		QPoint newPoint(startCenter.x() - m_radius + m_radius / 8, startCenter.y() - m_radius + m_radius / 8);
		painter.drawEllipse(QRect(newPoint, QSize(m_radius, m_radius)));
		QFont font = painter.font();
		font.setPointSize(10); // Set the font size
		painter.setFont(font);
		QFontMetricsF metrics(font);
		QRectF textRect = metrics.boundingRect(label);

		QRectF backgroundRect = textRect.adjusted(-5, -2, 5, 2); // Add some padding around the text
		painter.setBrush(QColor(255, 255, 255, 127)); // Semi-transparent white background
		painter.setPen(Qt::NoPen); // No border for the background
		painter.drawRect(backgroundRect);
		textRect.moveCenter(QPointF(newPoint.x(), newPoint.y() - 10)); // Adjust the 10 if needed

		painter.setBrush(QBrush(Qt::black));
		painter.setPen(Qt::black);
		painter.drawText(textRect, Qt::AlignCenter, label);
	}


}

void Interfata::DrawArrowPDA(QPainter& painter, const PDTransition* transition)
{
	QString stackFromLabel;
	QString stackToLabel;
	if (transition->GetStackHead().compare(""))
		stackFromLabel = QString("λ");
	else
		stackFromLabel = QString(transition->GetStackHead());

	if (transition->GetNextStateStackHead() == "")
		stackToLabel = QString("λ");
	else
		stackToLabel = QString(transition->GetNextStateStackHead());

	const auto& [startCenter, endCenter] = transition->GetCoord();
	QString label = transition->GetValue();
	QString coma = QString(",");
	QString pipe = QString("|");

	if (transition->GetType() == TransitionType::base) {
		double angle = std::atan2((endCenter.y() - startCenter.y()), (endCenter.x() - startCenter.x()));

		const double arrowSize = 10;

		QPoint startBorder = startCenter + QPoint(cos(angle) * m_radius / 2, sin(angle) * m_radius / 2);
		QPoint endBorder = endCenter - QPoint(cos(angle) * (m_radius / 2 + arrowSize), sin(angle) * (m_radius / 2 + arrowSize));

		painter.drawLine(startBorder, endBorder);

		QPolygon arrowHead;
		arrowHead << endCenter - QPoint(cos(angle) * m_radius / 2, sin(angle) * m_radius / 2)
			<< endCenter - QPoint(cos(angle + M_PI / 6) * arrowSize + cos(angle) * m_radius / 2,
				sin(angle + M_PI / 6) * arrowSize + sin(angle) * m_radius / 2)
			<< endCenter - QPoint(cos(angle - M_PI / 6) * arrowSize + cos(angle) * m_radius / 2,
				sin(angle - M_PI / 6) * arrowSize + sin(angle) * m_radius / 2);
		painter.drawPolygon(arrowHead);

		// Calculate the midpoint of the line
		QPointF midPoint = (startBorder + endBorder) / 2.0;

		// Calculate a perpendicular offset
		double textOffset = 10; // This is the padding amount
		QPointF perpOffset(-sin(angle) * textOffset, cos(angle) * textOffset);

		// Offset the midpoint by the perpendicular offset
		midPoint += perpOffset;

		// Draw the text with a background
		QFont font = painter.font();
		QFontMetrics metrics(font);
		QRect textRect = metrics.boundingRect(label);
		QRect stackFromRect = metrics.boundingRect(stackFromLabel);
		QRect stackToRect = metrics.boundingRect(stackToLabel);
		QRect comaToRect = metrics.boundingRect(coma);
		QRect pipeRect = metrics.boundingRect(pipe);
		textRect.moveCenter(midPoint.toPoint());
		comaToRect.moveCenter(midPoint.toPoint() + QPoint(5, 0));
		stackFromRect.moveCenter(midPoint.toPoint() + QPoint(10, 0));
		pipeRect.moveCenter(midPoint.toPoint() + QPoint(15, 0));
		stackToRect.moveCenter(midPoint.toPoint() + QPoint(25, 0));


		// Optionally, draw a background rectangle behind the text
		QRect backgroundRect = textRect.adjusted(-5, -2, 5, 2);// Add some padding around the text
		QRect backgroundRect2 = stackFromRect.adjusted(-5, -2, 5, 2);// Add some padding around the text
		QRect backgroundRect3 = stackToRect.adjusted(-5, -2, 5, 2);// Add some padding around the text
		QRect backgroundRect4 = comaToRect.adjusted(-5, -2, 5, 2);// Add some padding around the text
		QRect backgroundRect5 = pipeRect.adjusted(-5, -2, 5, 2);// Add some padding around the text
		painter.setBrush(QColor(255, 255, 255, 200)); // Semi-transparent white background
		painter.setPen(Qt::NoPen); // No border for the background
		painter.drawRect(backgroundRect);
		painter.drawRect(backgroundRect2);
		painter.drawRect(backgroundRect3);
		painter.drawRect(backgroundRect4);
		painter.drawRect(backgroundRect5);

		// Set the pen back to black for the text and draw the text
		painter.setPen(Qt::black);
		painter.drawText(textRect, Qt::AlignCenter, label);
		painter.drawText(comaToRect, Qt::AlignCenter, coma);
		painter.drawText(stackFromRect, Qt::AlignCenter, stackFromLabel);
		painter.drawText(pipeRect, Qt::AlignCenter, pipe);
		painter.drawText(stackToRect, Qt::AlignCenter, stackToLabel);

	}
	else if (transition->GetType() == TransitionType::self_pointing)
	{
		painter.setBrush(QBrush(Qt::transparent));
		QPoint newPoint(startCenter.x() - m_radius + m_radius / 8, startCenter.y() - m_radius + m_radius / 8);
		painter.drawEllipse(QRect(newPoint, QSize(m_radius, m_radius)));
		QFont font = painter.font();
		font.setPointSize(10); // Set the font size
		painter.setFont(font);
		// ...

// Calculate the dimensions of the text
		QFontMetricsF metrics(font);
		QRectF textRect = metrics.boundingRect(label);
		QRectF stackFromRect = metrics.boundingRect(stackFromLabel);
		QRectF stackToRect = metrics.boundingRect(stackToLabel);
		QRectF comaToRect = metrics.boundingRect(coma);
		QRectF pipeRect = metrics.boundingRect(pipe);

		// Calculate the dimensions of the background rectangles
		QRectF backgroundRect = textRect.adjusted(-5, -2, 5, 2);
		QRectF backgroundRect2 = stackFromRect.adjusted(-5, -2, 5, 2);
		QRectF backgroundRect3 = stackToRect.adjusted(-5, -2, 5, 2);
		QRectF backgroundRect4 = comaToRect.adjusted(-5, -2, 5, 2);
		QRectF backgroundRect5 = pipeRect.adjusted(-5, -2, 5, 2);

		// Draw the background rectangles with a semi-transparent white color
		painter.setBrush(QColor(255, 255, 255, 200));
		painter.setPen(Qt::NoPen);
		painter.drawRect(backgroundRect);
		painter.drawRect(backgroundRect2);
		painter.drawRect(backgroundRect3);
		painter.drawRect(backgroundRect4);
		painter.drawRect(backgroundRect5);

		// Move the text and draw it
		textRect.moveCenter(QPointF(newPoint.x() - 15, newPoint.y() - 15));
		comaToRect.moveCenter(QPointF(newPoint.x() - 10, newPoint.y() - 15));
		stackFromRect.moveCenter(QPointF(newPoint.x() - 5, newPoint.y() - 15));
		pipeRect.moveCenter(QPointF(newPoint.x() + 5, newPoint.y() - 15));
		stackToRect.moveCenter(QPointF(newPoint.x() + 20, newPoint.y() - 15));

		painter.setBrush(QBrush(Qt::black));
		painter.setPen(Qt::black);
		painter.drawText(textRect, Qt::AlignCenter, label);
		painter.drawText(comaToRect, Qt::AlignCenter, coma);
		painter.drawText(stackFromRect, Qt::AlignCenter, stackFromLabel);
		painter.drawText(pipeRect, Qt::AlignCenter, pipe);
		painter.drawText(stackToRect, Qt::AlignCenter, stackToLabel);

	}
}

void Interfata::DrawPreviousState(QPainter& painter, State* state, QString word)
{
	const auto& [x, y] = state->GetCoordinate();
	painter.setPen(QPen(Qt::blue));
	QRect r(x - m_radius / 2, y - m_radius / 2, m_radius, m_radius);
	QRect textRect(x + m_radius / 2 + 5, y - m_radius / 2 - 5, m_radius, 15);
	painter.drawEllipse(r);
	painter.drawText(r, Qt::AlignCenter, state->GetName());
	painter.drawText(textRect, Qt::AlignTop, word);
}

void Interfata::DrawCurrentState(QPainter& painter, State* state, QString word)
{
	const auto& [x, y] = state->GetCoordinate();
	painter.setPen(QPen(Qt::red));
	QRect r(x - m_radius / 2, y - m_radius / 2, m_radius, m_radius);
	QRect textRect(x + m_radius / 2 + 5, y - m_radius / 2 - 5, m_radius, 15);
	painter.drawEllipse(r);
	painter.drawText(r, Qt::AlignCenter, state->GetName());
	painter.drawText(textRect, Qt::AlignTop, word);
}

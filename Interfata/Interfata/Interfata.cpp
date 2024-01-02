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
	m_newTransitions({ std::nullopt, std::nullopt }),
	m_applicationState(ApplicationState::Non_Animating),
	m_currentWord("")
{
	ui.setupUi(this);
	m_automaton = new FiniteAutomaton();
	QObject::connect(ui.stateManager1, &QRadioButton::toggled, this, &Interfata::HandleStateManager1);
	QObject::connect(ui.stateManager2, &QRadioButton::toggled, this, &Interfata::HandleStateManager2);
	QObject::connect(ui.stateManager3, &QRadioButton::toggled, this, &Interfata::HandleStateManager3);
	QObject::connect(ui.stateManager4, &QRadioButton::toggled, this, &Interfata::HandleStateManager4);
	QObject::connect(ui.readWordButton, &QRadioButton::clicked, this, &Interfata::CheckOneWord);
	QObject::connect(ui.wordsFromFile, &QRadioButton::clicked, this, &Interfata::CheckWordsFromFile);
}

Interfata::~Interfata()
{}

void Interfata::paintEvent(QPaintEvent* event)
{
	std::vector<State*> states = m_automaton->GetStatesUi();
	std::vector<Transition*> transitions = m_automaton->GetTransitionsUi();

	if (states.empty() && transitions.empty())
		return;

	QPainter p(this);

	for (const auto& trans : transitions) {

		DrawArrow(p, trans);
	}

	p.setBrush(QBrush(Qt::white));

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

}

void Interfata::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		if (m_newTransitions.first.has_value() && m_newTransitions.second.has_value()) {

			QString value = (ui.addLambda->isChecked()) ? QString::fromUtf8("\xce\xbb") : QInputDialog::getText(nullptr, "Add a transition", "Enter your transition:", QLineEdit::Normal,
				QString('a'));
			if (value.isEmpty()) {
				m_newTransitions ={ std::nullopt, std::nullopt };
				return;
			}

			if (m_newTransitions.first.value() != m_newTransitions.second.value())
				m_automaton->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, TransitionType::base);
			else
				m_automaton->AddTransition(m_newTransitions.first.value(), m_newTransitions.second.value(), value, TransitionType::self_pointing);
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
	QString value = QInputDialog::getText(nullptr, "Check word in automaton", "Enter your word:", QLineEdit::Normal, "");
	QMessageBox messageBox;
	messageBox.setFixedSize(500, 200);
	messageBox.setWindowTitle("Info");

	if (!m_automaton->IsValid()) {
		return;
	}

	m_currentWord = value;
	m_applicationState = ApplicationState::Animating;


	if (m_automaton->CheckWord(std::string(value.toUtf8().constData())))
		messageBox.setText("Word has been accepted");
	else
		messageBox.setText("Word has not been accepted");

	//m_transitionsHistory.clear();
	m_transitionsHistory = m_automaton->GetTransitionForWord();

	for (uint8_t i = 0; i < m_transitionsHistory.size(); i++) {
		m_AnimationStep = i;
		repaint();	
		QThread::sleep(1);
	}

	//m_applicationState = ApplicationState::Non_Animating;
	messageBox.exec();
}

void Interfata::CheckWordsFromFile()
{
	QString filter = "Text files (*.txt);;All files (*.*)"; // File filter for dialog
	QString filePath = QFileDialog::getOpenFileName(this, "Open Text File", QDir::homePath(), filter);

	if (!filePath.isEmpty()) {
		QFile file(filePath);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QMessageBox::warning(this, "Read Error", "Could not open the file for reading.");
			return;
		}

		QTextStream in(&file);
		// Now 'in' can be used to read from the file
		QString fileContent = in.readAll();

		// Do something with the content...
		// For example, if you want to set it to a QTextEdit:
		// ui->textEdit->setPlainText(fileContent);

		file.close();
	}
	else {
		// No file was selected
	}
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

void Interfata::DrawPreviousState(QPainter& painter, State* state, QString word)
{
	const auto& [x, y] = state->GetCoordinate();
	painter.setPen(QPen(Qt::blue));
	QRect r(x - m_radius / 2, y - m_radius / 2, m_radius, m_radius);
	QRect textRect(x + m_radius/2+5, y - m_radius/2-5, m_radius, 15 );
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

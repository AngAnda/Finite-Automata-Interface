#include "CheckWords.h"

CheckWords::CheckWords(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CheckWordsClass())
{
	ui->setupUi(this);
}

CheckWords::~CheckWords()
{
	delete ui;
}

void CheckWords::AddAcceptedWords(const QString& word) {
    ui->listWidgetAccepted->addItem(word);
}

void CheckWords::AddRejectedWords(const QString& word) {
    ui->listWidgetRejected->addItem(word);
}

void CheckWords::Clear() {
    ui->listWidgetAccepted->clear();
    ui->listWidgetRejected->clear();
}
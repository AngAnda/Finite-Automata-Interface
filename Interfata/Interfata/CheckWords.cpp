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

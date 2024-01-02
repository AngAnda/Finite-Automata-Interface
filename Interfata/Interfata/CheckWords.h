#pragma once

#include <QWidget>
#include "ui_CheckWords.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CheckWordsClass; };
QT_END_NAMESPACE

class CheckWords : public QWidget
{
	Q_OBJECT

public:
	CheckWords(QWidget *parent = nullptr);
	~CheckWords();
	void AddAcceptedWords(std::string word);
	void AddRejectedWords(std::string word);
	void Clear();

private:
	Ui::CheckWordsClass *ui;
};

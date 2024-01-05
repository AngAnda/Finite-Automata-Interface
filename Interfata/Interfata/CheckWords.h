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
	void AddAcceptedWords(const QString& word);
	void AddRejectedWords(const QString& word);
	void Clear();

private:
	Ui::CheckWordsClass *ui;
};

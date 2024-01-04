#include "Interfata.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	Interfata w;
	w.show();
	return a.exec();
}

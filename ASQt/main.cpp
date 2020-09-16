#include "ASQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	ASQt w;

	return w.Open() ? a.exec() : 0;
}
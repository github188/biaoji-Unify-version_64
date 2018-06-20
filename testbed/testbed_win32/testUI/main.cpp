#include "testgui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	testgui w;
	w.show();
	return a.exec();
}

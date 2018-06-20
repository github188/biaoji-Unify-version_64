#include "testui.h"

testUI::testUI(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	clk.start(20);
	QObject::connect(&clk, SIGNAL(timeout()), this, SLOT(start()));
	QObject::connect(&clk, SIGNAL(timeout()), this, SLOT(process()));
}

testUI::~testUI()
{

}



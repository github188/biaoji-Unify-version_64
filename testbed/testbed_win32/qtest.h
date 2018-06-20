#ifndef QTTEST_H
#define QTTEST_H

#include <QtWidgets/QMainWindow>
#include "mytest.h"

class Qttest : public QDialog
{
    Q_OBJECT

public:
    Qttest(QWidget *parent = 0);
    ~Qttest();

private:
   // Ui::QttestClass ui;
	Ui::Dialog ui;
};

#endif // QTTEST_H
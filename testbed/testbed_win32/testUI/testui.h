#ifndef TESTUI_H
#define TESTUI_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QtWidgets/QMainWindow>
#include "ui_testui.h"
#include <QTimer>
#include <vector>
using namespace cv;
#define PI 3.14159

class testUI : public QMainWindow
{
	Q_OBJECT

public:
	testUI(QWidget *parent = 0);
	~testUI();
	Mat image1;
	QImage show_image1;
	QImage show_image2;
	QTimer clk;
	int flag_1 ;
	int flag_2 ;

private:
	Ui::testguiClass ui;
private slots:
	void start();
	void process();
	void flag1();//开始按钮
	void flag2();//处理按钮
};

#endif // TESTUI_H

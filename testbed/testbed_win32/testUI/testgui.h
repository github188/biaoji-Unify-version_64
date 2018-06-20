#ifndef TESTGUI_H
#define TESTGUI_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include <QStatusBar>
#include <QTextCodec> 
#include "ui_testgui.h"
#include <QTimer>
#include <vector>
#include"HYAMR_meterReg.h"
using namespace cv;
#define PI 3.14159

typedef struct
{
	MPOINT maskPt[10];
	MLong lMaskPtNum;
}MaskParam;

class testgui : public QMainWindow
{
	Q_OBJECT

public:
	testgui(QWidget *parent = 0);
	~testgui();
	
	double step;
	double lTmpMin;
	Mat image1;
	Mat image2;
	QImage show_image1;
	QImage show_image2;
	QTimer clk;
	int flag_1 ;
	int flag_2 ;
	int MeterReadRecog(HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam);	//表盘识别
protected:  
    void mousePressEvent(QMouseEvent *e);       //--鼠标按下事件  
   // void mouseMoveEvent(QMouseEvent *e);    //--鼠标移动事件  
    void mouseReleaseEvent(QMouseEvent *e); //--鼠标释放（松开）事件  
    void mouseDoubleClickEvent(QMouseEvent *e); //--鼠标双击事件  
private:
	Ui::testguiClass ui;
	QLabel *statusLabel;                //---显示鼠标移动时的实时位置   
    QLabel *mousePointLabel;        //---显示鼠标位置 
private slots:
	
	//void process();
	//void pwithe();
	
	void start();
	void flag1();//开始按钮
	void flag2();//处理按钮
	void flag3();
	void minnum();
	void buchang();
	void zhizhennum();
	void color1();
	void color2();
	void withe();
	void xingzhuang();
	void leixing();
	void qufendu();
	void fenxi();
};

//class MouseEvent : public QMainWindow  
//{  
//    Q_OBJECT  
//public:  
//    MouseEvent(QWidget *parent = 0);  
//  
//protected:  
//    void mousePressEvent(QMouseEvent *e);       //--鼠标按下事件  
//    void mouseMoveEvent(QMouseEvent *e);    //--鼠标移动事件  
//    void mouseReleaseEvent(QMouseEvent *e); //--鼠标释放（松开）事件  
//    void mouseDoubleClickEvent(QMouseEvent *e); //--鼠标双击事件  
//  
//private:  
//    QLabel *statusLabel;                //---显示鼠标移动时的实时位置   
//    QLabel *mousePointLabel;        //---显示鼠标位置  
//};  
#endif // TESTGUI_H

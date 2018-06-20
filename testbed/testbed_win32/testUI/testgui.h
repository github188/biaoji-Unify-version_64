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
	int MeterReadRecog(HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam);	//����ʶ��
protected:  
    void mousePressEvent(QMouseEvent *e);       //--��갴���¼�  
   // void mouseMoveEvent(QMouseEvent *e);    //--����ƶ��¼�  
    void mouseReleaseEvent(QMouseEvent *e); //--����ͷţ��ɿ����¼�  
    void mouseDoubleClickEvent(QMouseEvent *e); //--���˫���¼�  
private:
	Ui::testguiClass ui;
	QLabel *statusLabel;                //---��ʾ����ƶ�ʱ��ʵʱλ��   
    QLabel *mousePointLabel;        //---��ʾ���λ�� 
private slots:
	
	//void process();
	//void pwithe();
	
	void start();
	void flag1();//��ʼ��ť
	void flag2();//����ť
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
//    void mousePressEvent(QMouseEvent *e);       //--��갴���¼�  
//    void mouseMoveEvent(QMouseEvent *e);    //--����ƶ��¼�  
//    void mouseReleaseEvent(QMouseEvent *e); //--����ͷţ��ɿ����¼�  
//    void mouseDoubleClickEvent(QMouseEvent *e); //--���˫���¼�  
//  
//private:  
//    QLabel *statusLabel;                //---��ʾ����ƶ�ʱ��ʵʱλ��   
//    QLabel *mousePointLabel;        //---��ʾ���λ��  
//};  
#endif // TESTGUI_H

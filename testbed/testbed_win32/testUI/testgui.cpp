#include "testgui.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#include "putText.h"

#include "wyMatchRigidBodyIF.h"
#include "wyMatchRigidBodyStruct.h"

#ifdef DEBUG_DEMO
#define WY_DEBUG_PRINT printf
#else
#define WY_DEBUG_PRINT
#endif

#define GO(FUNCTION)		{if((res = FUNCTION) != 0) goto EXT;}
#define WORK_BUFFER 100000000
#define MR_MEM_SIZE (100*1024*1024)

#define MAX_BUFFER_LEN 5

// save descriptor info
#define MR_READ_FILE_PARA "D:\\MeterRead.dat"

#define SOURCE_ROAD	"1.mp4"
#define SOURCE_JPG  "1.jpg"



typedef struct
{
	MLong cx;
	MLong cy;
	MLong r;
}MyCircleParam;

typedef struct
{
	MLong left;
	MLong top;
	MLong right;
	MLong bottom;
}MyRect;

int mouseX=-1,mouseY=-1,mouseFlag=0;
int flagEnd = 0;
MVoid onMouse(int Event,int x,int y,int flags,void* param );

unsigned char *gTrainTarget = MNull;
unsigned char *gCurTarget = MNull;
unsigned char *gMeterDes = MNull;
MLong gDesSize;

int MeterReadTrain(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam);	//表盘训练
//int MeterReadRecog(HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam);	//表盘识别
int RidMatchTrain(int trainType, int rotateEn);	//开关状态训练
int RidMatchRecog();	//开关状态识别
void markPoints(IplImage *srcImage, MPOINT *ptList, MLong lPtNum);
void setMaskPic(IplImage *maskImage, MaskParam *pParam,MLong *Dist);
void FittingCircle(MPOINT *pPtList, MLong lPtLen, MLong *xc, MLong *yc, MLong *r);

MFloat lScale = 1.0;


HYAMR_INTERACTIVE_PARA para = {0};
MaskParam mParam = {0};

//IplImage *pFrame = NULL; 

//void testgui::createHandles()
//{
//	pMem = NULL;
//	pMem = malloc(MR_MEM_SIZE);
//	if (!pMem)
//	{
//		WY_DEBUG_PRINT("malloc 100*1024*1024 error!!!\n");
//		//return -1;
//	}
//	hMemMgr = NULL;
//	hMemMgr = HYAMR_MemMgrCreate(pMem, MR_MEM_SIZE);
//	if (!hMemMgr)
//	{
//		WY_DEBUG_PRINT("HYAMR_MemMgrCreate error!!!\n");
//		free(pMem);
//		pMem = MNull;
//		//return -1;
//	}
//	hHYMRDHand = NULL;
//	HYAMR_Init (hMemMgr, &hHYMRDHand);
//	if (!hHYMRDHand)
//	{
//		WY_DEBUG_PRINT("HYAMR_Init error!!!\n");
//		HYAMR_MemMgrDestroy (hMemMgr);
//		free(pMem);
//		pMem = MNull;
//		//return -1;
//	}
//}
testgui::testgui(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//createHandles();
	QString a=ui.buchang->text();
	QString b=ui.zuixiaozhi->text();
	step=a.toDouble();
	lTmpMin=b.toDouble();
	para.lLineNum=ui.zhizhenshu->currentIndex()+1;
	para.lLineColor[0] = HY_LINE_BLACK;
	para.lLineColor[1] = HY_LINE_BLACK;
	para.bWhiteBackground=MTrue;
	para.lLineWidth = 2;
	para.picLevel = HY_LOW_BLUR;

	
	 ////----Qt5解决中文乱码  
  //  QTextCodec *codec = QTextCodec::codecForName("GB18030");  
  //    
  //  //---显示鼠标移动时的实时位置   
  //  statusLabel = new QLabel();  
  //  statusLabel->setText(codec->toUnicode("当前位置:"));  
  //  statusLabel->setFixedWidth(100);  
  //
  //  //---显示鼠标位置  
  //  mousePointLabel = new QLabel();  
  //  mousePointLabel->setText("");  
  //  mousePointLabel->setFixedWidth(100);  
  //    
  //  //---在状态栏增加控件  
  //  statusBar()->addPermanentWidget(statusLabel);  
  //  statusBar()->addPermanentWidget(mousePointLabel);  
  //
  //  //---设置当前窗体对鼠标追踪，默认为false，false表示不追踪  
  //  setMouseTracking(true);  
  //
  //  //----设置窗口属性  
  //  setWindowTitle(codec->toUnicode("鼠标事件信息"));  
  //  //----窗口大小  
  //  resize(400, 150);  

	//clk.start(20);
	//QObject::connect(&clk, SIGNAL(timeout()), this, SLOT(display()));
	//QObject::connect(&clk, SIGNAL(timeout()), this, SLOT(process()));
	//QObject::connect(&clk, SIGNAL(timeout()), this, SLOT(pwithe()));

}

testgui::~testgui()
{

}

void testgui::flag1()
{
	void* hMemMgr;
	void* hHYMRDHand ;
	void* pMem ;
	int i,j;
	int rect[4] = {0};
	int mouseParam[3];
	CvPoint startPt = {0};
	CvPoint endPt = {0};
	int flag = 0;
	CvSize ImgSize;
	MLong lMemSize;
	HYAMR_IMAGES wy_testImage = {0};
	HYAMR_IMAGES wy_maskImage = {0};
	unsigned char *pData = MNull;
	IplImage *testImage = MNull;
	IplImage *tmpImage = MNull;
	IplImage *maskImage = MNull;
	pMem = NULL;
	pMem = malloc(MR_MEM_SIZE);
	if (!pMem)
	{
		WY_DEBUG_PRINT("malloc 100*1024*1024 error!!!\n");
		//return -1;
	}
	hMemMgr = NULL;
	hMemMgr = HYAMR_MemMgrCreate(pMem, MR_MEM_SIZE);
	if (!hMemMgr)
	{
		WY_DEBUG_PRINT("HYAMR_MemMgrCreate error!!!\n");
		free(pMem);
		pMem = MNull;
		//return -1;
	}
	hHYMRDHand = NULL;
	HYAMR_Init (hMemMgr, &hHYMRDHand);
	if (!hHYMRDHand)
	{
		WY_DEBUG_PRINT("HYAMR_Init error!!!\n");
		HYAMR_MemMgrDestroy (hMemMgr);
		free(pMem);
		pMem = MNull;
		//return -1;
	}
	IplImage *pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);

	ImgSize.height = pFrame->height / lScale;
	ImgSize.width = pFrame->width / lScale;
	testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
	cvResize(pFrame, testImage, CV_INTER_LINEAR);
	cvSaveImage("D:\\testImage.bmp", testImage);
	cvNamedWindow("TrainImage", 0);
	Mat m=Mat(testImage,true);
	putTextZH(m,"圈定目标对象\n",Point(50,50),Scalar(0,0,255),40,"微软雅黑");
	testImage=cvCloneImage(&(IplImage)m);
	cvSaveImage("D:\\testImage.bmp", testImage);

	cvSetMouseCallback("TrainImage", onMouse, (void*)mouseParam);
	tmpImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
	cvResize(testImage, tmpImage, CV_INTER_LINEAR);
	mouseParam[0]=-1;
	mouseParam[1]=-1;
	mouseParam[2]=-1;
	while (1)
	{
		if (mouseParam[2]==CV_EVENT_LBUTTONDOWN)
		{
			startPt.x=mouseParam[0];
			startPt.y=mouseParam[1];
			endPt.x=mouseParam[0];
			endPt.y=mouseParam[1];
			
			cvReleaseImage(&tmpImage);
			tmpImage = cvLoadImage("D:\\testImage.bmp",CV_LOAD_IMAGE_COLOR);
			cvRectangle(tmpImage, startPt, endPt, CV_RGB(0,0,255), 1);
			flag=1;
			cvShowImage("TrainImage",tmpImage);
		}
		if (mouseParam[2]==CV_EVENT_MOUSEMOVE && flag==1)
		{
			endPt.x=mouseParam[0];
			endPt.y=mouseParam[1];
		
			cvReleaseImage(&tmpImage);
			tmpImage = cvLoadImage("D:\\testImage.bmp",CV_LOAD_IMAGE_COLOR);
			cvRectangle(tmpImage, startPt, endPt, CV_RGB(0,0,255), 1);
			cvShowImage("TrainImage",tmpImage);
		}
		if (1 == flagEnd)
		{
			break;
		}
		cvShowImage("TrainImage",tmpImage);
		cvWaitKey(10);
	}
	
	maskImage = cvCreateImage(ImgSize, 8, 1);
	pData = (unsigned char *)(maskImage->imageData);
	for (j=0;j<ImgSize.height;j++)
	{
		for (i=0;i<ImgSize.width;i++)
		{
			if (j>=startPt.y&&j<=endPt.y&&i>=startPt.x&&i<=endPt.x)
			{
				pData[j*maskImage->widthStep+i]=255;
			}
			else
			{
				pData[j*maskImage->widthStep+i]=0;
			}
		}
	}
	cvSaveImage("D:\\maskImage.bmp",maskImage);

	rect[0] = maskImage->width;
	rect[1] = maskImage->height;
	for (j=0;j<maskImage->height;j++)
	{
		for (i=0;i<maskImage->width;i++)
		{
			if (pData[j*maskImage->widthStep+i]>100)
			{
				//count++;
				if(rect[0]>i) 
				{
					rect[0]=i;
				}
				if(rect[2]<i)
				{
					rect[2]=i;
				}
				if (rect[1]>j)
				{
					rect[1]=j;
				}
				if(rect[3]<j)
				{
					rect[3]=j;
				}
			}
		}
	}

	wy_testImage.lWidth = testImage->width;
	wy_testImage.lHeight = testImage->height;
	wy_testImage.pixelArray.chunky.lLineBytes = testImage->widthStep;
	wy_testImage.pixelArray.chunky.pPixel = testImage->imageData;
	wy_testImage.lPixelArrayFormat = HYAMR_IMAGE_BGR;
	//mask
	wy_maskImage.lWidth = maskImage->width;
	wy_maskImage.lHeight = maskImage->height;
	wy_maskImage.pixelArray.chunky.lLineBytes = maskImage->widthStep;
	wy_maskImage.pixelArray.chunky.pPixel = maskImage->imageData;
	wy_maskImage.lPixelArrayFormat = HYAMR_IMAGE_GRAY;

	WY_DEBUG_PRINT("~~~~~~~~~~Train meter~~~~~~~~~~~~\n");

	if (0 != HYAMR_TrainTemplateFromMask (hHYMRDHand, &wy_testImage, &wy_maskImage, "OK", 0))
	{
		WY_DEBUG_PRINT("HYAMR_TrainTemplateFromMask error!\n");
		//res = -1;
		//goto EXT;
	}

	lMemSize = testImage->width * testImage->height;
	if (MNull != gMeterDes)
	{
		memset(gMeterDes, 0, lMemSize * sizeof(unsigned char));
	}
	else
	{
		gMeterDes = (unsigned char *)malloc(lMemSize * sizeof(unsigned char));
	}
	//if (0 != HYAMR_SaveTDescriptorsGroup(hHYMRDHand, MR_READ_FILE_PARA))
	if(0 != HYAMR_SaveDesMem (hHYMRDHand, gMeterDes, lMemSize, &gDesSize))
	{
		WY_DEBUG_PRINT("HYAMR_SaveTDescriptorsGroup error!\n");
		//res = -1;
		//goto EXT;
	}

	//训练的时候为什么要进行配准 一旦不准 就全不准了
	HYAMR_GetObjRect(hHYMRDHand, &wy_testImage, "OK", 0.75);	// 进行一次匹配操作，获取目标对象位置

	if(MNull != gCurTarget)
		memset(gCurTarget, 0, 32 * sizeof(unsigned char));
	else
		gCurTarget = (unsigned char *)malloc(32 * sizeof(unsigned char));

	if(MNull != gTrainTarget)
		memset(gTrainTarget, 0, 32 * sizeof(unsigned char));
	else
		gTrainTarget = (unsigned char *)malloc(32 * sizeof(unsigned char));

	
	//if(0 != HYAMR_SaveTargetInfoToMem (hHYMRDHand, gCurTarget, gTrainTarget, 1))
	//	//if (0 != HYAMR_SaveTargetInfo(hHYMRDHand, MR_READ_FILE_TARGET, MR_TRAIN_TARGET, 1))
	//{
	//	WY_DEBUG_PRINT("HYAMR_SaveTargetInfo error!\n");
	//	res = -1;
	//	goto EXT;
	//}

	if(0 != HYAMR_SaveTargetInfoToMemTmp (hHYMRDHand, gCurTarget, gTrainTarget, 1,rect))
		//if (0 != HYAMR_SaveTargetInfo(hHYMRDHand, MR_READ_FILE_TARGET, MR_TRAIN_TARGET, 1))
	{
		WY_DEBUG_PRINT("HYAMR_SaveTargetInfo error!\n");
		//res = -1;
		//goto EXT;
	}
	IplImage *yolo = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	image1=Mat(yolo,true);
	putTextZH(image1,"点击标刻度\n",Point(50,yolo->height/2),Scalar(0,0,255),40,"微软雅黑");
	cvtColor(image1, image1, CV_BGR2RGB);
	show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
	ui.pic_1->setPixmap(QPixmap::fromImage(show_image1)); 
	cvReleaseImage(&yolo);
	cvReleaseImage(&testImage);
	cvReleaseImage(&tmpImage);
	cvReleaseImage(&maskImage);
	cvReleaseImage(&pFrame);
	HYAMR_Uninit (hHYMRDHand);
	HYAMR_MemMgrDestroy (hMemMgr);
	if(MNull!=pMem)
	{	
		free(pMem);
		pMem = MNull;
	}
	cvDestroyWindow("TrainImage");
}

void testgui::flag2()
{
	int i, j;
	int lPtNumTmp;
	MPOINT ptListTmp[50];
	IplImage *pFrame = MNull;
	IplImage *testImage = MNull;
	CvSize ImgSize;
	int mouseParam[3];
	pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	ImgSize.height = pFrame->height / lScale;
	ImgSize.width = pFrame->width / lScale;
	testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
	cvResize(pFrame, testImage, CV_INTER_LINEAR);
	cvNamedWindow("TrainImage", 0);
	Mat m=Mat(testImage,true);
	putTextZH(m,"大刻度点从小往大标记: 右键添加，左键删除，回车键结束\n",Point(50,50),Scalar(0,0,255),40,"微软雅黑");
	testImage=cvCloneImage(&(IplImage)m);
	cvShowImage("TrainImage", testImage);
	cvWaitKey(1);
	WY_DEBUG_PRINT("大刻度点从小往大标记: 右键添加，左键删除\n");
	mouseParam[0]=-1;
	mouseParam[1]=-1;
	mouseParam[2]=-1;
	cvSetMouseCallback("TrainImage", onMouse, (void*)mouseParam);
	lPtNumTmp = 0;
	while(1)
	{
		if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
		{
			if (mouseParam[0]>=0 && mouseParam[0]<testImage->width
				&& mouseParam[1]>=0 && mouseParam[1]<testImage->height)
			{
				ptListTmp[lPtNumTmp].x = mouseParam[0];
				ptListTmp[lPtNumTmp].y = mouseParam[1];
				lPtNumTmp++;
				cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),2,CV_RGB(255,0,0),-1,8,0);
			}
			WY_DEBUG_PRINT("lPtNum=%d\n", lPtNumTmp);
			mouseParam[2] = -1;
		}
		else if (CV_EVENT_LBUTTONDOWN == mouseParam[2])		// delete white circle
		{
			for(i=0; i<lPtNumTmp; i++)
			{
				if (abs(mouseParam[0]-ptListTmp[i].x)<5 && abs(mouseParam[1]-ptListTmp[i].y)<5)
				{
					cvCircle(testImage, cvPoint(ptListTmp[i].x, ptListTmp[i].y), 2, CV_RGB(255,255,255), -1, 8, 0);
					for(j=i; j<lPtNumTmp-1; j++)
						ptListTmp[j] = ptListTmp[j+1];
					lPtNumTmp--;
					break;
				}
			}
			mouseParam[2] = -1;
		}
		cvShowImage("TrainImage",testImage);
		if(13==cvWaitKey(10))	// Enter
			break;
	}
	WY_DEBUG_PRINT("lPtNum=%d\n", lPtNumTmp);
	cvDestroyWindow("TrainImage");
	para.lPtNum = lPtNumTmp;
	for (i=0; i<lPtNumTmp; i++)
	{
		para.ptPosList[i] = ptListTmp[i];
	}
	for (i=0; i<para.lPtNum; i++)
	{
		para.dPtValList[i] = (MDouble)lTmpMin;
		lTmpMin=lTmpMin+step;
	}
	IplImage *yolo = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	image1=Mat(yolo,true);
	putTextZH(image1,"填写步长，最小值\n点击标表心\n",Point(50,yolo->height/2),Scalar(0,0,255),40,"微软雅黑");
	cvtColor(image1, image1, CV_BGR2RGB);
	show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
	ui.pic_1->setPixmap(QPixmap::fromImage(show_image1)); 
	cvReleaseImage(&yolo);
	cvReleaseImage(&testImage);
	cvReleaseImage(&pFrame);
}
void testgui::buchang()
{
	int i;
	QString a=ui.buchang->text();
	QString c=ui.zuixiaozhi->text();
	double b=a.toDouble();
	double d=c.toDouble();
	step=b;
	lTmpMin=d;
	for (i=0; i<para.lPtNum; i++)
	{
		para.dPtValList[i] = (MDouble)lTmpMin;
		lTmpMin=lTmpMin+step;
	}
}
void testgui::minnum()
{
	int i;
	QString a=ui.buchang->text();
	QString c=ui.zuixiaozhi->text();
	double b=a.toDouble();
	double d=c.toDouble();
	step=b;
	lTmpMin=d;
	for (i=0; i<para.lPtNum; i++)
	{
		para.dPtValList[i] = (MDouble)lTmpMin;
		lTmpMin=lTmpMin+step;
	}
}
void testgui::flag3()
{
	if(para.lPtNum>2)
	{
		int mouseParam[3];
		int lPtNumTmp;
		IplImage *pFrame = MNull;
		IplImage *testImage = MNull;
		CvSize ImgSize;	
		pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
		ImgSize.height = pFrame->height / lScale;
		ImgSize.width = pFrame->width / lScale;
		testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
		cvResize(pFrame, testImage, CV_INTER_LINEAR);
		cvNamedWindow("TrainImage", 0);
		Mat m=Mat(testImage,true);
		putTextZH(m,"请圈定圆心位置,右键添加，回车键结束\n",Point(50,50),Scalar(0,0,255),40,"微软雅黑");
		testImage=cvCloneImage(&(IplImage)m);
		cvShowImage("TrainImage", testImage);
		cvWaitKey(1);
		WY_DEBUG_PRINT("请圈定圆心位置,右键添加：\n");
		mouseParam[0]=-1;
		mouseParam[1]=-1;
		mouseParam[2]=-1;
		cvSetMouseCallback("TrainImage", onMouse, (void*)mouseParam);
		lPtNumTmp = 0;
		while(1)
		{
			if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
			{
				if (mouseParam[0]>=0 && mouseParam[0]<testImage->width
					&& mouseParam[1]>=0 && mouseParam[1]<testImage->height)
				{
					para.circleCoord.x = mouseParam[0];
					para.circleCoord.y = mouseParam[1];
					lPtNumTmp++;
					cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),2,CV_RGB(255,0,0),-1,8,0);
				}
				WY_DEBUG_PRINT("lPtNum=%d\n", lPtNumTmp);
				mouseParam[2] = -1;
			}
			cvShowImage("TrainImage",testImage);
			if(13==cvWaitKey(10))	// Enter
				break;
		}
		cvDestroyWindow("TrainImage");
		IplImage *yolo = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
		image1=Mat(yolo,true);
		putTextZH(image1,"组合框根据实际情况选择\n",Point(50,yolo->height/2),Scalar(0,0,255),40,"微软雅黑");
		cvtColor(image1, image1, CV_BGR2RGB);
		show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
		ui.pic_1->setPixmap(QPixmap::fromImage(show_image1)); 
		cvReleaseImage(&yolo);
		cvReleaseImage(&testImage);
		cvReleaseImage(&pFrame);
	}
}
void testgui::zhizhennum()
{
	para.lLineNum=ui.zhizhenshu->currentIndex()+1;
}
void testgui::color1()
{
	if (0==ui.color1->currentIndex())
	{
		para.lLineColor[0] = HY_LINE_BLACK;
	}
	else if (1==ui.color1->currentIndex())
	{
		para.lLineColor[0] = HY_LINE_WHITE;
	}
	else if(2==ui.color1->currentIndex())
	{
		para.lLineColor[0] = HY_LINE_RED;
	}
	else
	{
		para.lLineColor[0] = HY_LINE_YELLOW;
	}
}
void testgui::color2()
{
	
	if (0==ui.color2->currentIndex())
	{
		para.lLineColor[1] = HY_LINE_BLACK;
	}
	else if (1==ui.color2->currentIndex())
	{
		para.lLineColor[1] = HY_LINE_WHITE;
	}
	else if(2==ui.color2->currentIndex())
	{
		para.lLineColor[1] = HY_LINE_RED;
	}
	else
	{
		para.lLineColor[1] = HY_LINE_YELLOW;
	}
	
}
void testgui::withe()
{
	if(0==ui.beijing->currentIndex())
	{
		para.bWhiteBackground=MTrue;
	}
	else
	{
		para.bWhiteBackground = MFalse;
	}
}
void testgui::xingzhuang()
{
	if(0==ui.xingzhuang->currentIndex())
	{
		para.lLineWidth = 2;
	}
	else
	{
		MPOINT ptListTmp[50];
		int dist[5];
		int mouseParam[3];
		int lPtNumTmp;
		MLong lHaarWidth=0;
		double temp;
		IplImage *pFrame = MNull;
		IplImage *testImage = MNull;
		CvSize ImgSize;	
		pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
		ImgSize.height = pFrame->height / lScale;
		ImgSize.width = pFrame->width / lScale;
		testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
		cvResize(pFrame, testImage, CV_INTER_LINEAR);
		cvNamedWindow("TrainImage", 0);
		Mat m=Mat(testImage,true);
		putTextZH(m,"请在指针线上标记3组点用于计算指针线宽（包含指针线两端）\n",Point(50,50),Scalar(0,0,255),40,"微软雅黑");
		testImage=cvCloneImage(&(IplImage)m);

		cvShowImage("TrainImage", testImage);
		cvWaitKey(1);
		WY_DEBUG_PRINT("请在指针线上标记3组点用于计算指针线宽（包含指针线两端）:\n");
		mouseParam[0]=-1;
		mouseParam[1]=-1;
		mouseParam[2]=-1;
		cvSetMouseCallback("TrainImage", onMouse, (void*)mouseParam);
		lPtNumTmp = 0;
		while(1)
		{
			if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
			{
				if (mouseParam[0]>=0 && mouseParam[0]<testImage->width
					&& mouseParam[1]>=0 && mouseParam[1]<testImage->height)
				{
					ptListTmp[lPtNumTmp].x = mouseParam[0];
					ptListTmp[lPtNumTmp].y = mouseParam[1];
					lPtNumTmp++;
					printf("%d,%d\n", mouseParam[0], mouseParam[1]);
					cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),2,CV_RGB(0,255,0),-1,8,0);
				}
				mouseParam[2] = -1;
			}
			cvShowImage("TrainImage",testImage);
			if(13==cvWaitKey(10) || lPtNumTmp>=6)	// Enter
				break;
		}
		cvDestroyWindow("TrainImage");
		for (int i=0;i<lPtNumTmp/2;i++)
		{
			temp = (ptListTmp[2*i].x-ptListTmp[2*i+1].x)*(ptListTmp[2*i].x-ptListTmp[2*i+1].x)+
				(ptListTmp[2*i].y-ptListTmp[2*i+1].y)*(ptListTmp[2*i].y-ptListTmp[2*i+1].y);
			dist[i] = sqrt(temp);
			lHaarWidth +=dist[i];
		}
		lHaarWidth /=lPtNumTmp/2;
		//HYAMR_CalcHaarWidth(hHYMRDHand, &wy_testImage, pOutPattern->lLineColor, ptListTmp, lPtNumTmp, &lTmpVal);
		para.lLineWidth = lHaarWidth;
		//pOutPattern->lLineWidth = lHaarWidth;
		cvReleaseImage(&testImage);
		cvReleaseImage(&pFrame);
	}
}
void testgui::leixing()
{
	int lPtNumTmp=0;
	int lTmpVal=ui.leixing->currentIndex()+1;
	switch(lTmpVal)
	{
		case 1:
			lPtNumTmp = 0;
			break;
		case 2:
			lPtNumTmp = 3;
			break;
		case 3:
			lPtNumTmp = 6;
			break;
		case 4:
			lPtNumTmp = 8;
			break;
		case 5:
			lPtNumTmp = 5;
			break;
		case 6:
			lPtNumTmp = 7;
			break;
		case 7:
			lPtNumTmp = 9;
			break;

		default:
			lPtNumTmp = 0;
	}
	if (lPtNumTmp>0)
	{
		int mouseParam[3];
		IplImage *pFrame = MNull;
		IplImage *testImage = MNull;
		CvSize ImgSize;	
		pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
		ImgSize.height = pFrame->height / lScale;
		ImgSize.width = pFrame->width / lScale;
		testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
		cvResize(pFrame, testImage, CV_INTER_LINEAR);
		cvNamedWindow("TrainImage", 0);
		char text[256]={0};
		sprintf(text,"标记%d个点，构造MASK图\n",lPtNumTmp);
		Mat m=Mat(testImage,true);
		putTextZH(m,text,Point(50,50),Scalar(0,0,255),40,"微软雅黑");
		testImage=cvCloneImage(&(IplImage)m);
		cvShowImage("TrainImage", testImage);
		cvWaitKey(1);
		WY_DEBUG_PRINT("标记%d个点，构造MASK图\n", lPtNumTmp);
		mouseParam[0]=-1;
		mouseParam[1]=-1;
		mouseParam[2]=-1;
		cvSetMouseCallback("TrainImage", onMouse, (void*)mouseParam);
		mParam.lMaskPtNum = lPtNumTmp;
		//pMaskParam->lMaskPtNum = lPtNumTmp;
		lPtNumTmp = 0;
		while(1)
		{
			if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
			{
				if (mouseParam[0]>=0 && mouseParam[0]<testImage->width
					&& mouseParam[1]>=0 && mouseParam[1]<testImage->height)
				{
					mParam.maskPt[lPtNumTmp].x = mouseParam[0];
					mParam.maskPt[lPtNumTmp].y = mouseParam[1];
					lPtNumTmp++;
					printf("%d,%d\n", mouseParam[0], mouseParam[1]);
					cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),2,CV_RGB(0,255,0),-1,8,0);
				}
				mouseParam[2] = -1;
			}
			cvShowImage("TrainImage",testImage);
			if(13==cvWaitKey(10) || lPtNumTmp>=mParam.lMaskPtNum)	// Enter
				break;
		}
		cvDestroyWindow("TrainImage");
		cvReleaseImage(&testImage);
		cvReleaseImage(&pFrame);
	}
}
void testgui::qufendu()
{
	int lTmpVal = ui.qufendu->currentIndex()+1;
	if (1==lTmpVal)
	{
		para.picLevel = HY_LOW_BLUR;
	}
	else if(2==lTmpVal)
	{
		para.picLevel = HY_HIGH_BLUR;
	}
	else
	{
		para.picLevel = HY_OTHER_BLUR;
	}
}
void testgui::fenxi()
{
	MeterReadRecog(para, &mParam);
}
int testgui::MeterReadRecog(HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam)
{
	int res = 0;
	inPara.Dist = 0xFFFF;

	MHandle hMemMgr = MNull;
	MHandle hHYMRDHand = MNull;
	MVoid *pMem = MNull;

	CvCapture *pCapture = MNull;
	IplImage *pFrame = MNull;
	IplImage *testImage = MNull;
	IplImage *maskImage = MNull;
	HYAMR_IMAGES wy_testImage = {0};
	HYAMR_IMAGES wy_maskImage = {0};
	CvSize ImgSize ={0};

	MLong lFrameNum = 0;
	CvVideoWriter *pWriter = MNull;

	HYAMR_READ_PARA pReadPara[2] = {0};
	MDouble matchRate;
	int i;

	MDouble dResult;
	MDouble buffer[2][MAX_BUFFER_LEN];
	MLong lBufferLen;
	char textContent[100];
	FILE *fileFp = MNull;
	CvFont font = {0};

//***********************  alloc memory  *******************************
	pMem = malloc(MR_MEM_SIZE);
	if (!pMem)
	{
		WY_DEBUG_PRINT("malloc 50*1024*1024 error!!!\n");
		return -1;
	}

	hMemMgr = HYAMR_MemMgrCreate (pMem, MR_MEM_SIZE);
	if (!hMemMgr)
	{
		WY_DEBUG_PRINT("HYAMR_MemMgrCreate error!!!\n");
		free(pMem);
		pMem = MNull;
		return -1;
	}

	HYAMR_Init (hMemMgr, &hHYMRDHand);
	if (!hHYMRDHand)
	{
		WY_DEBUG_PRINT("HYAMR_Init error!!!\n");
		HYAMR_MemMgrDestroy (hMemMgr);
		free(pMem);
		pMem = MNull;

		return -1;
	}

//***************************  get video  ******************************************************
	pCapture = cvCaptureFromFile(SOURCE_ROAD);
	if (MNull == pCapture)
	{
		WY_DEBUG_PRINT("get video file error!\n");
		goto EXT;
	}
	
	pFrame = cvQueryFrame(pCapture);
	//pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	ImgSize.height = pFrame->height / lScale;
	ImgSize.width = pFrame->width / lScale;
	testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
	cvResize(pFrame, testImage, CV_INTER_LINEAR);

	maskImage = cvCreateImage(ImgSize, 8, 1);
	setMaskPic(maskImage, pMaskParam,&inPara.Dist);

	wy_maskImage.lWidth = maskImage->width;
	wy_maskImage.lHeight = maskImage->height;
	wy_maskImage.pixelArray.chunky.lLineBytes = maskImage->widthStep;
	wy_maskImage.pixelArray.chunky.pPixel = maskImage->imageData;
	wy_maskImage.lPixelArrayFormat = HYAMR_IMAGE_GRAY;

	pWriter = cvCreateVideoWriter("D:\\result_test2.avi", CV_FOURCC('X', 'V', 'I', 'D'), 5, ImgSize);
	if (MNull == pWriter)
	{
		WY_DEBUG_PRINT("pWriter error!\n");
		goto EXT;
	}

	WY_DEBUG_PRINT("~~~~~~~~~~Read meter~~~~~~~~~~~~\n");
	if(0 != HYAMR_GetDesMem (hHYMRDHand, gMeterDes))
	{
		WY_DEBUG_PRINT("HYAMR_GetTemplateFromText error !\n");
		res = -1;
		goto EXT;
	}
	//读取了配准后的目标位置坐标
	if(0 != HYAMR_GetTaregetInfoFromMem (hHYMRDHand, gCurTarget, gTrainTarget, testImage->width, testImage->height))
	{
		WY_DEBUG_PRINT("HYAMR_GetTargetInfo error!\n");
		res = -1;
		goto EXT;
	}

	if (0 != HYAMR_SetParam (hHYMRDHand, &wy_maskImage, &inPara))
	{
		WY_DEBUG_PRINT("HYAMR_SetParam error!\n");
		res = -1;
		goto EXT;
	}

	font = cvFont(2, 1);
	cvNamedWindow("Result", 0);
	lFrameNum = 0;

	matchRate = 0.45;
	  
	//while(1)
	while(pFrame = cvQueryFrame(pCapture))
	{
		//pFrame = cvLoadImage("C:\\a.jpg", CV_LOAD_IMAGE_COLOR);
		//if(lFrameNum%24 > 0)continue;
		lFrameNum++;
		/*if(lFrameNum<90)
			continue;*/
		if(!pFrame)
			break;
		cvResize(pFrame, testImage, CV_INTER_LINEAR);
		WY_DEBUG_PRINT("lFrameNum=%d\n", lFrameNum);
		
		//sprintf(textContent1, "D:\\sf6 images\\testImage_%d.bmp", lFrameNum);
		//cvSaveImage(textContent1, testImage);

		wy_testImage.lWidth = testImage->width;
		wy_testImage.lHeight = testImage->height;
		wy_testImage.pixelArray.chunky.lLineBytes = testImage->widthStep;
		wy_testImage.pixelArray.chunky.pPixel = testImage->imageData;
		wy_testImage.lPixelArrayFormat = HYAMR_IMAGE_BGR;

		// Recognise the pointer line
		if (0 != HYAMR_GetLineParam(hHYMRDHand, &wy_testImage, &inPara, "OK", matchRate, gCurTarget, gTrainTarget, pReadPara))
		{
			WY_DEBUG_PRINT("HYAMR_GetLineParam error!\n");
			res=-2;
			break;
		}

		for (i=0; i<inPara.lPtNum; i++)
		{
			pReadPara[0].ptInfo[i].ptVal = pReadPara[1].ptInfo[i].ptVal = inPara.dPtValList[i];
		}
		if(lFrameNum<MAX_BUFFER_LEN)
			lBufferLen = lFrameNum;
		else
			lBufferLen = MAX_BUFFER_LEN;

		for (i=0; i<inPara.lLineNum; i++)
		{
			HYAMR_GetMeterResult(hHYMRDHand, *(pReadPara+i), &dResult);

			buffer[i][(lFrameNum-1)%MAX_BUFFER_LEN] = dResult;
			dResult = HYAMR_FindMidian(buffer[i], lBufferLen);
			/*if (dResult<35 || dResult>42)
			{
				WY_DEBUG_PRINT("err**********\n");
			}*/
			//sprintf(textContent, "lFrameNum=%d  lineNum[%d] result[%.4f]", lFrameNum, i, dResult);
			//cvPutText(testImage, textContent, cvPoint(400, 150+10*i), &font, CV_RGB(0,255,0));
			WY_DEBUG_PRINT("result=%.4f\n\n", dResult);
			//value=dResult;
			//display();
			if(i == 0)
			{
				ui.result->setText(QString::number(dResult,10,6));
				ui.result->displayText();
			}
			else if(i == 1)
			{
				ui.result2->setText(QString::number(dResult,10,6));
				ui.result2->displayText();
			}
			sprintf(textContent, "D:\\testResult_%d.dat", i);
			fileFp = fopen(textContent, "ab+");
			fprintf(fileFp, "lineNum[%d] result[%.4f] lineInfo[%.4f, %.4f] end[%d,%d]",
				i, dResult, pReadPara[i].dCoeffK, pReadPara[i].dCoeffB, pReadPara[i].ptEnd.x, pReadPara[i].ptEnd.y);
			fclose(fileFp);
		}
		
		cvWriteFrame(pWriter, testImage);
		char text[256]={0};
		sprintf(text,"Frame=%d\n",lFrameNum);
		image1=Mat(testImage,true);
		putTextZH(image1,text,Point(50,100),Scalar(0,0,255),40,"微软雅黑");
		cvtColor(image1, image1, CV_BGR2RGB);
		show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
		ui.pic_1->setPixmap(QPixmap::fromImage(show_image1));

		/*sprintf(text,"表计值为%.2f\n",dResult);
		image2=Mat(testImage,true);
		putTextZH(image2,text,Point(50,100),Scalar(0,255,255),40,"微软雅黑");
		testImage=cvCloneImage(&(IplImage)image2);*/
		cvShowImage("Result", testImage);
		cvWaitKey(500);	// delay 2s
	}
	if(res==-2)
	{
		image1=Mat(testImage,true);
		putTextZH(image1,"HYAMR_GetLineParam error!\n",Point(50,50),Scalar(0,0,255),40,"微软雅黑");
		cvtColor(image1, image1, CV_BGR2RGB);
		show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
		ui.pic_1->setPixmap(QPixmap::fromImage(show_image1));
	}
	else
	{
		image1=Mat(testImage,true);
		putTextZH(image1,"程序运行完毕，点击结束按钮关闭\n",Point(50,50),Scalar(0,0,255),40,"微软雅黑");
		cvtColor(image1, image1, CV_BGR2RGB);
		show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
		ui.pic_1->setPixmap(QPixmap::fromImage(show_image1));
	}
	cvReleaseVideoWriter(&pWriter);

EXT:
	HYAMR_Uninit (hHYMRDHand);
	HYAMR_MemMgrDestroy (hMemMgr);
	if(MNull!=pMem)
	{	
		free(pMem);
		pMem = MNull;
	}

	if (MNull != gMeterDes)
	{
		free(gMeterDes);
		gMeterDes = MNull;
	}

	if(MNull!=gCurTarget)
	{
		free(gCurTarget);
		gCurTarget = MNull;
	}
	if(MNull!=gTrainTarget)
	{
		free(gTrainTarget);
		gTrainTarget = MNull;
	}

	cvReleaseImage(&testImage);
	cvReleaseImage(&maskImage);
	return res;
}
void setMaskPic(IplImage *maskImage, MaskParam *pParam,MLong *Dist)
{
	MByte *pData;
	MLong lWidth, lHeight, lStride;
	MLong lExt;
	MyCircleParam outerCircle, innerCircle, tmpCircle;
	MLong lPoxY;
	MLong i,j;
	MPOINT *tmpPt;
	MPOINT pt1, pt2;
	MLong lDistance1, lDistance2;
	MDouble tmp;

	lWidth = maskImage->width;
	lHeight = maskImage->height;
	lStride = maskImage->widthStep;
	lExt = lStride - lWidth;
	pData = (MByte*)maskImage->imageData;
	tmpPt = pParam->maskPt;

	switch(pParam->lMaskPtNum)
	{
		case 3:
		{
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
			for (j=0; j<lHeight; j++, pData+=lExt)
			{
				for (i=0; i<lWidth; i++, pData++)
				{
					tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
					lDistance1 = (MLong)(sqrt(tmp));
					if (lDistance1 <= outerCircle.r)
					{
						*pData = 255;
					}
					else
					{
						*pData = 0;
					}
				}
			}
			break;
		}
		case 5:
		{
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //前面三点拟合圆
			for (j=0; j<lHeight; j++, pData+=lExt)
			{
				for (i=0; i<lWidth; i++, pData++)
				{
					tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
					lDistance1 = (MLong)(sqrt(tmp));
					if (lDistance1 <= outerCircle.r &&((i<tmpPt[3].x || i>tmpPt[4].x) || (j<tmpPt[3].y || j>tmpPt[4].y)))
					{
						*pData = 255;
					}
					else
					{
						*pData = 0;
					}
				}
			}
			break;
		}
		case 6:
			{   
				//为圆环
				FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
				FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
				if (outerCircle.r < innerCircle.r)
				{
					tmpCircle = outerCircle;
					outerCircle = innerCircle;
					innerCircle = tmpCircle;
				}
				*Dist = outerCircle.r - innerCircle.r ;

				for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
						lDistance1 = (MLong)(sqrt(tmp));
						tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
						lDistance2 = (MLong)(sqrt(tmp));
						if (lDistance1<=outerCircle.r && lDistance2>=innerCircle.r)
						{
							*pData = 255;
						}
						else
						{
							*pData = 0;
						}
					}
				}
				

				break;
			}

		case 7:
		{
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //前面三点拟合圆
			for (j=0; j<lHeight; j++, pData+=lExt)
			{
				for (i=0; i<lWidth; i++, pData++)
				{
					tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
					lDistance1 = (MLong)(sqrt(tmp));
					if (lDistance1 <= outerCircle.r &&((i<tmpPt[3].x || i>tmpPt[4].x) || (j<tmpPt[3].y || j>tmpPt[4].y)) && ((i<tmpPt[5].x || i>tmpPt[6].x) || (j<tmpPt[5].y || j>tmpPt[6].y)))
					{
						*pData = 255;
					}
					else
					{
						*pData = 0;
					}
				}
			}
			break;
		}
		case 8:
			{
				FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
				FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
				if (outerCircle.r < innerCircle.r)
				{
					tmpCircle = outerCircle;
					outerCircle = innerCircle;
					innerCircle = tmpCircle;
				}
				pt1 = tmpPt[6];
				pt2 = tmpPt[7];
				lPoxY = (pt1.y + pt2.y)>>1;
				for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
						lDistance1 = (MLong)(sqrt(tmp));
						tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
						lDistance2 = (MLong)(sqrt(tmp));
						if((i>pt1.x && i<pt2.x && j>pt1.y && j<pt2.y)||lDistance1>=outerCircle.r || lDistance2<=innerCircle.r)
						{
							*pData = 0;
						}
					}
				}

				/*for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
						lDistance1 = (MLong)(sqrt(tmp));
						tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
						lDistance2 = (MLong)(sqrt(tmp));
						if (lDistance1>=outerCircle.r || (lDistance2<=innerCircle.r && j<=lPoxY))
						{
							*pData = 0;
						}
						else
						{
							*pData = 255;
						}
					}
				}*/

				break;
			}
		case 9:
		{
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //前面三点拟合圆
			for (j=0; j<lHeight; j++, pData+=lExt)
			{
				for (i=0; i<lWidth; i++, pData++)
				{
					tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
					lDistance1 = (MLong)(sqrt(tmp));
					if (lDistance1 <= outerCircle.r &&((i<tmpPt[3].x || i>tmpPt[4].x) || (j<tmpPt[3].y || j>tmpPt[4].y))
						&& ((i<tmpPt[5].x || i>tmpPt[6].x) || (j<tmpPt[5].y || j>tmpPt[6].y)) && ((i<tmpPt[7].x || i>tmpPt[8].x) || (j<tmpPt[7].y || j>tmpPt[8].y)))
					{
						*pData = 255;
					}
					else
					{
						*pData = 0;
					}
				}
			}
			break;
		}
		case 0:
		default:
			{
				for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						*pData = 255;
					}
				}
				break;
			}
	}
	//cvSaveImage("D:\\maskImage.bmp", maskImage);
	//cvShowImage("maskImage", maskImage);
	cvWaitKey(10);
}
void FittingCircle(MPOINT *pPtList, MLong lPtLen, 
					   MLong *xc, MLong *yc, MLong *r)
{
	MLong i;
	MDouble X1=0,Y1=0,X2=0,Y2=0,X3=0,Y3=0,X1Y1=0,X1Y2=0,X2Y1=0;
	MDouble C,D,E,G,H,N;
	MDouble a,b,c;
	if (lPtLen<3)
	{
		return;
	}

	for (i=0;i<lPtLen;i++)
	{
		X1 = X1 + pPtList[i].x;
		Y1 = Y1 + pPtList[i].y;
		X2 = X2 + pPtList[i].x*pPtList[i].x;
		Y2 = Y2 + pPtList[i].y*pPtList[i].y;
		X3 = X3 + pPtList[i].x*pPtList[i].x*pPtList[i].x;
		Y3 = Y3 + pPtList[i].y*pPtList[i].y*pPtList[i].y;
		X1Y1 = X1Y1 + pPtList[i].x*pPtList[i].y;
		X1Y2 = X1Y2 + pPtList[i].x*pPtList[i].y*pPtList[i].y;
		X2Y1 = X2Y1 + pPtList[i].x*pPtList[i].x*pPtList[i].y;
	}

	N = lPtLen;
	C = N*X2 - X1*X1;
	D = N*X1Y1 - X1*Y1;
	E = N*X3 + N*X1Y2 - (X2+Y2)*X1;
	G = N*Y2 - Y1*Y1;
	H = N*X2Y1 + N*Y3 - (X2+Y2)*Y1;
	if (C*G-D*D!=0)
	{
		a = (H*D-E*G)/(C*G-D*D);
		b = (H*C-E*D)/(D*D-G*C);
		c = -(a*X1 + b*Y1 + X2 + Y2)/N;
	}
	else
	{
		//拟合不出来
		a = 0;
		b = 0;
		c = 0;
	}


	if (xc)
	{
		*xc=(MLong)(a/(-2));
	}
	if (yc)
	{
		*yc=(MLong)(b/(-2));
	}
	if (r)
	{
		*r=(MLong)(sqrt(a*a+b*b-4*c)/2);
	}	
}
void testgui::start()
{
	CvCapture *pCapture = MNull;
	IplImage *pFrame = NULL;
	pCapture = cvCaptureFromFile(SOURCE_ROAD);
	if (MNull == pCapture)
	{
		WY_DEBUG_PRINT("get video file error!\n");
		goto EXT;
	}
	
	pFrame = cvQueryFrame(pCapture);
	cvSaveImage("1.jpg",pFrame);
	image1=Mat(pFrame,true);
	putTextZH(image1,"点击配准区域\n",Point(50,pFrame->height/2),Scalar(0,0,255),40,"微软雅黑");
	cvtColor(image1, image1, CV_BGR2RGB);
	show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
	ui.pic_1->setPixmap(QPixmap::fromImage(show_image1)); 
	EXT:
		cvReleaseCapture(&pCapture);
}




//VideoCapture capture1(0);

//void testgui::start()
//{
//	if (flag_1 == 1)//////flag函数
//	{
//		//capture1 >> image1;
//		pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
//		image1=Mat(pFrame,true);
//		cvtColor(image1, image1, CV_BGR2RGB);
//		show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
//		//namedWindow("1");
//		//imshow("1", image1);
//		ui.pic_1->setPixmap(QPixmap::fromImage(show_image1)); //pic--显示图片窗
//	}
//	if (flag_1 == 2)//////flag函数
//	{
//		//capture1 >> image1;
//		pFrame = cvLoadImage(SOURCE_JPG1, CV_LOAD_IMAGE_COLOR);
//		image1=Mat(pFrame,true);
//		cvtColor(image1, image1, CV_BGR2RGB);
//		show_image1 = QImage((const unsigned char*)image1.data, image1.cols, image1.rows, image1.cols*image1.channels(), QImage::Format_RGB888);
//		//namedWindow("1");
//		//imshow("1", image1);
//		ui.pic_1->setPixmap(QPixmap::fromImage(show_image1)); //pic--显示图片窗
//	}
//}
//void testgui::pwithe()
//{
//	
//	
//}
//void testgui::process()
//{
//	if (flag_2 == 1)
//	{
//
//
//		Mat temp;
//		Mat gray;
//		cvtColor(image1, temp, CV_BGR2RGB);
//		cvtColor(temp, gray, CV_RGB2GRAY);
//
//		Mat midImage;
//		Canny(gray, midImage, 50, 200, 3);
//		Mat detImage;
//		cvtColor(midImage, detImage, CV_GRAY2BGR);
//
//		/*vector<Vec4i>lines;
//		HoughLinesP(midImage, lines, 1, CV_PI / 180, 90, 105, 10);
//		for (size_t i = 0; i < lines.size(); i++)
//		{
//		Vec4i l = lines[i];
//		line(detImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
//		}*/
//
//		QImage iGray((const unsigned char*)(gray.data), gray.cols, gray.rows, QImage::Format_Indexed8);
//		ui.pic_2->setPixmap(QPixmap::fromImage(iGray));
//
//		//show_image2 = QImage((const unsigned char*)detImage.data, detImage.cols, detImage.rows, detImage.cols*detImage.channels(), QImage::Format_RGB888);
//		//ui.pic_2->setPixmap(QPixmap::fromImage(show_image2));
//		//namedWindow("1");
//		//imshow("1", detImage);
//	}
//}


void testgui::mousePressEvent(QMouseEvent *e)  
{  
    //----Qt5解决中文乱码  
    QTextCodec *codec = QTextCodec::codecForName("GB18030");  
  
    //----QMouseEvent类提供的x()和y()可获取鼠标相对窗口的位置  
    QString str = "("+QString :: number(e->x()) +", "+QString::number(e->y())+")";  
  
    //---点击左键  
    if (Qt ::LeftButton == e->button())  
    {  
        statusBar()->showMessage(codec->toUnicode("左键:") + str);  
    }  
    //---点击左键  
    if (Qt ::RightButton == e->button())  
    {  
        statusBar()->showMessage(codec->toUnicode("右键:") + str);  
    }  
    //---点击左键  
    if (Qt ::MidButton == e->button())  
    {  
        statusBar()->showMessage(codec->toUnicode("中键:") + str);  
    }  
}  
  
//---鼠标释放（松开）事件  
void testgui::mouseReleaseEvent(QMouseEvent *e)  
{  
    //----Qt5解决中文乱码  
    QTextCodec *codec = QTextCodec::codecForName("GB18030");  
  
    //----QMouseEvent类提供的x()和y()可获取鼠标相对窗口的位置  
    QString str = "("+QString :: number(e->x()) +", "+QString::number(e->y())+")";  
    statusBar()->showMessage(codec->toUnicode("鼠标位置:") + str, 3000);  
}  
  
//--s鼠标双击事件  
void testgui::mouseDoubleClickEvent(QMouseEvent *e)  
{  
    //---没有实现功能  
}  
  
//--鼠标移动事件  
/*void testgui::mouseMoveEvent(QMouseEvent *e)  
{  
    mousePointLabel->setText("("+QString::number(e->x())+", "+QString :: number(e->y())+")");  
}*/ 

MVoid onMouse(int Event,int x,int y,int flags,void* param )
{
	if (Event==CV_EVENT_LBUTTONDOWN)
	{
		int *Data=(int*)param;
		Data[0]=x;
		Data[1]=y;
		Data[2]=1;
	}
	if (Event==CV_EVENT_RBUTTONDOWN)
	{
		int *Data=(int*)param;
		Data[0]=x;
		Data[1]=y;
		Data[2]=CV_EVENT_RBUTTONDOWN;
	}
	if (Event==CV_EVENT_MOUSEMOVE)
	{
		int *Data=(int*)param;
		Data[0]=x;
		Data[1]=y;
		Data[2]=CV_EVENT_MOUSEMOVE;
	}
	if (Event==CV_EVENT_LBUTTONUP)
	{
		int *Data=(int*)param;
		Data[0]=x;
		Data[1]=y;
		Data[2]=CV_EVENT_LBUTTONUP;
		flagEnd=1;
	}
}
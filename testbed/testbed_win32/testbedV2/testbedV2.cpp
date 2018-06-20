#include <iostream>
#include <opencv2/opencv.hpp>
#include "HY_MeterRecogV2.h"
#include <time.h>
using namespace std;
#define DEBUG_DEMO

#ifdef DEBUG_DEMO
#define WY_DEBUG_PRINT printf
#else
#define WY_DEBUG_PRINT
#endif

#define SOURCE_JPG  "F:/SF6表/001.png"//表盘训练所用图片路径
MFloat lScale = 1.0;
#pragma comment(lib, "../Release/HYMeterRecogV2.lib")
int mouseX=-1,mouseY=-1,mouseFlag=0;
int flagEnd = 0;
MVoid onMouse(int Event,int x,int y,int flags,void* param );
int MeterReadTrain(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam);	//表盘训练
int MeterRecog();
int main(){
#if 0                  //生成配置文件biaoji.mat
	HYAMR_INTERACTIVE_PARA para = {0};
	MaskParam mParam = {0};
	MeterReadTrain(&para, &mParam);
#else
	MeterRecog();    //检测
#endif
	system("pause");

	return 0;
}
float sec(clock_t clocks)
{
	return (float)clocks/1000;
}
int MeterRecog(){
	clock_t time;
	HYAMR_Meter_Result MeterResult={0};
	MeterDESCRIP Meterdescrip={0};
	HYAMR_IMAGES img={0};
	CvCapture *pCapture = MNull;
	IplImage *srcImage = NULL;
	//***************************  get video  ******************************************************
	/*pCapture = cvCaptureFromFile(SOURCE_ROAD);
	if (MNull == pCapture)
	{
	WY_DEBUG_PRINT("get video file error!\n");
	goto EXT;
	}

	srcImage = cvQueryFrame(pCapture);*/
	//srcImage = cvLoadImage("D:/肉眼能识别读数的/A_18003_2018-04-20T14-30-08.bmp", CV_LOAD_IMAGE_COLOR);//读取图片
	//srcImage = cvLoadImage("F:/SF6表/006.png", CV_LOAD_IMAGE_COLOR);//读取图片
	//srcImage = cvLoadImage("F:/0522/2/670.jpg", CV_LOAD_IMAGE_COLOR);//读取图片
	srcImage = cvLoadImage("../../../testimage/t (1).png", CV_LOAD_IMAGE_COLOR);//读取图片
	img.lWidth = srcImage->width;
	img.lHeight = srcImage->height;
	img.pixelArray.chunky.lLineBytes = srcImage->widthStep;
	img.pixelArray.chunky.pPixel = srcImage->imageData;
	img.lPixelArrayFormat = HYAMR_IMAGE_BGR;//opencv读取彩图默认通道顺序为'BGR' 
	//自己手动设定的参数
	Meterdescrip.MeterType=2;//类型2：sf6
	/*Meterdescrip.MeterRect.left=806;
	Meterdescrip.MeterRect.top=445;
	Meterdescrip.MeterRect.right=1149;
	Meterdescrip.MeterRect.bottom=774;*/
	//while (srcImage = cvQueryFrame(pCapture))
	//while(1)
	{
		//if (!srcImage)
		//	break;
		time=clock();
		MeterReadRecogV2(&img,Meterdescrip,&MeterResult);
		printf("%f seconds,MeterValue=%f\n",sec(clock()-time),MeterResult.MeterValue);
	}
EXT:
	cvReleaseImage(&srcImage);        //读取图片的释放
	//cvReleaseCapture(&pCapture);    //读取视频的释放
	return 0;
}
/*******************************/
int MeterReadTrain(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam)
{
	FILE *fp = fopen("E:/biaoji.mat", "w");//配置文件路径
	int res = 0;
	int count = 0;
	int rect[4] = {0};

	MHandle hMemMgr = MNull;
	MHandle hHYMRDHand = MNull;
	MVoid *pMem = MNull;

	CvCapture *pCapture = MNull;
	IplImage *pFrame = MNull;
	IplImage *testImage = MNull;
	IplImage *tmpImage = MNull;
	IplImage *maskImage = MNull;
	CvSize ImgSize;

	HYAMR_IMAGES wy_testImage = {0};
	HYAMR_IMAGES wy_maskImage = {0};

	int i, j;
	int lPtNumTmp;
	MPOINT ptListTmp[50];
	int dist[5];
	int mouseParam[3];
	CvPoint startPt = {0};
	CvPoint endPt = {0};
	int flag = 0;
	MLong lTmpVal;
	MDouble lTmpMin;
	MDouble step;
	MLong lMemSize;
	MLong lHaarWidth=0;
	double temp;
	unsigned char *pData = MNull;


	
	pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	ImgSize.height = pFrame->height / lScale;
	ImgSize.width = pFrame->width / lScale;
	testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
	cvResize(pFrame, testImage, CV_INTER_LINEAR);
	cvSaveImage("D:\\testImage.bmp", testImage);
	//cvSaveImage("D:\\switch.bmp", testImage);
	cvNamedWindow("TrainImage", 0);
	
	cvSetMouseCallback("TrainImage", onMouse, (void*)mouseParam);
	tmpImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
	cvResize(testImage, tmpImage, CV_INTER_LINEAR);

	mouseParam[0]=-1;
	mouseParam[1]=-1;
	mouseParam[2]=-1;

	WY_DEBUG_PRINT("请圈定目标对象：\n");
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
	WY_DEBUG_PRINT("start(%d,%d), end(%d,%d)\n", startPt.x, startPt.y, endPt.x, endPt.y);

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
	pOutPattern->startPt.x=rect[0];//left
	fprintf(fp, "%ld ", pOutPattern->startPt.x);
	pOutPattern->endPt.x=rect[2];//right
	fprintf(fp, "%ld ", pOutPattern->endPt.x);
	pOutPattern->startPt.y=rect[1];//top
	fprintf(fp, "%ld ", pOutPattern->startPt.y);
	pOutPattern->endPt.y=rect[3];//bot
	fprintf(fp, "%ld ", pOutPattern->endPt.y);
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
			if (mouseParam[0]>=0 && mouseParam[0]<wy_testImage.lWidth
				&& mouseParam[1]>=0 && mouseParam[1]<wy_testImage.lHeight)
			{
				ptListTmp[lPtNumTmp].x = mouseParam[0];
				ptListTmp[lPtNumTmp].y = mouseParam[1];
				lPtNumTmp++;
				cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),1,CV_RGB(255,0,0),-1,8,0);
			}
			WY_DEBUG_PRINT("ptListTmp[%d].x=%d ptListTmp[%d].y=%d\n", lPtNumTmp,mouseParam[0],lPtNumTmp,mouseParam[1]);
			WY_DEBUG_PRINT("lPtNum=%d\n", lPtNumTmp);
			mouseParam[2] = -1;
		}
		else if (CV_EVENT_LBUTTONDOWN == mouseParam[2])		// delete white circle
		{
			for(i=0; i<lPtNumTmp; i++)
			{
				if (abs(mouseParam[0]-ptListTmp[i].x)<5 && abs(mouseParam[1]-ptListTmp[i].y)<5)
				{
					cvCircle(testImage, cvPoint(ptListTmp[i].x, ptListTmp[i].y), 1, CV_RGB(255,255,255), -1, 8, 0);
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
	pOutPattern->lPtNum = lPtNumTmp;
	fprintf(fp, "%d ", pOutPattern->lPtNum);
	for (i=0; i<lPtNumTmp; i++)
	{
		pOutPattern->ptPosList[i] = ptListTmp[i];
		fprintf(fp, "%ld ", pOutPattern->ptPosList[i].x);
		fprintf(fp, "%ld ", pOutPattern->ptPosList[i].y);
	}

	WY_DEBUG_PRINT("请输入步长\n");
	scanf("%lf",&step);
	WY_DEBUG_PRINT("请输入最小值\n");
	scanf("%lf",&lTmpMin);
	for (i=0; i<lPtNumTmp; i++)
	{
		pOutPattern->dPtValList[i] = (MDouble)lTmpMin;
		printf("pOutPattern->dPtValList[%d] = %lf\n",i,lTmpMin);
		fprintf(fp, "%f ", pOutPattern->dPtValList[i]);
		lTmpMin=lTmpMin+step;
	}

	

	if(pOutPattern->lPtNum>2)
	{
		WY_DEBUG_PRINT("请圈定圆心位置,右键添加：\n");
		//WY_DEBUG_PRINT("大刻度点标记: 右键添加，左键删除\n");
		mouseParam[0]=-1;
		mouseParam[1]=-1;
		mouseParam[2]=-1;
		cvSetMouseCallback("TrainImage", onMouse, (void*)mouseParam);
		lPtNumTmp = 0;
		while(1)
		{
			if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
			{
				if (mouseParam[0]>=0 && mouseParam[0]<wy_testImage.lWidth
					&& mouseParam[1]>=0 && mouseParam[1]<wy_testImage.lHeight)
				{
					pOutPattern->circleCoord.x = mouseParam[0];
					fprintf(fp, "%ld ", pOutPattern->circleCoord.x);
					pOutPattern->circleCoord.y = mouseParam[1];
					fprintf(fp, "%ld ", pOutPattern->circleCoord.y);
					lPtNumTmp++;
					cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),1,CV_RGB(255,0,0),-1,8,0);
				}
				WY_DEBUG_PRINT("pOutPattern->circleCoord.x=%d pOutPattern->circleCoord.y=%d\n",mouseParam[0],mouseParam[1]);
				WY_DEBUG_PRINT("lPtNum=%d\n", lPtNumTmp);
				mouseParam[2] = -1;
			}
			cvShowImage("TrainImage",testImage);
			if(13==cvWaitKey(10))	// Enter
				break;
		}
	}

	

	
	
	
	WY_DEBUG_PRINT("表盘中指针线有几条(1/2?)\n");
	scanf("%d", &lTmpVal);
	while(1>lTmpVal || 2<lTmpVal)
	{
		WY_DEBUG_PRINT("目前只支持1或2根指针线的仪表读数，请重新输入指针线条数\n");
		scanf("%d", &lTmpVal);
	}
	pOutPattern->lLineNum = lTmpVal;
	fprintf(fp, "%ld ", pOutPattern->lLineNum);
	if(pOutPattern->lLineNum ==1)
	{
		for (i=0; i<pOutPattern->lLineNum; i++)
		{
			WY_DEBUG_PRINT("请输入第%d根指针线颜色（1:黑色 2:白色 3:红色 4:黄色）\n", i+1);
			scanf("%d", &lTmpVal);
			while(1>lTmpVal || 4<lTmpVal)
			{
				WY_DEBUG_PRINT("暂不支持其他颜色，请重新确认指针颜色\n");
				scanf("%d\n", &lTmpVal);
			}
			if (1==lTmpVal)
			{
				pOutPattern->lLineColor[i] = HY_LINE_BLACK;
			}
			else if (2==lTmpVal)
			{
				pOutPattern->lLineColor[i] = HY_LINE_WHITE;
			}
			else if(3==lTmpVal)
			{
				pOutPattern->lLineColor[i] = HY_LINE_RED;
			}
			else
			{
				pOutPattern->lLineColor[i] = HY_LINE_YELLOW;
			}
			fprintf(fp, "%ld ", pOutPattern->lLineColor[i]);
		}
	}
	else
	{
		//指针线不同的情况下
		WY_DEBUG_PRINT("两根指针请选择 目前只支持 1.一红一黑 2.一红一白 3.两红\n");  
		scanf("%d",&lTmpVal);
		if(1==lTmpVal)
		{
			pOutPattern->lLineColor[1] = HY_LINE_RED;
			pOutPattern->lLineColor[0] = HY_LINE_BLACK;
		}
		else if(2==lTmpVal)
		{
			pOutPattern->lLineColor[0] = HY_LINE_WHITE;
			pOutPattern->lLineColor[1] = HY_LINE_RED;
		}
		else 
		{
			pOutPattern->lLineColor[0] = HY_LINE_RED;
			pOutPattern->lLineColor[1] = HY_LINE_RED;
		}
		fprintf(fp, "%ld ", pOutPattern->lLineColor[0]);
		fprintf(fp, "%ld ", pOutPattern->lLineColor[1]);
	}
	WY_DEBUG_PRINT("表盘背景是白色的吗？1.是 2.不是\n");
	scanf("%d", &lTmpVal);
	if (1==lTmpVal)
	{
		pOutPattern->bWhiteBackground = MTrue;
	}
	else
	{
		pOutPattern->bWhiteBackground = MFalse;
	}
	fprintf(fp, "%ld ", pOutPattern->bWhiteBackground);
	WY_DEBUG_PRINT("表盘指针很细还是正常？1.正常 2.很细\n");
	scanf("%d", &lTmpVal);
	fprintf(fp, "%ld ", lTmpVal);
	if (2==lTmpVal)
	{
		pOutPattern->lLineWidth = 2;
		fprintf(fp, "%ld ", pOutPattern->lLineWidth);
	}
	else
	{
		
		WY_DEBUG_PRINT("请在指针线上标记3组点用于计算指针线宽（包含指针线两端）:\n");
		lPtNumTmp = 0;
		while(1)
		{
			if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
			{
				if (mouseParam[0]>=0 && mouseParam[0]<testImage->width
					&& mouseParam[1]>=0 && mouseParam[1]<testImage->height)
				{
					ptListTmp[lPtNumTmp].x = mouseParam[0];
					fprintf(fp, "%ld ", ptListTmp[lPtNumTmp].x);
					ptListTmp[lPtNumTmp].y = mouseParam[1];
					fprintf(fp, "%ld ", ptListTmp[lPtNumTmp].y);
					lPtNumTmp++;
					printf("%d,%d\n", mouseParam[0], mouseParam[1]);
					//cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),1,CV_RGB(0,255,0),-1,8,0);
				}
				mouseParam[2] = -1;
			}
			cvShowImage("TrainImage",testImage);
			if(13==cvWaitKey(10) || lPtNumTmp>=6)	// Enter
				break;
		}
	
		for (int i=0;i<lPtNumTmp/2;i++)
		{
			temp = (ptListTmp[2*i].x-ptListTmp[2*i+1].x)*(ptListTmp[2*i].x-ptListTmp[2*i+1].x)+
				(ptListTmp[2*i].y-ptListTmp[2*i+1].y)*(ptListTmp[2*i].y-ptListTmp[2*i+1].y);
			dist[i] = sqrt(temp);
			lHaarWidth +=dist[i];
		}
		lHaarWidth /=lPtNumTmp/2;
		//HYAMR_CalcHaarWidth(hHYMRDHand, &wy_testImage, pOutPattern->lLineColor, ptListTmp, lPtNumTmp, &lTmpVal);
		pOutPattern->lLineWidth = lHaarWidth;
		printf("pOutPattern->lLineWidth=%d\n",lHaarWidth);
	}
	//fprintf(fp, "%ld ", pOutPattern->lLineWidth);
	WY_DEBUG_PRINT("请设计表盘MASK类型：（1.矩形 2.圆形 3.圆环 4.圆环+矩形 5.圆形+矩形 6.圆形+2个矩形 7.圆形+3个矩形 8.其它）\n");
	scanf("%d", &lTmpVal);
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
		WY_DEBUG_PRINT("标记%d个点，构造MASK图\n", lPtNumTmp);
		pMaskParam->lMaskPtNum = lPtNumTmp;
		fprintf(fp, "%ld ", pMaskParam->lMaskPtNum);
		lPtNumTmp = 0;
		while(1)
		{
			if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
			{
				if (mouseParam[0]>=0 && mouseParam[0]<testImage->width
					&& mouseParam[1]>=0 && mouseParam[1]<testImage->height)
				{
					pMaskParam->maskPt[lPtNumTmp].x = mouseParam[0];
					fprintf(fp, "%ld ", pMaskParam->maskPt[lPtNumTmp].x);
					pMaskParam->maskPt[lPtNumTmp].y = mouseParam[1];
					fprintf(fp, "%ld ", pMaskParam->maskPt[lPtNumTmp].y);
					lPtNumTmp++;
					printf("%d,%d\n", mouseParam[0], mouseParam[1]);
					cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),1,CV_RGB(0,255,0),-1,8,0);
				}
				mouseParam[2] = -1;
			}
			cvShowImage("TrainImage",testImage);
			if(13==cvWaitKey(10) || lPtNumTmp>=pMaskParam->lMaskPtNum)	// Enter
				break;
		}
	}

	WY_DEBUG_PRINT("指针线与表盘背景区分度 1:清晰 2:模糊 3:其他\n");
	scanf("%d", &lTmpVal);
	while(lTmpVal<0 || lTmpVal>3)
	{
		WY_DEBUG_PRINT("error input!!! input again\n");
		scanf("%d\n", &lTmpVal);
	}
	if (1==lTmpVal)
	{
		pOutPattern->picLevel = HY_LOW_BLUR;
	}
	else if(2==lTmpVal)
	{
		pOutPattern->picLevel = HY_HIGH_BLUR;
	}
	else
	{
		pOutPattern->picLevel = HY_OTHER_BLUR;
	}
	fprintf(fp, "%ld ", pOutPattern->picLevel);
	WY_DEBUG_PRINT("~~~~~~~~train success~~~~~~~~~~\n");

EXT:
	
	cvReleaseImage(&testImage);
	cvReleaseImage(&tmpImage);
	cvReleaseImage(&maskImage);
	fclose(fp);
	return res;
}

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
/*******************************/
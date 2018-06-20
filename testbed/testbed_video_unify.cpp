// testBed.cpp : main project file.
#include <opencv2/opencv.hpp>
//#include"amcomdef.h"
//#include"time.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <math.h>


// meter Recognise
//#include"HYAMR_meterReg.h"
#include "watch_transform.h"

// rig match
#include "wyMatchRigidBodyIF.h"
#include "wyMatchRigidBodyStruct.h"

#pragma comment(lib, "./Release/watch_match.lib")
//#ifdef _DEBUG
//#pragma comment(lib, "../Debug/HYAlgorithm.lib")
//#else
//#pragma comment(lib, "../Release/HYAlgorithm.lib")
//#endif
//#pragma comment(lib, "opencv_imgproc244d.lib")
//#pragma comment(lib, "opencv_core244d.lib")
//#pragma comment(lib, "opencv_highgui244d.lib")

#define DEBUG_DEMO

#ifdef DEBUG_DEMO
#define WY_DEBUG_PRINT printf
#else
#define WY_DEBUG_PRINT
#endif

#define GO(FUNCTION)		{if((res = FUNCTION) != 0) goto EXT;}
#define WORK_BUFFER 100000000
#define MR_MEM_SIZE (100*1024*1024)

#define MAX_BUFFER_LEN 5
#define GrayStep(x)			(( (x) + 3 ) & (~3) )
// save descriptor info
#define MR_READ_FILE_PARA "D:\\MeterRead.dat"

#define SOURCE_ROAD	"D:/123.0.89.40_01_20161125113007701.mp4"
//#define SOURCE_JPG  "D:/biaoji/sf6new/��׼ͼ.bmp"
#define SOURCE_JPG  "F:/0606/ywywb1_6.jpg"



//typedef struct
//{
//	MLong cx;
//	MLong cy;
//	MLong r;
//}MyCircleParam;

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

int ReadMeter(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine,const char* data_path);

int MeterRead_Recog(HYLPAMR_IMAGES src,HYAMR_IMAGES wy_maskImage,HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam,HYAMR_Meter_Result *MResult);
int MeterReadTrain_data(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam);
int MeterReadTrain(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam);	//����ѵ��
int MeterReadRecog(HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam);	//����ʶ��
int RidMatchTrain(int trainType, int rotateEn);	//����״̬ѵ��
int RidMatchRecog();	//����״̬ʶ��
void markPoints(IplImage *srcImage, MPOINT *ptList, MLong lPtNum);
void setMaskPic(IplImage *maskImage, MaskParam *pParam,MLong *Dist);
void setMaskPicIn(HYLPAMR_IMAGES maskImage ,MaskParam *pParam,MLong *Dist);
void FittingCircle(MPOINT *pPtList, MLong lPtLen, MLong *xc, MLong *yc, MLong *r);
void paintpoint(IplImage *src,HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine);
MFloat lScale = 1.0;

int main(int argc, char* argv[])
{
	HYAMR_INTERACTIVE_PARA para = {0};
	//HYAMR_INTERACTIVE_PARA *para1=(HYAMR_INTERACTIVE_PARA *)calloc(1,sizeof(HYAMR_INTERACTIVE_PARA));
	MaskParam mParam = {0};
	//MaskParam *mParam1=(MaskParam *)calloc(1,sizeof(MaskParam));
	WidthType HaarLine={0};
	//WidthType *HaarLine1=(WidthType *)calloc(1,sizeof(WidthType));
	//HYAMR_Meter_Result MResult={0};
	HYAMR_Meter_Result *MResult=(HYAMR_Meter_Result *)calloc(2,sizeof(HYAMR_Meter_Result));
	IplImage *srcImage = MNull;
	IplImage *src = MNull;
	IplImage *baseImage = MNull;
	HYAMR_IMAGES img={0};
	HYAMR_IMAGES wy_maskImage = {0};
	char *peizhifile="D:/biaoji/sf6new/biaoji_18p.mat";
	char *imgfile="D:/biaoji/sf6new/ƥ��ģ��.jpg";
	//char *peizhifile="D:/z/biaoji.mat";
	//char *imgfile="D:/z/ƥ��ģ��.jpg";
	srcImage = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	src = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	baseImage = cvLoadImage("D:/biaoji/sf6new/��׼ͼ.jpg", CV_LOAD_IMAGE_COLOR);
	img.lWidth = srcImage->width;
	img.lHeight = srcImage->height;
	img.pixelArray.chunky.lLineBytes = srcImage->widthStep;
	img.pixelArray.chunky.pPixel = srcImage->imageData;
	img.lPixelArrayFormat = HYAMR_IMAGE_BGR;
	// maskͼ
	wy_maskImage.lHeight=img.lHeight;
	wy_maskImage.lWidth=img.lWidth;
	wy_maskImage.pixelArray.chunky.lLineBytes=GrayStep(wy_maskImage.lWidth);	wy_maskImage.pixelArray.chunky.pPixel=(MByte *)calloc(wy_maskImage.lHeight*wy_maskImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	wy_maskImage.lPixelArrayFormat=HYAMR_IMAGE_GRAY;
	//MeterReadTrain(&para, &mParam);

	ReadMeter(&para, &mParam,&HaarLine,"E:/ywywb1_6_1.mat");
	//ReadMeter(&para, &mParam,&HaarLine,peizhifile);

	//MeterReadTrain_data(&para, &mParam);
//	Sleep(2000);
	WY_DEBUG_PRINT("########  start recognise\n\n");
	//paintpoint(src, &para, &mParam, &HaarLine);
	/**************************************
	paintpoint(baseImage, &para, &mParam, &HaarLine);
		action(img, &wy_maskImage,&para, &mParam, &HaarLine,&para.Dist,imgfile,0.6,1.4);
	//action(img, &para, &mParam, &HaarLine,imgfile,0.6,1.4);
	//printf("%d %d %d %d\n",para.startPt.x,para.startPt.y,para.endPt.x,para.endPt.y);
	paintpoint(src, &para, &mParam, &HaarLine);
	/**************************************/
	MeterReadRecog(para, &mParam);
	/*for(int i=0;i<2;i++){
		printf("MResult[%d].MeterValue=%f\n",i,MResult[i].MeterValue);
	}*/
	//MeterRead_Recog(&img,wy_maskImage,para, &mParam,MResult);  //setMaskPicInû����

	//MeterRead_Recog(&img,para, &mParam,MResult);
	/*for(int i=0;i<2;i++){
		printf("MResult[%d].MeterValue=%f\n",i,MResult[i].MeterValue);
	}*/
	system("pause");

	return 0;
}


int MeterReadRecog123(HYLPAMR_IMAGES src,MeterDESCRIP Meterdescrip,HYAMR_Meter_Result *MeterResult){
	HYAMR_INTERACTIVE_PARA para = {0};
	MaskParam mParam = {0};
	WidthType HaarLine={0};
	IplImage *srcImage = MNull;
	HYAMR_IMAGES img={0};
	srcImage = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	img.lWidth = srcImage->width;
	img.lHeight = srcImage->height;
	img.pixelArray.chunky.lLineBytes = srcImage->widthStep;
	img.pixelArray.chunky.pPixel = srcImage->imageData;
	img.lPixelArrayFormat = HYAMR_IMAGE_BGR;
	ReadMeter(&para, &mParam,&HaarLine,"E:/biaoji.mat");
	//action();

	//MeterRead_Recog(&img,para, &mParam);
}
int ReadMeter(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine, const char* data_path)
{

	FILE *fp = fopen(data_path, "r");
	int TmpVal=0;
	MPOINT ptListTmp[50];
	if (fp == MNull)
	{
		printf("data loading failed!\n ");
		return -1;
	}

	//pOutPattern = (HYAMR_INTERACTIVE_PARA*)malloc(100 * sizeof(HYAMR_INTERACTIVE_PARA));

	

	//����ʼ����յ�����
	fscanf(fp, "%ld", &(pOutPattern->startPt.x));
	//printf("%d\n",pOutPattern->startPt.x);
	fscanf(fp, "%ld", &(pOutPattern->endPt.x));
	fscanf(fp, "%ld", &(pOutPattern->startPt.y));
	fscanf(fp, "%ld", &(pOutPattern->endPt.y));

	fscanf(fp, "%ld", &(pOutPattern->lPtNum));
	//��̶ȵ�����
	for (int i = 0; i < pOutPattern->lPtNum; i++)
	{
		fscanf(fp, "%ld", &(pOutPattern->ptPosList[i].x));
		fscanf(fp, "%ld", &(pOutPattern->ptPosList[i].y));
	}
	//��ÿ�����Ӧ�Ŀ̶�ֵ
	for (int i = 0; i <pOutPattern->lPtNum; i++){
		fscanf(fp, "%lf", &(pOutPattern->dPtValList[i]));
	}
	//��Ȧ��Բ��
	fscanf(fp, "%ld", &(pOutPattern->circleCoord.x));
	fscanf(fp, "%ld", &(pOutPattern->circleCoord.y));
	//��ָ������
	fscanf(fp, "%d", &(pOutPattern->lLineNum));
	//��ָ����ɫ
	if (pOutPattern->lLineNum == 1)
	{
		for (int i = 0; i < pOutPattern->lLineNum; i++)
		{
			fscanf(fp, "%ld", &(pOutPattern->lLineColor[i]));
		}
	}
	else
	{
		fscanf(fp, "%ld", &(pOutPattern->lLineColor[0]));
		fscanf(fp, "%ld", &(pOutPattern->lLineColor[1]));
		
	}
	//����̱����Ƿ��ɫ
	fscanf(fp, "%ld", &(pOutPattern->bWhiteBackground));

	fscanf(fp, "%ld", &(HaarLine->LType));
	TmpVal=HaarLine->LType;
	//fscanf(fp, "%ld", &TmpVal);
	//�����ָ����
	if(TmpVal==2){
		fscanf(fp, "%ld", &(pOutPattern->lLineWidth));
	}
	else{
		int dist[5];
		double temp=0;
		int lHaarWidth=0;
		int lPtNumTmp=6;
		for(int i=0;i<6;i++){
			
			fscanf(fp, "%ld ", &(HaarLine->ptLine[i].x));
			fscanf(fp, "%ld ", &(HaarLine->ptLine[i].y));
			//printf("%d %d\n",HaarLine->ptLine[i].x,HaarLine->ptLine[i].y);
			ptListTmp[i].x=HaarLine->ptLine[i].x;
			ptListTmp[i].y=HaarLine->ptLine[i].y;
			//fscanf(fp, "%ld ", &(ptListTmp[i].x));
			//fscanf(fp, "%ld ", &(ptListTmp[i].y));
		}
		for (int i=0;i<lPtNumTmp/2;i++)
		{
			temp = (ptListTmp[2*i].x-ptListTmp[2*i+1].x)*(ptListTmp[2*i].x-ptListTmp[2*i+1].x)+
				(ptListTmp[2*i].y-ptListTmp[2*i+1].y)*(ptListTmp[2*i].y-ptListTmp[2*i+1].y);
			dist[i] = sqrt(temp);
			lHaarWidth +=dist[i];
		}
		lHaarWidth /=lPtNumTmp/2;
		pOutPattern->lLineWidth = lHaarWidth;
	}


	//--------------------------------------//
	//MASKͼ
	//pMaskParam = (MaskParam*)malloc(100 * sizeof(MaskParam));
	//���빹��Maskͼ�ĵ���
	fscanf(fp, "%ld", &(pMaskParam->lMaskPtNum));//
	
	for (int i = 0; i < pMaskParam->lMaskPtNum; i++)
	{
		fscanf(fp, "%ld", &(pMaskParam->maskPt[i].x));
		fscanf(fp, "%ld", &(pMaskParam->maskPt[i].y));
	}
	//---------------------------------------//
	//��ָ��������̱������ֶ�
	fscanf(fp, "%ld", &(pOutPattern->picLevel));
	
	fclose(fp);
	return 0;
}
void paintpoint(IplImage *src,HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine){
	CvPoint startPt = {0};
	CvPoint endPt = {0};
	startPt.x=pOutPattern->startPt.x;
	startPt.y=pOutPattern->startPt.y;
	endPt.x=pOutPattern->endPt.x;
	endPt.y=pOutPattern->endPt.y;
	cvRectangle(src, startPt, endPt, CV_RGB(0,0,255), 1);
	for(int i=0;i<pOutPattern->lPtNum;i++){
		cvCircle(src,cvPoint(pOutPattern->ptPosList[i].x,pOutPattern->ptPosList[i].y),5,CV_RGB(255,0,0),-1,8,0);
	}
	cvCircle(src,cvPoint(pOutPattern->circleCoord.x,pOutPattern->circleCoord.y),5,CV_RGB(0,0,255),-1,8,0);
	for(int i=0;i<pMaskParam->lMaskPtNum;i++){
		cvCircle(src,cvPoint(pMaskParam->maskPt[i].x,pMaskParam->maskPt[i].y),5,CV_RGB(0,255,0),-1,8,0);
	}
	printf("lLineWidth=%d\n",pOutPattern->lLineWidth);
	cvSaveImage("D:\\paintpoint.bmp", src);
	cvShowImage("src", src);
	cvWaitKey(10);
}
int MeterReadTrain_data(HYAMR_INTERACTIVE_PARA *&pOutPattern, MaskParam *pMaskParam){
	WY_DEBUG_PRINT("Ȧ��Ŀ�����\n");
	pOutPattern->startPt.x=235;//left
	pOutPattern->endPt.x=948;//right
	pOutPattern->startPt.y=63;//top
	pOutPattern->endPt.y=916;//bot

	WY_DEBUG_PRINT("�̶ȵ���Ŀ\n");
	pOutPattern->lPtNum = 11;

	WY_DEBUG_PRINT("��̶ȵ��С��������\n");
	pOutPattern->ptPosList[0].x =811;
	pOutPattern->ptPosList[0].y =687;
	pOutPattern->ptPosList[1].x =741;
	pOutPattern->ptPosList[1].y =780;
	pOutPattern->ptPosList[2].x =641;
	pOutPattern->ptPosList[2].y =824;
	pOutPattern->ptPosList[3].x =548;
	pOutPattern->ptPosList[3].y =806;
	pOutPattern->ptPosList[4].x =459;
	pOutPattern->ptPosList[4].y =727;
	pOutPattern->ptPosList[5].x =406;
	pOutPattern->ptPosList[5].y =592;
	pOutPattern->ptPosList[6].x =385;
	pOutPattern->ptPosList[6].y =453;
	pOutPattern->ptPosList[7].x =407;
	pOutPattern->ptPosList[7].y =321;
	pOutPattern->ptPosList[8].x =488;
	pOutPattern->ptPosList[8].y =214;
	pOutPattern->ptPosList[9].x =587;
	pOutPattern->ptPosList[9].y =176;
	pOutPattern->ptPosList[10].x =683;
	pOutPattern->ptPosList[10].y =181;
	/*for (int i=0; i<pOutPattern->lPtNum; i++)
	{
		pOutPattern->ptPosList[i].x =1;
		pOutPattern->ptPosList[i].y =1;
	}*/
	int step=1;//����

	int lTmpMin=0;//��Сֵ
	WY_DEBUG_PRINT("ÿ�����Ӧ�Ŀ̶�ֵ\n");
	for (int i=0; i<pOutPattern->lPtNum; i++)
	{
		pOutPattern->dPtValList[i] = (MDouble)lTmpMin;//ÿ�����Ӧ�Ŀ̶�ֵ
		printf("pOutPattern->dPtValList[%d] = %d\n",i,lTmpMin);
		lTmpMin=lTmpMin+step;
	}

	WY_DEBUG_PRINT("Ȧ��Բ��\n");
	if(pOutPattern->lPtNum>2)//Ȧ��Բ��
	{
		pOutPattern->circleCoord.x = 595;
		pOutPattern->circleCoord.y = 510;
	}

	WY_DEBUG_PRINT("������ָ�����м���(1/2?)\n");
	int lTmpVal=1;
	while(1>lTmpVal || 2<lTmpVal)
	{
		WY_DEBUG_PRINT("Ŀǰֻ֧��1��2��ָ���ߵ��Ǳ����������������ָ��������\n");
		scanf("%d", &lTmpVal);
	}
	pOutPattern->lLineNum = lTmpVal;

	WY_DEBUG_PRINT("����ָ������ɫ\n");
	if(pOutPattern->lLineNum ==1)
	{
		for (int i=0; i<pOutPattern->lLineNum; i++)
		{
			pOutPattern->lLineColor[i] = HY_LINE_BLACK;
		}
	}
	else
	{
		pOutPattern->lLineColor[1] = HY_LINE_RED;
		pOutPattern->lLineColor[0] = HY_LINE_BLACK;
	}
	WY_DEBUG_PRINT("���̱����ǰ�ɫ����1.�� 2.����\n");
	pOutPattern->bWhiteBackground = MTrue;

	WY_DEBUG_PRINT("����ָ����\n");
	pOutPattern->lLineWidth = 34;

	WY_DEBUG_PRINT("����Ʊ���MASK���ͣ���1.���� 2.Բ�� 3.Բ�� 4.Բ��+���� 5.Բ��+���� 6.Բ��+2������ 7.Բ��+3������ 8.������\n");
	lTmpVal=2;
	int lPtNumTmp=0;
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
	WY_DEBUG_PRINT("���%d���㣬����MASKͼ\n", lPtNumTmp);
	pMaskParam->lMaskPtNum = lPtNumTmp;
	pMaskParam->maskPt[0].x = 549;
	pMaskParam->maskPt[0].y = 100;
	pMaskParam->maskPt[1].x = 355;
	pMaskParam->maskPt[1].y = 762;
	pMaskParam->maskPt[2].x = 863;
	pMaskParam->maskPt[2].y = 623;
	/*for(int i=0;i<pMaskParam->lMaskPtNum;i++){
		pMaskParam->maskPt[i].x = 1;
		pMaskParam->maskPt[i].y = 1;
	}*/
	WY_DEBUG_PRINT("ָ��������̱������ֶ� 1:���� 2:ģ�� 3:����\n");
	pOutPattern->picLevel = HY_LOW_BLUR;

	return 0;
}
int MeterReadTrain(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam)
{
	FILE *fp = fopen("E:/biaoji.mat", "w");
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
	MLong lTmpMin;
	MLong step;
	MLong lMemSize;
	MLong lHaarWidth=0;
	double temp;
	unsigned char *pData = MNull;


	pMem = malloc(MR_MEM_SIZE);
	if (!pMem)
	{
		WY_DEBUG_PRINT("malloc 100*1024*1024 error!!!\n");
		return -1;
	}

	hMemMgr = HYAMR_MemMgrCreate(pMem, MR_MEM_SIZE);
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

	//lMeterType = METER_TYPE;
	/*pCapture = cvCaptureFromFile(SOURCE_ROAD);
	if (MNull == pCapture)
	{
		WY_DEBUG_PRINT("get video file error!\n");
		goto EXT;
	}

	pFrame = cvQueryFrame(pCapture);*/
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

	WY_DEBUG_PRINT("��Ȧ��Ŀ�����\n");
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

	WY_DEBUG_PRINT("~~~~~~~~~~Train meter~~~~~~~~~~~~\n");

	if (0 != HYAMR_TrainTemplateFromMask (hHYMRDHand, &wy_testImage, &wy_maskImage, "OK", 0))
	{
		WY_DEBUG_PRINT("HYAMR_TrainTemplateFromMask error!\n");
		res = -1;
		goto EXT;
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
		res = -1;
		goto EXT;
	}

	//ѵ����ʱ��ΪʲôҪ������׼ һ����׼ ��ȫ��׼��
	HYAMR_GetObjRect(hHYMRDHand, &wy_testImage, "OK", 0.75);	// ����һ��ƥ���������ȡĿ�����λ��

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
		res = -1;
		goto EXT;
	}

//	cvNamedWindow("testImage",0);
	cvShowImage("TrainImage", testImage);
	cvWaitKey(1);
	WY_DEBUG_PRINT("��̶ȵ��С������: �Ҽ���ӣ����ɾ��\n");
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

	WY_DEBUG_PRINT("�����벽��\n");
	scanf("%d",&step);
	WY_DEBUG_PRINT("��������Сֵ\n");
	scanf("%d",&lTmpMin);
	for (i=0; i<lPtNumTmp; i++)
	{
		pOutPattern->dPtValList[i] = (MDouble)lTmpMin;
		printf("pOutPattern->dPtValList[%d] = %d\n",i,lTmpMin);
		fprintf(fp, "%f ", pOutPattern->dPtValList[i]);
		lTmpMin=lTmpMin+step;
	}

	/*WY_DEBUG_PRINT("����������%d���̶�ֵ\n", lPtNumTmp);
	for (i=0; i<lPtNumTmp; i++)
	{
		scanf("%d", &lTmpVal);
		pOutPattern->dPtValList[i] = (MDouble)lTmpVal;
	}*/

	if(pOutPattern->lPtNum>2)
	{
		WY_DEBUG_PRINT("��Ȧ��Բ��λ��,�Ҽ���ӣ�\n");
		//WY_DEBUG_PRINT("��̶ȵ���: �Ҽ���ӣ����ɾ��\n");
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

	//WY_DEBUG_PRINT("�Ҽ�ȷ����ʼ�̶ȵ�\n");
	//while(1)
	//{
	//	if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
	//	{
	//		if (mouseParam[0]>=0 && mouseParam[0]<wy_testImage.lWidth
	//			&& mouseParam[1]>=0 && mouseParam[1]<wy_testImage.lHeight)
	//		{
	//			pOutPattern->ptStart.x = mouseParam[0];
	//			pOutPattern->ptStart.y = mouseParam[1];
	//			cvCircle(testImage,cvPoint(mouseParam[0],mouseParam[1]),1,CV_RGB(0,255,0),-1,8,0);
	//		}
	//		mouseParam[2] = -1;
	//	}
	//	cvShowImage("TrainImage",testImage);
	//	if(13==cvWaitKey(10))	// Enter
	//		break;
	//}

	
	
	/*
	int value=0;
	cvCreateTrackbar("ָ��������","TrainImage",&value,2,track_bar);
	cvWaitKey();
	value=0;
	if(cvGetTrackbarPos("ָ��������","TrainImage")==1)
	{
		cvCreateTrackbar("ָ������ɫ","TrainImage",&value,4,track_bar);
	}
	else
	{
		cvCreateTrackbar("ָ������ɫ","TrainImage",&value,3,track_bar);
	}
	cvWaitKey();
	value=0;
	cvCreateTrackbar("��ɫ����","TrainImage",&value,1,track_bar);
	cvWaitKey();
	value=0;
	cvCreateTrackbar("����ָ���ϸ","TrainImage",&value,1,track_bar);
	cvWaitKey();
	value=0;
	cvCreateTrackbar("����MASK����","TrainImage",&value,1,track_bar);
	cvWaitKey();
	*/
	WY_DEBUG_PRINT("������ָ�����м���(1/2?)\n");
	scanf("%d", &lTmpVal);
	while(1>lTmpVal || 2<lTmpVal)
	{
		WY_DEBUG_PRINT("Ŀǰֻ֧��1��2��ָ���ߵ��Ǳ����������������ָ��������\n");
		scanf("%d", &lTmpVal);
	}
	pOutPattern->lLineNum = lTmpVal;
	fprintf(fp, "%ld ", pOutPattern->lLineNum);
	if(pOutPattern->lLineNum ==1)
	{
		for (i=0; i<pOutPattern->lLineNum; i++)
		{
			WY_DEBUG_PRINT("�������%d��ָ������ɫ��1:��ɫ 2:��ɫ 3:��ɫ 4:��ɫ��\n", i+1);
			scanf("%d", &lTmpVal);
			while(1>lTmpVal || 4<lTmpVal)
			{
				WY_DEBUG_PRINT("�ݲ�֧��������ɫ��������ȷ��ָ����ɫ\n");
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
		//ָ���߲�ͬ�������
		WY_DEBUG_PRINT("����ָ����ѡ�� Ŀǰֻ֧�� 1.һ��һ�� 2.һ��һ�� 3.����\n");  
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
	WY_DEBUG_PRINT("���̱����ǰ�ɫ����1.�� 2.����\n");
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
	WY_DEBUG_PRINT("����ָ���ϸ����������1.���� 2.��ϸ\n");
	scanf("%d", &lTmpVal);
	fprintf(fp, "%ld ", lTmpVal);
	if (2==lTmpVal)
	{
		pOutPattern->lLineWidth = 2;
		fprintf(fp, "%ld ", pOutPattern->lLineWidth);
	}
	else
	{
		/*int value=0;
		cvCreateTrackbar("ָ���߿��","TrainImage",&value,100,track_bar);
		cvWaitKey();
		pOutPattern->lLineWidth = cvGetTrackbarPos("ָ���߿��","TrainImage");
		printf("pOutPattern->lLineWidth=%d\n",pOutPattern->lLineWidth);
		cvWaitKey();*/
		WY_DEBUG_PRINT("����ָ�����ϱ��3������ڼ���ָ���߿�����ָ�������ˣ�:\n");
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
	
		//pOutPattern->lLineWidthL=;
		//pOutPattern->lLineWidthM=;
		//pOutPattern->lLineWidthS=;
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
	WY_DEBUG_PRINT("����Ʊ���MASK���ͣ���1.���� 2.Բ�� 3.Բ�� 4.Բ��+���� 5.Բ��+���� 6.Բ��+2������ 7.Բ��+3������ 8.Բ��+3���� 9.Բ��+6���� 10.Բ��+9���� 11.3Բ��+Բ�� 12.������\n");
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
		case 8:
			lPtNumTmp = 12;
			break;
		case 9:
			lPtNumTmp = 18;
			break;
		case 10:
			lPtNumTmp = 24;
			break;
		case 11:
			lPtNumTmp = 10;
			break;
		default:
			lPtNumTmp = 0;
	}

	if (lPtNumTmp>0)
	{
		WY_DEBUG_PRINT("���%d���㣬����MASKͼ\n", lPtNumTmp);
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

	WY_DEBUG_PRINT("ָ��������̱������ֶ� 1:���� 2:ģ�� 3:����\n");
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
	HYAMR_Uninit (hHYMRDHand);
	HYAMR_MemMgrDestroy (hMemMgr);
	if(MNull!=pMem)
	{	
		free(pMem);
		pMem = MNull;
	}
	cvReleaseImage(&testImage);
	cvReleaseImage(&tmpImage);
	cvReleaseImage(&maskImage);
	fclose(fp);
	return res;
}
int MeterRead_Recog(HYLPAMR_IMAGES src,HYAMR_IMAGES wy_maskImage,HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam,HYAMR_Meter_Result *MResult)
{
	int res = 0;
	inPara.Dist = 0xFFFF;

	MHandle hMemMgr = MNull;
	MHandle hHYMRDHand = MNull;
	MVoid *pMem = MNull;

	//CvCapture *pCapture = MNull;
	//IplImage *pFrame = MNull;
	//IplImage *testImage = MNull;
	//IplImage *maskImage = MNull;
	HYAMR_IMAGES wy_testImage = {0};

	//HYAMR_IMAGES wy_maskImage = {0};
	CvSize ImgSize ={0};

	MLong lFrameNum = 0;
	//CvVideoWriter *pWriter = MNull;

	HYAMR_READ_PARA pReadPara[2] = {0};
	MDouble matchRate;
	int i;

	MDouble dResult;
	MDouble buffer[2][MAX_BUFFER_LEN];
	MLong lBufferLen;
	char textContent[100];
	FILE *fileFp = MNull;
	//CvFont font = {0};

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

	//pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
	ImgSize.height = src->lHeight;
	ImgSize.width = src->lWidth;
	//testImage = cvCreateImage(ImgSize, pFrame->depth, pFrame->nChannels);
	//cvResize(pFrame, testImage, CV_INTER_LINEAR);

	//maskImage = cvCreateImage(ImgSize, 8, 1);
	//wy_maskImage.lHeight=src->lHeight;
	//wy_maskImage.lWidth=src->lWidth;
	//wy_maskImage.pixelArray.chunky.lLineBytes=GrayStep(wy_maskImage.lWidth);
	//wy_maskImage.pixelArray.chunky.pPixel=(MByte *)calloc(wy_maskImage.lHeight*wy_maskImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	//wy_maskImage.lPixelArrayFormat=HYAMR_IMAGE_GRAY;
	//setMaskPic(maskImage, pMaskParam,&inPara.Dist);
	//setMaskPicIn(&wy_maskImage, pMaskParam,&inPara.Dist);

	/*wy_maskImage.lWidth = maskImage->width;
	wy_maskImage.lHeight = maskImage->height;
	wy_maskImage.pixelArray.chunky.lLineBytes = maskImage->widthStep;
	wy_maskImage.pixelArray.chunky.pPixel = maskImage->imageData;
	wy_maskImage.lPixelArrayFormat = HYAMR_IMAGE_GRAY;*/

	/*pWriter = cvCreateVideoWriter("D:\\result_test2.avi", CV_FOURCC('X', 'V', 'I', 'D'), 5, ImgSize);
	if (MNull == pWriter)
	{
		WY_DEBUG_PRINT("pWriter error!\n");
		goto EXT;
	}*/

	if (0 != HYAMR_SetParam (hHYMRDHand, &wy_maskImage, &inPara))
	{
		WY_DEBUG_PRINT("HYAMR_SetParam error!\n");
		res = -1;
		goto EXT;
	}

	//font = cvFont(2, 1);
	//cvNamedWindow("Result", 0);
	lFrameNum = 0;

	//matchRate = 0.45;
	  
	//while(1)
	//while(pFrame = cvQueryFrame(pCapture))
	//{
		//pFrame = cvLoadImage("C:\\a.jpg", CV_LOAD_IMAGE_COLOR);
		lFrameNum++;
		/*if(lFrameNum<90)
			continue;*/
		//if(!pFrame)
		//	break;
		//cvResize(pFrame, testImage, CV_INTER_LINEAR);
		WY_DEBUG_PRINT("lFrameNum=%d\n", lFrameNum);
		
		//sprintf(textContent1, "D:\\sf6 images\\testImage_%d.bmp", lFrameNum);
		//cvSaveImage(textContent1, testImage);

		//wy_testImage.lWidth = testImage->width;
		//wy_testImage.lHeight = testImage->height;
		//wy_testImage.pixelArray.chunky.lLineBytes = testImage->widthStep;
		//wy_testImage.pixelArray.chunky.pPixel = testImage->imageData;
		//wy_testImage.lPixelArrayFormat = HYAMR_IMAGE_BGR;

		// Recognise the pointer line
		if (0 != HYAMR_GetLineParamNoMem(hHYMRDHand, src, &inPara,pReadPara))
		//if (0 != HYAMR_GetLineParam(hHYMRDHand, &wy_testImage, &inPara, "OK", matchRate, gCurTarget, gTrainTarget, pReadPara))
		{
			WY_DEBUG_PRINT("HYAMR_GetLineParam error!\n");
			//continue;
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
			/*sprintf(textContent, "lFrameNum=%d  lineNum[%d] result[%.4f]", lFrameNum, i, dResult);
			cvPutText(testImage, textContent, cvPoint(400, 150+10*i), &font, CV_RGB(0,255,0));*/
			WY_DEBUG_PRINT("result=%.4f\n\n", dResult);

			sprintf(textContent, "D:\\testResult_%d.dat", i);
			fileFp = fopen(textContent, "ab+");
			fprintf(fileFp, "lineNum[%d] result[%.4f] lineInfo[%.4f, %.4f] end[%d,%d]",
				i, dResult, pReadPara[i].dCoeffK, pReadPara[i].dCoeffB, pReadPara[i].ptEnd.x, pReadPara[i].ptEnd.y);
			fclose(fileFp);
			//MResult[0];
			//MResult[1];

			MResult[i].dCoeffB=pReadPara[i].dCoeffB;
			MResult[i].dCoeffK=pReadPara[i].dCoeffK;
			MResult[i].ptEnd.x=pReadPara[i].ptEnd.x;
			MResult[i].ptEnd.y=pReadPara[i].ptEnd.y;
			MResult[i].MeterValue=dResult;
		}
		
		//cvWriteFrame(pWriter, testImage);

		//cvShowImage("Result", testImage);
		cvWaitKey(500);	// delay 2s
	//}
	//cvReleaseVideoWriter(&pWriter);

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
	//free(wy_maskImage.pixelArray.chunky.pPixel);
	//cvReleaseImage(&testImage);
	//cvReleaseImage(&maskImage);
	return res;
}
int MeterReadRecog(HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam)
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
	/*pCapture = cvCaptureFromFile(SOURCE_ROAD);
	if (MNull == pCapture)
	{
		WY_DEBUG_PRINT("get video file error!\n");
		goto EXT;
	}
	
	pFrame = cvQueryFrame(pCapture);*/
	pFrame = cvLoadImage(SOURCE_JPG, CV_LOAD_IMAGE_COLOR);
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
	/*if(0 != HYAMR_GetDesMem (hHYMRDHand, gMeterDes))
	{
		WY_DEBUG_PRINT("HYAMR_GetTemplateFromText error !\n");
		res = -1;
		goto EXT;
	}*/
	//��ȡ����׼���Ŀ��λ������
	/*if(0 != HYAMR_GetTaregetInfoFromMem (hHYMRDHand, gCurTarget, gTrainTarget, testImage->width, testImage->height))
	{
		WY_DEBUG_PRINT("HYAMR_GetTargetInfo error!\n");
		res = -1;
		goto EXT;
	}*/

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
	  
	while(1)
	//while(pFrame = cvQueryFrame(pCapture))
	{
		//pFrame = cvLoadImage("C:\\a.jpg", CV_LOAD_IMAGE_COLOR);
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
		if (0 != HYAMR_GetLineParamNoMem(hHYMRDHand, &wy_testImage, &inPara, pReadPara))
		//if (0 != HYAMR_GetLineParam(hHYMRDHand, &wy_testImage, &inPara, "OK", matchRate, gCurTarget, gTrainTarget, pReadPara))
		{
			WY_DEBUG_PRINT("HYAMR_GetLineParam error!\n");
			continue;
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
			/*sprintf(textContent, "lFrameNum=%d  lineNum[%d] result[%.4f]", lFrameNum, i, dResult);
			cvPutText(testImage, textContent, cvPoint(400, 150+10*i), &font, CV_RGB(0,255,0));*/
			WY_DEBUG_PRINT("result=%.4f\n\n", dResult);

			sprintf(textContent, "D:\\testResult_%d.dat", i);
			fileFp = fopen(textContent, "ab+");
			fprintf(fileFp, "lineNum[%d] result[%.4f] lineInfo[%.4f, %.4f] end[%d,%d]",
				i, dResult, pReadPara[i].dCoeffK, pReadPara[i].dCoeffB, pReadPara[i].ptEnd.x, pReadPara[i].ptEnd.y);
			fclose(fileFp);
		}
		
		cvWriteFrame(pWriter, testImage);

		cvShowImage("Result", testImage);
		cvWaitKey(500);	// delay 2s
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

void markPoints(IplImage *srcImage, MPOINT *ptList, MLong lPtNum)
{
	MLong lTmpNum;
	int mouseParam[3];
	MPOINT tmpPt;

	cvShowImage("markImage", srcImage);
	cvWaitKey(1);
	mouseParam[0]=-1;
	mouseParam[1]=-1;
	mouseParam[2]=-1;
	cvSetMouseCallback("markImage", onMouse, (void*)mouseParam);

	lTmpNum = 0;
	while(1)
	{
		if (CV_EVENT_RBUTTONDOWN == mouseParam[2])	// add red circle
		{
			if (mouseParam[0]>=0 && mouseParam[0]<srcImage->width
				&& mouseParam[1]>=0 && mouseParam[1]<srcImage->height)
			{
				tmpPt.x = mouseParam[0];
				tmpPt.y = mouseParam[1];
				*(ptList + lTmpNum) = tmpPt;
				lTmpNum++;
				cvCircle(srcImage,cvPoint(mouseParam[0],mouseParam[1]),1,CV_RGB(0,0,255),-1,8,0);
			}
			mouseParam[2] = -1;
		}
		cvShowImage("markImage",srcImage);
		if(13==cvWaitKey(10) || lTmpNum>=lPtNum)	// Enter
			break;
	}
}

void setMaskPic(IplImage *maskImage, MaskParam *pParam,MLong *Dist)
{
	MByte *pData;
	MLong lWidth, lHeight, lStride;
	MLong lExt;
	MyCircleParam outerCircle, innerCircle, middleCircle, tmpCircle;
	MLong lPoxY;
	MLong i,j;
	MPOINT *tmpPt;
	MPOINT pt1, pt2;
	MPOINT pt3, pt4, pt5, pt6;
	MPOINT pt7, pt8, pt9, pt10;
	MPOINT pt11, pt12, pt13, pt14;
	MPOINT pt15, pt16, pt17, pt18;
	MPOINT pt19, pt20, pt21, pt22;
	MPOINT pt23, pt24, pt25, pt26;
	MLong lDistance1, lDistance2, lDistance3;
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
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //ǰ���������Բ
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
				//ΪԲ��
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
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //ǰ���������Բ
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
						else
						{
							*pData = 255;
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
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //ǰ���������Բ
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
		case 10:
			{
				FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
				FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
				FittingCircle(tmpPt+6, 3, &middleCircle.cx, &middleCircle.cy, &middleCircle.r);
				if (innerCircle.r > middleCircle.r)
				{
					tmpCircle = middleCircle;
					middleCircle = innerCircle;
					innerCircle = tmpCircle;
				}
				if (innerCircle.r > outerCircle.r)
				{
					tmpCircle = outerCircle;
					outerCircle = innerCircle;
					innerCircle = tmpCircle;
				}
				if(middleCircle.r > outerCircle.r)
				{
					tmpCircle = outerCircle;
					outerCircle = middleCircle;
					middleCircle = tmpCircle;
				}
				*Dist = outerCircle.r - innerCircle.r ;
				pt1 = tmpPt[9];//Բ��
				for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
						lDistance1 = (MLong)(sqrt(tmp));
						tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
						lDistance2 = (MLong)(sqrt(tmp));
						tmp = (i - middleCircle.cx)*(i - middleCircle.cx)+(j - middleCircle.cy)*(j - middleCircle.cy);
						lDistance3 = (MLong)(sqrt(tmp));
						if(lDistance1>=outerCircle.r || lDistance2<=innerCircle.r)
						{
							*pData = 0;
						}
						else if(lDistance1 < outerCircle.r && lDistance3 > middleCircle.r)
						{
							*pData = 128;
						}
						else
						{
							*pData = 255;
						}
					}
				}
				break;
			}
		case 12:
			{
				FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
				FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
				if (outerCircle.r < innerCircle.r)
				{
					tmpCircle = outerCircle;
					outerCircle = innerCircle;
					innerCircle = tmpCircle;
				}
				*Dist = outerCircle.r - innerCircle.r ;
				pt1 = tmpPt[6];
				pt2 = tmpPt[7];
				pt3 = tmpPt[8];
				pt4 = tmpPt[9];
				pt5 = tmpPt[10];
				pt6 = tmpPt[11];
				for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
						lDistance1 = (MLong)(sqrt(tmp));
						tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
						lDistance2 = (MLong)(sqrt(tmp));
						if(lDistance1>=outerCircle.r || lDistance2<=innerCircle.r)
						{
							*pData = 0;
						}
						else if((i>pt5.x && i<pt6.x && j>pt5.y && j<pt6.y)||(i>pt3.x && i<pt4.x && j>pt3.y && j<pt4.y)||(i>pt1.x && i<pt2.x && j>pt1.y && j<pt2.y))
						{
							*pData = 128;
						}
						else
						{
							*pData = 255;
						}
					}
				}
				break;
			}
			case 18:
			{
				FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
				FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
				if (outerCircle.r < innerCircle.r)
				{
					tmpCircle = outerCircle;
					outerCircle = innerCircle;
					innerCircle = tmpCircle;
				}
				*Dist = outerCircle.r - innerCircle.r ;
				pt1 = tmpPt[6];
				pt2 = tmpPt[7];
				pt3 = tmpPt[8];
				pt4 = tmpPt[9];
				pt5 = tmpPt[10];
				pt6 = tmpPt[11];
				pt7 = tmpPt[12];
				pt8 = tmpPt[13];
				pt9 = tmpPt[14];
				pt10 = tmpPt[15];
				pt11 = tmpPt[16];
				pt12 = tmpPt[17];
				for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
						lDistance1 = (MLong)(sqrt(tmp));
						tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
						lDistance2 = (MLong)(sqrt(tmp));
						if(lDistance1>=outerCircle.r || lDistance2<=innerCircle.r)
						{
							*pData = 0;
						}
						else if((i>pt11.x && i<pt12.x && j>pt11.y && j<pt12.y)||(i>pt9.x && i<pt10.x && j>pt9.y && j<pt10.y)||(i>pt7.x && i<pt8.x && j>pt7.y && j<pt8.y)||
							(i>pt5.x && i<pt6.x && j>pt5.y && j<pt6.y)||(i>pt3.x && i<pt4.x && j>pt3.y && j<pt4.y)||(i>pt1.x && i<pt2.x && j>pt1.y && j<pt2.y))
						{
							*pData = 128;
						}
						else
						{
							*pData = 255;
						}
					}
				}
				break;
			}
			case 24:
			{
				FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
				FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
				if (outerCircle.r < innerCircle.r)
				{
					tmpCircle = outerCircle;
					outerCircle = innerCircle;
					innerCircle = tmpCircle;
				}
				*Dist = outerCircle.r - innerCircle.r ;
				pt1 = tmpPt[6];
				pt2 = tmpPt[7];
				pt3 = tmpPt[8];
				pt4 = tmpPt[9];
				pt5 = tmpPt[10];
				pt6 = tmpPt[11];
				pt7 = tmpPt[12];
				pt8 = tmpPt[13];
				pt9 = tmpPt[14];
				pt10 = tmpPt[15];
				pt11 = tmpPt[16];
				pt12 = tmpPt[17];
				pt13 = tmpPt[18];
				pt14 = tmpPt[19];
				pt15 = tmpPt[20];
				pt16 = tmpPt[21];
				pt17 = tmpPt[22];
				pt18 = tmpPt[23];
				for (j=0; j<lHeight; j++, pData+=lExt)
				{
					for (i=0; i<lWidth; i++, pData++)
					{
						tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
						lDistance1 = (MLong)(sqrt(tmp));
						tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
						lDistance2 = (MLong)(sqrt(tmp));
						if(lDistance1>=outerCircle.r || lDistance2<=innerCircle.r)
						{
							*pData = 0;
						}
						else if((i>pt17.x && i<pt18.x && j>pt17.y && j<pt18.y)||(i>pt15.x && i<pt16.x && j>pt15.y && j<pt16.y)||(i>pt13.x && i<pt14.x && j>pt13.y && j<pt14.y)||
							(i>pt11.x && i<pt12.x && j>pt11.y && j<pt12.y)||(i>pt9.x && i<pt10.x && j>pt9.y && j<pt10.y)||(i>pt7.x && i<pt8.x && j>pt7.y && j<pt8.y)||
							(i>pt5.x && i<pt6.x && j>pt5.y && j<pt6.y)||(i>pt3.x && i<pt4.x && j>pt3.y && j<pt4.y)||(i>pt1.x && i<pt2.x && j>pt1.y && j<pt2.y))
						{
							*pData = 128;
						}
						else
						{
							*pData = 255;
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
	cvSaveImage("D:\\maskImage.bmp", maskImage);
	cvShowImage("maskImage", maskImage);
	cvWaitKey(10);
}
void setMaskPicIn(HYLPAMR_IMAGES maskImage ,MaskParam *pParam,MLong *Dist)
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

	//lWidth = maskImage->width;
	//lHeight = maskImage->height;
	//lStride = maskImage->widthStep;
	lWidth = maskImage->lWidth;
	lHeight = maskImage->lHeight;
	lStride = maskImage->pixelArray.chunky.lLineBytes;
	lExt = lStride - lWidth;
	//pData = (MByte*)maskImage->imageData;
	pData = (MByte*)maskImage->pixelArray.chunky.pPixel;
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
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //ǰ���������Բ
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
				//ΪԲ��
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
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //ǰ���������Բ
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
			FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //ǰ���������Բ
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
		//��ϲ�����
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


//MRESULT GetRectFromMask(IplImage *pMask,rect pMaskRect)
//{
//	MRECT rect;
//	MInt16 x,y;
//	MUInt8* pMaskData=(MUInt8*)(pMask->pBlockData);
//	int count = 0;
//	MRESULT res = LI_ERR_NONE;
//	rect.left=pMask->lWidth;
//	rect.right=0;
//	rect.top=pMask->lHeight;
//	rect.bottom=0;
//	
//	for (y=0;y<pMask->lHeight;y++)
//	{
//		for (x=0;x<pMask->lWidth;x++)
//		{
//			if (*(pMaskData+y*pMask->lBlockLine+x)>100)
//			{
//				count++;
//				if(rect.left>x) 
//				{
//					rect.left=x;
//				}
//				if(rect.right<x)
//				{
//					rect.right=x;
//				}
//				if (rect.top>y)
//				{
//					rect.top=y;
//				}
//				if(rect.bottom<y)
//				{
//					rect.bottom=y;
//				}
//			}
//		}
//	}
//	if (0==count)
//	{
//		res = LI_ERR_NO_FIND;
//		return res;
//	}
//	pMaskRect->bottom=rect.bottom;
//	pMaskRect->left=rect.left;
//	pMaskRect->right=rect.right;
//	pMaskRect->top=rect.top;
//	return res;
//}
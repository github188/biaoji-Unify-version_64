#include <iostream>
#include "HY_MeterRecogV2.h"
#include <opencv2/opencv.hpp>
using namespace std;

#pragma comment(lib, "../Release/watch_match.lib")
#define GrayStep(x)			(( (x) + 3 ) & (~3) )
int ReadMeter(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine, const char* data_path);
int main(int argc, char *argv[])
{
	// Default settings used in HDevelop (can be omitted) 
	int ret = 0;
	IplImage* maskImage = cvLoadImage("D:/tmp.bmp", 1);
	//IplImage* tmpImage = cvLoadImage("C:/watch/基准图.bmp", 1);
	HYAMR_IMAGES src = { 0 };
	src.lWidth = maskImage->width;
	src.lHeight = maskImage->height;
	src.pixelArray.chunky.lLineBytes = maskImage->widthStep;
	src.pixelArray.chunky.pPixel = maskImage->imageData;
	src.lPixelArrayFormat = HYAMR_IMAGE_BGR;
	HYAMR_INTERACTIVE_PARA point = { 0 };
	MaskParam mask = { 0 };
	WidthType width = { 0 };
	char *peizhifile = "./biaoji/sf6/biaoji.mat";
	ReadMeter(&point, &mask, &width, peizhifile);
	//paintpoint(tmpImage, &point, &mask, &width);
	//width = (WidthType*)malloc(11 * sizeof(double));
	//point = (HYAMR_INTERACTIVE_PARA*)malloc(100 * sizeof(double));
	//mask = (MaskParam*)malloc(11 * sizeof(double));
	int i, j;
	int c;
	char *a = "./biaoji/sf6/匹配模板.jpg";
	HYAMR_IMAGES wy_maskImage = { 0 };
	wy_maskImage.lHeight = 32;
	wy_maskImage.lWidth = 416;
	wy_maskImage.pixelArray.chunky.lLineBytes = GrayStep(3*wy_maskImage.lWidth);
	wy_maskImage.pixelArray.chunky.pPixel = (MByte *)calloc(wy_maskImage.lHeight*wy_maskImage.pixelArray.chunky.lLineBytes, sizeof(MByte));
	wy_maskImage.lPixelArrayFormat = HYAMR_IMAGE_BGR;
	MLong lWidth, lHeight, lStride;
	SF6PT sf = { 0 };
	ReadMeter(&point, &mask, &width, peizhifile);
	//c = action(src, &wy_maskImage, &point, &mask, &width, &point.Dist, a, 0.6, 1.4);
	sf6(src, &wy_maskImage, &point, &mask, &width, &sf, &point.Dist, a, 0.9, 1.9);
	//IplImage* blank = cvLoadImage("D:/blank", 0);
	lWidth = wy_maskImage.lWidth;
	lHeight = wy_maskImage.lHeight;
	lStride = wy_maskImage.pixelArray.chunky.lLineBytes;
	
	//unsigned char *data = (unsigned char *)blank->imageData;
	
	//paintpoint(maskImage, &point, &mask, &width);
	printf("%d,%d\n", point.ptPosList[0].x, point.ptPosList[0].y);
	printf("%d,%d", mask.maskPt[0].x, mask.maskPt[0].y);
	getchar();
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

	

	//存起始点和终点坐标
	fscanf(fp, "%ld", &(pOutPattern->startPt.x));
	//printf("%d\n",pOutPattern->startPt.x);
	fscanf(fp, "%ld", &(pOutPattern->endPt.x));
	fscanf(fp, "%ld", &(pOutPattern->startPt.y));
	fscanf(fp, "%ld", &(pOutPattern->endPt.y));

	fscanf(fp, "%ld", &(pOutPattern->lPtNum));
	//存刻度点坐标
	for (int i = 0; i < pOutPattern->lPtNum; i++)
	{
		fscanf(fp, "%ld", &(pOutPattern->ptPosList[i].x));
		fscanf(fp, "%ld", &(pOutPattern->ptPosList[i].y));
	}
	//存每个点对应的刻度值
	for (int i = 0; i <pOutPattern->lPtNum; i++){
		fscanf(fp, "%lf", &(pOutPattern->dPtValList[i]));
	}
	//存圈定圆心
	fscanf(fp, "%ld", &(pOutPattern->circleCoord.x));
	fscanf(fp, "%ld", &(pOutPattern->circleCoord.y));
	//存指针条数
	fscanf(fp, "%d", &(pOutPattern->lLineNum));
	//存指针颜色
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
	//存表盘背景是否白色
	fscanf(fp, "%ld", &(pOutPattern->bWhiteBackground));

	fscanf(fp, "%ld", &(HaarLine->LType));
	TmpVal=HaarLine->LType;
	//fscanf(fp, "%ld", &TmpVal);
	//存表盘指针宽度
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
	//MASK图
	//pMaskParam = (MaskParam*)malloc(100 * sizeof(MaskParam));
	//存入构造Mask图的点数
	fscanf(fp, "%ld", &(pMaskParam->lMaskPtNum));//
	
	for (int i = 0; i < pMaskParam->lMaskPtNum; i++)
	{
		fscanf(fp, "%ld", &(pMaskParam->maskPt[i].x));
		fscanf(fp, "%ld", &(pMaskParam->maskPt[i].y));
	}
	//---------------------------------------//
	//存指针线与表盘背景区分度
	fscanf(fp, "%ld", &(pOutPattern->picLevel));
	
	fclose(fp);
	return 0;
}
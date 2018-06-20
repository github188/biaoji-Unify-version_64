#include <iostream>
#include "HY_MeterRecogV2.h"
#include <opencv2/opencv.hpp>
using namespace std;

#pragma comment(lib, "../../testbed/testbed_win32/Release/watch_match.lib")
#define GrayStep(x)			(( (x) + 3 ) & (~3) )
int ReadMeter(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine, const char* data_path);
int MeterReadRecogV2(HYLPAMR_IMAGES src,MeterDESCRIP Meterdescrip,HYAMR_Meter_Result *MeterResult)
{
	char *peizhifile;
	char *imgfile;
	HYAMR_INTERACTIVE_PARA para = {0};
	MaskParam mParam = {0};
	WidthType HaarLine={0};
	SF6PT pPtPos={0};
	HYAMR_IMAGES img={0};
	HYAMR_IMAGES wy_maskImage = {0};
	HYAMR_IMAGES IndexImage={0};
	IplImage *srcImage=cvLoadImage("D:/tmp.bmp");
	img.lWidth = srcImage->width;
	img.lHeight = srcImage->height;
	img.pixelArray.chunky.lLineBytes = srcImage->widthStep;
	img.pixelArray.chunky.pPixel = srcImage->imageData;
	img.lPixelArrayFormat = HYAMR_IMAGE_BGR;//opencv��ȡ��ͼĬ��ͨ��˳��Ϊ'BGR' 
	peizhifile="./biaoji/sf6/biaoji.mat";
	imgfile="./biaoji/sf6/ƥ��ģ��.jpg";
	// maskͼ
	wy_maskImage.lHeight=img.lHeight;
	wy_maskImage.lWidth=img.lWidth;
	wy_maskImage.pixelArray.chunky.lLineBytes=GrayStep(wy_maskImage.lWidth);
	wy_maskImage.pixelArray.chunky.pPixel=(MByte *)calloc(wy_maskImage.lHeight*wy_maskImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	wy_maskImage.lPixelArrayFormat=HYAMR_IMAGE_GRAY;
	IndexImage.lPixelArrayFormat=HYAMR_IMAGE_BGR;
	IndexImage.lWidth=416;
	IndexImage.lHeight=32;
	IndexImage.pixelArray.chunky.lLineBytes=GrayStep(3*IndexImage.lWidth);
	IndexImage.pixelArray.chunky.pPixel=(MByte *)calloc(IndexImage.lHeight*IndexImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	ReadMeter(&para, &mParam,&HaarLine,peizhifile);
	//action(img, &wy_maskImage,&para, &mParam, &HaarLine,&para.Dist,imgfile,0.9,1.4);
	sf6(img, &IndexImage,&para, &mParam, &HaarLine,&pPtPos,&para.Dist,imgfile,0.9,1.9);

	return 0;
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
#include <iostream>
#include "HY_MeterRecogV2.h"

using namespace std;

#pragma comment(lib, "../../testbed/testbed_win32/Release/watch_match.lib")

typedef struct
{
	MLong cx;
	MLong cy;
	MLong r;
}MyCircleParam;
#define DEBUG_DEMO

#ifdef DEBUG_DEMO
#define WY_DEBUG_PRINT printf
#else
#define WY_DEBUG_PRINT
#endif
#define MR_MEM_SIZE (100*1024*1024)
#define MAX_BUFFER_LEN 5
#define GrayStep(x)			(( (x) + 3 ) & (~3) )
int SafetyRead(HYLPAMR_IMAGES src,HYAMR_INTERACTIVE_PARA *inPara, MaskParam *pMaskParam);
int ReadMeter(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine, const char* data_path);
int MeterRead_Recog(HYLPAMR_IMAGES src,HYAMR_IMAGES wy_maskImage,HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam,HYAMR_Meter_Result *MResult);
void setMaskPicIn(HYLPAMR_IMAGES maskImage ,MaskParam *pParam,MLong *Dist);
void FittingCircle(MPOINT *pPtList, MLong lPtLen,MLong *xc, MLong *yc, MLong *r);
int MeterReadRecogV2(HYLPAMR_IMAGES src,MeterDESCRIP Meterdescrip,HYAMR_Meter_Result *MeterResult){
	int res=0;
	char *peizhifile;
	char *imgfile;
	int i=0,j=0;
	MByte *srcData;
	MByte *dstData;
	HYAMR_INTERACTIVE_PARA para = {0};
	MaskParam mParam = {0};
	WidthType HaarLine={0};
	HYAMR_Meter_Result *MResult=(HYAMR_Meter_Result *)calloc(2,sizeof(HYAMR_Meter_Result));
	//HYAMR_Meter_Result MResult={0};
	HYAMR_IMAGES img={0};
	HYAMR_IMAGES wy_maskImage = {0};
	int QRW=Meterdescrip.MeterRect.right-Meterdescrip.MeterRect.left;//二维码
	int QRH=Meterdescrip.MeterRect.bottom-Meterdescrip.MeterRect.top;
	srcData = (MByte*)src->pixelArray.chunky.pPixel;
	
	img.lPixelArrayFormat = src->lPixelArrayFormat;
	if(src->lPixelArrayFormat==HYAMR_IMAGE_BGR){
		int Hstart=Meterdescrip.MeterRect.top-3*QRH;
		int Hend=Meterdescrip.MeterRect.bottom;
		int Wstart=Meterdescrip.MeterRect.left-QRW;
		int Wend=Meterdescrip.MeterRect.right+QRW;
		if(Hstart<0)Hstart=0;
		if(Hend>src->lHeight)Hend=src->lHeight;
		if(Wstart<0)Wstart=0;
		if(Wend>src->lWidth)Wend=src->lWidth;
		//printf("(%d,%d),(%d,%d)\n",Wstart,Hstart,Wend,Hend);
		img.lWidth=Wend-Wstart;
		img.lHeight=Hend-Hstart;
		img.pixelArray.chunky.lLineBytes=GrayStep(3*img.lWidth);
		img.pixelArray.chunky.pPixel=(MByte *)calloc(img.lHeight*img.pixelArray.chunky.lLineBytes,sizeof(MByte));
		dstData = (MByte*)img.pixelArray.chunky.pPixel;
		for(j=Hstart;j<Hend;j++){//ROI:Wstart,Hstart,Wend,Hend
			for(i=Wstart;i<Wend;i++){
				dstData[0+(i-Wstart)*3+(j-Hstart)*img.pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+0];
				dstData[1+(i-Wstart)*3+(j-Hstart)*img.pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+1];
				dstData[2+(i-Wstart)*3+(j-Hstart)*img.pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+2];
			}
		}
	}
	// mask图
	wy_maskImage.lHeight=img.lHeight;
	wy_maskImage.lWidth=img.lWidth;
	wy_maskImage.pixelArray.chunky.lLineBytes=GrayStep(wy_maskImage.lWidth);
	wy_maskImage.pixelArray.chunky.pPixel=(MByte *)calloc(wy_maskImage.lHeight*wy_maskImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	wy_maskImage.lPixelArrayFormat=HYAMR_IMAGE_GRAY;
	//仪表类型
	if(Meterdescrip.MeterType==1){
		peizhifile="D:/biaoji/bileiqi/biaoji.mat";
		imgfile="D:/biaoji/bileiqi/匹配模板.jpg";
	}
	else if(Meterdescrip.MeterType==2){
		peizhifile="D:/biaoji/sf6new/biaoji_12p.mat";
		imgfile="D:/biaoji/sf6new/匹配模板.jpg";
		//peizhifile="D:/biaoji/sf6backup/biaoji_backupbig.mat";
		//imgfile="D:/biaoji/sf6backup/匹配模板.jpg";
	}
	else{
		printf("未知仪表类型！！！\n");
		res=-1;
		goto EXT;
	}
	if(0>ReadMeter(&para, &mParam,&HaarLine,peizhifile)){//读取配置文件
		printf("配置文件不存在!!!\n");
		res=-2;
		goto EXT;
	}
		//ReadMeter(&para, &mParam,&HaarLine,peizhifile);
	para.Dist = 0xFFFF;
	try
	{
		res=action(img, &wy_maskImage,&para, &mParam, &HaarLine,&para.Dist,imgfile,0.6,1.4);
		if(res<0){
			printf("配准失败，仪表参数无法转换!!!\n");goto EXT;
		}
	}
	catch(...)
	{
		res=-3;
		goto EXT;
	}
	//printf("para.lLineWidth=%d\n",para.lLineWidth);
	//printf("para.Dist=%d\n",para.Dist);

	//if(0>action(img, &para, &mParam, &HaarLine,imgfile,0.6,1.4)){//配准
	//	printf("配准失败，仪表参数无法转换!!!\n");
	//	res=-3;
	//	goto EXT;
	//}
	if(0>SafetyRead(&img,&para, &mParam)){
		printf("配准失败，仪表参数转换出界!!!\n");
		res=-4;
		goto EXT;
	}
		//action(img, &para, &mParam, &HaarLine,imgfile,0.6,1.4);
	if(0>MeterRead_Recog(&img,wy_maskImage,para, &mParam,MResult)){//读数
		printf("仪表读数失败!!!\n");
		res=-5;
		goto EXT;
	}
		//MeterRead_Recog(&img,para, &mParam,MResult);
	MeterResult->dCoeffB=MResult[0].dCoeffB;
	MeterResult->dCoeffK=MResult[0].dCoeffK;
	MeterResult->MeterValue=MResult[0].MeterValue;
	MeterResult->ptEnd.x=MResult[0].ptEnd.x;
	MeterResult->ptEnd.y=MResult[0].ptEnd.y;
	//printf("MeterValue=%f\n",MeterResult->MeterValue);
EXT:
	free(MResult);
	free(img.pixelArray.chunky.pPixel);
	free(wy_maskImage.pixelArray.chunky.pPixel);
	return res;
}
int SafetyRead(HYLPAMR_IMAGES src,HYAMR_INTERACTIVE_PARA *inPara, MaskParam *pMaskParam){
	if(inPara->startPt.x<0 || inPara->startPt.x>=src->lWidth)return -1;
	if(inPara->startPt.y<0 || inPara->startPt.y>=src->lHeight)return -1;
	if(inPara->endPt.x<0 || inPara->endPt.x>=src->lWidth)return -1;
	if(inPara->endPt.y<0 || inPara->endPt.y>=src->lHeight)return -1;

	for(int i=0;i<inPara->lPtNum;i++){
		if(inPara->ptPosList[i].x<0 ||inPara->ptPosList[i].x>=src->lWidth)return -1;
		if(inPara->ptPosList[i].y<0 ||inPara->ptPosList[i].y>=src->lHeight)return -1;
	}
	if(inPara->circleCoord.x<0 ||inPara->circleCoord.x>=src->lWidth)return -1;
	if(inPara->circleCoord.y<0 ||inPara->circleCoord.y>=src->lHeight)return -1;

	if(inPara->lLineWidth <=0)return -1;
	if(inPara->lLineWidth <=2)inPara->lLineWidth = 2;

	for(int i=0;i<pMaskParam->lMaskPtNum;i++){
		if(pMaskParam->maskPt[i].x<0 ||pMaskParam->maskPt[i].x>=src->lWidth)return -1;
		if(pMaskParam->maskPt[i].y<0 ||pMaskParam->maskPt[i].y>=src->lHeight)return -1;
	}
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
int MeterRead_Recog(HYLPAMR_IMAGES src,HYAMR_IMAGES wy_maskImage,HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam,HYAMR_Meter_Result *MResult)
{
	int res = 0;
	//inPara.Dist = 0xFFFF;

	MHandle hMemMgr = MNull;
	MHandle hHYMRDHand = MNull;
	MVoid *pMem = MNull;

	
	HYAMR_IMAGES wy_testImage = {0};

	//HYAMR_IMAGES wy_maskImage = {0};

	MLong lFrameNum = 0;

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

	
	/*wy_maskImage.lHeight=src->lHeight;
	wy_maskImage.lWidth=src->lWidth;
	wy_maskImage.pixelArray.chunky.lLineBytes=GrayStep(wy_maskImage.lWidth);
	wy_maskImage.pixelArray.chunky.pPixel=(MByte *)calloc(wy_maskImage.lHeight*wy_maskImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	wy_maskImage.lPixelArrayFormat=HYAMR_IMAGE_GRAY;
	setMaskPicIn(&wy_maskImage, pMaskParam,&inPara.Dist);*/

	

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

		//WY_DEBUG_PRINT("lFrameNum=%d\n", lFrameNum);
		
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
			res = -1;
			goto EXT;
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
			//WY_DEBUG_PRINT("result=%.4f\n\n", dResult);

			//sprintf(textContent, "D:\\testResult_%d.dat", i);
			//fileFp = fopen(textContent, "ab+");
			//fprintf(fileFp, "lineNum[%d] result[%.4f] lineInfo[%.4f, %.4f] end[%d,%d]",
			//	i, dResult, pReadPara[i].dCoeffK, pReadPara[i].dCoeffB, pReadPara[i].ptEnd.x, pReadPara[i].ptEnd.y);
			//fclose(fileFp);

			MResult[i].dCoeffB=pReadPara[i].dCoeffB;
			MResult[i].dCoeffK=pReadPara[i].dCoeffK;
			MResult[i].ptEnd.x=pReadPara[i].ptEnd.x;
			MResult[i].ptEnd.y=pReadPara[i].ptEnd.y;
			MResult[i].MeterValue=dResult;
		}
		
		//cvWriteFrame(pWriter, testImage);

		//cvShowImage("Result", testImage);
		//cvWaitKey(500);	// delay 2s
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

	//free(wy_maskImage.pixelArray.chunky.pPixel);

	

	//cvReleaseImage(&testImage);
	//cvReleaseImage(&maskImage);
	return res;
}
void setMaskPicIn(HYLPAMR_IMAGES maskImage ,MaskParam *pParam,MLong *Dist)
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
				*Dist = outerCircle.r - innerCircle.r ;
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
				pt1 = tmpPt[9];//圆心
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
	//cvSaveImage("D:\\maskImage.bmp", maskImage);
	//cvShowImage("maskImage", maskImage);
	//cvWaitKey(10);
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
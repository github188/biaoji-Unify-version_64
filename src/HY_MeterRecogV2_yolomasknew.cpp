#include <iostream>
#include "HY_MeterRecogV2.h"
#include "HY_Network.h"
//#include <opencv2/opencv.hpp>
#include <time.h>
using namespace std;

#pragma comment(lib, "../../testbed/testbed_win32/Release/watch_match.lib")
#pragma comment(lib, "../../testbed/testbed_win32/Release/HY_Network_CPU.lib")


#define DEBUG_DEMO

#ifdef DEBUG_DEMO
#define WY_DEBUG_PRINT printf
#else
#define WY_DEBUG_PRINT
#endif
#define MR_MEM_SIZE (100*1024*1024)
#define MAX_BUFFER_LEN 5
#define GrayStep(x)			(( (x) + 3 ) & (~3) )
int yibiaoROI(HYLPAMR_IMAGES src,HYLPAMR_IMAGES dst,MeterDESCRIP Meterdescrip,char *cfgfile,char *weightfile);
int BileiqiROI(HYLPAMR_IMAGES src,HYLPAMR_IMAGES dst,char *cfgfile,char *weightfile);
int SafetyRead(HYLPAMR_IMAGES src,HYAMR_INTERACTIVE_PARA *inPara, MaskParam *pMaskParam);
int ReadMeter(HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam,WidthType *HaarLine, const char* data_path);
int MeterRead_Recog(HYLPAMR_IMAGES src,HYAMR_IMAGES wy_maskImage,HYAMR_INTERACTIVE_PARA inPara, MaskParam *pMaskParam,HYAMR_Meter_Result *MResult);
void setMaskPicIn(HYLPAMR_IMAGES maskImage ,MaskParam *pParam,MLong *Dist);
void FittingCircle(MPOINT *pPtList, MLong lPtLen,MLong *xc, MLong *yc, MLong *r);
/**************opencv***********
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
/*************************/
float sec(clock_t clocks)
{
	return (float)clocks/1000;
}
int MeterReadRecogV2(HYLPAMR_IMAGES src,MeterDESCRIP Meterdescrip,HYAMR_Meter_Result *MeterResult){
	int res=0;
	char *peizhifile;
	char *imgfile;
	char *cfgfile;
	char *weightfile;
	int i=0,j=0;
	MByte *srcData;
	MByte *dstData;
	HYAMR_INTERACTIVE_PARA para = {0};
	MaskParam mParam = {0};
	WidthType HaarLine={0};
	SF6PT pPtPos={0};
	HYAMR_Meter_Result *MResult=(HYAMR_Meter_Result *)calloc(2,sizeof(HYAMR_Meter_Result));
	float MaxProb;
	int Maxindex=0;
	HYAMR_IMAGES img={0};
	HYAMR_IMAGES dst={0};
	HYAMR_IMAGES IndexImage={0};
	HYAMR_IMAGES wy_maskImage = {0};
	cfgfile="./biaoji/model/tiny-yolo-voc.cfg";
	weightfile="./biaoji/model/tiny-yolo-voc_final.weights";
	if(0>yibiaoROI(src,&img,Meterdescrip,cfgfile,weightfile))
	{
		printf("仪表定位失败\n");
		res=-1;
		free(MResult);
		return res;
	}
	//yibiaoROI(src,&img,Meterdescrip,cfgfile,weightfile);
	//opencv  仪表图打印
	/*MByte *tmpData; 
	IplImage *tmp=cvCreateImage(cvSize(img.lWidth,img.lHeight),IPL_DEPTH_8U,3);
	tmpData = (MByte*)tmp->imageData;
	dstData = (MByte*)img.pixelArray.chunky.pPixel;
	for(j=0;j<img.lHeight;j++){
		for(i=0;i<3*img.lWidth;i++){
			tmpData[i+j*tmp->widthStep]=dstData[i+j*img.pixelArray.chunky.lLineBytes];
		}
	}
	cvSaveImage("D:\\tmp.bmp", tmp);*/
	//cvReleaseImage(&tmp);
	// mask图
	wy_maskImage.lHeight=img.lHeight;
	wy_maskImage.lWidth=img.lWidth;
	wy_maskImage.pixelArray.chunky.lLineBytes=GrayStep(wy_maskImage.lWidth);
	wy_maskImage.pixelArray.chunky.pPixel=(MByte *)calloc(wy_maskImage.lHeight*wy_maskImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	wy_maskImage.lPixelArrayFormat=HYAMR_IMAGE_GRAY;
	//dst避雷器框选刻度图
	dst.lPixelArrayFormat = img.lPixelArrayFormat;
	dst.lWidth=img.lWidth;
	dst.lHeight=img.lHeight;
	dst.pixelArray.chunky.lLineBytes=GrayStep(3*dst.lWidth);
	dst.pixelArray.chunky.pPixel=(MByte *)calloc(dst.lHeight*dst.pixelArray.chunky.lLineBytes,sizeof(MByte));
	//sf6表盘刻度长条图
	IndexImage.lPixelArrayFormat=HYAMR_IMAGE_BGR;
	IndexImage.lWidth=416;
	IndexImage.lHeight=32;
	IndexImage.pixelArray.chunky.lLineBytes=GrayStep(3*IndexImage.lWidth);
	IndexImage.pixelArray.chunky.pPixel=(MByte *)calloc(IndexImage.lHeight*IndexImage.pixelArray.chunky.lLineBytes,sizeof(MByte));
	//仪表类型
	if(Meterdescrip.MeterType==1){
		peizhifile="./biaoji/bileiqi/biaoji.mat";
		imgfile="./biaoji/bileiqi/匹配模板.jpg";
		cfgfile="./biaoji/bileiqi/model/tiny-yolo-voc.cfg";
		weightfile="./biaoji/bileiqi/model/tiny-yolo-voc_final.weights";
		if(0>BileiqiROI(&img,&dst,cfgfile,weightfile)){
			printf("未找到刻度区域\n");
			res=-6;
			goto EXT;
		}
		//opencv  刻度区域打印
		/*IplImage *tmp1=cvCreateImage(cvSize(dst.lWidth,dst.lHeight),IPL_DEPTH_8U,3);
		tmpData = (MByte*)tmp1->imageData;
		dstData = (MByte*)dst.pixelArray.chunky.pPixel;
		for(j=0;j<dst.lHeight;j++){
			for(i=0;i<3*dst.lWidth;i++){
				tmpData[i+j*tmp1->widthStep]=dstData[i+j*dst.pixelArray.chunky.lLineBytes];
			}
		}
		cvSaveImage("D:\\tmp1.bmp", tmp1);*/
	}
	else if(Meterdescrip.MeterType==2){
		peizhifile="./biaoji/sf6/biaoji.mat";
		imgfile="./biaoji/sf6/匹配模板.jpg";
		//peizhifile="D:/biaoji/sf6new/biaoji_6p.mat";
		//imgfile="D:/biaoji/sf6new/匹配模板.jpg";
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
	clock_t time;
	time=clock();
	try
	{
		if(Meterdescrip.MeterType==1)
			res=action(dst, &wy_maskImage,&para, &mParam, &HaarLine,&para.Dist,imgfile,0.9,1.4);
		else
			//res=action(img, &wy_maskImage,&para, &mParam, &HaarLine,&para.Dist,imgfile,0.9,2.1);
			res=sf6(img, &IndexImage,&para, &mParam, &HaarLine,&pPtPos,&para.Dist,imgfile,0.9,2.1);
		if(res<0){
			printf("配准失败，仪表参数无法转换!!!\n");goto EXT;
		}
	}
	catch(...)
	{
		res=-3;
		goto EXT;
	}
	printf("action %f seconds\n",sec(clock()-time));
	//opencv  sf6刻度区域拉直打印
	/*MByte *indexData; 
	IplImage *indeximg=cvCreateImage(cvSize(IndexImage.lWidth,IndexImage.lHeight),IPL_DEPTH_8U,3);
	indexData = (MByte*)indeximg->imageData;
	dstData = (MByte*)IndexImage.pixelArray.chunky.pPixel;
	for(j=0;j<IndexImage.lHeight;j++){
		for(i=0;i<3*IndexImage.lWidth;i++){
			indexData[i+j*indeximg->widthStep]=dstData[i+j*IndexImage.pixelArray.chunky.lLineBytes];
		}
	}
	cvSaveImage("D:\\indeximg.bmp", indeximg);
	cvReleaseImage(&indeximg);*/
	//paintpoint(tmp,&para, &mParam, &HaarLine);
	if(0>SafetyRead(&img,&para, &mParam)){
		printf("配准失败，仪表参数转换出界!!!\n");
		res=-4;
		goto EXT;
	}
		//action(img, &para, &mParam, &HaarLine,imgfile,0.6,1.4);
	if(Meterdescrip.MeterType==1)
	{
		if(0>MeterRead_Recog(&img,wy_maskImage,para, &mParam,MResult)){//读数
			printf("仪表读数失败!!!\n");
			res=-5;
			goto EXT;
		}
	}
	else
	{
		clock_t time;
		YoloNet_detect yolo;
		MByte *IndexData;
		MPOINT sf6finger={0};
		int fingernum=0;
		int dMinDist=0xFFFF;
		int tmpgreater=0;
		int tmpless=0;
		int sf6Distance[100]={0};
		float thresh=0.1;
		int Indexnum;
		int Indexboxnum=1000;
		float **Indexboxout;
		Indexboxout=(float **)calloc(Indexboxnum,sizeof(float *));
		for(i=0;i<Indexboxnum;i++)Indexboxout[i]=(float *)calloc(6,sizeof(float));
		IndexData=(MByte*)IndexImage.pixelArray.chunky.pPixel;
		cfgfile="./biaoji/sf6/model/tiny-yolo-voc.cfg";
		weightfile="./biaoji/sf6/model/tiny-yolo-voc_final.weights";
		time=clock();
		yolo.initNetData(cfgfile,weightfile);
		yolo.object_detection(IndexData,IndexImage.lHeight,IndexImage.lWidth,3,(int)IndexImage.pixelArray.chunky.lLineBytes,thresh,&Indexnum,Indexboxout);
		printf("fingerfind %f seconds\n",sec(clock()-time));
		yolo.ReleaseNetwork();
		for(i=0;i<Indexnum;i++)
		{
			if(Indexboxout[i][5]==1)
				fingernum++; 
		}
		if(fingernum==0){
			printf("未找到sf6指针\n");
			res=-1;
			goto EXT;
		}
		Maxindex=0;
		MaxProb=0;
		for(i=0;i<Indexnum;i++){
			if(Indexboxout[i][5]==1)
			{
				if(Indexboxout[i][4] > MaxProb){
					MaxProb=Indexboxout[i][4];
					Maxindex=i;
				}
			}
		}
		//printf("left=%f right=%f top=%f bot=%f\n",Indexboxout[Maxindex][0],Indexboxout[Maxindex][1],Indexboxout[Maxindex][2],Indexboxout[Maxindex][3]);
		sf6finger.x=(Indexboxout[Maxindex][1]+Indexboxout[Maxindex][0])/2;
		sf6finger.y=(Indexboxout[Maxindex][3]+Indexboxout[Maxindex][2])/2;
		//printf("x=%d\n",sf6finger.x);
		
		pPtPos.lPtNum=11;//找到多少个点由sf6函数传给我
		/************
		double temp=0.0;
		for(i=0;i<pPtPos.lPtNum-1;i++)
		{
			for(j=0;j<pPtPos.lPtNum-1-i;j++)
				if(pPtPos.ptPosList[j].x>pPtPos.ptPosList[j+1].x)
				{
					temp=pPtPos.ptPosList[j].x;
					pPtPos.ptPosList[j].x=pPtPos.ptPosList[j+1].x;
					pPtPos.ptPosList[j+1].x=temp;
					temp=pPtPos.ptPosList[j].y;
					pPtPos.ptPosList[j].y=pPtPos.ptPosList[j+1].y;
					pPtPos.ptPosList[j+1].y=temp;
				}
		}
		pPtPos.dPtValList[0]=0.3;
		pPtPos.dPtValList[1]=0.2;
		pPtPos.dPtValList[2]=0.1;
		pPtPos.dPtValList[3]=0.0;
		pPtPos.dPtValList[4]=-0.1;
		pPtPos.dPtValList[5]=0.9;
		pPtPos.dPtValList[6]=0.8;
		pPtPos.dPtValList[7]=0.7;
		pPtPos.dPtValList[8]=0.6;
		pPtPos.dPtValList[9]=0.5;
		pPtPos.dPtValList[10]=0.4;
		/************/
		Maxindex=0;
		for(i=0;i<pPtPos.lPtNum;i++)//寻找离指针最近的刻度
		{
			sf6Distance[i]=pPtPos.ptPosList[i].x-sf6finger.x;
			if(dMinDist > abs(sf6Distance[i]))
			{
				dMinDist=abs(sf6Distance[i]);
				Maxindex=i;
			}
		}
		if(sf6Distance[Maxindex]==0)//指针在刻度上
		{
			MResult[0].MeterValue=pPtPos.dPtValList[Maxindex];
		}
		else//指针不在刻度上
		{
			for(i=0;i<pPtPos.lPtNum;i++)
			{
				if(sf6Distance[i] > 0)
					tmpgreater++;
				else
					tmpless++;
			}
			if(tmpgreater == pPtPos.lPtNum)//指针在最右边,另一侧为Maxindex-1
			{
				MResult[0].MeterValue=(pPtPos.dPtValList[Maxindex]*(IndexImage.lWidth-abs(sf6Distance[Maxindex-1]))+pPtPos.dPtValList[Maxindex-1]*abs(sf6Distance[Maxindex]))/(IndexImage.lWidth-abs(sf6Distance[Maxindex-1])+abs(sf6Distance[Maxindex]));	
			}
			else if(tmpless == pPtPos.lPtNum)//指针在最左边,另一侧为Maxindex+1
			{
				MResult[0].MeterValue=(pPtPos.dPtValList[Maxindex]*(IndexImage.lWidth-abs(sf6Distance[Maxindex+1]))+pPtPos.dPtValList[Maxindex+1]*abs(sf6Distance[Maxindex]))/(IndexImage.lWidth-abs(sf6Distance[Maxindex+1])+abs(sf6Distance[Maxindex]));
			}
			else//指针左右都有刻度
			{
				if(abs(sf6Distance[Maxindex+1])>abs(sf6Distance[Maxindex-1]))//另一侧为Maxindex-1
				{
					MResult[0].MeterValue=(pPtPos.dPtValList[Maxindex]*abs(sf6Distance[Maxindex-1])+pPtPos.dPtValList[Maxindex-1]*abs(sf6Distance[Maxindex]))/(abs(sf6Distance[Maxindex-1])+abs(sf6Distance[Maxindex]));
				}
				else//另一侧为Maxindex+1
				{
					MResult[0].MeterValue=(pPtPos.dPtValList[Maxindex]*abs(sf6Distance[Maxindex+1])+pPtPos.dPtValList[Maxindex+1]*abs(sf6Distance[Maxindex]))/(abs(sf6Distance[Maxindex+1])+abs(sf6Distance[Maxindex]));
				}
			}
		}
		for(i=0;i<Indexboxnum;i++){
			free(Indexboxout[i]);
			Indexboxout[i]=NULL;
		}
		free(Indexboxout);
		Indexboxout=NULL;
	}
		//MeterRead_Recog(&img,para, &mParam,MResult);
	MeterResult->dCoeffB=MResult[0].dCoeffB;
	MeterResult->dCoeffK=MResult[0].dCoeffK;
	MeterResult->MeterValue=MResult[0].MeterValue;
	MeterResult->ptEnd.x=MResult[0].ptEnd.x;
	MeterResult->ptEnd.y=MResult[0].ptEnd.y;
	//printf("MeterValue=%f\n",MeterResult->MeterValue);
EXT:
	if(MResult)
		free(MResult);
	if(IndexImage.pixelArray.chunky.pPixel)
		free(IndexImage.pixelArray.chunky.pPixel);
	if(img.pixelArray.chunky.pPixel)
		free(img.pixelArray.chunky.pPixel);//yibiaoROI函数内申请空间
	if(dst.pixelArray.chunky.pPixel)
		free(dst.pixelArray.chunky.pPixel);
	if(wy_maskImage.pixelArray.chunky.pPixel)
		free(wy_maskImage.pixelArray.chunky.pPixel);
	return res;
}

int yibiaoROI(HYLPAMR_IMAGES src,HYLPAMR_IMAGES dst,MeterDESCRIP Meterdescrip,char *cfgfile,char *weightfile)
{
	clock_t time;
	YoloNet_detect yolo;
	MByte *srcData;
	MByte *dstData;
	int res=0;
	float thresh=0.24;
	int i=0,j=0;
	int Maxindex=0;
	float MaxProb;
	int num;
	int boxnum=1000;
	float **boxout;
	srcData = (MByte*)src->pixelArray.chunky.pPixel;
	boxout=(float **)calloc(boxnum,sizeof(float *));
	for(i=0;i<boxnum;i++)boxout[i]=(float *)calloc(6,sizeof(float));
	time=clock();
	yolo.initNetData(cfgfile,weightfile);
	yolo.object_detection(srcData,src->lHeight,src->lWidth,3,(int)src->pixelArray.chunky.lLineBytes,thresh,&num,boxout);
	printf("yibiaoROI %f seconds\n",sec(clock()-time));
	if(num==0){
		printf("未找到仪表\n");
		res=-1;
		goto EXT;
	}
	if(0)  //选取距离二维码最近的仪表
	{
		float MinDis;
		int QRw=0,QRh=0,Meterw=0,Meterh=0;
		double Pdistance[100]={0};
		MPOINT QRcenter={0};
		MPOINT Metercenter={0};
		QRcenter.x=(Meterdescrip.MeterRect.left+Meterdescrip.MeterRect.right)/2;
		QRcenter.y=(Meterdescrip.MeterRect.top+Meterdescrip.MeterRect.bottom)/2;
		QRw=Meterdescrip.MeterRect.right-Meterdescrip.MeterRect.left;
		QRh=Meterdescrip.MeterRect.bottom-Meterdescrip.MeterRect.top;
		for(i=0;i<num;i++)
		{
			Metercenter.x=(boxout[i][0]+boxout[i][1])/2;
			Metercenter.y=(boxout[i][2]+boxout[i][3])/2;
			Pdistance[i]=sqrt((pow((Metercenter.x-QRcenter.x),2.0)+pow((Metercenter.y-QRcenter.y),2.0)));
		}
		MinDis=Pdistance[0];
		for(i=1;i<num;i++)
		{
			if(Pdistance[i]<MinDis)
			{
				MinDis=Pdistance[i];
				Maxindex=i;
			}
		}
		Meterw=boxout[Maxindex][1]-boxout[Maxindex][0];
		Meterh=boxout[Maxindex][3]-boxout[Maxindex][2];
		if(Pdistance[Maxindex] > Meterh + QRh)
		{
			printf("未找到与二维码配对的仪表\n");
			res=-1;
			goto EXT;
		}
	}
	else
	{
		MaxProb=boxout[0][4];   //简单的仪表选取（最大置信度）
		for(i=1;i<num;i++){
			if(boxout[i][4] > MaxProb){
				MaxProb=boxout[i][4];
				Maxindex=i;
			}
		}
	}
	dst->lPixelArrayFormat = src->lPixelArrayFormat;
	if(src->lPixelArrayFormat==HYAMR_IMAGE_BGR){
		int W=0;
		int H=0;
		int Hstart=boxout[Maxindex][2];
		int Hend=boxout[Maxindex][3];
		int Wstart=boxout[Maxindex][0];
		int Wend=boxout[Maxindex][1];
		W=Wend-Wstart;  //对yolo框选区域进行扩大
		H=Hend-Hstart;
		Hstart=Hstart-0.1*H;
		Hend=Hend+0.1*H;
		Wstart=Wstart-0.1*W;
		Wend=Wend+0.1*W;
		if(Hstart<0)Hstart=0;   //越界检查
		if(Hend>src->lHeight)Hend=src->lHeight;
		if(Wstart<0)Wstart=0;
		if(Wend>src->lWidth)Wend=src->lWidth;
		//printf("(%d,%d),(%d,%d)\n",Wstart,Hstart,Wend,Hend);
		dst->lWidth=Wend-Wstart;
		dst->lHeight=Hend-Hstart;
		dst->pixelArray.chunky.lLineBytes=GrayStep(3*dst->lWidth);
		dst->pixelArray.chunky.pPixel=(MByte *)calloc(dst->lHeight*dst->pixelArray.chunky.lLineBytes,sizeof(MByte));
		dstData = (MByte*)dst->pixelArray.chunky.pPixel;
		for(j=Hstart;j<Hend;j++){//ROI:Wstart,Hstart,Wend,Hend
			for(i=Wstart;i<Wend;i++){
				dstData[0+(i-Wstart)*3+(j-Hstart)*dst->pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+0];
				dstData[1+(i-Wstart)*3+(j-Hstart)*dst->pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+1];
				dstData[2+(i-Wstart)*3+(j-Hstart)*dst->pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+2];
			}
		}
	}
EXT:
	for(i=0;i<boxnum;i++){
		free(boxout[i]);
		boxout[i]=NULL;
	}
	free(boxout);
	boxout=NULL;
	yolo.ReleaseNetwork();
	return res;
}
int BileiqiROI(HYLPAMR_IMAGES src,HYLPAMR_IMAGES dst,char *cfgfile,char *weightfile)
{
	YoloNet_detect yolo;
	int res=0;
	int i=0,j=0;
	MByte *srcData;
	MByte *dstData;
	float thresh=0.24;
	int num=0;
	int boxnum=1000;
	float **boxout;
	float MaxProb;
	int Maxindex=0;
	boxout=(float **)calloc(boxnum,sizeof(float *));
	for(i=0;i<boxnum;i++)boxout[i]=(float *)calloc(6,sizeof(float));
	srcData = (MByte*)src->pixelArray.chunky.pPixel;
	yolo.initNetData(cfgfile,weightfile);
	yolo.object_detection(srcData,src->lHeight,src->lWidth,3,(int)src->pixelArray.chunky.lLineBytes,thresh,&num,boxout);
	if(num==0){
		res=-1;
		goto EXT;
	}
	MaxProb=boxout[0][4];
	for(i=1;i<num;i++){
		if(boxout[i][4] > MaxProb){
			MaxProb=boxout[i][4];
			Maxindex=i;
		}
	}
	if(src->lPixelArrayFormat==HYAMR_IMAGE_BGR){
		int Hstart=boxout[Maxindex][2];
		int Hend=boxout[Maxindex][3];
		int Wstart=boxout[Maxindex][0];
		int Wend=boxout[Maxindex][1];
		if(Hstart<0)Hstart=0;
		if(Hend>src->lHeight)Hend=src->lHeight;
		if(Wstart<0)Wstart=0;
		if(Wend>src->lWidth)Wend=src->lWidth;
		//printf("(%d,%d),(%d,%d)\n",Wstart,Hstart,Wend,Hend);
		dstData = (MByte*)dst->pixelArray.chunky.pPixel;
		for(j=0;j<src->lHeight;j++){//ROI:Wstart,Hstart,Wend,Hend
			for(i=0;i<src->lWidth;i++){
				if(j>=Hstart && j<=Hend && i>=Wstart && i<=Wend){
					dstData[0+i*3+j*dst->pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+0];
					dstData[1+i*3+j*dst->pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+1];
					dstData[2+i*3+j*dst->pixelArray.chunky.lLineBytes]=srcData[i*3+j*src->pixelArray.chunky.lLineBytes+2];
				}
				else{
					dstData[0+i*3+j*dst->pixelArray.chunky.lLineBytes]=255;
					dstData[1+i*3+j*dst->pixelArray.chunky.lLineBytes]=255;
					dstData[2+i*3+j*dst->pixelArray.chunky.lLineBytes]=255;
				}
			}
		}
	}
EXT:
	for(i=0;i<boxnum;i++){
		free(boxout[i]);
		boxout[i]=NULL;
	}
	free(boxout);
	boxout=NULL;
	yolo.ReleaseNetwork();
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
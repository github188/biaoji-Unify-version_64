#include "AirSwitchRecog.h"
//#include <stdio.h>
#include "darknet.h"
#include "math.h"
#ifdef _DEBUG
#pragma comment(lib, "../Debug/yolo.lib")

#else
#pragma comment(lib, "../Release/yolo.lib")

#endif


#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

typedef struct 
{
	network net;
	float lThresh;
}ADRParam;

typedef struct 
{
	int x;
	int y;
}APoint;

typedef struct{
    int num;
	float **Bbox;
} ABboxOut;


typedef struct{
	int flag;
	int first;
	int next;
	APoint CPoint;
    float *AirSwitchForm;
} sortable_AirSwitchForm;

image HYA_ipl_to_image(AR_IMAGES *pImg)
{
	unsigned char *data = (unsigned char *)pImg->pixelArray.chunky.pPixel;
	int h = pImg->lHeight;
	int w = pImg->lWidth;
	int c = 3;
	int step = pImg->pixelArray.chunky.lLineBytes;
    image out = make_image(w, h, c);
    int i, j, k, count=0;;

    for(k= 0; k < c; ++k){
        for(i = 0; i < h; ++i){
            for(j = 0; j < w; ++j){
                out.data[count++] = data[i*step + j*c + k]/255.;
            }
        }
    }
    return out;
}
image HYA_ipl_to_imageROI(AR_IMAGES *pImg,int srcx,int srcy,int srcw,int srch)
{
	unsigned char *data = (unsigned char *)pImg->pixelArray.chunky.pPixel;
	int h = pImg->lHeight;
	int w = pImg->lWidth;
	int c = 3;
	int step = pImg->pixelArray.chunky.lLineBytes;
	//int step = srcw*3;
    image out = make_image(srcw, srch, c);
    int i, j, k, count=0;
   // printf("x=%d y=%d w=%d h=%d\n",srcx,srcy,srcw,srch);
    for(k= 0; k < c; ++k){
        for(i = srcy; i < srcy+srch; ++i){
            for(j = srcx; j < srcx+srcw; ++j){
                out.data[count++] = data[i*step + j*c + k]/255.;
            }
        }
    }
    return out;
}


int HYAR_Init(void *hMemMgr, void **pMRHandle)
{
	int res = 0;

	ADRParam *pDRHandle=NULL;
	if (!pMRHandle)
	{
		res= -1;
		goto EXT;
	}
	pDRHandle=(ADRParam *)malloc(1*sizeof(ADRParam));
	*pMRHandle=pDRHandle;
EXT:
	return res;
}
int HYAR_Uninit(void *hMRHandle)
{
	ADRParam *pDRParam = (ADRParam*)hMRHandle;
	//一定要注意释放模型参数

	free_network(pDRParam->net);
	

	if (pDRParam)
		free (pDRParam);
	pDRParam = NULL;
	return 0;
}

int HYAR_SetParam(void *hMRHandle,char *cfgfile, char *weightfile,float Thresh)
{
	int res=0;
	ADRParam *pDRParam = (ADRParam*)hMRHandle;
	
	pDRParam->lThresh = Thresh;
	pDRParam->net = parse_network_cfg(cfgfile);
	if(weightfile){
        load_weights(&(pDRParam->net), weightfile);
    }
    set_batch_network(&(pDRParam->net), 1);
	return 0;
}
int ATPointDis(APoint a,APoint b)
{
	return sqrt(pow((a.x-b.x),2.0)+pow((a.y-b.y),2.0));
}
int Acomp(const void *pa, const void *pb)
{
	float diff;
    sortable_AirSwitchForm a = *(sortable_AirSwitchForm *)pa;
    sortable_AirSwitchForm b = *(sortable_AirSwitchForm *)pb;
	if(a.CPoint.y - b.CPoint.y>(a.AirSwitchForm[1] - a.AirSwitchForm[0])/2)
		diff=1;
	else if(b.CPoint.y - a.CPoint.y>(a.AirSwitchForm[1] - a.AirSwitchForm[0])/2)
		diff=-1;
	else if(a.CPoint.x - b.CPoint.x>0)
		diff=1;
	else if(a.CPoint.x - b.CPoint.x<0)
		diff=-1;
   // float diff = a.CPoint.x - b.CPoint.x ;
    if(diff < 0) return -1;//a
    else if(diff > 0) return 1;//b
    return 0;
}

int Aboxcomp(const void *pa, const void *pb)
{
    float* a = *(float* *)pa;
    float* b = *(float* *)pb;
    float diff = a[4] - b[4] ;
    if(diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}

int Aroicomp(const void *pa, const void *pb)
{
	float diff;
    MARECT a = *(MARECT *)pa;
    MARECT b = *(MARECT *)pb;
	if(a.top < b.top)
		diff=-1;
    if(diff < 0) return -1;//a
    else if(diff > 0) return 1;//b
    return 0;
}

void AGetOverLappingRate(MARECT rt1,MARECT rt2,long *minRate, long *maxRate)
{
	long lSizeRt1, lSizeRt2;
	long lMaxYDist = MAX(rt2.bottom,rt1.bottom)-MIN(rt2.top,rt1.top);
	long lMaxXDist = MAX(rt2.right,rt1.right)-MIN(rt2.left,rt1.left);
	if (!minRate && !maxRate) return;

	lSizeRt1 = (rt1.bottom-rt1.top)*(rt1.right-rt1.left);
	lSizeRt2 = (rt2.bottom-rt2.top)*(rt2.right-rt2.left);

	if (lSizeRt1<=0 || lSizeRt2 <=0) 
	{
		if (minRate) *minRate = 0;
		if (maxRate) *maxRate = 100;
	}

	///如果两个矩形有交集，则距离为0
	if (lMaxXDist<= (rt2.right-rt2.left+rt1.right-rt1.left)
	&& lMaxYDist <= (rt2.bottom-rt2.top+rt1.bottom-rt1.top))
	{

		long lSizeOverlap = (MIN(rt2.right, rt1.right)-MAX(rt2.left, rt1.left))
						*(MIN(rt2.bottom,rt1.bottom)-MAX(rt2.top, rt1.top));
		if (maxRate) *maxRate=lSizeOverlap*100/lSizeRt2;
		//if (minRate) *minRate = MIN(lSizeOverlap*100/lSizeRt1, lSizeOverlap*100/lSizeRt2);
		//if (maxRate) *maxRate = MAX(lSizeOverlap*100/lSizeRt1, lSizeOverlap*100/lSizeRt2);
	}
	else
	{
		if (minRate) *minRate = 0;
		if (maxRate) *maxRate = 0;
	}
	return;
}


int HYAR_AirSwitchRecog(void *hMRHandle,AR_IMAGES *pImg , HYAR_RESULT_LIST *pResultList)
{
	int res =0;
	int i=0,j=0;
	int total=100;
	double max=0.0000;
	float thresh = 0.0f;
	int group=1;
	int tmp,num=0;
	int Digitnum[10]={0};
	int normalHeight = 0;
	int lResultNum=0;
	MARECT targetarea;
	image out ;
	//int maxindex=-1;
	int MinDist,MinDistX;
	int previousX[100];
	ABboxOut bbox={0};
	ABboxOut tmpbbox={0};
	MARECT *ROI;
	int *flag;
	int type=1;
	int k=0;
	MARECT rtArea;
	sortable_AirSwitchForm *s;
    ADRParam *pDRParam = (ADRParam*)hMRHandle;
	total=pResultList->lMaxResultNum;
	bbox.Bbox = (float **)calloc(total, sizeof(float *));  //free
	for(i = 0; i < total; ++i) bbox.Bbox[i] = (float*)calloc(6, sizeof(float));
	tmpbbox.Bbox = (float **)calloc(total, sizeof(float *));  //free
	for(i = 0; i < total; ++i) tmpbbox.Bbox[i] = (float*)calloc(6, sizeof(float));

	/*for(i = 0; i < total; ++i){ 
		printf("_%d:%f\n",i,bbox.Bbox[i][5]);
	}*/

	thresh = pDRParam->lThresh;	

	
	bbox.num = 0;
	flag = (int*)calloc(pResultList->lAreaNum,sizeof(int));
	ROI = (MARECT*)calloc(pResultList->lAreaNum,sizeof(MARECT));
	
	for(i=0;i<pResultList->lAreaNum;i++)//将坐标还原到未裁剪的时候
	{
		pResultList->LRArea[i].left += pResultList->MatchArea.left;
		pResultList->LRArea[i].top += pResultList->MatchArea.top; 	
		pResultList->LRArea[i].right += pResultList->MatchArea.left;
		pResultList->LRArea[i].bottom += pResultList->MatchArea.top ; 
	}
	for(i=0;i<pResultList->lAreaNum;i++)//对空开排数的讨论
	{
		if(flag[i]!=0)continue;
		if(flag[i]==0)
		{
			flag[i]=type;
			ROI[flag[i]]=pResultList->LRArea[i];
			type=type+1;
		}
		for(j=0;j<pResultList->lAreaNum;j++)
		{
			if(i==j)continue;
			if(flag[j]!=0)continue;

			if(pResultList->LRArea[j].top < pResultList->LRArea[i].bottom && pResultList->LRArea[j].bottom > pResultList->LRArea[i].top)
			{
				flag[j]=flag[i];
				ROI[flag[i]].top=ROI[flag[i]].top < pResultList->LRArea[j].top?ROI[flag[i]].top:pResultList->LRArea[j].top;
				ROI[flag[i]].left=ROI[flag[i]].left < pResultList->LRArea[j].left?ROI[flag[i]].left:pResultList->LRArea[j].left;
				ROI[flag[i]].bottom=ROI[flag[i]].bottom > pResultList->LRArea[j].bottom?ROI[flag[i]].bottom:pResultList->LRArea[j].bottom;
				ROI[flag[i]].right=ROI[flag[i]].right > pResultList->LRArea[j].right?ROI[flag[i]].right:pResultList->LRArea[j].right;
			}
		}
	}
	for(i=0;i<pResultList->lAreaNum;i++)//将坐标还原到裁剪的时候
	{
		pResultList->LRArea[i].left -= pResultList->MatchArea.left;
		pResultList->LRArea[i].top -= pResultList->MatchArea.top; 	
		pResultList->LRArea[i].right -= pResultList->MatchArea.left;
		pResultList->LRArea[i].bottom -= pResultList->MatchArea.top ; 
	}
	qsort(ROI, type, sizeof(float*),Aroicomp);//空开排数从上到下排序
	
	for(i=1;i<type;i++)//单排空开识别
	{
		int _x,_y,_w,_h;
		//IplImage *RectImg=NULL;
		if(type ==2)
		{
			rtArea.bottom = pResultList->MatchArea.bottom;
			rtArea.top = pResultList->MatchArea.top;
			rtArea.left = pResultList->MatchArea.left;
			rtArea.right = pResultList->MatchArea.right;
		}
		else
		{
		    if(i==1)
		   {
				
					rtArea.bottom = (ROI[i].bottom+ROI[i+1].top)/2;
					rtArea.top = (ROI[i].top+pResultList->MatchArea.top)/2;
		   }
		   else if(i ==type-1)
		   {
			   rtArea.top=rtArea.bottom;
			   rtArea.bottom = (ROI[i].bottom+pResultList->MatchArea.bottom)/2;//+pResultList->offset.y;
		   }
		   else
		   {
				rtArea.top=rtArea.bottom;
				rtArea.bottom = (ROI[i].bottom+ROI[i+1].top)/2;//+pResultList->offset.y;
		   }

			rtArea.left = (ROI[i].left+pResultList->MatchArea.left)/2;//+pResultList->offset.x;
			rtArea.right = (ROI[i].right+pResultList->MatchArea.right)/2;//+pResultList->offset.x;
		}
		/*
       if(i==1)
	   {
			if(type > 2)
			{
				rtArea.bottom = (ROI[i].bottom+ROI[i+1].top)/2;//+pResultList->offset.y;
				rtArea.top = (ROI[i].top+pResultList->MatchArea.top)/2;
			}
		   else
		   {
			   //rtArea.bottom = (ROI[i].bottom+pResultList->MatchArea.bottom)/2;//+pResultList->offset.y;
			   rtArea.bottom = pResultList->MatchArea.bottom;
			   rtArea.top = pResultList->MatchArea.top;
			}
			//rtArea.top = (ROI[i].top+pResultList->MatchArea.top)/2;//+pResultList->offset.y;
	   }
	   else if(i ==type-1)
	   {
		   rtArea.top=rtArea.bottom;
		   rtArea.bottom = (ROI[i].bottom+pResultList->MatchArea.bottom)/2;//+pResultList->offset.y;
	   }
	   else
	   {
			rtArea.top=rtArea.bottom;
			rtArea.bottom = (ROI[i].bottom+ROI[i+1].top)/2;//+pResultList->offset.y;
	   }

		rtArea.left = (ROI[i].left+pResultList->MatchArea.left)/2;//+pResultList->offset.x;
		rtArea.right = (ROI[i].right+pResultList->MatchArea.right)/2;//+pResultList->offset.x;
		*/
		//for(j=0;j<pResultList->lAreaNum;j++)//在裁剪图中坐标
		//{
		//	if(flag[j]==i)
		//	{
		//		pResultList->LRArea[j].left -= rtArea.left;
		//		pResultList->LRArea[j].top -= rtArea.top; 	
		//		pResultList->LRArea[j].right -= rtArea.left;
		//		pResultList->LRArea[j].bottom -= rtArea.top ; 

		//	}
		//}
		
		//if(rtArea.bottom >= pImg->lHeight)rtArea.bottom=rtArea.bottom-1;
		//if(rtArea.right >= pImg->lWidth )rtArea.right=rtArea.right-1;
		//if(rtArea.left < 0 )rtArea.left=0;
		//if(rtArea.top < 0 )rtArea.top =0;
		_w=rtArea.right-rtArea.left;
		_h=rtArea.bottom-rtArea.top;
		_x=rtArea.left-pResultList->MatchArea.left;//在图中的相对坐标
		_y=rtArea.top-pResultList->MatchArea.top;
		if(_x+_w > pImg->lWidth)
			_w=pImg->lWidth-_x;
		if(_y+_h > pImg->lHeight)
			_h=pImg->lHeight-_y;
		if(_x < 0)
			_x=0;
		if(_y < 0)
			_y=0;
		out = HYA_ipl_to_imageROI(pImg,_x,_y,_w,_h);
		/*if(rtArea.right-pResultList->MatchArea.left < pImg->lWidth && rtArea.bottom-pResultList->MatchArea.top < pImg->lHeight && rtArea.left-pResultList->MatchArea.left > 0 && rtArea.top-pResultList->MatchArea.top > 0)
			out = HYA_ipl_to_imageROI(pImg,rtArea.left-pResultList->MatchArea.left,rtArea.top-pResultList->MatchArea.top,rtArea.right-rtArea.left,rtArea.bottom-rtArea.top);
		else
		{
			int _w=rtArea.right-rtArea.left;
			int _h=rtArea.bottom-rtArea.top;
			int _x=rtArea.left-pResultList->MatchArea.left;
			int _y=rtArea.top-pResultList->MatchArea.top;
			if(_x+_w > pImg->lWidth)
				_w=pImg->lWidth-_x;
			if(_y+_h > pImg->lHeight)
				_h=pImg->lHeight-_y;
			if(_x < 0)
				_x=0;
			if(_y < 0)
				_y=0;
			out = HYA_ipl_to_imageROI(_x,_y,_w,_h);
		}*/
		tmpbbox.num = 0;
		
		YoloDetector(pDRParam->net, out, thresh,0.5,&tmpbbox.num,tmpbbox.Bbox);
		free_image(out);
		for(j=0;j<tmpbbox.num;j++)//坐标还原
		{
			bbox.Bbox[bbox.num][0] =tmpbbox.Bbox[j][0]+rtArea.left-pResultList->MatchArea.left;
			bbox.Bbox[bbox.num][1] =tmpbbox.Bbox[j][1]+rtArea.left-pResultList->MatchArea.left;
			bbox.Bbox[bbox.num][2] =tmpbbox.Bbox[j][2]+rtArea.top-pResultList->MatchArea.top;
			bbox.Bbox[bbox.num][3] =tmpbbox.Bbox[j][3]+rtArea.top-pResultList->MatchArea.top;
			bbox.Bbox[bbox.num][4] =tmpbbox.Bbox[j][4];
			bbox.Bbox[bbox.num][5] =tmpbbox.Bbox[j][5];
			bbox.num+=1;
		}

	}
	//out =   HYA_ipl_to_image(pImg);
	//bbox.num = 0;
	//YoloDetector(pDRParam->net, out, thresh,0.5,&bbox.num,bbox.Bbox);
	/*for(i = 0; i < bbox.num; ++i){ 
		printf("%d:%f confi=%f left=%f right=%f top=%f bot=%f\n",i,bbox.Bbox[i][5],bbox.Bbox[i][4],bbox.Bbox[i][0],bbox.Bbox[i][1],bbox.Bbox[i][2],bbox.Bbox[i][3]);
	}*/
	
	qsort(bbox.Bbox, bbox.num, sizeof(float*), Aboxcomp);
	
	
	s = (sortable_AirSwitchForm*)calloc(bbox.num, sizeof(sortable_AirSwitchForm));

	//printf("%d\n",bbox.num);
	if(bbox.num < 1){		//
		res = -1;
		goto EXT;
	}

	normalHeight = bbox.Bbox[0][3]-bbox.Bbox[0][2];
	
	j=0;
	for(i = 0; i < bbox.num; ++i){ 
		//
		MARECT rtArea1,rtArea2;
		float a;
		long lMaxRate;
		s[i].AirSwitchForm =bbox.Bbox[i]; 
		//printf("0%d:%f\n",i,s[i].LinkForm[4]);
		s[i].CPoint.x = (s[i].AirSwitchForm[0]+s[i].AirSwitchForm[1])/2 ;
		s[i].CPoint.y = (s[i].AirSwitchForm[2]+s[i].AirSwitchForm[3])/2 ;

		//rtArea1.left=s[i].AirSwitchForm[0];
		//rtArea1.right=s[i].AirSwitchForm[1];
		//rtArea1.top=s[i].AirSwitchForm[2];
		//rtArea1.bottom=s[i].AirSwitchForm[3];
		//for(j=0;j<bbox.num;j++){
		//	if(i==j)continue;
		//	if(s[j].flag < 0)continue;
		//	s[j].AirSwitchForm =bbox.Bbox[j];
		//	rtArea2.left=s[j].AirSwitchForm[0];
		//	rtArea2.right=s[j].AirSwitchForm[1];
		//	rtArea2.top=s[j].AirSwitchForm[2];
		//	rtArea2.bottom=s[j].AirSwitchForm[3];
		//	lMaxRate=0;
		//	AGetOverLappingRate(rtArea1,rtArea2,NULL,&lMaxRate);
		//	//if(lMaxRate > 0)printf("lMaxRate=%d\n",lMaxRate);
		//	if(lMaxRate > 25)
		//	{
		//		if(bbox.Bbox[j][4] < bbox.Bbox[i][4])
		//			s[j].flag = -1;
		//		else
		//			s[i].flag = -1;
		//	}
		//}
	    a=fabs(1-(1.0*normalHeight)/(s[i].AirSwitchForm[3]-s[i].AirSwitchForm[2]));
		//if(fabs(a)>0.3)
		{
			//printf("-1\n");
			//s[i].flag = -1;
		}
		//else
		//if(0)
		{
			s[i].flag = 0;//选取实际数目个目标
			j=j+1;
			if(j> pResultList->lRealResultNum)s[i].flag=-1;
		}
	}
	
	qsort(s, bbox.num, sizeof(sortable_AirSwitchForm), Acomp);  //从左到右排序,从上到下排序
	/*for(j = 0; j < pResultList->lRealResultNum; ++j)
	{
		MARECT tmpArea;
		
		tmpArea.left=pResultList->LRArea[j].left+pResultList->offset.x;
		tmpArea.right=pResultList->LRArea[j].right+pResultList->offset.x;
		tmpArea.top=pResultList->LRArea[j].top+pResultList->offset.y;
		tmpArea.bottom=pResultList->LRArea[j].bottom+pResultList->offset.y;
		printf("left=%d\n",pResultList->LRArea[j].left);
		printf("x=%d y=%d\n",pResultList->offset.x,pResultList->offset.y);
		printf("0:left=%d right=%d top=%d bot=%d\n",tmpArea.left,tmpArea.right,tmpArea.top,tmpArea.bottom);
	}*/

	for(j = 0; j < pResultList->lRealResultNum; ++j)
	{
		long lMaxRate=0;
		int previous=-1;
		targetarea.left=pResultList->LRArea[j].left;//+pResultList->offset.x;
		targetarea.right=pResultList->LRArea[j].right;//+pResultList->offset.x;
		targetarea.top=pResultList->LRArea[j].top;//+pResultList->offset.y;
		targetarea.bottom=pResultList->LRArea[j].bottom;//+pResultList->offset.y;
		pResultList->pResult[j].flag=0;
		for(i = 0; i < bbox.num; ++i){
			MARECT tmpArea;
			long tmpRate;
			long tmpRate1;
			long tmpRate2;
			tmpArea.left=s[i].AirSwitchForm[0];
			tmpArea.right=s[i].AirSwitchForm[1];
			tmpArea.top=s[i].AirSwitchForm[2];
			tmpArea.bottom=s[i].AirSwitchForm[3];
			AGetOverLappingRate(tmpArea,targetarea,NULL,&tmpRate1);
			AGetOverLappingRate(targetarea,tmpArea,NULL,&tmpRate2);
			tmpRate=MAX(tmpRate1,tmpRate2);
		//	printf("%d:%d rate=%d\n",j,i,tmpRate);
			if(tmpRate > 20 && tmpRate > lMaxRate)
			{
				pResultList->pResult[j].flag=1;
				pResultList->pResult[j].dVal=s[i].AirSwitchForm[5];
				if(pResultList->pResult[j].dVal == 0)pResultList->pResult[j].dVal=1;
				else if(pResultList->pResult[j].dVal == 1)pResultList->pResult[j].dVal=0;
				pResultList->pResult[j].Target.left=s[i].AirSwitchForm[0];
				pResultList->pResult[j].Target.right=s[i].AirSwitchForm[1];
				pResultList->pResult[j].Target.top=s[i].AirSwitchForm[2];
				pResultList->pResult[j].Target.bottom=s[i].AirSwitchForm[3];
				lMaxRate=tmpRate;
			}
		}
		lResultNum+=1;

	}
	//for(i = 0; i < bbox.num; ++i){ 
	//	long lMaxRate=0;
	//	int previous=-1;
	//	/*if(s[i].flag<0)
	//		continue;*/
	//	targetarea.left=s[i].AirSwitchForm[0];
	//	targetarea.right=s[i].AirSwitchForm[1];
	//	targetarea.top=s[i].AirSwitchForm[2];
	//	targetarea.bottom=s[i].AirSwitchForm[3];
	//	//printf("1:left=%d right=%d top=%d bot=%d\n",targetarea.left,targetarea.right,targetarea.top,targetarea.bottom);
	//	for(j = 0; j < pResultList->lRealResultNum; ++j)
	//	{
	//		MARECT tmpArea;
	//		long tmpRate;
	//		tmpArea.left=pResultList->LRArea[j].left+pResultList->offset.x;
	//		tmpArea.right=pResultList->LRArea[j].right+pResultList->offset.x;
	//		tmpArea.top=pResultList->LRArea[j].top+pResultList->offset.y;
	//		tmpArea.bottom=pResultList->LRArea[j].bottom+pResultList->offset.y;
	//		
	//		//printf("1:left=%d right=%d top=%d bot=%d\n",pResultList->pResult[j].Target.left,pResultList->pResult[j].Target.right,pResultList->pResult[j].Target.top,pResultList->pResult[j].Target.bottom);
	//		AGetOverLappingRate(targetarea,tmpArea,NULL,&tmpRate);
	//		//printf("rate=%d\n",tmpRate);
	//		if(tmpRate > 20 && tmpRate > lMaxRate)
	//		{
	//			if(previous != -1)
	//			{
	//				pResultList->pResult[previous].flag=0;
	//			}
	//			pResultList->pResult[j].flag=1;
	//			pResultList->pResult[j].dVal=s[i].AirSwitchForm[5];
	//			if(pResultList->pResult[j].dVal == 0)pResultList->pResult[j].dVal=1;
	//			else if(pResultList->pResult[j].dVal == 1)pResultList->pResult[j].dVal=0;
	//			pResultList->pResult[j].Target.left=s[i].AirSwitchForm[0];
	//			pResultList->pResult[j].Target.right=s[i].AirSwitchForm[1];
	//			pResultList->pResult[j].Target.top=s[i].AirSwitchForm[2];
	//			pResultList->pResult[j].Target.bottom=s[i].AirSwitchForm[3];

	//			//printf("2:left=%d right=%d top=%d bot=%d\n",pResultList->pResult[j].Target.left,pResultList->pResult[j].Target.right,pResultList->pResult[j].Target.top,pResultList->pResult[j].Target.bottom);
	//			previous=j;
	//			lMaxRate=tmpRate;
	//		}
	//	}
	//	lResultNum+=1;
	//}
	pResultList->lResultNum=pResultList->lRealResultNum;

	

EXT:
	for(i = 0; i < total; ++i) 
	{
		free(tmpbbox.Bbox[i]);
	}
	free(tmpbbox.Bbox);
	for(i = 0; i < total; ++i) 
	{
		free(bbox.Bbox[i]);
	}
	free(bbox.Bbox);
	if(flag==NULL)
	free(flag);
	//printf("%d\n",ROI[0].top);
	//printf("%d\n",ROI->top);
	if(ROI==NULL)
	free(ROI);
	if(s==NULL)
	free(s);
	

	return res;
}



#include "DigitalRecog.h"
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
} DDRParam;

typedef struct 
{
	int x;
	int y;
}DPoint;

typedef struct{
    int num;
	float **Bbox;
} DBboxOut;


typedef struct{
	int flag;
	int next;
	DPoint CPoint;
    float *DigitForm;
} sortable_DigitForm;

image HYD_ipl_to_image(DR_IMAGES *pImg)
{
	unsigned char *data = (unsigned char *)pImg->pixelArray.chunky.pPixel;
	int h = pImg->lHeight;
	int w = pImg->lWidth;
	int c = 3;
	int step = pImg->pixelArray.chunky.lLineBytes;
    image out = make_image(w, h, c);  //out.data = calloc(h*w*c, sizeof(float));
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


int HYDR_Init(void *hMemMgr, void **pMRHandle)
{
	int res = 0;

	DDRParam *pDRHandle=NULL;
	if (!pMRHandle)
	{
		res= -1;
		goto EXT;
	}
	pDRHandle=(DDRParam *)malloc(1*sizeof(DDRParam));
	*pMRHandle=pDRHandle;
EXT:
	return res;
}
int HYDR_Uninit(void *hMRHandle)
{
	DDRParam *pDRParam = (DDRParam*)hMRHandle;
	//一定要注意释放模型参数

	free_network(pDRParam->net);
	

	if (pDRParam)
		free (pDRParam);
	pDRParam = NULL;
	return 0;
}

int HYDR_SetParam(void *hMRHandle,const char *cfgfile,const char *weightfile,float Thresh)
{
	int res=0;
	DDRParam *pDRParam = (DDRParam*)hMRHandle;
	
	pDRParam->lThresh = Thresh;

	pDRParam->net = parse_network_cfg(cfgfile);
	if(weightfile){
        load_weights(&(pDRParam->net), weightfile);
    }
    set_batch_network(&(pDRParam->net), 1);
	return 0;
}


int DTPointDis(DPoint a,DPoint b)
{
	return sqrt(pow((a.x-b.x),2.0)+pow((a.y-b.y),2.0));
}
int Dcomp(const void *pa, const void *pb)
{
    sortable_DigitForm a = *(sortable_DigitForm *)pa;
    sortable_DigitForm b = *(sortable_DigitForm *)pb;
    float diff = a.CPoint.x - b.CPoint.x ;
    if(diff < 0) return -1;
    else if(diff > 0) return 1;
    return 0;
}
int Dboxcomp(const void *pa, const void *pb)
{
    float* a = *(float* *)pa;
    float* b = *(float* *)pb;
    float diff = a[4] - b[4] ;
    if(diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}
void DGetOverLappingRate(MDRECT rt1,MDRECT rt2,long *minRate, long *maxRate)
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
		if (minRate) *minRate = MIN(lSizeOverlap*100/lSizeRt1, lSizeOverlap*100/lSizeRt2);
		if (maxRate) *maxRate = MAX(lSizeOverlap*100/lSizeRt1, lSizeOverlap*100/lSizeRt2);
	}
	else
	{
		if (minRate) *minRate = 0;
		if (maxRate) *maxRate = 0;
	}
	return;
}

int HYDR_DigitRecog(void *hMRHandle,DR_IMAGES *pImg , HYDR_RESULT_LIST *pResultList)
{
	int res =0;
	int i=0,j=0;
	int total=10;
	double max=0.0000;
	float thresh = 0.0f;
	int group=1;
	int tmp,num=0;
	int Digitnum[10]={0};
	int normalHeight = 0;
	MDRECT rtArea1,rtArea2;
	image out ;
	//int maxindex=-1;
	int MinDist,MinDistX;
	int beta=0;
	DBboxOut bbox={0};
	sortable_DigitForm *s;
    DDRParam *pDRParam = (DDRParam*)hMRHandle;
	bbox.Bbox = (float **)calloc(total, sizeof(float *));  //1m
	for(i = 0; i < total; ++i) bbox.Bbox[i] = (float*)calloc(6, sizeof(float));

	thresh = pDRParam->lThresh;	
	out =   HYD_ipl_to_image(pImg);//2m
	bbox.num = 0;
	if(0 > YoloDetector(pDRParam->net, out, thresh,0.5,&bbox.num,bbox.Bbox))
	{
		//printf("error:0 > YoloDetector\n");
		res=-2;
		goto EXT;
	}
	//printf("0:bbox.num=%d\n",bbox.num);
	qsort(bbox.Bbox, bbox.num, sizeof(float*), Dboxcomp);
	bbox.num=(bbox.num>6)?6:bbox.num;
	/*
	if(bbox.num>6)
	{
		bbox.Bbox[5][4];
	}*/
	//
	free_image(out);//2f
	s = (sortable_DigitForm*)calloc(bbox.num, sizeof(sortable_DigitForm));//3m

	
	if(bbox.num < 2){	
		//printf("error:bbox.num=%d\n",bbox.num);
		res = -1;
		goto EXT;
	}

	normalHeight = bbox.Bbox[0][3]-bbox.Bbox[0][2];
	

	for(i = 0; i < bbox.num; ++i){ 
		//排除异常高度,重叠区域
		float a;
		long lMaxRate;
		if(s[i].flag < 0)continue;
		s[i].DigitForm =bbox.Bbox[i]; 
		s[i].CPoint.x = (s[i].DigitForm[0]+s[i].DigitForm[1])/2 ;
		s[i].CPoint.y = (s[i].DigitForm[2]+s[i].DigitForm[3])/2 ;
		rtArea1.left=s[i].DigitForm[0];
		rtArea1.right=s[i].DigitForm[1];
		rtArea1.top=s[i].DigitForm[2];
		rtArea1.bottom=s[i].DigitForm[3];
		for(j=0;j<bbox.num;j++){
			if(i==j)continue;
			if(s[j].flag < 0)continue;
			s[j].DigitForm =bbox.Bbox[j];
			rtArea2.left=s[j].DigitForm[0];
			rtArea2.right=s[j].DigitForm[1];
			rtArea2.top=s[j].DigitForm[2];
			rtArea2.bottom=s[j].DigitForm[3];
			lMaxRate=0;
			DGetOverLappingRate(rtArea1,rtArea2,NULL,&lMaxRate);
			//if(lMaxRate > 0)printf("lMaxRate=%d\n",lMaxRate);
			if(lMaxRate > 25)
			{
				if(bbox.Bbox[j][4] < bbox.Bbox[i][4])
					s[j].flag = -1;
				else
					s[i].flag = -1;
			}
		}
	   /* a=fabs(1-(1.0*normalHeight)/(s[i].DigitForm[3]-s[i].DigitForm[2]));
		if(fabs(a)>0.2)
		{
			
			s[i].flag = 0;
		}
		else
		{
			s[i].flag = 0;
			
		}*/
	}
	
	//printf("bbox.num=%d\n",bbox.num);
	qsort(s, bbox.num, sizeof(sortable_DigitForm), Dcomp);  //从左到右排序
	/*for(i = 0; i < bbox.num; ++i){
		printf("num=%f,percent=%f\n",bbox.Bbox[i][5],bbox.Bbox[i][4]);
	}*/


	for(i = 0; i < bbox.num; ++i){
		if(s[i].flag<0)
			continue;
		if(s[i].flag==0)
		{
			s[i].flag=group;
			group=group+1;
		}
		MinDist=0x7FFFFFFF,MinDistX=-1;
		for(j = 0; j < bbox.num; ++j)
		{  
			if(i==j)continue;
			if(s[j].flag!=0)continue;
			if(DTPointDis(s[i].CPoint,s[j].CPoint) < MinDist){
				MinDist=DTPointDis(s[i].CPoint,s[j].CPoint);
				MinDistX=j;
			}
		}
		if(MinDist < 2*normalHeight){
			s[i].next=MinDistX;
			s[MinDistX].flag=s[i].flag;
		}
	}

	for(i = 0;i<group-1;++i){
		tmp=0;
		for(j = 0; j < bbox.num; ++j){
			if(s[j].flag<0)continue;
			if(s[j].flag!=i+1)continue;
			tmp=tmp+1;
			/*if(s[j].DigitForm[5] > 9)
			{
				pResultList->pResult[pResultList->lResultNum].dVal=s[j].DigitForm[5]-10;
				beta+=1;
			}
			else
				pResultList->pResult[pResultList->lResultNum].dVal=s[j].DigitForm[5];*/
			pResultList->pResult[pResultList->lResultNum].dVal=s[j].DigitForm[5];
			pResultList->pResult[pResultList->lResultNum].Target.left=s[j].DigitForm[0];
			pResultList->pResult[pResultList->lResultNum].Target.right=s[j].DigitForm[1];
			pResultList->pResult[pResultList->lResultNum].Target.top=s[j].DigitForm[2];
			pResultList->pResult[pResultList->lResultNum].Target.bottom=s[j].DigitForm[3];
			while(s[j].next > 0){
				/*if(beta >= 1)
				{
					if(s[s[j].next].DigitForm[5] > 9)
						printf("error digitalrecog\n");
					else
					{
						pResultList->pResult[pResultList->lResultNum].dVal=pResultList->pResult[pResultList->lResultNum].dVal*10+s[s[j].next].DigitForm[5];
						beta+=1;
					}
				}
				else
				{
					if(s[s[j].next].DigitForm[5] > 9)
					{
						beta+=1;
						pResultList->pResult[pResultList->lResultNum].dVal=pResultList->pResult[pResultList->lResultNum].dVal*10+s[s[j].next].DigitForm[5]-10;
					}
					else
						pResultList->pResult[pResultList->lResultNum].dVal=pResultList->pResult[pResultList->lResultNum].dVal*10+s[s[j].next].DigitForm[5];
				}*/
				pResultList->pResult[pResultList->lResultNum].dVal=pResultList->pResult[pResultList->lResultNum].dVal*10+s[s[j].next].DigitForm[5];
				pResultList->pResult[pResultList->lResultNum].Target.left = MIN(pResultList->pResult[pResultList->lResultNum].Target.left,s[s[j].next].DigitForm[0]);
				pResultList->pResult[pResultList->lResultNum].Target.right = MAX(pResultList->pResult[pResultList->lResultNum].Target.right,s[s[j].next].DigitForm[1]);
				pResultList->pResult[pResultList->lResultNum].Target.top = MIN(pResultList->pResult[pResultList->lResultNum].Target.top,s[s[j].next].DigitForm[2]);
				pResultList->pResult[pResultList->lResultNum].Target.bottom = MAX(pResultList->pResult[pResultList->lResultNum].Target.bottom,s[s[j].next].DigitForm[3]);
				tmp=tmp+1;
				j=s[j].next;
			}
			  //printf("%f  %d\n",pResultList->pResult[pResultList->lResultNum].dVal,beta);
			/*if(beta != 0)
				pResultList->pResult[pResultList->lResultNum].dVal=1.0*pResultList->pResult[pResultList->lResultNum].dVal/pow(10,(beta-1));
			printf("val=%f",pResultList->pResult[pResultList->lResultNum].dVal);*/
			if(tmp >=3)
				pResultList->lResultNum+=1;
			break;
			
		}
	}

EXT:
	for(i = 0; i < total; ++i) //1f
	{
		free(bbox.Bbox[i]);
	}
	free(bbox.Bbox);
	if(s)
		free(s);//3f
	

	return res;
}



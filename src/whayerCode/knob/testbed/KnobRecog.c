#include "KnobRecog.h"
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
}KDRParam;

typedef struct 
{
	int x;
	int y;
}KPoint;

typedef struct{
    int num;
	float **Bbox;
} KBboxOut;


typedef struct{
	int flag;
	int first;
	int next;
	KPoint CPoint;
    float *KnobForm;
} sortable_KnobForm;

image HYK_ipl_to_image(KR_IMAGES *pImg)
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


int HYKR_Init(void *hMemMgr, void **pMRHandle)
{
	int res = 0;

	KDRParam *pDRHandle=NULL;
	if (!pMRHandle)
	{
		res= -1;
		goto EXT;
	}
	pDRHandle=(KDRParam *)malloc(1*sizeof(KDRParam));
	*pMRHandle=pDRHandle;
EXT:
	return res;
}
int HYKR_Uninit(void *hMRHandle)
{
	KDRParam *pDRParam = (KDRParam*)hMRHandle;
	//一定要注意释放模型参数

	free_network(pDRParam->net);
	

	if (pDRParam)
		free (pDRParam);
	pDRParam = NULL;
	return 0;
}

int HYKR_SetParam(void *hMRHandle,char *cfgfile, char *weightfile,float Thresh)
{
	int res=0;
	KDRParam *pDRParam = (KDRParam*)hMRHandle;
	
	pDRParam->lThresh = Thresh;
	pDRParam->net = parse_network_cfg(cfgfile);
	if(weightfile){
        load_weights(&(pDRParam->net), weightfile);
    }
    set_batch_network(&(pDRParam->net), 1);
	return 0;
}
int TPointDis(KPoint a,KPoint b)
{
	return sqrt(pow((a.x-b.x),2.0)+pow((a.y-b.y),2.0));
}
int Kcomp(const void *pa, const void *pb)
{
	float diff;
    sortable_KnobForm a = *(sortable_KnobForm *)pa;
    sortable_KnobForm b = *(sortable_KnobForm *)pb;
	if(a.CPoint.y - b.CPoint.y>(a.KnobForm[3] - a.KnobForm[2])/2)
		diff=1;
	else if(b.CPoint.y - a.CPoint.y>(a.KnobForm[3] - a.KnobForm[2])/2)
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

int Kboxcomp(const void *pa, const void *pb)
{
    float* a = *(float* *)pa;
    float* b = *(float* *)pb;
    float diff = a[4] - b[4] ;
    if(diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}

void KGetOverLappingRate(MKRECT rt1,MKRECT rt2,long *minRate, long *maxRate)
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

int HYKR_KnobRecog(void *hMRHandle,KR_IMAGES *pImg , HYKR_RESULT_LIST *pResultList)
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
	MKRECT targetarea;
	image out ;
	//int maxindex=-1;
	int MinDist,MinDistX;
	int previousX[100];
	KBboxOut bbox={0};
	sortable_KnobForm *s;
    KDRParam *pDRParam = (KDRParam*)hMRHandle;
	total=pResultList->lMaxResultNum;
	bbox.Bbox = (float **)calloc(total, sizeof(float *));  //free
	for(i = 0; i < total; ++i) bbox.Bbox[i] = (float*)calloc(6, sizeof(float));

	thresh = pDRParam->lThresh;	
	out =   HYK_ipl_to_image(pImg);
	bbox.num = 0;
	//printf("----------------------把手启动分析-------------------\n");
	YoloDetector(pDRParam->net, out, thresh,0.5,&bbox.num,bbox.Bbox);
	/*printf("偏移x=%d y=%d\n",pResultList->offset.x,pResultList->offset.y);
	printf("bbox.num=%d\n",bbox.num);
	for(i = 0; i < bbox.num; ++i){ 
		printf("%d:%f  left=%f right=%f top=%f bot=%f\n",i,bbox.Bbox[i][5],bbox.Bbox[i][0],bbox.Bbox[i][1],bbox.Bbox[i][2],bbox.Bbox[i][3]);
	}*/
	qsort(bbox.Bbox, bbox.num, sizeof(float*), Kboxcomp);
	free_image(out);
	
	s = (sortable_KnobForm*)calloc(bbox.num, sizeof(sortable_KnobForm));

	//printf("%d\n",bbox.num);
	if(bbox.num < 1){		//
		res = -1;
		goto EXT;
	}

	normalHeight = bbox.Bbox[0][3]-bbox.Bbox[0][2];
	
	j=0;
	for(i = 0; i < bbox.num; ++i){ 
		//
		float a;
		s[i].KnobForm =bbox.Bbox[i]; 
		//printf("%d:%f\n",i,s[i].KnobForm[5]);
		s[i].CPoint.x = (s[i].KnobForm[0]+s[i].KnobForm[1])/2 ;
		s[i].CPoint.y = (s[i].KnobForm[2]+s[i].KnobForm[3])/2 ;
	    a=fabs(1-(1.0*normalHeight)/(s[i].KnobForm[3]-s[i].KnobForm[2]));
		//if(fabs(a)>0.3)
		{
			//printf("-1\n");
			//s[i].flag = -1;
		}
		//else
		{
			s[i].flag = 0;//选取实际数目个目标
			j=j+1;
			if(j> pResultList->lRealResultNum)s[i].flag=-1;
		}
	}
	
	qsort(s, bbox.num, sizeof(sortable_KnobForm), Kcomp);  //从左到右排序,从上到下排序
   /* for(j = 0; j < pResultList->lRealResultNum; ++j)
	{
		MKRECT tmpArea;
		
		tmpArea.left=pResultList->LRArea[j].left+pResultList->offset.x;
		tmpArea.right=pResultList->LRArea[j].right+pResultList->offset.x;
		tmpArea.top=pResultList->LRArea[j].top+pResultList->offset.y;
		tmpArea.bottom=pResultList->LRArea[j].bottom+pResultList->offset.y;
		printf("orign:left=%d right=%d top=%d bot=%d\n",pResultList->LRArea[j].left,pResultList->LRArea[j].right,pResultList->LRArea[j].top,pResultList->LRArea[j].bottom);
		printf("x=%d  y=%d\n",pResultList->offset.x,pResultList->offset.y);
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
			MKRECT tmpArea;
			long tmpRate;
			long tmpRate1;
			long tmpRate2;
			tmpArea.left=s[i].KnobForm[0];
			tmpArea.right=s[i].KnobForm[1];
			tmpArea.top=s[i].KnobForm[2];
			tmpArea.bottom=s[i].KnobForm[3];
			KGetOverLappingRate(targetarea,tmpArea,NULL,&tmpRate1);
			KGetOverLappingRate(tmpArea,targetarea,NULL,&tmpRate2);
			tmpRate=MAX(tmpRate1,tmpRate2);
			//printf("rate=%d\n",tmpRate);
			if(tmpRate > 20 && tmpRate > lMaxRate)
			{
				pResultList->pResult[j].flag=1;
				pResultList->pResult[j].dVal=s[i].KnobForm[5];
				pResultList->pResult[j].Target.left=s[i].KnobForm[0];
				pResultList->pResult[j].Target.right=s[i].KnobForm[1];
				pResultList->pResult[j].Target.top=s[i].KnobForm[2];
				pResultList->pResult[j].Target.bottom=s[i].KnobForm[3];
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
	//	targetarea.left=s[i].KnobForm[0];
	//	targetarea.right=s[i].KnobForm[1];
	//	targetarea.top=s[i].KnobForm[2];
	//	targetarea.bottom=s[i].KnobForm[3];
	//	//printf("1:left=%d right=%d top=%d bot=%d\n",targetarea.left,targetarea.right,targetarea.top,targetarea.bottom);
	//	for(j = 0; j < pResultList->lRealResultNum; ++j)
	//	{
	//		MKRECT tmpArea;
	//		long tmpRate;
	//		tmpArea.left=pResultList->LRArea[j].left+pResultList->offset.x;
	//		tmpArea.right=pResultList->LRArea[j].right+pResultList->offset.x;
	//		tmpArea.top=pResultList->LRArea[j].top+pResultList->offset.y;
	//		tmpArea.bottom=pResultList->LRArea[j].bottom+pResultList->offset.y;
	//		KGetOverLappingRate(targetarea,tmpArea,NULL,&tmpRate);
	//		//printf("rate=%d\n",tmpRate);
	//		if(tmpRate > 20 && tmpRate > lMaxRate)
	//		{
	//			if(previous != -1)
	//			{
	//				pResultList->pResult[previous].flag=0;
	//			}
	//			pResultList->pResult[j].flag=1;
	//			pResultList->pResult[j].dVal=s[i].KnobForm[5];
	//			pResultList->pResult[j].Target.left=s[i].KnobForm[0];
	//			pResultList->pResult[j].Target.right=s[i].KnobForm[1];
	//			pResultList->pResult[j].Target.top=s[i].KnobForm[2];
	//			pResultList->pResult[j].Target.bottom=s[i].KnobForm[3];
	//			//printf("2:left=%d right=%d top=%d bot=%d\n",pResultList->pResult[j].Target.left,pResultList->pResult[j].Target.right,pResultList->pResult[j].Target.top,pResultList->pResult[j].Target.bottom);
	//			previous=j;
	//			lMaxRate=tmpRate;
	//		}
	//	}
	//	lResultNum+=1;
	//	
	//}
	pResultList->lResultNum=pResultList->lRealResultNum;

	
	

EXT:
	for(i = 0; i < total; ++i) 
	{
		free(bbox.Bbox[i]);
	}
	free(bbox.Bbox);
	if(s==NULL)
	free(s);
	

	return res;
}



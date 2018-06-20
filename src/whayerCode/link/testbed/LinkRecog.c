#include "LinkRecog.h"
//#include <stdio.h>
#include "darknet.h"
#include "math.h"
#include <stdio.h>
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
}LDRParam;

typedef struct 
{
	int x;
	int y;
}LPoint;

typedef struct{
    int num;
	float **Bbox;
} LBboxOut;


typedef struct{
	int flag;
	int first;
	int next;
	LPoint CPoint;
    float *LinkForm;
} sortable_LinkForm;

image HYL_ipl_to_image(LR_IMAGES *pImg)
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


int HYLR_Init(void *hMemMgr, void **pMRHandle)
{
	int res = 0;

	LDRParam *pDRHandle=NULL;
	if (!pMRHandle)
	{
		res= -1;
		goto EXT;
	}
	pDRHandle=(LDRParam *)malloc(1*sizeof(LDRParam));
	*pMRHandle=pDRHandle;
EXT:
	return res;
}
int HYLR_Uninit(void *hMRHandle)
{
	LDRParam *pDRParam = (LDRParam*)hMRHandle;
	//一定要注意释放模型参数

	free_network(pDRParam->net);
	

	if (pDRParam)
		free (pDRParam);
	pDRParam = NULL;
	return 0;
}

int HYLR_SetParam(void *hMRHandle,char *cfgfile, char *weightfile,float Thresh)
{
	int res=0;
	LDRParam *pDRParam = (LDRParam*)hMRHandle;
	
	pDRParam->lThresh = Thresh;
	pDRParam->net = parse_network_cfg(cfgfile);
	if(weightfile){
        load_weights(&(pDRParam->net), weightfile);
    }
    set_batch_network(&(pDRParam->net), 1);
	return 0;
}
int TPointDis(LPoint a,LPoint b)
{
	return sqrt(pow((a.x-b.x),2.0)+pow((a.y-b.y),2.0));
}
int Lcomp(const void *pa, const void *pb)
{
	float diff;
    sortable_LinkForm a = *(sortable_LinkForm *)pa;
    sortable_LinkForm b = *(sortable_LinkForm *)pb;
	if(a.CPoint.y - b.CPoint.y>(a.LinkForm[3] - a.LinkForm[2])/2)
		diff=1;
	else if(b.CPoint.y - a.CPoint.y>(a.LinkForm[3] - a.LinkForm[2])/2)
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

int Lboxcomp(const void *pa, const void *pb)
{
    float* a = *(float* *)pa;
    float* b = *(float* *)pb;
    float diff = a[4] - b[4] ;
    if(diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}

void LGetOverLappingRate(MLRECT rt1,MLRECT rt2,long *minRate, long *maxRate)
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


int HYLR_LinkRecog(void *hMRHandle,LR_IMAGES *pImg , HYLR_RESULT_LIST *pResultList)
{
	//FILE *fp=fopen("E:/HYLR_LinkRecog.txt","w");
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
	MLRECT targetarea;
	image out ;
	//int maxindex=-1;
	int MinDist,MinDistX;
	int previousX[100];
	LBboxOut bbox={0};
	sortable_LinkForm *s;
    LDRParam *pDRParam = (LDRParam*)hMRHandle;
	total=pResultList->lMaxResultNum;
	bbox.Bbox = (float **)calloc(total, sizeof(float *));  //free
	for(i = 0; i < total; ++i) bbox.Bbox[i] = (float*)calloc(6, sizeof(float));

	/*for(i = 0; i < total; ++i){ 
		printf("_%d:%f\n",i,bbox.Bbox[i][5]);
	}*/

	thresh = pDRParam->lThresh;	
	out =   HYL_ipl_to_image(pImg);
	//printf("h=%d w=%d\n",out.h,out.w);
	
	bbox.num = 0;
	//printf("----------------------压板启动分析111-------------------\n");
	YoloDetector(pDRParam->net, out, thresh,0.5,&bbox.num,bbox.Bbox);
	//printf("----------------------压板分析完毕222-------------------\n");
	/*printf("偏移x=%d y=%d\n",pResultList->offset.x,pResultList->offset.y);
	printf("bbox.num=%d\n",bbox.num);
	for(i = 0; i < bbox.num; ++i){ 
		printf("%d:%f  left=%f right=%f top=%f bot=%f\n",i,bbox.Bbox[i][5],bbox.Bbox[i][0],bbox.Bbox[i][1],bbox.Bbox[i][2],bbox.Bbox[i][3]);
	}*/

	qsort(bbox.Bbox, bbox.num, sizeof(float*), Lboxcomp);
	free_image(out);
	
	s = (sortable_LinkForm*)calloc(bbox.num, sizeof(sortable_LinkForm));
	//fprintf(fp,"bbox.num=%d\n",bbox.num);
	//printf("%d\n",bbox.num);
	if(bbox.num < 1){		//
		res = -1;
		goto EXT;
	}

	normalHeight = bbox.Bbox[0][3]-bbox.Bbox[0][2];
	
	j=0;
	//printf("bbox.num=%d\n",bbox.num);
	for(i = 0; i < bbox.num; ++i){ 
		//
		float a;
		s[i].LinkForm =bbox.Bbox[i]; 
		//printf("%d:%f\n",i,s[i].LinkForm[5]);
		s[i].CPoint.x = (s[i].LinkForm[0]+s[i].LinkForm[1])/2 ;
		s[i].CPoint.y = (s[i].LinkForm[2]+s[i].LinkForm[3])/2 ;
	    a=fabs(1-(1.0*normalHeight)/(s[i].LinkForm[3]-s[i].LinkForm[2]));
		//if(fabs(a)>0.5)
		{
			//printf("-1\n");
			//s[i].flag = -1;
		}
		//else
		{
			s[i].flag = 0;//选取实际数目个目标
			j=j+1;
			//printf("j=%d\n",j);
			if(j> pResultList->lRealResultNum){
				s[i].flag=-1;
				//printf("j=%d,flag=-1\n",j);
			}
		}
	}
	
	qsort(s, bbox.num, sizeof(sortable_LinkForm), Lcomp);  //从左到右排序,从上到下排序
	
	/*for(j = 0; j < pResultList->lRealResultNum; ++j)
	{
		MLRECT tmpArea;
		
		tmpArea.left=pResultList->LRArea[j].left+pResultList->offset.x;
		tmpArea.right=pResultList->LRArea[j].right+pResultList->offset.x;
		tmpArea.top=pResultList->LRArea[j].top+pResultList->offset.y;
		tmpArea.bottom=pResultList->LRArea[j].bottom+pResultList->offset.y;
		fprintf(fp,"偏移前手框位置%d:left=%d right=%d top=%d bot=%d\n",j,pResultList->LRArea[j].left,pResultList->LRArea[j].right,pResultList->LRArea[j].top,pResultList->LRArea[j].bottom);
		printf("偏移前手框位置%d:left=%d right=%d top=%d bot=%d\n",j,pResultList->LRArea[j].left,pResultList->LRArea[j].right,pResultList->LRArea[j].top,pResultList->LRArea[j].bottom);
		
	}*/
	//fprintf(fp,"偏移:x=%d  y=%d\n",pResultList->offset.x,pResultList->offset.y);
	//printf("偏移:x=%d  y=%d\n",pResultList->offset.x,pResultList->offset.y);
	for(j = 0; j < pResultList->lRealResultNum; ++j)
	{
		long lMaxRate=0;
		int previous=-1;
		targetarea.left=pResultList->LRArea[j].left;//+pResultList->offset.x;
		targetarea.right=pResultList->LRArea[j].right;//+pResultList->offset.x;
		targetarea.top=pResultList->LRArea[j].top;//+pResultList->offset.y;
		targetarea.bottom=pResultList->LRArea[j].bottom;//+pResultList->offset.y;
		pResultList->pResult[j].flag=0;
		//fprintf(fp,"待对应框的目标%d:left=%d right=%d top=%d bot=%d\n",j,targetarea.left,targetarea.right,targetarea.top,targetarea.bottom);
		for(i = 0; i < bbox.num; ++i){
			MLRECT tmpArea;
			long tmpRate;
			long tmpRate1;
			long tmpRate2;
			tmpArea.left=s[i].LinkForm[0];
			tmpArea.right=s[i].LinkForm[1];
			tmpArea.top=s[i].LinkForm[2];
			tmpArea.bottom=s[i].LinkForm[3];
			LGetOverLappingRate(tmpArea,targetarea,NULL,&tmpRate1);
			LGetOverLappingRate(targetarea,tmpArea,NULL,&tmpRate2);
			tmpRate=MAX(tmpRate1,tmpRate2);
			/*if(tmpRate > 0)
			{
				fprintf(fp,"重叠百分比%d:rate=%d\n",i,tmpRate);
				printf("重叠百分比%d:rate=%d\n",i,tmpRate);
			}*/
			if(tmpRate > 20 && tmpRate > lMaxRate)
			{
				/*if(previous != -1)
				{
					pResultList->pResult[previous].flag=0;
				}*/
				pResultList->pResult[j].flag=1;
				pResultList->pResult[j].dVal=s[i].LinkForm[5];
				pResultList->pResult[j].Target.left=s[i].LinkForm[0];
				pResultList->pResult[j].Target.right=s[i].LinkForm[1];
				pResultList->pResult[j].Target.top=s[i].LinkForm[2];
				pResultList->pResult[j].Target.bottom=s[i].LinkForm[3];
				//fprintf(fp,"选择重叠的位置:left=%d right=%d top=%d bot=%d\n",tmpArea.left,tmpArea.right,tmpArea.top,tmpArea.bottom);
				//printf("选择重叠的位置:left=%d right=%d top=%d bot=%d\n\n",tmpArea.left,tmpArea.right,tmpArea.top,tmpArea.bottom);
				//fprintf(fp,"输出重叠的位置:left=%d right=%d top=%d bot=%d\n",pResultList->pResult[j].Target.left,pResultList->pResult[j].Target.right,pResultList->pResult[j].Target.top,pResultList->pResult[j].Target.bottom);
				//printf("输出重叠的位置:left=%d right=%d top=%d bot=%d\n\n",pResultList->pResult[j].Target.left,pResultList->pResult[j].Target.right,pResultList->pResult[j].Target.top,pResultList->pResult[j].Target.bottom);
				//previous=i;
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
	//	targetarea.left=s[i].LinkForm[0];
	//	targetarea.right=s[i].LinkForm[1];
	//	targetarea.top=s[i].LinkForm[2];
	//	targetarea.bottom=s[i].LinkForm[3];
	//	fprintf(fp,"待对应手框的目标%d:left=%d right=%d top=%d bot=%d\n",i,targetarea.left,targetarea.right,targetarea.top,targetarea.bottom);
	//	printf("待对应手框的目标%d:left=%d right=%d top=%d bot=%d\n",i,targetarea.left,targetarea.right,targetarea.top,targetarea.bottom);
	//	for(j = 0; j < pResultList->lRealResultNum; ++j)
	//	{
	//		MLRECT tmpArea;
	//		long tmpRate;
	//		tmpArea.left=pResultList->LRArea[j].left+pResultList->offset.x;
	//		tmpArea.right=pResultList->LRArea[j].right+pResultList->offset.x;
	//		tmpArea.top=pResultList->LRArea[j].top+pResultList->offset.y;
	//		tmpArea.bottom=pResultList->LRArea[j].bottom+pResultList->offset.y;
	//		LGetOverLappingRate(targetarea,tmpArea,NULL,&tmpRate);
	//		if(tmpRate > 0)
	//		{
	//			fprintf(fp,"重叠百分比%d:rate=%d\n",j,tmpRate);
	//			printf("重叠百分比%d:rate=%d\n",j,tmpRate);
	//		}
	//		if(tmpRate > 20 && tmpRate > lMaxRate)
	//		{
	//			if(previous != -1)
	//			{
	//				pResultList->pResult[previous].flag=0;
	//			}
	//			pResultList->pResult[j].flag=1;
	//			pResultList->pResult[j].dVal=s[i].LinkForm[5];
	//			pResultList->pResult[j].Target.left=s[i].LinkForm[0];
	//			pResultList->pResult[j].Target.right=s[i].LinkForm[1];
	//			pResultList->pResult[j].Target.top=s[i].LinkForm[2];
	//			pResultList->pResult[j].Target.bottom=s[i].LinkForm[3];
	//			fprintf(fp,"选择重叠的位置:left=%d right=%d top=%d bot=%d\n\n",tmpArea.left,tmpArea.right,tmpArea.top,tmpArea.bottom);
	//			printf("选择重叠的位置:left=%d right=%d top=%d bot=%d\n\n",tmpArea.left,tmpArea.right,tmpArea.top,tmpArea.bottom);
	//			//printf("选择重叠的位置:left=%d right=%d top=%d bot=%d\n",pResultList->pResult[j].Target.left,pResultList->pResult[j].Target.right,pResultList->pResult[j].Target.top,pResultList->pResult[j].Target.bottom);
	//			previous=j;
	//			lMaxRate=tmpRate;
	//		}
	//	}
	//	lResultNum+=1;
	//	
	//}
	//fclose(fp);
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



#include "SwitchRecog.h"
#include "darknet.h"
#include <math.h>




typedef struct{
    int num;
	float **Bbox;
} SBboxOut;

typedef struct 
{
	network net;
	float lThresh;
}SSRParam;
typedef struct 
{
	int x;
	int y;
}SPoint;
typedef struct{
	int flag;
	int next;
	SPoint CPoint;
    float *bbox;
} SDBbox;

image HYS_ipl_to_image(SR_IMAGES *pImg)
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


int HYSR_Init(void *hMemMgr, void **pMRHandle)
{
	int res = 0;
	SSRParam *pSRHandle=NULL;
	if (!pMRHandle)
	{
		res= -1;
		goto EXT;
	}
	//if (!pMRHandle) return -1;
	pSRHandle=(SSRParam *)malloc(1*sizeof(SSRParam));
	*pMRHandle=pSRHandle;
    EXT:
	return res;
}
int HYSR_Uninit(void *hMRHandle)
{
	SSRParam *pSRParam = (SSRParam*)hMRHandle;
	free_network(pSRParam->net);


	if (pSRParam)
		free (pSRParam);
	pSRParam = NULL;
	return 0;
}
int HYSR_SetParam(void *hMRHandle,char *cfgfile, char *weightfile,float Thresh)
{
	int res=0;
	SSRParam *pSRParam = (SSRParam*)hMRHandle;

	pSRParam->lThresh = Thresh;
	pSRParam->net = parse_network_cfg(cfgfile);
	if(weightfile){
        load_weights(&(pSRParam->net), weightfile);
    }
    set_batch_network(&(pSRParam->net), 1);
	return 0;
	
}
int STPointDis(SPoint a,SPoint b)
{
	return sqrt(pow((a.x-b.x),2.0)+pow((a.y-b.y),2.0));
}


int HYSR_SwitchRecog(void *hMRHandle,SR_IMAGES *pImg,HYSR_RESULT *result)
{
	int res =0;
	int i=0,j=0;
	int total=10;
	double max=0.0000;
	int group=1;
	int Maxindex=0;
	int MinDist,MinDistX;
	float MaxProb;
    float thresh;
	image out;
	SBboxOut bbox_out={0};
	
	//sortable_DigitForm *s;
	SSRParam *pSRParam = (SSRParam*)hMRHandle;
	bbox_out.Bbox = (float**)calloc(total, sizeof(float *));  //free
	for(i = 0; i < total; ++i) bbox_out.Bbox[i] = (float*)calloc(6, sizeof(float));
	//yolo检测输出
	thresh = pSRParam->lThresh;	

	if(!pImg)
	{
		res = -1;
		goto EXT;
	}
    out =   HYS_ipl_to_image(pImg);

	YoloDetector(pSRParam->net,out,thresh,0.5,&bbox_out.num,bbox_out.Bbox);

	free_image(out);

	if(bbox_out.num == 0)
	{
		printf("未找到开关\n");
		return -1;
		goto EXT;
	}
	

	MaxProb = bbox_out.Bbox[0][4];
	for(i = 1;i<bbox_out.num; i++)
	{
		if(bbox_out.Bbox[i][4]>MaxProb)
		{
			MaxProb= bbox_out.Bbox[i][4];
			Maxindex =i;
		}
	}

	
	result->rtTarget.left = bbox_out.Bbox[Maxindex][0];
	result->rtTarget.top = bbox_out.Bbox[Maxindex][2];
	result->rtTarget.right = bbox_out.Bbox[Maxindex][1];
	result->rtTarget.bottom = bbox_out.Bbox[Maxindex][3];
	result->dlabel = bbox_out.Bbox[Maxindex][5];
	result->dConfidence = MaxProb;
EXT:

	for(i = 0; i < total; ++i)
	{
		free(bbox_out.Bbox[i]);
	}
	free(bbox_out.Bbox);

	return res;
}



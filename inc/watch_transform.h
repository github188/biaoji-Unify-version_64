#ifndef WATCH_TRANSFORM_H
#define WATCH_TRANSFORM_H

#include"amcomdef.h"
#include"HYAMR_meterReg.h"

#ifdef  __cplusplus  
extern "C" {
#endif 
	//__declspec(dllexport) int action(HYAMR_IMAGES src,HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam, WidthType *width, char *modelpath,double Minsize,double Maxsize);
	__declspec(dllexport) int action(HYAMR_IMAGES src,HYLPAMR_IMAGES mask,HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam, WidthType *width, MLong *Dist,char *modelpath,double Minsize,double Maxsize);
	__declspec(dllexport) int sf6(HYAMR_IMAGES src,HYAMR_IMAGES *dst,HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam, WidthType *width, SF6PT *SF, MLong *Dist,char *modelpath,double Minsize,double Maxsize);
#ifdef  __cplusplus  
}
#endif
#endif
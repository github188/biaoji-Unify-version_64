#ifndef WATCH_TRANSFORM_H
#define WATCH_TRANSFORM_H

#include"amcomdef.h"
#include"HYAMR_meterReg.h"

#ifdef  __cplusplus  
extern "C" {
#endif 

	__declspec(dllexport) int action(HYAMR_IMAGES src,HYAMR_INTERACTIVE_PARA *pOutPattern, MaskParam *pMaskParam, WidthType *width, char *modelpath,double Minsize,double Maxsize);
#ifdef  __cplusplus  
}
#endif
#endif
#ifndef __ProtectionLink_SqueezeNet_S1_h__
#define __ProtectionLink_SqueezeNet_S1_h__


#include"amcomdef.h"
//#ifndef MRESULT 
//#define MRESULT int
//#endif

#define BUFFERSIZEINSULATOR_SQUEEZENET_S1_BBOX_PRED 1
#define BUFFERSIZEINSULATOR_SQUEEZENET_S1_CLS_PROB 1
#define BUFFERSIZEINSULATOR_SQUEEZENET_S1_ROIS 1

MRESULT ProtectionLink_SqueezeNet_S1_run_C3(unsigned char* bgr, int iw, int ih, int widthStep, int* label);
#endif
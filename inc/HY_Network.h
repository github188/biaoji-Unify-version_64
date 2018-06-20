#ifndef HY_NETWORK_H
#define HY_NETWORK_H
#pragma warning(disable:4267)
/************************************************************************/
/*   底层库函数：集成GPU、CPU版本的不同网络层函数以及功能模块函数       */
/************************************************************************/

#define CUDA_NUM_THREADS 128


#define NUM_OBJECT 1  //对象
#define NUM_CLASSES 2  //分类

// CUDA: number of blocks for threads.
inline int CUDA_GET_BLOCKS(const int N) {
	return (N + CUDA_NUM_THREADS - 1) / CUDA_NUM_THREADS;
}
// CUDA: grid stride looping
#define CUDA_KERNEL_LOOP(i, n) \
  for (int i = blockIdx.x * blockDim.x + threadIdx.x; \
       i < (n); \
       i += blockDim.x * gridDim.x)

#define DIV_THEN_CEIL(x, y)  (((x) + (y) - 1) / (y))

//----------------.cu----------------------------//

#define HY_Network_EXPORTS

#ifdef __cplusplus //指明函数的编译方式，以得到没有任何修饰的函数名
extern "C"
{
#endif

#ifdef HY_Network_EXPORTS
#define HY_Network_API __declspec(dllexport) //导出符号宏定义
#else
#define HY_Network_API __declspec(dllimport)
#endif

	
	
	HY_Network_API void createHandles(void **cudnnhandles);//建立cudnn句柄，gpu调用底层函数需要创建cudnn库的句柄
	HY_Network_API void destroyHandles(void *&cudnnhandles);//销毁cudnn句柄

	//----------------GPU basement--------------------------//
	//GPU前向卷积函数
	HY_Network_API void HYConvForward_gpu(void *handles, int& n, int& c, int& h, int& w, const float* inputs, float** outputs, int ichannels, int ochannels, int kerner, const float* weights, const float* bias, int pad_w, int pad_h, int stepw, int steph);

	//GPU连接函数
	HY_Network_API void HYConnect_gpu(void *handles, int& n, int& c, int&h, int&w, const float* inputs, float** outputs, int outputnum, const float* weights, const float* bias);

	//GPU前向Softmax函数
	HY_Network_API void HYSoftmaxForward_gpu(void *handles, int n, int c, int h, int w, const float* inputs, float** outputs);

	//GPU前向Lrn函数
	HY_Network_API void HYLrnForward_gpu(void ** handles, int n, int c, int h, int w, const float* inputs, float** outputs);

	//GPU前向Activation函数
	HY_Network_API void HYActivationForward_gpu(void *handles, int n, int c, int h, int w, const float* inputs, float** outputs);

	//GPU前向Pool(max)函数
	HY_Network_API void HYPoolForward_gpu(void *handles, int& n, int& c, int& h, int& w, const float* inputs, float** outputs, int kw, int kh, int stepw, int steph, int pad_w, int pad_h);
	
	//GPU前向Pool(avg)函数
	HY_Network_API void HYPoolForward_avg_gpu(void * handle_gpu, int& n, int& c, int& h, int& w,float* inputs, float** outputs, int kw, int kh, int stepw, int steph, int pad_w, int pad_h);

	//fp16.cu		
	HY_Network_API void HYAddBias_gpu(const float *bias, float **outputs, int outputnumber, int n);

	HY_Network_API void HYConcatForward_gpu_input2(int c1, int c2, int h, int w, const float* inputs1, const float* inputs2, float** dstData);

	HY_Network_API void HYConcatForward_gpu_input3(int c1, int c2, int c3, int h, int w, float* inputs1, float* inputs2, float* inputs3, float** dstData);

	HY_Network_API void HYDeconvForward_gpu(const float* inputs, int iw, int ih, int ichannels, int groups, const float* weights, int ww, int wh, int pad_w, int pad_h, int stepw, int steph, float** outputs, int ochannels);


	HY_Network_API void HYRoiPoolForward_gpu(const float* inputs, int& width_, int& height_, int channels_, const float* rois, int num_rois, int pooled_width_, int pooled_height_, float spatial_scale_, float** outputs, int noutput);


	HY_Network_API void HYProposalForward_gpu(const float* inputs_cls, int iw, int ih, int ichannels, float imagew, float imageh, const float* bbox_deltas, float** rois_output, float** score_output, float feat_stride_, int pre_nms_topN_, int post_nms_topN_, float nms_thresh_, int min_size_, float* anchors_base, int num_anchors_, int* num_keep);

	//-----------------Module function------------------------//
	//PVA网络Inception_3a层 功能函数
	 void Inception_3a_gpu(void * handles, int &n, int &c, int &h, int &w,
		const float *inc3a_conv1_weights, const float *inc3a_conv1_bias, const float *inc3a_conv3_1_weights,
		const float *inc3a_conv3_1_bias, const float *inc3a_conv3_2_weights, const float *inc3a_conv3_2_bias,
		const float *inc3a_conv5_1_weights, const float *inc3a_conv5_1_bias, const float *inc3a_conv5_2_weights,
		const float *inc3a_conv5_2_bias, const float *inc3a_conv5_3_weights, const float *inc3a_conv5_3_bias,
		const float* inputs, float **outputs);

	//PVA网络Inception_4a层 功能函数
	 void Inception_4a_gpu(void * handles, int &n, int &c, int &h, int &w,
		const float *inc4a_conv1_weights, const float *inc4a_conv1_bias, const float *inc4a_conv3_1_weights,
		const float *inc4a_conv3_1_bias, const float *inc4a_conv3_2_weights, const float *inc4a_conv3_2_bias,
		const float *inc4a_conv5_1_weights, const float *inc4a_conv5_1_bias, const float *inc4a_conv5_2_weights,
		const float *inc4a_conv5_2_bias, const float *inc4a_conv5_3_weights, const float *inc4a_conv5_3_bias,
		const float* inputs, float **outputs);

	//PVA网络Inception_3层 中间层功能函数（Inception_3b、3c、3d、3e）
	 void Inception_3_gpu(void *handles, int &n, int &c, int &h, int &w, const float * conv1_weights, const float *conv1_bias, const float * conv3_1_weights, const float *conv3_1_bias, const float * conv3_2_weights, const float *conv3_2_bias, const float * conv5_1_weights, const float *conv5_1_bias, const float * conv5_2_weights, const float *conv5_2_bias, const float * conv5_3_weights, const float *conv5_3_bias, const float* inputs, float **outputs);

	//PVA网络Inception_4层 中间层功能函数（Inception_4b、4c、4d、4e）
	 void Inception_4_gpu(void *handles, int &n, int &c, int &h, int &w, const float * conv1_weights, const float *conv1_bias, const float * conv3_1_weights, const float *conv3_1_bias, const float * conv3_2_weights, const float *conv3_2_bias, const float * conv5_1_weights, const float *conv5_1_bias, const float * conv5_2_weights, const float *conv5_2_bias, const float * conv5_3_weights, const float *conv5_3_bias, const float* inputs, float **outputs);

	//PVA网络Hyper_feature层函数
	 void Hyper_feature_gpu(void *handles, int &n, int &c, int &h, int &w, const float* inputs_conv3_relu, const float* upsample_weights, const float* convf_weights, const float* convf_bias, const float* inputs_inc4e_concat2, const float* inputs_inc3e_concat2, float **outputs);

	//PVA网络Rpn层函数
	 void Rpn_gpu(void *handles, int &n, int &c, int &h, int &w, const float *rpn_conv1_weights, const float *rpn_conv1_bias, const float *rpn_bbox_pred_weights, const float *rpn_bbox_pred_bias, const float*rpn_cls_score_weights, const float*rpn_cls_score_bias, const float* inputs, float **outputs_rpn_bbox_perd, float **outputs_rpn_cls_score);


	//-------------------CPU basement--------------------------//
	//CPU卷积函数
	HY_Network_API void HYConv_cpu(int& n, int& c, int& h, int& w, const float* inputs, float*& outputs, int ichannels, int ochannels, int kerner, const float* weights, const float* bias, int pad_w, int pad_h, int stepw, int steph);

	//CPU连接函数
	HY_Network_API void HYConnect_cpu(int& n, int& c, int&h, int&w, const float* inputs, float*& outputs, int outputnum, const float* weights, const float* bias);

	//CPUSoftmax函数
	HY_Network_API void HYSoftmax_cpu(int n, int c, int h, int w, const float* inputs, float*& outputs);

	//CPUActivation函数
	HY_Network_API void HYActivation_cpu(int n, int c, int h, int w, const float* inputs, float*& outputs);

	//CPU池化函数(max)
	HY_Network_API void HYPool_cpu(int& n, int& c, int& h, int& w, const float* inputs, float*& outputs, int kw, int kh, int stepw, int steph, int pad_w, int pad_h);
	
	//CPU池化函数(avg)
	HY_Network_API void HYPool_avg_cpu(int& n, int& c, int& h, int& w, const float* inputs, float*& outputs, int kw, int kh, int stepw, int steph, int pad_w, int pad_h);
	
	//CPUConcat函数
	HY_Network_API void HYConcat_cpu(const float* inputs, int iw, int ih,int ichannels, float*& outputs, int* ptrout);

	//CPU反卷积函数
	HY_Network_API void HYDeconv_cpu(
		const float* inputs, int& iw, int& ih, int& ichannels, int groups,
		const float* weights, int ww, int wh, int pad_w, int pad_h, int stepw, int steph,
		float*& outputs, int ochannels);

	//CPU Roipooling函数
	HY_Network_API void HYRoipooling_cpu(const float* inputs, int width_, int height_, int channels_,
		const float* rois, int num_rois, int pooled_width_, int pooled_height_, float spatial_scale_,
		float* outputs, int noutput);

	//CPU Rpn函数
	HY_Network_API void HYRpn_cpu(const float* inputs_cls, int iw, int ih, int ichannels, int imagew, int imageh,
		const float* bbox_deltas, float*& rois, int feat_stride_,
		int pre_nms_topN_, int post_nms_topN_, float nms_thresh_, int min_size_,
		float* anchors_, int num_anchors_, int* num_keep);

	//-----------------Module function------------------------//
	 void Inception_3a_cpu(int &n, int &c, int &h, int &w,
		const float *inc3a_conv1_weights, const float *inc3a_conv1_bias, const float *inc3a_conv3_1_weights,
		const float *inc3a_conv3_1_bias, const float *inc3a_conv3_2_weights, const float *inc3a_conv3_2_bias,
		const float *inc3a_conv5_1_weights, const float *inc3a_conv5_1_bias, const float *inc3a_conv5_2_weights,
		const float *inc3a_conv5_2_bias, const float *inc3a_conv5_3_weights, const float *inc3a_conv5_3_bias,
		const float* inputs, float*& outputs);
	 void Inception_4a_cpu(int &n, int &c, int &h, int &w,
		const float *inc4a_conv1_weights, const float *inc4a_conv1_bias, const float *inc4a_conv3_1_weights,
		const float *inc4a_conv3_1_bias, const float *inc4a_conv3_2_weights, const float *inc4a_conv3_2_bias,
		const float *inc4a_conv5_1_weights, const float *inc4a_conv5_1_bias, const float *inc4a_conv5_2_weights,
		const float *inc4a_conv5_2_bias, const float *inc4a_conv5_3_weights, const float *inc4a_conv5_3_bias,
		const float* inputs, float *&outputs);

	 void Inception_3_cpu(int &n, int &c, int &h, int &w, const float * conv1_weights, const float *conv1_bias, const float * conv3_1_weights, const float *conv3_1_bias, const float * conv3_2_weights, const float *conv3_2_bias, const float * conv5_1_weights, const float *conv5_1_bias, const float * conv5_2_weights, const float *conv5_2_bias, const float * conv5_3_weights, const float *conv5_3_bias, const float* inputs, float *&outputs);

	 void Inception_4_cpu(int &n, int &c, int &h, int &w, const float * conv1_weights, const float *conv1_bias, const float * conv3_1_weights, const float *conv3_1_bias, const float * conv3_2_weights, const float *conv3_2_bias, const float * conv5_1_weights, const float *conv5_1_bias, const float * conv5_2_weights, const float *conv5_2_bias, const float * conv5_3_weights, const float *conv5_3_bias, const float* inputs, float *&outputs);

	 void Hyper_feature_cpu(int &n, int &c, int &h, int &w, const float* inputs_conv3_relu, const float* upsample_weights, const float* convf_weights, const float* convf_bias, const float* inputs_inc4e_concat2, const float* inputs_inc3e_concat2, float *&outputs);

	 void Rpn_cpu(int &n, int &c, int &h, int &w, const float *rpn_conv1_weights, const float *rpn_conv1_bias, const float *rpn_bbox_pred_weights, const float *rpn_bbox_pred_bias, const float*rpn_cls_score_weights, const float*rpn_cls_score_bias, const float* inputs, float *&outputs_rpn_bbox_perd, float *&outputs_rpn_cls_score);

	
	/********************************************************************************************/
	/*  搭建神经网络，不同的网络放在不同的class中，目前具有PVANET、SqueezeNet、YoloNet模型，
	放在 pva_detect、SqueezeNet_detect、YoloNet_detect 类中                                   */
	/********************************************************************************************/
	//PVANET 检测
	class HY_Network_API pva_detect{

	public:
		void * PvaHandls;   //GPU句柄变量，必须定义此变量 

		//定义各层 weight、bias变量
		float* conv1_weights; float* conv1_bias;
		float* conv2_weights; float* conv2_bias;
		float* conv3_weights; float* conv3_bias;

		float* inc3a_conv1_weights; float* inc3a_conv1_bias;
		float* inc3a_conv3_1_weights; float* inc3a_conv3_1_bias;
		float* inc3a_conv3_2_weights; float* inc3a_conv3_2_bias;
		float* inc3a_conv5_1_weights; float* inc3a_conv5_1_bias;
		float* inc3a_conv5_2_weights; float* inc3a_conv5_2_bias;
		float* inc3a_conv5_3_weights; float* inc3a_conv5_3_bias;

		float* inc3b_conv1_weights; float* inc3b_conv1_bias;
		float* inc3b_conv3_1_weights; float* inc3b_conv3_1_bias;
		float* inc3b_conv3_2_weights; float* inc3b_conv3_2_bias;
		float* inc3b_conv5_1_weights; float* inc3b_conv5_1_bias;
		float* inc3b_conv5_2_weights; float* inc3b_conv5_2_bias;
		float* inc3b_conv5_3_weights; float* inc3b_conv5_3_bias;

		float* inc3c_conv1_weights; float* inc3c_conv1_bias;
		float* inc3c_conv3_1_weights; float* inc3c_conv3_1_bias;
		float* inc3c_conv3_2_weights; float* inc3c_conv3_2_bias;
		float* inc3c_conv5_1_weights; float* inc3c_conv5_1_bias;
		float* inc3c_conv5_2_weights; float* inc3c_conv5_2_bias;
		float* inc3c_conv5_3_weights; float* inc3c_conv5_3_bias;

		float* inc3d_conv1_weights; float* inc3d_conv1_bias;
		float* inc3d_conv3_1_weights; float* inc3d_conv3_1_bias;
		float* inc3d_conv3_2_weights; float* inc3d_conv3_2_bias;
		float* inc3d_conv5_1_weights; float* inc3d_conv5_1_bias;
		float* inc3d_conv5_2_weights; float* inc3d_conv5_2_bias;
		float* inc3d_conv5_3_weights; float* inc3d_conv5_3_bias;

		float* inc3e_conv1_weights; float* inc3e_conv1_bias;
		float* inc3e_conv3_1_weights; float* inc3e_conv3_1_bias;
		float* inc3e_conv3_2_weights; float* inc3e_conv3_2_bias;
		float* inc3e_conv5_1_weights; float* inc3e_conv5_1_bias;
		float* inc3e_conv5_2_weights; float* inc3e_conv5_2_bias;
		float* inc3e_conv5_3_weights; float* inc3e_conv5_3_bias;
		//-----------------------------------------------------------------------//
		float* inc4a_conv1_weights; float* inc4a_conv1_bias;
		float* inc4a_conv3_1_weights; float* inc4a_conv3_1_bias;
		float* inc4a_conv3_2_weights; float* inc4a_conv3_2_bias;
		float* inc4a_conv5_1_weights; float* inc4a_conv5_1_bias;
		float* inc4a_conv5_2_weights; float* inc4a_conv5_2_bias;
		float* inc4a_conv5_3_weights; float* inc4a_conv5_3_bias;

		float* inc4b_conv1_weights; float* inc4b_conv1_bias;
		float* inc4b_conv3_1_weights; float* inc4b_conv3_1_bias;
		float* inc4b_conv3_2_weights; float* inc4b_conv3_2_bias;
		float* inc4b_conv5_1_weights; float* inc4b_conv5_1_bias;
		float* inc4b_conv5_2_weights; float* inc4b_conv5_2_bias;
		float* inc4b_conv5_3_weights; float* inc4b_conv5_3_bias;

		float* inc4c_conv1_weights; float* inc4c_conv1_bias;
		float* inc4c_conv3_1_weights; float* inc4c_conv3_1_bias;
		float* inc4c_conv3_2_weights; float* inc4c_conv3_2_bias;
		float* inc4c_conv5_1_weights; float* inc4c_conv5_1_bias;
		float* inc4c_conv5_2_weights; float* inc4c_conv5_2_bias;
		float* inc4c_conv5_3_weights; float* inc4c_conv5_3_bias;

		float* inc4d_conv1_weights; float* inc4d_conv1_bias;
		float* inc4d_conv3_1_weights; float* inc4d_conv3_1_bias;
		float* inc4d_conv3_2_weights; float* inc4d_conv3_2_bias;
		float* inc4d_conv5_1_weights; float* inc4d_conv5_1_bias;
		float* inc4d_conv5_2_weights; float* inc4d_conv5_2_bias;
		float* inc4d_conv5_3_weights; float* inc4d_conv5_3_bias;

		float* inc4e_conv1_weights; float* inc4e_conv1_bias;
		float* inc4e_conv3_1_weights; float* inc4e_conv3_1_bias;
		float* inc4e_conv3_2_weights; float* inc4e_conv3_2_bias;
		float* inc4e_conv5_1_weights; float* inc4e_conv5_1_bias;
		float* inc4e_conv5_2_weights; float* inc4e_conv5_2_bias;
		float* inc4e_conv5_3_weights; float* inc4e_conv5_3_bias;

		float* upsample_weights; /*float* upsample_bias;*/
		float* convf_weights; float* convf_bias;
		float* rpn_conv1_weights; float* rpn_conv1_bias;
		float* rpn_bbox_pred_weights; float* rpn_bbox_pred_bias;
		float* rpn_cls_score_weights; float* rpn_cls_score_bias;
		float* fc6_L_weights; float* fc6_L_bias;
		float* fc6_U_weights; float* fc6_U_bias;
		float* fc7_L_weights; float* fc7_L_bias;
		float* fc7_U_weights; float* fc7_U_bias;
		float* cls_score1_weights; float* cls_score1_bias;
		float* bbox_pred1_weights; float* bbox_pred1_bias;

		pva_detect();
		~pva_detect();

		void release();
		// PVANET检测函数，即PVANET搭建
		void object_detection(void *PvaHandles, unsigned char* imagdata, int height, int width, int widstep, int* num_keep, float*& rois_output_cpu, float *&dstData_cls_prob, float *&dstData_bbox_pred1);

		// 初始化PVANET的数据，即导入各层的weight、bias
		int initNetData(const char* pvamodel_path);


	};

	//SqueezeNet 检测
	class HY_Network_API SqueezeNet_detect{

	public:
		void * SqueezeHandls ;   //GPU句柄变量，必须定义此变量 

		//定义各层 weight、bias变量
		float* conv1_weights; float* conv1_bias;

		float* fire2_squeeze1x1_weights; float* fire2_expand1x1_weights; float* fire2_expand3x3_weights;
		float* fire3_squeeze1x1_weights; float* fire3_expand1x1_weights; float* fire3_expand3x3_weights;

		float* fire2_squeeze1x1_bias; float* fire2_expand1x1_bias; float* fire2_expand3x3_bias;
		float* fire3_squeeze1x1_bias; float* fire3_expand1x1_bias; float* fire3_expand3x3_bias;
		
		float* rpn_output_weights; float* rpn_output_bias;
		float* rpn_bbox_pred_weights; float* rpn_bbox_pred_bias;
		float* rpn_cls_score_weights; float* rpn_cls_score_bias;
		
		float* fc6_32_weights; float* fc6_32_bias;
		float* fc7_32_weights; float* fc7_32_bias;
		float* cls_score1_weights; float* cls_score1_bias;
		float* bbox_pred1_weights; float* bbox_pred1_bias;

		SqueezeNet_detect();
		~SqueezeNet_detect();

		void release();
		// SqueezeNet_insu检测函数，即SqueezeNet检测模型搭建
		void object_detection(void *SqueezeHandles, unsigned char* imagdata, int height, int width, int widstep, int* num_keep, float*& rois_output_cpu, float *&dstData_cls_prob, float *&dstData_bbox_pred1);

		// 初始化SqueezeNet检测模型的数据，即导入各层的weight、bias
		int initNetData(const char* squeezeNetmodel_path);


	};
	
	//yolo 检测
	class HY_Network_API YoloNet_detect{
		
	public:		
		YoloNet_detect();
		~YoloNet_detect();
		// 初始化YoloNet的数据，即导入各层的weight、bias
		void initNetData(char *netfilename, char *weightfilename);
		void initNetData_adapt(char *netfilename, char *weightfilename, int w, int h);

		void object_detection(unsigned char* imagdata, int height, int width, int channels, int widstep, float thresh,int *numDST, float **boxout);
		void ReleaseNetwork();
	};

	//SqueezeNet 分类
	class HY_Network_API SqueezeNet_classify{

	public:
		void * SqueezeHandls ;   //GPU句柄变量，必须定义此变量 

		//定义各层 weight、bias变量
		
		float* conv1_weights; float* conv1_bias;
		float* fire2_squeeze1x1_weights; float* fire2_expand1x1_weights; float* fire2_expand3x3_weights;
		float* fire3_squeeze1x1_weights; float* fire3_expand1x1_weights; float* fire3_expand3x3_weights;
		float* fire4_squeeze1x1_weights; float* fire4_expand1x1_weights; float* fire4_expand3x3_weights;
		float* fire5_squeeze1x1_weights; float* fire5_expand1x1_weights; float* fire5_expand3x3_weights;
		float* fire6_squeeze1x1_weights; float* fire6_expand1x1_weights; float* fire6_expand3x3_weights;
		float* fire7_squeeze1x1_weights; float* fire7_expand1x1_weights; float* fire7_expand3x3_weights;
		float* fire8_squeeze1x1_weights; float* fire8_expand1x1_weights; float* fire8_expand3x3_weights;
		float* fire9_squeeze1x1_weights; float* fire9_expand1x1_weights; float* fire9_expand3x3_weights;

		float* fire2_squeeze1x1_bias; float* fire2_expand1x1_bias; float* fire2_expand3x3_bias;
		float* fire3_squeeze1x1_bias; float* fire3_expand1x1_bias; float* fire3_expand3x3_bias;
		float* fire4_squeeze1x1_bias; float* fire4_expand1x1_bias; float* fire4_expand3x3_bias;
		float* fire5_squeeze1x1_bias; float* fire5_expand1x1_bias; float* fire5_expand3x3_bias;
		float* fire6_squeeze1x1_bias; float* fire6_expand1x1_bias; float* fire6_expand3x3_bias;
		float* fire7_squeeze1x1_bias; float* fire7_expand1x1_bias; float* fire7_expand3x3_bias;
		float* fire8_squeeze1x1_bias; float* fire8_expand1x1_bias; float* fire8_expand3x3_bias;
		float* fire9_squeeze1x1_bias; float* fire9_expand1x1_bias; float* fire9_expand3x3_bias;

		float* conv10_weights; float* conv10_bias;



		SqueezeNet_classify();
		~SqueezeNet_classify();

		void release();
		// SqueezeNet分类函数，即SqueezeNet分类模型搭建
		void object_classification(void *SqueezeHandles, unsigned char* imagdata, int height, int width, int widstep, int* label);

		// 初始化SqueezeNet分类模型的数据，即导入各层的weight、bias
		int initNetData(const char* SqueezeNetmodel_path);


	};



#ifdef __cplusplus
}
#endif

#endif





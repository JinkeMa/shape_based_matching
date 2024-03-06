
#include<cuda_runtime.h>
//Ҫʹ�����ñ���blockDim�ȣ���Ҫ������ͷ�ļ�
#include<device_launch_parameters.h>

#include<random>

#include<opencv.hpp>

using namespace cv;


#define K 256

//ʹ��global_mem
__global__ void cu_mul(float* a,float* b,float* c)
{
	int idx = blockDim.x*blockIdx.x + threadIdx.x;
	c[idx] = a[idx] * b[idx];
}

//ʹ��shared_mem,һάgrid,һάblock
__global__ void cu_mul2(float* a, float* b, float* c, int num_elem)
{
	__shared__ float sa[256];
	__shared__ float sb[256];
	//__shared__ float sc[256];
	int idx = blockDim.x * blockIdx.x + threadIdx.x;

	if (idx < num_elem)
	{
		sa[threadIdx.x] = a[idx];
		sb[threadIdx.x] = b[idx];
	}
	
	__syncthreads();
	
	if (idx < num_elem)
	{
		c[idx] = sa[threadIdx.x] * sb[threadIdx.x];
	}
}

//transpose,ֻʹ��ȫ���ڴ�,1 elem/thread
__global__ void cu_transpose(float* a, float* b, int num_elem)
{
	//һά����
	//int idx = (gridDim.x * blockDim.x * blockIdx.y * blockDim.y + blockDim.x * blockDim.y * blockIdx.x + threadIdx.y * blockDim.x + threadIdx.x);
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if (x < num_elem && y < num_elem)
	{
		b[y * num_elem + x] = a[x * num_elem + y];
	}

}

int main()
{
	//maximum 1024 threads for every block
	const int num_row = 256 * 256;
	float a[num_row], b[num_row];
	memset(b, 1.0, num_row * sizeof(float));
	//�����ڴ�
	float* A, *B, *C;
	cudaMalloc(&A, num_row * sizeof(float));
	cudaMalloc(&B, num_row * sizeof(float));
	cudaMalloc(&C, num_row * sizeof(float));


	//��ȡ����
	cv::Mat img = cv::imread("C:/Users/user/Desktop/pic.png", 0);
	cv::resize(img, img, Size{ 256,256 });
	img.convertTo(img, CV_32F, 1.0/255,0);

	//�ƶ�����
	cudaMemcpy(A, img.data, num_row * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(B, b, num_row * sizeof(float), cudaMemcpyHostToDevice);
	
	//32 x 32 threads/block
	dim3 block{ 32,32 };
	cu_transpose <<<64, block >>> (A, B, 256);
	//����
	cudaMemcpy(b, B, num_row * sizeof(float), cudaMemcpyDeviceToHost);

	cv::Mat imgb{ Size{256,256},CV_32F };
	memcpy(imgb.data, b, num_row * sizeof(float));

	std::cout << sizeof(float);

	return 1;


}
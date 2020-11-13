#pragma once

#ifdef ENABLE_CUDA

#include <string>

typedef struct
{
    int orgW, orgH;
    int W, H;
    int passes, pushColorCount;
    float strengthColor, strengthGradient;
}ACCudaParamAnime4K09;

typedef struct
{
    int orgW, orgH;
    int HDNLevel;
}ACCudaParamACNet;

void cuRunKernelAnime4K09(const unsigned char* inputData, unsigned char* outputData, ACCudaParamAnime4K09* param);
void cuRunKernelACNet(const unsigned char* inputData, unsigned char* outputData, ACCudaParamACNet* param);

void cuInitCuda(const unsigned int id);
void cuReleaseCuda() noexcept;

int cuGetDeviceCount() noexcept;
std::string cuGetDeviceInfo(const unsigned int id);
std::string cuGetCudaInfo();
bool cuCheckDeviceSupport(const unsigned int id) noexcept;

#endif
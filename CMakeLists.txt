cmake_minimum_required(VERSION 3.0)
project(Anime4KCPP)

if(APPLE)
    # using brew's llvm (see https://stackoverflow.com/a/54715120/1410221)
    include_directories("/usr/local/include" "/usr/local/opt/llvm/include")
    link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${TOP_DIR}/install)

set(Qt_Version "5" CACHE STRING "5 or 6")
set(CUDA_Minimum_CC "35" CACHE PATH "Set the minimum cuda compute capability, older than 30")
set(CUDA_CC "75" CACHE PATH "Set the cuda compute capability, older than 30")

option(Build_GUI "Build GUI or not" OFF)
option(Build_CLI "Build CLI or not" ON)
option(Build_VapourSynth_Plugin "Build Anime4KCPP for VapourSynth plugin or not" OFF)
option(Build_AviSynthPlus_Plugin "Build Anime4KCPP for AviSynthPlus plugin or not" OFF)
option(Build_C_Wrapper "Build C wrapper of Anime4KCPP or not" OFF)
option(Build_C_Wrapper_With_Core "Build C wrapper and core in one file" ON)
option(Built_In_Kernel "Built-in kernel or not" ON)
option(Use_TBB "Use TBB for parallel processing" OFF)
option(Use_Eigen3 "Use Eigen3 for CPU processor acceleration" OFF)
option(Ryzen_Optimization "Accelerate the AMD Ryzen CPU" OFF)
option(Enable_CUDA "Enable CUDA module" OFF)
option(Enable_OpenCL "Enable OpenCL module" ON)
option(Enable_NCNN "Enable ncnn module" OFF)
option(Enable_Fast_Math "Enable fast math" OFF)
option(Enable_AVX2 "Enable on AVX2 acceleration, AVX2 and FMA are required" OFF)
option(Enable_OpenCV_DNN "Use OpenCV DNN module for CPU processing, not for DSFilter, models need to be loaded from the outside" OFF)
option(Enable_Video "Enable video processor module" ON)
option(Enable_Preview_GUI "Enable image preview in GUI window for core" ON)
option(Enable_Image_IO "Enable image IO for core" ON)
option(Disable_Parallel "Disable parallel processing" OFF)

if(APPLE)
    option(Use_Legacy_OpenCL_API "Compatible with old GPU which only supports OpenCL1.2" ON)
else()
    option(Use_Legacy_OpenCL_API "Compatible with old GPU which only supports OpenCL1.2" OFF)
endif()

include(${TOP_DIR}/cmake/Detection.cmake)

if(WIN32 AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    option(Use_OpenCV_with_MSVC_for_Clang "Use OpenCV build by MSVC for Clang" ON)
    option(Use_OpenCV_with_MINGW_for_Clang "Use OpenCV build by MinGW for Clang" OFF)
    if(Use_OpenCV_with_MSVC_for_Clang AND Use_OpenCV_with_MINGW_for_Clang)
        message(FATAL_ERROR "Use_OpenCV_with_MSVC_for_Clang and Use_OpenCV_with_MINGW_for_Clang cannot be true at same time")
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if(Enable_Fast_Math)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
    endif()
    if(Enable_AVX2 OR Ryzen_Optimization)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
    endif()
else()
    if(NOT Use_TBB)
        find_package(OpenMP)
        if (OpenMP_FOUND)
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
            add_definitions(-DUSE_OPENMP)
        endif()
    endif()
    if(UNIX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    endif()
    if(Enable_Fast_Math)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
    endif()
    if(Enable_AVX2 OR Ryzen_Optimization)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
    set (CMAKE_BUILD_TYPE "Release")
endif()

if(HAS_FILESYSTEM)
    option(Use_Boost_filesystem "Use filesystem lib from boost instead of STL" OFF)
else()
    message (
        STATUS 
        "Failed to complie has_filesystem.cpp, will use boost::filesystem instead of std::filesystem\n"
        ${HAS_FILESYSTEM_MSG}
    )
    option(Use_Boost_filesystem "Use filesystem lib from boost instead of STL" ON)
endif()

if(WIN32)
    set(OpenCL_Provider "Intel" CACHE STRING "one of Intel;Nvidia;AMD;Auto")
else()
    set(OpenCL_Provider "Auto" CACHE STRING "one of Intel;Nvidia;AMD;Auto")
endif()

if(WIN32)
    option(Build_DS_Filter "Build Anime4KCPP for DirectShow or not" OFF)
    set(DirectShow_SDK_PATH "DirectShow BaseClass SDK path" CACHE PATH "Where to look for DirectShow SDK")
endif()

set(VapourSynth_SDK_PATH "VapourSynth SDK path" CACHE PATH "Where to look for VapourSynth SDK")
set(AviSynthPlus_SDK_PATH "AviSynthPlus SDK path" CACHE PATH "Where to look for AviSynthPlus SDK")

if(Use_TBB)
    set(TBB_INCLUDE_PATH "TBB include path" CACHE PATH "Where to look for TBB include")
    set(TBB_LIB_PATH "TBB lib path" CACHE PATH "Where to look for TBB lib")
    add_definitions(-DUSE_TBB)
endif()

if (OpenCL_Provider MATCHES "Intel")
    if(DEFINED ENV{INTELOCLSDKROOT})
        set(CMAKE_PREFIX_PATH $ENV{INTELOCLSDKROOT} CACHE PATH "Intel OpenCL SDK")
    else()
        message (
            WARNING "Can't find Intel OpenCL SDK, set OpenCL_Provider to Auto\n"
        )
    endif()
elseif(OpenCL_Provider MATCHES "AMD")
    if(DEFINED ENV{OCL_ROOT})
        set(CMAKE_PREFIX_PATH $ENV{OCL_ROOT} CACHE PATH "AMD OpenCL SDK")
    else()
        message (
            WARNING "Can't find AMD OpenCL SDK, set OpenCL_Provider to Auto\n"
        )
    endif()
elseif(OpenCL_Provider MATCHES "Nvidia")
    if(DEFINED ENV{CUDA_PATH})
        set(CMAKE_PREFIX_PATH $ENV{CUDA_PATH} CACHE PATH "Nvidia OpenCL SDK")
    else()
        message (
            WARNING "Can't find Nvidia OpenCL SDK, set OpenCL_Provider to Auto\n"
        )
    endif()
elseif(NOT OpenCL_Provider MATCHES "Auto")
    message (
        WARNING "Unkonw value of OpenCL_Provider, set to Auto\n"
    )
endif()

message(STATUS  
"Building information:\n"
"   Build CLI ${Build_CLI}\n"
"   Build GUI ${Build_GUI}\n"
"   Build VapourSynth plugin ${Build_VapourSynth_Plugin}\n"
"   Build AviSynthPlus plugin ${Build_AviSynthPlus_Plugin}\n"
"   Build C wrapper ${Build_C_Wrapper}\n\n"
"   Build C wrapper with core ${Build_C_Wrapper_With_Core}\n"
"   Built-in kernel ${Built_In_Kernel}\n\n"
"   Use Boost filesystem ${Use_Boost_filesystem}\n"
"   Use TBB ${Use_TBB}\n"
"   Use Eigen3 ${Use_Eigen3}\n"
"   Use Legacy OpenCL API ${Use_Legacy_OpenCL_API}\n\n"
"   Ryzen Optimization ${Ryzen_Optimization}\n"
"   Enable OpenCL ${Enable_OpenCL}\n"
"   Enable CUDA ${Enable_CUDA}\n"
"   Enable NCNN ${Enable_NCNN}\n"
"   Enable AVX2 ${Enable_AVX2}\n"
"   Enable OpenCV DNN ${Enable_OpenCV_DNN}\n"
"   Enable video ${Enable_Video}\n"
"   Enable Preview GUI ${Enable_Preview_GUI}\n"
"   Enable image IO ${Enable_Image_IO}\n"
"   Disable Parallel ${Disable_Parallel}\n"
)
if(WIN32)
    message(STATUS
    "Extra building information for Windows:\n"
    "   Build DSFilter ${Build_DS_Filter}\n"
    )
endif()

if(Enable_CUDA)
    add_definitions(-DENABLE_CUDA)
endif()

if(Enable_OpenCL)
    add_definitions(-DENABLE_OPENCL)
endif()

if(Enable_NCNN)
    set(ncnn_DIR "ncnn library path" CACHE PATH "Directory that contains ncnnConfig.cmake")
endif()

if(Ryzen_Optimization)
    add_definitions(-DUSE_RYZEN)
endif()

if(Use_Eigen3)
    if(NOT EIGEN3_INCLUDE_DIR)
        find_package(Eigen3)
        if(NOT Eigen3_FOUND)
            set(EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}")
            if(NOT EIGEN3_INCLUDE_DIR)
                message (
                    WARNING "Unable to find eigen3 automatically, please set the header file location (EIGEN3_INCLUDE_DIR) manually\n"
                )
                set(EIGEN3_INCLUDE_DIR "" CACHE PATH "Directory that contains Eigen3 headers")
            endif()
        endif()
    endif()
    add_definitions(-DUSE_EIGEN3)
endif()

if(Disable_Parallel)
    add_definitions(-DDISABLE_PARALLEL)
endif()

message(STATUS  
"C++ compiler flags:\n"
    ${CMAKE_CXX_FLAGS}
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OS_64_Bit ON)
else()
    set(OS_64_Bit OFF)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

macro(SUBDIRLIST result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${curdir}/${child} AND EXISTS ${curdir}/${child}/CMakeLists.txt)
            list(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBDIR})
endforeach()

#!/bin/bash

rm -f CMakeCache.txt
rm -f *.cmake
rm -rf CMakeFiles

MY_PROJECT_SOURCE=../../../
MY_PROJECT_INSTALL=../install.dir

export CUDA_ROOT=/usr/local/cuda
export CPATH=$CPATH:$CUDA_ROOT/include
export LIBRARY_PATH=$LIBRARY_PATH:$CUDA_ROOT/lib64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CUDA_ROOT/lib64

cmake                                                                                       \
-D CMAKE_INSTALL_PREFIX=${MY_PROJECT_INSTALL}                                               \
-D CMAKE_CXX_COMPILER=clang++-6.0                                                           \
-D CMAKE_BUILD_TYPE=Release                                                                 \
-D CMAKE_VERBOSE_MAKEFILE:BOOL=ON                                                           \
-D DEVICE_BACKEND=NVIDIA                                                                    \
-D CUDA_COMMON_INCLUDE_DIR="/root/workspace/NVIDIA_CUDA-10.1_Samples/common/inc"            \
-D CMAKE_CUDA_FLAGS="-ccbin clang++-6.0 -m64 -Xcompiler -fopenmp -lineinfo --source-in-ptx -keep  -Xptxas -v -gencode=arch=compute_60,code=sm_60 -Xptxas -v -gencode=arch=compute_70,code=sm_70" \
${MY_PROJECT_SOURCE}


#-D CMAKE_CUDA_COMPILER="/package/install/cuda_10.0/bin/nvcc"                                \
#-D CMAKE_CUDA_FLAGS="-ccbin clang++ -m64 -Xcompiler -fopenmp -lineinfo --source-in-ptx -keep -Xptxas -v -gencode=arch=compute_61,code=sm_61" \
#-D CMAKE_CUDA_FLAGS="-ccbin clang++ -m64 -Xcompiler -fopenmp -lineinfo --source-in-ptx -keep -Xptxas -v -gencode=arch=compute_61,code=sm_61 -Xptxas -v -Xptxas -v -maxrregcount=128" \
#-D CMAKE_CUDA_FLAGS="-ccbin clang++ -m64 -Xcompiler -fopenmp -lineinfo --source-in-ptx -keep -Xptxas -v -gencode=arch=compute_61,code=sm_61 -Xptxas -v -gencode=arch=compute_70,code=sm_70" \
#-D CMAKE_CUDA_FLAGS="-ccbin clang++ -m64 -Xcompiler -fopenmp -lineinfo --source-in-ptx -keep -Xptxas -v -gencode=arch=compute_61,code=sm_61 -Xptxas -v -gencode=arch=compute_70,code=sm_70 -Xptxas -v -maxrregcount=128" \

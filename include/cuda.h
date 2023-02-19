#pragma once

#ifndef __GPU_IS_CUDA_INITED
  bool __GPU_IS_CUDA_INITED = 0;
  #define __GPU_IS_CUDA_INITED __GPU_IS_CUDA_INITED
#endif

#ifdef __GPU_USE_FAKE_CUDA
  #include _WITCH_PATH(ETC/fake_cuda/cuda.h)
#else
  #include <cuda.h>
  #include <cuda_runtime.h>
#endif

#ifndef __GPU_CUDA
  #define __GPU_CUDA
#endif

#ifndef __GPU_CUDA_CTXCURRENT
  thread_local CUcontext __GPU_CUDA_CTXCURRENT = nullptr;
  #define __GPU_CUDA_CTXCURRENT __GPU_CUDA_CTXCURRENT
#endif

#ifndef cuCtxCreate
  CUresult __GPU_CUDA_cuCtxCreate(CUcontext *ctx, unsigned int flags, CUdevice dev){
    CUresult r = cuCtxCreate(ctx, flags, dev);
    if(r == CUDA_SUCCESS){
      __GPU_CUDA_CTXCURRENT = *ctx;
    }
    return r;
  }
  #define cuCtxCreate __GPU_CUDA_cuCtxCreate
#endif

#ifndef cuCtxSetCurrent
  CUresult __GPU_CUDA_cuCtxSetCurrent(CUcontext ctx){
    if(__GPU_CUDA_CTXCURRENT == ctx){
      return CUDA_SUCCESS;
    }
    CUresult r = cuCtxSetCurrent(ctx);
    if(r == CUDA_SUCCESS){
      __GPU_CUDA_CTXCURRENT = ctx;
    }
    return r;
  }
  #define cuCtxSetCurrent __GPU_CUDA_cuCtxSetCurrent
#endif

#pragma once

#include _WITCH_PATH(include/cuda.h)

#ifdef __GPU_USE_FAKE_CUVID
  #include _WITCH_PATH(ETC/fake_cuda/cuvid.h)
#else
  #include <nvcuvid.h>
#endif

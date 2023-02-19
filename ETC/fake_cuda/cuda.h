/* taken from https://docs.nvidia.com/cuda/cuda-driver-api/group__CUDA__TYPES.html */
typedef enum{
  CUDA_SUCCESS = 0,
  CUDA_ERROR_INVALID_VALUE = 1,
  CUDA_ERROR_OUT_OF_MEMORY = 2,
  CUDA_ERROR_NOT_INITIALIZED = 3,
  CUDA_ERROR_DEINITIALIZED = 4,
  CUDA_ERROR_PROFILER_DISABLED = 5,
  CUDA_ERROR_PROFILER_NOT_INITIALIZED = 6,
  CUDA_ERROR_PROFILER_ALREADY_STARTED = 7,
  CUDA_ERROR_PROFILER_ALREADY_STOPPED = 8,
  CUDA_ERROR_STUB_LIBRARY = 34,
  CUDA_ERROR_DEVICE_UNAVAILABLE = 46,
  CUDA_ERROR_NO_DEVICE = 100,
  CUDA_ERROR_INVALID_DEVICE = 101,
  CUDA_ERROR_DEVICE_NOT_LICENSED = 102,
  CUDA_ERROR_INVALID_IMAGE = 200,
  CUDA_ERROR_INVALID_CONTEXT = 201,
  CUDA_ERROR_CONTEXT_ALREADY_CURRENT = 202,
  CUDA_ERROR_MAP_FAILED = 205,
  CUDA_ERROR_UNMAP_FAILED = 206,
  CUDA_ERROR_ARRAY_IS_MAPPED = 207,
  CUDA_ERROR_ALREADY_MAPPED = 208,
  CUDA_ERROR_NO_BINARY_FOR_GPU = 209,
  CUDA_ERROR_ALREADY_ACQUIRED = 210,
  CUDA_ERROR_NOT_MAPPED = 211,
  CUDA_ERROR_NOT_MAPPED_AS_ARRAY = 212,
  CUDA_ERROR_NOT_MAPPED_AS_POINTER = 213,
  CUDA_ERROR_ECC_UNCORRECTABLE = 214,
  CUDA_ERROR_UNSUPPORTED_LIMIT = 215,
  CUDA_ERROR_CONTEXT_ALREADY_IN_USE = 216,
  CUDA_ERROR_PEER_ACCESS_UNSUPPORTED = 217,
  CUDA_ERROR_INVALID_PTX = 218,
  CUDA_ERROR_INVALID_GRAPHICS_CONTEXT = 219,
  CUDA_ERROR_NVLINK_UNCORRECTABLE = 220,
  CUDA_ERROR_JIT_COMPILER_NOT_FOUND = 221,
  CUDA_ERROR_UNSUPPORTED_PTX_VERSION = 222,
  CUDA_ERROR_JIT_COMPILATION_DISABLED = 223,
  CUDA_ERROR_UNSUPPORTED_EXEC_AFFINITY = 224,
  CUDA_ERROR_INVALID_SOURCE = 300,
  CUDA_ERROR_FILE_NOT_FOUND = 301,
  CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND = 302,
  CUDA_ERROR_SHARED_OBJECT_INIT_FAILED = 303,
  CUDA_ERROR_OPERATING_SYSTEM = 304,
  CUDA_ERROR_INVALID_HANDLE = 400,
  CUDA_ERROR_ILLEGAL_STATE = 401,
  CUDA_ERROR_NOT_FOUND = 500,
  CUDA_ERROR_NOT_READY = 600,
  CUDA_ERROR_ILLEGAL_ADDRESS = 700,
  CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES = 701,
  CUDA_ERROR_LAUNCH_TIMEOUT = 702,
  CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING = 703,
  CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED = 704,
  CUDA_ERROR_PEER_ACCESS_NOT_ENABLED = 705,
  CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE = 708,
  CUDA_ERROR_CONTEXT_IS_DESTROYED = 709,
  CUDA_ERROR_ASSERT = 710,
  CUDA_ERROR_TOO_MANY_PEERS = 711,
  CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED = 712,
  CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED = 713,
  CUDA_ERROR_HARDWARE_STACK_ERROR = 714,
  CUDA_ERROR_ILLEGAL_INSTRUCTION = 715,
  CUDA_ERROR_MISALIGNED_ADDRESS = 716,
  CUDA_ERROR_INVALID_ADDRESS_SPACE = 717,
  CUDA_ERROR_INVALID_PC = 718,
  CUDA_ERROR_LAUNCH_FAILED = 719,
  CUDA_ERROR_COOPERATIVE_LAUNCH_TOO_LARGE = 720,
  CUDA_ERROR_NOT_PERMITTED = 800,
  CUDA_ERROR_NOT_SUPPORTED = 801,
  CUDA_ERROR_SYSTEM_NOT_READY = 802,
  CUDA_ERROR_SYSTEM_DRIVER_MISMATCH = 803,
  CUDA_ERROR_COMPAT_NOT_SUPPORTED_ON_DEVICE = 804,
  CUDA_ERROR_MPS_CONNECTION_FAILED = 805,
  CUDA_ERROR_MPS_RPC_FAILURE = 806,
  CUDA_ERROR_MPS_SERVER_NOT_READY = 807,
  CUDA_ERROR_MPS_MAX_CLIENTS_REACHED = 808,
  CUDA_ERROR_MPS_MAX_CONNECTIONS_REACHED = 809,
  CUDA_ERROR_MPS_CLIENT_TERMINATED = 810,
  CUDA_ERROR_CDP_NOT_SUPPORTED = 811,
  CUDA_ERROR_CDP_VERSION_MISMATCH = 812,
  CUDA_ERROR_STREAM_CAPTURE_UNSUPPORTED = 900,
  CUDA_ERROR_STREAM_CAPTURE_INVALIDATED = 901,
  CUDA_ERROR_STREAM_CAPTURE_MERGE = 902,
  CUDA_ERROR_STREAM_CAPTURE_UNMATCHED = 903,
  CUDA_ERROR_STREAM_CAPTURE_UNJOINED = 904,
  CUDA_ERROR_STREAM_CAPTURE_ISOLATION = 905,
  CUDA_ERROR_STREAM_CAPTURE_IMPLICIT = 906,
  CUDA_ERROR_CAPTURED_EVENT = 907,
  CUDA_ERROR_STREAM_CAPTURE_WRONG_THREAD = 908,
  CUDA_ERROR_TIMEOUT = 909,
  CUDA_ERROR_GRAPH_EXEC_UPDATE_FAILURE = 910,
  CUDA_ERROR_EXTERNAL_DEVICE = 911,
  CUDA_ERROR_INVALID_CLUSTER_SIZE = 912,
  CUDA_ERROR_UNKNOWN = 999
}CUresult;

typedef enum{
  cudaSuccess = 0,
  cudaErrorInvalidValue = 1,
  cudaErrorMemoryAllocation = 2,
  cudaErrorInitializationError = 3,
  cudaErrorCudartUnloading = 4,
  cudaErrorProfilerDisabled = 5,
  cudaErrorProfilerNotInitialized = 6,
  cudaErrorProfilerAlreadyStarted = 7,
  cudaErrorProfilerAlreadyStopped = 8,
  cudaErrorInvalidConfiguration = 9,
  cudaErrorInvalidPitchValue = 12,
  cudaErrorInvalidSymbol = 13,
  cudaErrorInvalidHostPointer = 16,
  cudaErrorInvalidDevicePointer = 17,
  cudaErrorInvalidTexture = 18,
  cudaErrorInvalidTextureBinding = 19,
  cudaErrorInvalidChannelDescriptor = 20,
  cudaErrorInvalidMemcpyDirection = 21,
  cudaErrorAddressOfConstant = 22,
  cudaErrorTextureFetchFailed = 23,
  cudaErrorTextureNotBound = 24,
  cudaErrorSynchronizationError = 25,
  cudaErrorInvalidFilterSetting = 26,
  cudaErrorInvalidNormSetting = 27,
  cudaErrorMixedDeviceExecution = 28,
  cudaErrorNotYetImplemented = 31,
  cudaErrorMemoryValueTooLarge = 32,
  cudaErrorStubLibrary = 34,
  cudaErrorInsufficientDriver = 35,
  cudaErrorCallRequiresNewerDriver = 36,
  cudaErrorInvalidSurface = 37,
  cudaErrorDuplicateVariableName = 43,
  cudaErrorDuplicateTextureName = 44,
  cudaErrorDuplicateSurfaceName = 45,
  cudaErrorDevicesUnavailable = 46,
  cudaErrorIncompatibleDriverContext = 49,
  cudaErrorMissingConfiguration = 52,
  cudaErrorPriorLaunchFailure = 53,
  cudaErrorLaunchMaxDepthExceeded = 65,
  cudaErrorLaunchFileScopedTex = 66,
  cudaErrorLaunchFileScopedSurf = 67,
  cudaErrorSyncDepthExceeded = 68,
  cudaErrorLaunchPendingCountExceeded = 69,
  cudaErrorInvalidDeviceFunction = 98,
  cudaErrorNoDevice = 100,
  cudaErrorInvalidDevice = 101,
  cudaErrorDeviceNotLicensed = 102,
  cudaErrorSoftwareValidityNotEstablished = 103,
  cudaErrorStartupFailure = 127,
  cudaErrorInvalidKernelImage = 200,
  cudaErrorDeviceUninitialized = 201,
  cudaErrorMapBufferObjectFailed = 205,
  cudaErrorUnmapBufferObjectFailed = 206,
  cudaErrorArrayIsMapped = 207,
  cudaErrorAlreadyMapped = 208,
  cudaErrorNoKernelImageForDevice = 209,
  cudaErrorAlreadyAcquired = 210,
  cudaErrorNotMapped = 211,
  cudaErrorNotMappedAsArray = 212,
  cudaErrorNotMappedAsPointer = 213,
  cudaErrorECCUncorrectable = 214,
  cudaErrorUnsupportedLimit = 215,
  cudaErrorDeviceAlreadyInUse = 216,
  cudaErrorPeerAccessUnsupported = 217,
  cudaErrorInvalidPtx = 218,
  cudaErrorInvalidGraphicsContext = 219,
  cudaErrorNvlinkUncorrectable = 220,
  cudaErrorJitCompilerNotFound = 221,
  cudaErrorUnsupportedPtxVersion = 222,
  cudaErrorJitCompilationDisabled = 223,
  cudaErrorUnsupportedExecAffinity = 224,
  cudaErrorInvalidSource = 300,
  cudaErrorFileNotFound = 301,
  cudaErrorSharedObjectSymbolNotFound = 302,
  cudaErrorSharedObjectInitFailed = 303,
  cudaErrorOperatingSystem = 304,
  cudaErrorInvalidResourceHandle = 400,
  cudaErrorIllegalState = 401,
  cudaErrorSymbolNotFound = 500,
  cudaErrorNotReady = 600,
  cudaErrorIllegalAddress = 700,
  cudaErrorLaunchOutOfResources = 701,
  cudaErrorLaunchTimeout = 702,
  cudaErrorLaunchIncompatibleTexturing = 703,
  cudaErrorPeerAccessAlreadyEnabled = 704,
  cudaErrorPeerAccessNotEnabled = 705,
  cudaErrorSetOnActiveProcess = 708,
  cudaErrorContextIsDestroyed = 709,
  cudaErrorAssert = 710,
  cudaErrorTooManyPeers = 711,
  cudaErrorHostMemoryAlreadyRegistered = 712,
  cudaErrorHostMemoryNotRegistered = 713,
  cudaErrorHardwareStackError = 714,
  cudaErrorIllegalInstruction = 715,
  cudaErrorMisalignedAddress = 716,
  cudaErrorInvalidAddressSpace = 717,
  cudaErrorInvalidPc = 718,
  cudaErrorLaunchFailure = 719,
  cudaErrorCooperativeLaunchTooLarge = 720,
  cudaErrorNotPermitted = 800,
  cudaErrorNotSupported = 801,
  cudaErrorSystemNotReady = 802,
  cudaErrorSystemDriverMismatch = 803,
  cudaErrorCompatNotSupportedOnDevice = 804,
  cudaErrorMpsConnectionFailed = 805,
  cudaErrorMpsRpcFailure = 806,
  cudaErrorMpsServerNotReady = 807,
  cudaErrorMpsMaxClientsReached = 808,
  cudaErrorMpsMaxConnectionsReached = 809,
  cudaErrorMpsClientTerminated = 810,
  cudaErrorCdpNotSupported = 811,
  cudaErrorCdpVersionMismatch = 812,
  cudaErrorStreamCaptureUnsupported = 900,
  cudaErrorStreamCaptureInvalidated = 901,
  cudaErrorStreamCaptureMerge = 902,
  cudaErrorStreamCaptureUnmatched = 903,
  cudaErrorStreamCaptureUnjoined = 904,
  cudaErrorStreamCaptureIsolation = 905,
  cudaErrorStreamCaptureImplicit = 906,
  cudaErrorCapturedEvent = 907,
  cudaErrorStreamCaptureWrongThread = 908,
  cudaErrorTimeout = 909,
  cudaErrorGraphExecUpdateFailure = 910,
  cudaErrorExternalDevice = 911,
  cudaErrorInvalidClusterSize = 912,
  cudaErrorUnknown = 999,
  cudaErrorApiFailureBase = 10000
}cudaError;
typedef cudaError cudaError_t;

typedef enum{
  cudaMemcpyHostToHost = 0,
  cudaMemcpyHostToDevice = 1,
  cudaMemcpyDeviceToHost = 2,
  cudaMemcpyDeviceToDevice = 3,
  cudaMemcpyDefault = 4
}cudaMemcpyKind;

CUresult cuInit(unsigned int Flags){
  return CUDA_SUCCESS;
}

CUresult cuDeviceGetCount(int *count){
  *count = 1;
  return CUDA_SUCCESS;
}

typedef struct{
  uint8_t filler;
}CUdevice;

CUresult cuDeviceGet(CUdevice *device, int ordinal){
  if(ordinal != 0){
    return CUDA_ERROR_INVALID_DEVICE;
  }
  return CUDA_SUCCESS;
}

typedef struct{
  uint8_t filler;
}CUcontext_t;

typedef CUcontext_t *CUcontext;

CUresult cuCtxCreate(CUcontext *pctx, unsigned int flags, CUdevice dev){
  return CUDA_SUCCESS;
}

CUresult cuCtxDestroy(CUcontext ctx){
  return CUDA_SUCCESS;
}

CUresult cuCtxSetCurrent(CUcontext ctx){
  return CUDA_SUCCESS;
}

typedef struct{
  uint8_t filler;
}cudaArray;
typedef cudaArray *cudaArray_t;

cudaError_t cudaMemcpy2DToArray(cudaArray_t dst, size_t wOffset, size_t hOffset, const void *src, size_t spitch, size_t width, size_t height, cudaMemcpyKind kind){
  return cudaSuccess;
}

typedef struct CUstream_st *CUstream;

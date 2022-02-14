//
// Copyright (c) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once

#include <cstddef>

enum oroApi
{
    ORO_API_AUTOMATIC = 1<<0,
    ORO_API_HIP = 1<<1,
    ORO_API_CUDA = 1<<2,
};

enum oroError
{
	oroSuccess = 0,
	oroErrorUnknown = 999,
};

enum oroMemcpyKind
{
    oroMemcpyHostToHost = 0,
    oroMemcpyHostToDevice = 1,
    oroMemcpyDeviceToHost = 2,
    oroMemcpyDeviceToDevice = 3,
    oroMemcpyDefault = 4
};

typedef unsigned int oroU32;
typedef unsigned long long oroDeviceptr;

#ifdef _WIN32
#  define OROAPI __stdcall
#  define ORO_CB __stdcall
#else
#  define OROAPI
#  define ORO_CB
#endif

typedef int oroDevice;
typedef struct ioroCtx_t* oroCtx;
typedef struct ioroModule_t* oroModule;
typedef struct ioroModuleSymbol_t* oroFunction;
typedef struct ioroArray* oroArray;
typedef struct oroMipmaoroedArray_st* oroMipmaoroedArray;
typedef struct ioroEvent_t* oroEvent;
typedef struct ioroStream_t* oroStream;
typedef struct ioroPointerAttribute_t* oroPointerAttribute;
typedef unsigned long long oroTextureObject;
typedef void* oroExternalMemory_t;


typedef struct _orortcProgram* orortcProgram;

enum orortcResult
{
	ORORTC_SUCCESS = 0,
	ORORTC_ERROR_INTERNAL_ERROR = 11,
};


typedef enum oroDeviceAttribute {
  oroDeviceAttributeCudaCompatibleBegin = 0,
  oroDeviceAttributeEccEnabled = oroDeviceAttributeCudaCompatibleBegin, ///< Whether ECC suoroort is enabled.
  oroDeviceAttributeAccessPolicyMaxWindowSize,        ///< Cuda only. The maximum size of the window policy in bytes.
  oroDeviceAttributeAsyncEngineCount,                 ///< Cuda only. Asynchronous engines number.
  oroDeviceAttributeCanMapHostMemory,                 ///< Whether host memory can be maoroed into device address space
  oroDeviceAttributeCanUseHostPointerForRegisteredMem,///< Cuda only. Device can access host registered memory
                                                      ///< at the same virtual address as the CPU
  oroDeviceAttributeClockRate,                        ///< Peak clock frequency in kilohertz.
  oroDeviceAttributeComputeMode,                      ///< Compute mode that device is currently in.
  oroDeviceAttributeComputePreemptionSuoroorted,       ///< Cuda only. Device suoroorts Compute Preemption.
  oroDeviceAttributeConcurrentKernels,                ///< Device can possibly execute multiple kernels concurrently.
  oroDeviceAttributeConcurrentManagedAccess,          ///< Device can coherently access managed memory concurrently with the CPU
  oroDeviceAttributeCooperativeLaunch,                ///< Suoroort cooperative launch
  oroDeviceAttributeCooperativeMultiDeviceLaunch,     ///< Suoroort cooperative launch on multiple devices
  oroDeviceAttributeDeviceOverlap,                    ///< Cuda only. Device can concurrently copy memory and execute a kernel.
                                                      ///< Deprecated. Use instead asyncEngineCount.
  oroDeviceAttributeDirectManagedMemAccessFromHost,   ///< Host can directly access managed memory on
                                                      ///< the device without migration
  oroDeviceAttributeGlobalL1CacheSuoroorted,           ///< Cuda only. Device suoroorts caching globals in L1
  oroDeviceAttributeHostNativeAtomicSuoroorted,        ///< Cuda only. Link between the device and the host suoroorts native atomic operations
  oroDeviceAttributeIntegrated,                       ///< Device is integrated GPU
  oroDeviceAttributeIsMultiGpuBoard,                  ///< Multiple GPU devices.
  oroDeviceAttributeKernelExecTimeout,                ///< Run time limit for kernels executed on the device
  oroDeviceAttributeL2CacheSize,                      ///< Size of L2 cache in bytes. 0 if the device doesn't have L2 cache.
  oroDeviceAttributeLocalL1CacheSuoroorted,            ///< caching locals in L1 is suoroorted
  oroDeviceAttributeLuid,                             ///< Cuda only. 8-byte locally unique identifier in 8 bytes. Undefined on TCC and non-Windows platforms
  oroDeviceAttributeLuidDeviceNodeMask,               ///< Cuda only. Luid device node mask. Undefined on TCC and non-Windows platforms
  oroDeviceAttributeComputeCapabilityMajor,           ///< Major compute capability version number.
  oroDeviceAttributeManagedMemory,                    ///< Device suoroorts allocating managed memory on this system
  oroDeviceAttributeMaxBlocksPerMultiProcessor,       ///< Cuda only. Max block size per multiprocessor
  oroDeviceAttributeMaxBlockDimX,                     ///< Max block size in width.
  oroDeviceAttributeMaxBlockDimY,                     ///< Max block size in height.
  oroDeviceAttributeMaxBlockDimZ,                     ///< Max block size in depth.
  oroDeviceAttributeMaxGridDimX,                      ///< Max grid size  in width.
  oroDeviceAttributeMaxGridDimY,                      ///< Max grid size  in height.
  oroDeviceAttributeMaxGridDimZ,                      ///< Max grid size  in depth.
  oroDeviceAttributeMaxSurface1D,                     ///< Maximum size of 1D surface.
  oroDeviceAttributeMaxSurface1DLayered,              ///< Cuda only. Maximum dimensions of 1D layered surface.
  oroDeviceAttributeMaxSurface2D,                     ///< Maximum dimension (width, height) of 2D surface.
  oroDeviceAttributeMaxSurface2DLayered,              ///< Cuda only. Maximum dimensions of 2D layered surface.
  oroDeviceAttributeMaxSurface3D,                     ///< Maximum dimension (width, height, depth) of 3D surface.
  oroDeviceAttributeMaxSurfaceCubemap,                ///< Cuda only. Maximum dimensions of Cubemap surface.
  oroDeviceAttributeMaxSurfaceCubemapLayered,         ///< Cuda only. Maximum dimension of Cubemap layered surface.
  oroDeviceAttributeMaxTexture1DWidth,                ///< Maximum size of 1D texture.
  oroDeviceAttributeMaxTexture1DLayered,              ///< Cuda only. Maximum dimensions of 1D layered texture.
  oroDeviceAttributeMaxTexture1DLinear,               ///< Maximum number of elements allocatable in a 1D linear texture.
                                                      ///< Use cudaDeviceGetTexture1DLinearMaxWidth() instead on Cuda.
  oroDeviceAttributeMaxTexture1DMipmap,               ///< Cuda only. Maximum size of 1D mipmaoroed texture.
  oroDeviceAttributeMaxTexture2DWidth,                ///< Maximum dimension width of 2D texture.
  oroDeviceAttributeMaxTexture2DHeight,               ///< Maximum dimension hight of 2D texture.
  oroDeviceAttributeMaxTexture2DGather,               ///< Cuda only. Maximum dimensions of 2D texture if gather operations  performed.
  oroDeviceAttributeMaxTexture2DLayered,              ///< Cuda only. Maximum dimensions of 2D layered texture.
  oroDeviceAttributeMaxTexture2DLinear,               ///< Cuda only. Maximum dimensions (width, height, pitch) of 2D textures bound to pitched memory.
  oroDeviceAttributeMaxTexture2DMipmap,               ///< Cuda only. Maximum dimensions of 2D mipmaoroed texture.
  oroDeviceAttributeMaxTexture3DWidth,                ///< Maximum dimension width of 3D texture.
  oroDeviceAttributeMaxTexture3DHeight,               ///< Maximum dimension height of 3D texture.
  oroDeviceAttributeMaxTexture3DDepth,                ///< Maximum dimension depth of 3D texture.
  oroDeviceAttributeMaxTexture3DAlt,                  ///< Cuda only. Maximum dimensions of alternate 3D texture.
  oroDeviceAttributeMaxTextureCubemap,                ///< Cuda only. Maximum dimensions of Cubemap texture
  oroDeviceAttributeMaxTextureCubemapLayered,         ///< Cuda only. Maximum dimensions of Cubemap layered texture.
  oroDeviceAttributeMaxThreadsDim,                    ///< Maximum dimension of a block
  oroDeviceAttributeMaxThreadsPerBlock,               ///< Maximum number of threads per block.
  oroDeviceAttributeMaxThreadsPerMultiProcessor,      ///< Maximum resident threads per multiprocessor.
  oroDeviceAttributeMaxPitch,                         ///< Maximum pitch in bytes allowed by memory copies
  oroDeviceAttributeMemoryBusWidth,                   ///< Global memory bus width in bits.
  oroDeviceAttributeMemoryClockRate,                  ///< Peak memory clock frequency in kilohertz.
  oroDeviceAttributeComputeCapabilityMinor,           ///< Minor compute capability version number.
  oroDeviceAttributeMultiGpuBoardGroupID,             ///< Cuda only. Unique ID of device group on the same multi-GPU board
  oroDeviceAttributeMultiprocessorCount,              ///< Number of multiprocessors on the device.
  oroDeviceAttributeName,                             ///< Device name.
  oroDeviceAttributePageableMemoryAccess,             ///< Device suoroorts coherently accessing pageable memory
                                                      ///< without calling hipHostRegister on it
  oroDeviceAttributePageableMemoryAccessUsesHostPageTables, ///< Device accesses pageable memory via the host's page tables
  oroDeviceAttributePciBusId,                         ///< PCI Bus ID.
  oroDeviceAttributePciDeviceId,                      ///< PCI Device ID.
  oroDeviceAttributePciDomainID,                      ///< PCI Domain ID.
  oroDeviceAttributePersistingL2CacheMaxSize,         ///< Cuda11 only. Maximum l2 persisting lines capacity in bytes
  oroDeviceAttributeMaxRegistersPerBlock,             ///< 32-bit registers available to a thread block. This number is shared
                                                      ///< by all thread blocks simultaneously resident on a multiprocessor.
  oroDeviceAttributeMaxRegistersPerMultiprocessor,    ///< 32-bit registers available per block.
  oroDeviceAttributeReservedSharedMemPerBlock,        ///< Cuda11 only. Shared memory reserved by CUDA driver per block.
  oroDeviceAttributeMaxSharedMemoryPerBlock,          ///< Maximum shared memory available per block in bytes.
  oroDeviceAttributeSharedMemPerBlockOptin,           ///< Cuda only. Maximum shared memory per block usable by special opt in.
  oroDeviceAttributeSharedMemPerMultiprocessor,       ///< Cuda only. Shared memory available per multiprocessor.
  oroDeviceAttributeSingleToDoublePrecisionPerfRatio, ///< Cuda only. Performance ratio of single precision to double precision.
  oroDeviceAttributeStreamPrioritiesSuoroorted,        ///< Cuda only. Whether to suoroort stream priorities.
  oroDeviceAttributeSurfaceAlignment,                 ///< Cuda only. Alignment requirement for surfaces
  oroDeviceAttributeTccDriver,                        ///< Cuda only. Whether device is a Tesla device using TCC driver
  oroDeviceAttributeTextureAlignment,                 ///< Alignment requirement for textures
  oroDeviceAttributeTexturePitchAlignment,            ///< Pitch alignment requirement for 2D texture references bound to pitched memory;
  oroDeviceAttributeTotalConstantMemory,              ///< Constant memory size in bytes.
  oroDeviceAttributeTotalGlobalMem,                   ///< Global memory available on devicice.
  oroDeviceAttributeUnifiedAddressing,                ///< Cuda only. An unified address space shared with the host.
  oroDeviceAttributeUuid,                             ///< Cuda only. Unique ID in 16 byte.
  oroDeviceAttributeWarpSize,                         ///< Warp size in threads.
  oroDeviceAttributeCudaCompatibleEnd = 9999,
  oroDeviceAttributeAmdSpecificBegin = 10000,
  oroDeviceAttributeClockInstructionRate = oroDeviceAttributeAmdSpecificBegin,  ///< Frequency in khz of the timer used by the device-side "clock*"
  oroDeviceAttributeArch,                                     ///< Device architecture
  oroDeviceAttributeMaxSharedMemoryPerMultiprocessor,         ///< Maximum Shared Memory PerMultiprocessor.
  oroDeviceAttributeGcnArch,                                  ///< Device gcn architecture
  oroDeviceAttributeGcnArchName,                              ///< Device gcnArch name in 256 bytes
  oroDeviceAttributeHdpMemFlushCntl,                          ///< Address of the HDP_MEM_COHERENCY_FLUSH_CNTL register
  oroDeviceAttributeHdpRegFlushCntl,                          ///< Address of the HDP_REG_COHERENCY_FLUSH_CNTL register
  oroDeviceAttributeCooperativeMultiDeviceUnmatchedFunc,      ///< Suoroorts cooperative launch on multiple
                                                              ///< devices with unmatched functions
  oroDeviceAttributeCooperativeMultiDeviceUnmatchedGridDim,   ///< Suoroorts cooperative launch on multiple
                                                              ///< devices with unmatched grid dimensions
  oroDeviceAttributeCooperativeMultiDeviceUnmatchedBlockDim,  ///< Suoroorts cooperative launch on multiple
                                                              ///< devices with unmatched block dimensions
  oroDeviceAttributeCooperativeMultiDeviceUnmatchedSharedMem, ///< Suoroorts cooperative launch on multiple
                                                              ///< devices with unmatched shared memories
  oroDeviceAttributeIsLargeBar,                               ///< Whether it is LargeBar
  oroDeviceAttributeAsicRevision,                             ///< Revision of the GPU in this device
  oroDeviceAttributeCanUseStreamWaitValue,                    ///< '1' if Device suoroorts hipStreamWaitValue32() and
                                                              ///< hipStreamWaitValue64() , '0' otherwise.
  oroDeviceAttributeAmdSpecificEnd = 19999,
  oroDeviceAttributeVendorSpecificBegin = 20000,
  // Extended attributes for vendors
} oroDeviceAttribute;

typedef struct PPdevprop_st {
    int maxThreadsPerBlock;
    int maxThreadsDim[3];
    int maxGridSize[3];
    int sharedMemPerBlock;
    int totalConstantMemory;
    int SIMDWidth;
    int memPitch;
    int regsPerBlock;
    int clockRate;
    int textureAlign;
} PPdevprop;

typedef struct {
    // 32-bit Atomics
    unsigned hasGlobalInt32Atomics : 1;     ///< 32-bit integer atomics for global memory.
    unsigned hasGlobalFloatAtomicExch : 1;  ///< 32-bit float atomic exch for global memory.
    unsigned hasSharedInt32Atomics : 1;     ///< 32-bit integer atomics for shared memory.
    unsigned hasSharedFloatAtomicExch : 1;  ///< 32-bit float atomic exch for shared memory.
    unsigned hasFloatAtomicAdd : 1;  ///< 32-bit float atomic add in global and shared memory.

                                     // 64-bit Atomics
    unsigned hasGlobalInt64Atomics : 1;  ///< 64-bit integer atomics for global memory.
    unsigned hasSharedInt64Atomics : 1;  ///< 64-bit integer atomics for shared memory.

                                         // Doubles
    unsigned hasDoubles : 1;  ///< Double-precision floating point.

                              // Warp cross-lane operations
    unsigned hasWarpVote : 1;     ///< Warp vote instructions (__any, __all).
    unsigned hasWarpBallot : 1;   ///< Warp ballot instructions (__ballot).
    unsigned hasWarpShuffle : 1;  ///< Warp shuffle operations. (__shfl_*).
    unsigned hasFunnelShift : 1;  ///< Funnel two words into one with shift&mask caps.

                                  // Sync
    unsigned hasThreadFenceSystem : 1;  ///< __threadfence_system.
    unsigned hasSyncThreadsExt : 1;     ///< __syncthreads_count, syncthreads_and, syncthreads_or.

                                        // Misc
    unsigned hasSurfaceFuncs : 1;        ///< Surface functions.
    unsigned has3dGrid : 1;              ///< Grid and group dims are 3D (rather than 2D).
    unsigned hasDynamicParallelism : 1;  ///< Dynamic parallelism.
} oroDeviceArch;

typedef struct oroDeviceProp {
    char name[256];            ///< Device name.
    size_t totalGlobalMem;     ///< Size of global memory region (in bytes).
    size_t sharedMemPerBlock;  ///< Size of shared memory region (in bytes).
    int regsPerBlock;          ///< Registers per block.
    int warpSize;              ///< Warp size.
    int maxThreadsPerBlock;    ///< Max work items per work group or workgroup max size.
    int maxThreadsDim[3];      ///< Max number of threads in each dimension (XYZ) of a block.
    int maxGridSize[3];        ///< Max grid dimensions (XYZ).
    int clockRate;             ///< Max clock frequency of the multiProcessors in khz.
    int memoryClockRate;       ///< Max global memory clock frequency in khz.
    int memoryBusWidth;        ///< Global memory bus width in bits.
    size_t totalConstMem;      ///< Size of shared memory region (in bytes).
    int major;  ///< Major compute capability.  On HCC, this is an aororoximation and features may
                ///< differ from CUDA CC.  See the arch feature flags for portable ways to query
                ///< feature caps.
    int minor;  ///< Minor compute capability.  On HCC, this is an aororoximation and features may
                ///< differ from CUDA CC.  See the arch feature flags for portable ways to query
                ///< feature caps.
    int multiProcessorCount;          ///< Number of multi-processors (compute units).
    int l2CacheSize;                  ///< L2 cache size.
    int maxThreadsPerMultiProcessor;  ///< Maximum resident threads per multi-processor.
    int computeMode;                  ///< Compute mode.
    int clockInstructionRate;  ///< Frequency in khz of the timer used by the device-side "clock*"
                               ///< instructions.  New for HIP.
    oroDeviceArch arch;      ///< Architectural feature flags.  New for HIP.
    int concurrentKernels;     ///< Device can possibly execute multiple kernels concurrently.
    int pciDomainID;           ///< PCI Domain ID
    int pciBusID;              ///< PCI Bus ID.
    int pciDeviceID;           ///< PCI Device ID.
    size_t maxSharedMemoryPerMultiProcessor;  ///< Maximum Shared Memory Per Multiprocessor.
    int isMultiGpuBoard;                      ///< 1 if device is on a multi-GPU board, 0 if not.
    int canMapHostMemory;                     ///< Check whether HIP can map host memory
    int gcnArch;                              ///< DEPRECATED: use gcnArchName instead
    char gcnArchName[256];                    ///< AMD GCN Arch Name.
    int integrated;            ///< APU vs dGPU
    int cooperativeLaunch;            ///< HIP device suoroorts cooperative launch
    int cooperativeMultiDeviceLaunch; ///< HIP device suoroorts cooperative launch on multiple devices
    int maxTexture1DLinear;    ///< Maximum size for 1D textures bound to linear memory
    int maxTexture1D;          ///< Maximum number of elements in 1D images
    int maxTexture2D[2];       ///< Maximum dimensions (width, height) of 2D images, in image elements
    int maxTexture3D[3];       ///< Maximum dimensions (width, height, depth) of 3D images, in image elements
    unsigned int* hdpMemFlushCntl;      ///< Addres of HDP_MEM_COHERENCY_FLUSH_CNTL register
    unsigned int* hdpRegFlushCntl;      ///< Addres of HDP_REG_COHERENCY_FLUSH_CNTL register
    size_t memPitch;                 ///<Maximum pitch in bytes allowed by memory copies
    size_t textureAlignment;         ///<Alignment requirement for textures
    size_t texturePitchAlignment;    ///<Pitch alignment requirement for texture references bound to pitched memory
    int kernelExecTimeoutEnabled;    ///<Run time limit for kernels executed on the device
    int ECCEnabled;                  ///<Device has ECC suoroort enabled
    int tccDriver;                   ///< 1:If device is Tesla device using TCC driver, else 0
    int cooperativeMultiDeviceUnmatchedFunc;        ///< HIP device suoroorts cooperative launch on multiple
                                                    ///devices with unmatched functions
    int cooperativeMultiDeviceUnmatchedGridDim;     ///< HIP device suoroorts cooperative launch on multiple
                                                    ///devices with unmatched grid dimensions
    int cooperativeMultiDeviceUnmatchedBlockDim;    ///< HIP device suoroorts cooperative launch on multiple
                                                    ///devices with unmatched block dimensions
    int cooperativeMultiDeviceUnmatchedSharedMem;   ///< HIP device suoroorts cooperative launch on multiple
                                                    ///devices with unmatched shared memories
    int isLargeBar;                  ///< 1: if it is a large PCI bar device, else 0
    int asicRevision;                ///< Revision of the GPU in this device
    int managedMemory;               ///< Device suoroorts allocating managed memory on this system
    int directManagedMemAccessFromHost; ///< Host can directly access managed memory on the device without migration
    int concurrentManagedAccess;     ///< Device can coherently access managed memory concurrently with the CPU
    int pageableMemoryAccess;        ///< Device suoroorts coherently accessing pageable memory
                                     ///< without calling hipHostRegister on it
    int pageableMemoryAccessUsesHostPageTables; ///< Device accesses pageable memory via the host's page tables
} oroDeviceProp;

typedef enum PPpointer_attribute_enum {
    ORO_POINTER_ATTRIBUTE_CONTEXT = 1,
    ORO_POINTER_ATTRIBUTE_MEMORY_TYPE = 2,
    ORO_POINTER_ATTRIBUTE_DEVICE_POINTER = 3,
    ORO_POINTER_ATTRIBUTE_HOST_POINTER = 4,
    ORO_POINTER_ATTRIBUTE_SYNC_MEMOPS = 6,
    ORO_POINTER_ATTRIBUTE_BUFFER_ID = 7,
    ORO_POINTER_ATTRIBUTE_IS_MANAGED = 8,
    ORO_POINTER_ATTRIBUTE_DEVICE_ORDINAL = 9,
} PPpointer_attribute;

typedef enum oroFunction_attribute {
    ORO_FUNC_ATTRIBUTE_MAX_THREADS_PER_BLOCK = 0,
    ORO_FUNC_ATTRIBUTE_SHARED_SIZE_BYTES = 1,
    ORO_FUNC_ATTRIBUTE_CONST_SIZE_BYTES = 2,
    ORO_FUNC_ATTRIBUTE_LOCAL_SIZE_BYTES = 3,
    ORO_FUNC_ATTRIBUTE_NUM_REGS = 4,
    ORO_FUNC_ATTRIBUTE_PTX_VERSION = 5,
    ORO_FUNC_ATTRIBUTE_BINARY_VERSION = 6,
    ORO_FUNC_ATTRIBUTE_CACHE_MODE_CA = 7,
    ORO_FUNC_ATTRIBUTE_MAX_DYNAMIC_SHARED_SIZE_BYTES = 8,
    ORO_FUNC_ATTRIBUTE_PREFERRED_SHARED_MEMORY_CARVEOUT = 9,
    ORO_FUNC_ATTRIBUTE_MAX,
} oroFunction_attribute;

typedef enum oroFuncCache_t {
    oroFuncCachePreferNone = 0x00,
    oroFuncCachePreferShared = 0x01,
    oroFuncCachePreferL1 = 0x02,
    oroFuncCachePreferEqual = 0x03,
} oroFuncCache_t;

typedef enum oroSharedMemConfig {
    oroSharedMemBankSizeDefault = 0x00,
    oroSharedMemBankSizeFourByte = 0x01,
    oroSharedMemBankSizeEightByte = 0x02,
} oroSharedMemConfig;

typedef enum PPshared_carveout_enum {
    ORO_SHAREDMEM_CARVEOUT_DEFAULT,
    ORO_SHAREDMEM_CARVEOUT_MAX_SHARED = 100,
    ORO_SHAREDMEM_CARVEOUT_MAX_L1 = 0,
} PPshared_carveout;



typedef enum oroComputeMode {
    oroComputeModeDefault = 0,
    oroComputeModeProhibited = 2,
    oroComputeModeExclusiveProcess = 3,
} oroComputeMode;

typedef enum OROmem_advise_enum {
    ORO_MEM_ADVISE_SET_READ_MOSTLY = 1,
    ORO_MEM_ADVISE_UNSET_READ_MOSTLY = 2,
    ORO_MEM_ADVISE_SET_PREFERRED_LOCATION = 3,
    ORO_MEM_ADVISE_UNSET_PREFERRED_LOCATION = 4,
    ORO_MEM_ADVISE_SET_ACCESSED_BY = 5,
    ORO_MEM_ADVISE_UNSET_ACCESSED_BY = 6,
} PPmem_advise;

typedef enum OROmem_range_attribute_enum {
    ORO_MEM_RANGE_ATTRIBUTE_READ_MOSTLY = 1,
    ORO_MEM_RANGE_ATTRIBUTE_PREFERRED_LOCATION = 2,
    ORO_MEM_RANGE_ATTRIBUTE_ACCESSED_BY = 3,
    ORO_MEM_RANGE_ATTRIBUTE_LAST_PREFETCH_LOCATION = 4,
} PPmem_range_attribute;

typedef enum oroJitOption {
    oroJitOptionMaxRegisters = 0,
    oroJitOptionThreadsPerBlock,
    oroJitOptionWallTime,
    oroJitOptionInfoLogBuffer,
    oroJitOptionInfoLogBufferSizeBytes,
    oroJitOptionErrorLogBuffer,
    oroJitOptionErrorLogBufferSizeBytes,
    oroJitOptionOptimizationLevel,
    oroJitOptionTargetFromContext,
    oroJitOptionTarget,
    oroJitOptionFallbackStrategy,
    oroJitOptionGenerateDebugInfo,
    oroJitOptionLogVerbose,
    oroJitOptionGenerateLineInfo,
    oroJitOptionCacheMode,
    oroJitOptionSm3xOpt,
    oroJitOptionFastCompile,
    oroJitOptionNumOptions,
} oroJitOption;
/*
typedef enum HIPjit_target_enum {
    ORO_TARGET_COMPUTE_20 = 20,
    ORO_TARGET_COMPUTE_21 = 21,
    ORO_TARGET_COMPUTE_30 = 30,
    ORO_TARGET_COMPUTE_32 = 32,
    ORO_TARGET_COMPUTE_35 = 35,
    ORO_TARGET_COMPUTE_37 = 37,
    ORO_TARGET_COMPUTE_50 = 50,
    ORO_TARGET_COMPUTE_52 = 52,
    ORO_TARGET_COMPUTE_53 = 53,
    ORO_TARGET_COMPUTE_60 = 60,
    ORO_TARGET_COMPUTE_61 = 61,
    ORO_TARGET_COMPUTE_62 = 62,
    ORO_TARGET_COMPUTE_70 = 70,
    ORO_TARGET_COMPUTE_73 = 73,
    ORO_TARGET_COMPUTE_75 = 75,
} HIPjit_target;

typedef enum HIPjit_fallback_enum {
    ORO_PREFER_PTX = 0,
    ORO_PREFER_BINARY,
} HIPjit_fallback;

typedef enum HIPjit_cacheMode_enum {
    ORO_JIT_CACHE_OPTION_NONE = 0,
    ORO_JIT_CACHE_OPTION_CG,
    ORO_JIT_CACHE_OPTION_CA,
} HIPjit_cacheMode;

typedef enum HIPjitInputType_enum {
    ORO_JIT_INPUT_HIPBIN = 0,
    ORO_JIT_INPUT_PTX,
    ORO_JIT_INPUT_FATBINARY,
    ORO_JIT_INPUT_OBJECT,
    ORO_JIT_INPUT_LIBRARY,
    ORO_JIT_NUM_INPUT_TYPES,
} HIPjitInputType;

typedef struct HIPlinkState_st* HIPlinkState;

typedef enum hipGLDeviceList {
    hipGLDeviceListAll = 1,           ///< All hip devices used by current OpenGL context.
    hipGLDeviceListCurrentFrame = 2,  ///< Hip devices used by current OpenGL context in current
                                      ///< frame
                                      hipGLDeviceListNextFrame = 3      ///< Hip devices used by current OpenGL context in next
                                                                        ///< frame.
} hipGLDeviceList;

typedef enum hipGraphicsRegisterFlags {
    hipGraphicsRegisterFlagsNone = 0,
    hipGraphicsRegisterFlagsReadOnly = 1,  ///< HIP will not write to this registered resource
    hipGraphicsRegisterFlagsWriteDiscard =
    2,  ///< HIP will only write and will not read from this registered resource
    hipGraphicsRegisterFlagsSurfaceLoadStore = 4,  ///< HIP will bind this resource to a surface
    hipGraphicsRegisterFlagsTextureGather =
    8  ///< HIP will perform texture gather operations on this registered resource
} hipGraphicsRegisterFlags;

typedef enum HIPgraphicsRegisterFlags_enum {
    ORO_GRAPHICS_REGISTER_FLAGS_NONE = 0x00,
    ORO_GRAPHICS_REGISTER_FLAGS_READ_ONLY = 0x01,
    ORO_GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD = 0x02,
    ORO_GRAPHICS_REGISTER_FLAGS_SURFACE_LDST = 0x04,
    ORO_GRAPHICS_REGISTER_FLAGS_TEXTURE_GATHER = 0x08,
} HIPgraphicsRegisterFlags;

typedef enum HIPgraphicsMapResourceFlags_enum {
    ORO_GRAPHICS_MAP_RESOURCE_FLAGS_NONE = 0x00,
    ORO_GRAPHICS_MAP_RESOURCE_FLAGS_READ_ONLY = 0x01,
    ORO_GRAPHICS_MAP_RESOURCE_FLAGS_WRITE_DISCARD = 0x02,
} HIPgraphicsMapResourceFlags;

typedef enum HIParray_cubemap_face_enum {
    ORO_HIPBEMAP_FACE_POSITIVE_X = 0x00,
    ORO_HIPBEMAP_FACE_NEGATIVE_X = 0x01,
    ORO_HIPBEMAP_FACE_POSITIVE_Y = 0x02,
    ORO_HIPBEMAP_FACE_NEGATIVE_Y = 0x03,
    ORO_HIPBEMAP_FACE_POSITIVE_Z = 0x04,
    ORO_HIPBEMAP_FACE_NEGATIVE_Z = 0x05,
} HIParray_cubemap_face;

typedef enum hipLimit_t {
    ORO_LIMIT_STACK_SIZE = 0x00,
    ORO_LIMIT_PRINTF_FIFO_SIZE = 0x01,
    hipLimitMallocHeapSize = 0x02,
    ORO_LIMIT_DEV_RUNTIME_SYNC_DEPTH = 0x03,
    ORO_LIMIT_DEV_RUNTIME_PENDING_LAUNCH_COUNT = 0x04,
    ORO_LIMIT_MAX,
} hipLimit_t;

typedef enum hipResourceType {
    hipResourceTypeArray = 0x00,
    hipResourceTypeMipmaoroedArray = 0x01,
    hipResourceTypeLinear = 0x02,
    hipResourceTypePitch2D = 0x03,
} hipResourceType;

typedef enum hipError_t {
    hipSuccess = 0,
    hipErrorInvalidValue = 1,
    hipErrorOutOfMemory = 2,
    hipErrorNotInitialized = 3,
    hipErrorDeinitialized = 4,
    hipErrorProfilerDisabled = 5,
    hipErrorProfilerNotInitialized = 6,
    hipErrorProfilerAlreadyStarted = 7,
    hipErrorProfilerAlreadyStooroed = 8,
    hipErrorNoDevice = 100,
    hipErrorInvalidDevice = 101,
    hipErrorInvalidImage = 200,
    hipErrorInvalidContext = 201,
    hipErrorContextAlreadyCurrent = 202,
    hipErrorMapFailed = 205,
    hipErrorUnmapFailed = 206,
    hipErrorArrayIsMaoroed = 207,
    hipErrorAlreadyMaoroed = 208,
    hipErrorNoBinaryForGpu = 209,
    hipErrorAlreadyAcquired = 210,
    hipErrorNotMaoroed = 211,
    hipErrorNotMaoroedAsArray = 212,
    hipErrorNotMaoroedAsPointer = 213,
    hipErrorECCNotCorrectable = 214,
    hipErrorUnsuoroortedLimit = 215,
    hipErrorContextAlreadyInUse = 216,
    hipErrorPeerAccessUnsuoroorted = 217,
    hipErrorInvalidKernelFile = 218,
    hipErrorInvalidGraphicsContext = 219,
    hipErrorInvalidSource = 300,
    hipErrorFileNotFound = 301,
    hipErrorSharedObjectSymbolNotFound = 302,
    hipErrorSharedObjectInitFailed = 303,
    hipErrorOperatingSystem = 304,
    hipErrorInvalidHandle = 400,
    hipErrorNotFound = 500,
    hipErrorNotReady = 600,
    hipErrorIllegalAddress = 700,
    hipErrorLaunchOutOfResources = 701,
    hipErrorLaunchTimeOut = 702,
    hipErrorPeerAccessAlreadyEnabled = 704,
    hipErrorPeerAccessNotEnabled = 705,
    hipErrorSetOnActiveProcess = 708,
    hipErrorAssert = 710,
    hipErrorHostMemoryAlreadyRegistered = 712,
    hipErrorHostMemoryNotRegistered = 713,
    hipErrorLaunchFailure = 719,
    hipErrorCooperativeLaunchTooLarge = 720,
    hipErrorNotSuoroorted = 801,
    hipErrorUnknown = 999,
} hipError_t;
*/

typedef enum oroExternalMemoryHandleType_enum {
  oroExternalMemoryHandleTypeOpaqueFd = 1,
  oroExternalMemoryHandleTypeOpaqueWin32 = 2,
  oroExternalMemoryHandleTypeOpaqueWin32Kmt = 3,
  oroExternalMemoryHandleTypeD3D12Heap = 4,
  oroExternalMemoryHandleTypeD3D12Resource = 5,
  oroExternalMemoryHandleTypeD3D11Resource = 6,
  oroExternalMemoryHandleTypeD3D11ResourceKmt = 7,
} oroExternalMemoryHandleType;
typedef struct oroExternalMemoryHandleDesc_st {
  oroExternalMemoryHandleType type;
  union {
    int fd;
    struct {
      void *handle;
      const void *name;
    } win32;
  } handle;
  unsigned long long size;
  unsigned int flags;
} oroExternalMemoryHandleDesc;
typedef struct oroExternalMemoryBufferDesc_st {
  unsigned long long offset;
  unsigned long long size;
  unsigned int flags;
} oroExternalMemoryBufferDesc;

/**
* Stream CallBack struct
*/

#define __ORO_FUNC_DEC( funcName, args ) oroError OROAPI funcName args


__ORO_FUNC_DEC( oroGetErrorName, (oroError error, const char** pStr) );
__ORO_FUNC_DEC( oroGetErrorString, (oroError error, const char** pStr) );
__ORO_FUNC_DEC( oroInit, (unsigned int Flags) );
__ORO_FUNC_DEC( oroDriverGetVersion, (int* driverVersion) );
__ORO_FUNC_DEC( oroGetDevice, (int* device) );
__ORO_FUNC_DEC( oroGetDeviceCount, (int* count, oroApi api = ORO_API_AUTOMATIC ) );
__ORO_FUNC_DEC( oroGetDeviceProperties, (oroDeviceProp* props, int deviceId) );
__ORO_FUNC_DEC( oroDeviceGet, (oroDevice* device, int ordinal ) );
__ORO_FUNC_DEC( oroDeviceGetName, (char* name, int len, oroDevice dev) );
__ORO_FUNC_DEC( oroDeviceGetAttribute, (int* pi, oroDeviceAttribute attrib, oroDevice dev) );
__ORO_FUNC_DEC( oroDeviceComputeCapability, (int* major, int* minor, oroDevice dev) );
__ORO_FUNC_DEC( oroDevicePrimaryCtxRetain, (oroCtx* pctx, oroDevice dev) );
__ORO_FUNC_DEC( oroDevicePrimaryCtxRelease, (oroDevice dev) );
__ORO_FUNC_DEC( oroDevicePrimaryCtxSetFlags, (oroDevice dev, unsigned int flags) );
__ORO_FUNC_DEC( oroDevicePrimaryCtxGetState, (oroDevice dev, unsigned int* flags, int* active) );
__ORO_FUNC_DEC( oroDevicePrimaryCtxReset, (oroDevice dev) );
__ORO_FUNC_DEC( oroCtxCreate, (oroCtx* pctx, unsigned int flags, oroDevice dev) );
__ORO_FUNC_DEC( oroCtxDestroy, (oroCtx ctx) );
__ORO_FUNC_DEC( oroCtxPushCurrent, (oroCtx ctx) );
__ORO_FUNC_DEC( oroCtxPopCurrent, (oroCtx* pctx) );
__ORO_FUNC_DEC( oroCtxSetCurrent, (oroCtx ctx) );
__ORO_FUNC_DEC( oroCtxGetCurrent, (oroCtx* pctx) );
__ORO_FUNC_DEC( oroCtxGetDevice, (oroDevice* device) );
__ORO_FUNC_DEC( oroCtxGetFlags, (unsigned int* flags) );
__ORO_FUNC_DEC( oroCtxSynchronize, (void) );
__ORO_FUNC_DEC( oroDeviceSynchronize, (void) );
//oroError OROAPI oroCtxGetCacheConfig(hipFuncCache_t* pconfig);
//oroError OROAPI oroCtxSetCacheConfig(hipFuncCache_t config);
//oroError OROAPI oroCtxGetSharedMemConfig(hipSharedMemConfig* pConfig);
//oroError OROAPI oroCtxSetSharedMemConfig(hipSharedMemConfig config);
oroError OROAPI oroCtxGetApiVersion(oroCtx ctx, unsigned int* version);
oroError OROAPI oroModuleLoad(oroModule* module, const char* fname);
oroError OROAPI oroModuleLoadData(oroModule* module, const void* image);
oroError OROAPI oroModuleLoadDataEx(oroModule* module, const void* image, unsigned int numOptions, oroJitOption* options, void** optionValues);
oroError OROAPI oroModuleUnload(oroModule hmod);
oroError OROAPI oroModuleGetFunction(oroFunction* hfunc, oroModule hmod, const char* name);
oroError OROAPI oroModuleGetGlobal(oroDeviceptr* dptr, size_t* bytes, oroModule hmod, const char* name);
//oroError OROAPI oroModuleGetTexRef(textureReference** pTexRef, oroModule hmod, const char* name);
oroError OROAPI oroMemGetInfo(size_t* free, size_t* total);
oroError OROAPI oroMalloc(oroDeviceptr* dptr, size_t bytesize);
oroError OROAPI oroMalloc2(oroDeviceptr* dptr, size_t bytesize);
oroError OROAPI oroMemAllocPitch(oroDeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, unsigned int ElementSizeBytes);
oroError OROAPI oroFree(oroDeviceptr dptr);
oroError OROAPI oroFree2(oroDeviceptr dptr);
//oroError OROAPI oroMemGetAddressRange(oroDeviceptr* pbase, size_t* psize, oroDeviceptr dptr);
//oroError OROAPI oroHostMalloc(void** oro, size_t bytesize, unsigned int flags);
//oroError OROAPI oroHostFree(void* p);
//oroError OROAPI oroMemHostAlloc(void** oro, size_t bytesize, unsigned int Flags);
//oroError OROAPI oroHostGetDevicePointer(oroDeviceptr* pdptr, void* p, unsigned int Flags);
//oroError OROAPI oroHostGetFlags(unsigned int* pFlags, void* p);
//oroError OROAPI oroMallocManaged(oroDeviceptr* dptr, size_t bytesize, unsigned int flags);
//oroError OROAPI oroDeviceGetByPCIBusId(hipDevice_t* dev, const char* pciBusId);
//oroError OROAPI oroDeviceGetPCIBusId(char* pciBusId, int len, hipDevice_t dev);
//oroError OROAPI oroMemHostUnregister(void* p);
oroError OROAPI oroMemcpy(void *dst, void *src, size_t ByteCount, oroMemcpyKind kind);
//oroError OROAPI oroMemcpyPeer(oroDeviceptr dstDevice, hipCtx_t dstContext, oroDeviceptr srcDevice, hipCtx_t srcContext, size_t ByteCount);
oroError OROAPI oroMemcpyHtoD(oroDeviceptr dstDevice, void* srcHost, size_t ByteCount);
oroError OROAPI oroMemcpyDtoH(void* dstHost, oroDeviceptr srcDevice, size_t ByteCount);
oroError OROAPI oroMemcpyDtoD(oroDeviceptr dstDevice, oroDeviceptr srcDevice, size_t ByteCount);
//oroError OROAPI oroDrvMemcpy2DUnaligned(const hip_Memcpy2D* pCopy);
//oroError OROAPI oroMemcpyParam2D(const hip_Memcpy2D* pCopy);
//oroError OROAPI oroDrvMemcpy3D(const ORO_MEMCPY3D* pCopy);
//oroError OROAPI oroMemcpyHtoDAsync(oroDeviceptr dstDevice, const void* srcHost, size_t ByteCount, hipStream_t hStream);
//oroError OROAPI oroMemcpyDtoHAsync(void* dstHost, oroDeviceptr srcDevice, size_t ByteCount, hipStream_t hStream);
//oroError OROAPI oroMemcpyParam2DAsync(const hip_Memcpy2D* pCopy, hipStream_t hStream);
//oroError OROAPI oroDrvMemcpy3DAsync(const ORO_MEMCPY3D* pCopy, hipStream_t hStream);
oroError OROAPI oroMemset(oroDeviceptr dstDevice, unsigned int ui, size_t N);
oroError OROAPI oroMemsetD8(oroDeviceptr dstDevice, unsigned char ui, size_t N);
oroError OROAPI oroMemsetD16(oroDeviceptr dstDevice, unsigned short ui, size_t N);
oroError OROAPI oroMemsetD32(oroDeviceptr dstDevice, unsigned int ui, size_t N);
//oroError OROAPI oroMemsetD8Async(oroDeviceptr dstDevice, unsigned char uc, size_t N, oroStream hStream);
//oroError OROAPI oroMemsetD16Async(oroDeviceptr dstDevice, unsigned short us, size_t N, oroStream hStream);
//oroError OROAPI oroMemsetD32Async(oroDeviceptr dstDevice, unsigned int ui, size_t N, oroStream hStream);
//oroError OROAPI oroMemsetD2D8Async(oroDeviceptr dstDevice, size_t dstPitch, unsigned char uc, size_t Width, size_t Height, oroStream hStream);
//oroError OROAPI oroMemsetD2D16Async(oroDeviceptr dstDevice, size_t dstPitch, unsigned short us, size_t Width, size_t Height, oroStream hStream);
//oroError OROAPI oroMemsetD2D32Async(oroDeviceptr dstDevice, size_t dstPitch, unsigned int ui, size_t Width, size_t Height, oroStream hStream);
//oroError OROAPI oroArrayCreate(hArray ** pHandle, const ORO_ARRAY_DESCRIPTOR* pAllocateArray);
//oroError OROAPI oroArrayDestroy(hArray hArray);
//oroError OROAPI oroArray3DCreate(hArray * pHandle, const ORO_ARRAY3D_DESCRIPTOR* pAllocateArray);
oroError OROAPI oroPointerGetAttributes(oroPointerAttribute* attr, oroDeviceptr dptr);
oroError OROAPI oroStreamCreate(oroStream* stream);
//oroError OROAPI oroStreamCreateWithFlags(oroStream* phStream, unsigned int Flags);
//oroError OROAPI oroStreamCreateWithPriority(oroStream* phStream, unsigned int flags, int priority);
//oroError OROAPI oroStreamGetPriority(oroStream hStream, int* priority);
//oroError OROAPI oroStreamGetFlags(oroStream hStream, unsigned int* flags);
//oroError OROAPI oroStreamWaitEvent(oroStream hStream, hipEvent_t hEvent, unsigned int Flags);
//oroError OROAPI oroStreamAddCallback(oroStream hStream, hipStreamCallback_t callback, void* userData, unsigned int flags);
//oroError OROAPI oroStreamQuery(oroStream hStream);
//oroError OROAPI oroStreamSynchronize(oroStream hStream);
//oroError OROAPI oroStreamDestroy(oroStream hStream);
//oroError OROAPI oroEventCreateWithFlags(hipEvent_t* phEvent, unsigned int Flags);
//oroError OROAPI oroEventRecord(hipEvent_t hEvent, oroStream hStream);
//oroError OROAPI oroEventQuery(hipEvent_t hEvent);
//oroError OROAPI oroEventSynchronize(hipEvent_t hEvent);
//oroError OROAPI oroEventDestroy(hipEvent_t hEvent);
//oroError OROAPI oroEventElapsedTime(float* pMilliseconds, hipEvent_t hStart, hipEvent_t hEnd);
//oroError OROAPI oroFuncGetAttribute(int* pi, hipFunction_attribute attrib, hipFunction_t hfunc);
//oroError OROAPI oroFuncSetCacheConfig(hipFunction_t hfunc, hipFuncCache_t config);
oroError OROAPI oroModuleLaunchKernel(oroFunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, oroStream hStream, void** kernelParams, void** extra);
//oroError OROAPI oroDrvOccupancyMaxActiveBlocksPerMultiprocessor(int* numBlocks, hipFunction_t func, int blockSize, size_t dynamicSMemSize);
//oroError OROAPI oroDrvOccupancyMaxActiveBlocksPerMultiprocessorWithFlags(int* numBlocks, hipFunction_t func, int blockSize, size_t dynamicSMemSize, unsigned int flags);
//oroError OROAPI oroModuleOccupancyMaxPotentialBlockSize(int* minGridSize, int* blockSize, hipFunction_t func, size_t dynamicSMemSize, int blockSizeLimit);
//oroError OROAPI oroTexRefSetArray(hipTexRef hTexRef, hArray * hArray, unsigned int Flags);
//oroError OROAPI oroTexRefSetAddress(size_t* ByteOffset, hipTexRef hTexRef, hipDeviceptr_t dptr, size_t bytes);
//oroError OROAPI oroTexRefSetAddress2D(hipTexRef hTexRef, const ORO_ARRAY_DESCRIPTOR* desc, hipDeviceptr_t dptr, size_t Pitch);
//oroError OROAPI oroTexRefSetFormat(hipTexRef hTexRef, hipArray_Format fmt, int NumPackedComponents);
//oroError OROAPI oroTexRefSetAddressMode(hipTexRef hTexRef, int dim, hipTextureAddressMode am);
//oroError OROAPI oroTexRefSetFilterMode(hipTexRef hTexRef, hipTextureFilterMode fm);
//oroError OROAPI oroTexRefSetFlags(hipTexRef hTexRef, unsigned int Flags);
//oroError OROAPI oroTexRefGetAddress(hipDeviceptr_t* pdptr, hipTexRef hTexRef);
//oroError OROAPI oroTexRefGetArray(hArray ** phArray, hipTexRef hTexRef);
//oroError OROAPI oroTexRefGetAddressMode(hipTextureAddressMode* pam, hipTexRef hTexRef, int dim);
//oroError OROAPI oroTexObjectCreate(hipTextureObject_t* pTexObject, const hipResourceDesc* pResDesc, const hipTextureDesc* pTexDesc, const ORO_RESOURCE_VIEW_DESC* pResViewDesc);
//oroError OROAPI oroTexObjectDestroy(hipTextureObject_t texObject);
//oroError OROAPI oroDeviceCanAccessPeer(int* canAccessPeer, hipDevice_t dev, hipDevice_t peerDev);
//oroError OROAPI oroCtxEnablePeerAccess(hipCtx_t peerContext, unsigned int Flags);
//oroError OROAPI oroCtxDisablePeerAccess(hipCtx_t peerContext);
//oroError OROAPI oroDeviceGetP2PAttribute(int* value, hipDeviceP2PAttr attrib, hipDevice_t srcDevice, hipDevice_t dstDevice);
//oroError OROAPI oroGraphicsUnregisterResource(hipGraphicsResource resource);
//oroError OROAPI oroGraphicsResourceGetMaoroedMipmaoroedArray(hipMipmaoroedArray_t* pMipmaoroedArray, hipGraphicsResource resource);
//oroError OROAPI oroGraphicsResourceGetMaoroedPointer(hipDeviceptr_t* pDevPtr, size_t* pSize, hipGraphicsResource resource);
//oroError OROAPI oroGraphicsMapResources(unsigned int count, hipGraphicsResource* resources, oroStream hStream);
//oroError OROAPI oroGraphicsUnmapResources(unsigned int count, hipGraphicsResource* resources, oroStream hStream);
//oroError OROAPI oroGraphicsGLRegisterBuffer(hipGraphicsResource* pCudaResource, GLuint buffer, unsigned int Flags);
//oroError OROAPI oroGLGetDevices(unsigned int* pHipDeviceCount, int* pHipDevices, unsigned int hipDeviceCount, hipGLDeviceList deviceList);
oroError OROAPI oroImportExternalMemory(oroExternalMemory_t* extMem_out, const oroExternalMemoryHandleDesc* memHandleDesc);
oroError OROAPI oroExternalMemoryGetMappedBuffer(void **devPtr, oroExternalMemory_t extMem, const oroExternalMemoryBufferDesc* bufferDesc);
oroError OROAPI oroDestroyExternalMemory(oroExternalMemory_t extMem);
oroError OROAPI oroGetLastError(oroError oro_error);
orortcResult OROAPI orortcGetErrorString(orortcResult result);
orortcResult OROAPI orortcAddNameExpression(orortcProgram prog, const char* name_expression);
orortcResult OROAPI orortcCompileProgram(orortcProgram prog, int numOptions, const char** options);
orortcResult OROAPI orortcCreateProgram(orortcProgram* prog, const char* src, const char* name, int numHeaders, const char** headers, const char** includeNames);
orortcResult OROAPI orortcDestroyProgram(orortcProgram* prog);
orortcResult OROAPI orortcGetLoweredName(orortcProgram prog, const char* name_expression, const char** lowered_name);
orortcResult OROAPI orortcGetProgramLog(orortcProgram prog, char* log);
orortcResult OROAPI orortcGetProgramLogSize(orortcProgram prog, size_t* logSizeRet);
orortcResult OROAPI orortcGetCode(orortcProgram prog, char* code);
orortcResult OROAPI orortcGetCodeSize(orortcProgram prog, size_t* codeSizeRet);


enum {
	ORO_SUCCESS = 0,
	ORO_ERROR_OPEN_FAILED = -1,
	ORO_ERROR_ATEXIT_FAILED = -2,
	ORO_ERROR_OLD_DRIVER = -3,
};


int oroInitialize( oroApi api, oroU32 flags );
oroApi oroGetCurAPI( oroU32 flags );
void* getRawCtx( oroCtx ctx );
oroDevice getRawDevice( oroDevice dev );


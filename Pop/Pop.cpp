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

#include <Pop/Pop.h>
#include <contrib/cuew/include/cuew.h>
#include <contrib/hipew/include/hipew.h>
#include <stdio.h>
#include <string.h>


static Api s_api = API_HIP;

int ppInitialize( Api api, ppU32 flags )
{
	s_api = api;
	if( api == API_CUDA )
		return cuewInit( CUEW_INIT_CUDA | CUEW_INIT_NVRTC );
	if( api == API_HIP )
		return hipewInit( HIPEW_INIT_HIP );
	return PP_ERROR_OPEN_FAILED;
}
Api ppGetCurAPI(ppU32 flags)
{
	return s_api;
}


//=================================

inline
ppError hip2pp( hipError_t a )
{
	return (ppError)a;
}
inline
ppError cu2pp( CUresult a )
{
	return (ppError)a;
}
inline
ppError cuda2pp(cudaError_t a)
{
	return (ppError)a;
}
inline
CUcontext* ppCtx2cu( ppCtx* a )
{
	return (CUcontext*)a;
}
inline
hipCtx_t* ppCtx2hip( ppCtx* a )
{
	return (hipCtx_t*)a;
}
inline
pprtcResult hiprtc2pp( hiprtcResult a )
{
	return (pprtcResult)a;
}
inline
pprtcResult nvrtc2pp( nvrtcResult a )
{
	return (pprtcResult)a;
}

#define __PP_FUNC1( cuname, hipname ) if( s_api == API_CUDA ) return cu2pp( cu##cuname ); if( s_api == API_HIP ) return hip2pp( hip##hipname );
#define __PP_FUNC2( cudaname, hipname ) if( s_api == API_CUDA ) return cuda2pp( cuda##cudaname ); if( s_api == API_HIP ) return hip2pp( hip##hipname );
//#define __PP_FUNC1( cuname, hipname ) if( s_api == API_CUDA || API == API_CUDA ) return cu2pp( cu##cuname ); if( s_api == API_HIP || API == API_HIP ) return hip2pp( hip##hipname );
#define __PP_FUNC( name ) if( s_api == API_CUDA ) return cu2pp( cu##name ); if( s_api == API_HIP ) return hip2pp( hip##name );
#define __PP_CTXT_FUNC( name ) __PP_FUNC1(Ctx##name, name)
//#define __PP_CTXT_FUNC( name ) if( s_api == API_CUDA ) return cu2pp( cuCtx##name ); if( s_api == API_HIP ) return hip2pp( hip##name );
#define __PPRTC_FUNC1( cuname, hipname ) if( s_api == API_CUDA ) return nvrtc2pp( nvrtc##cuname ); if( s_api == API_HIP ) return hiprtc2pp( hiprtc##hipname );

#define __PP_FUNC_INSTANCE( funcName, args ) \
    template ppError PPAPI funcName <API_AUTOMATIC> args;\
    template ppError PPAPI funcName <API_CUDA> args;\
    template ppError PPAPI funcName <API_HIP> args;


ppError PPAPI ppGetErrorName(ppError error, const char** pStr)
{
	__PP_FUNC1(GetErrorName((CUresult)error, pStr),
		GetErrorName((hipError_t)error, pStr));
	return ppErrorUnknown;
}
ppError PPAPI ppGetErrorString(ppError error, const char** pStr)
{
	__PP_FUNC1(GetErrorString((CUresult)error, pStr),
		GetErrorString((hipError_t)error, pStr));
	return ppErrorUnknown;
}

template<Api API>
ppError PPAPI ppInit(unsigned int Flags)
{
/*
	if( s_api == API_CUDA || API == API_CUDA ) 
		printf("cuda\n"); 
	if( s_api == API_HIP || API == API_HIP ) 
		printf("hip\n");
*/
	__PP_FUNC( Init(Flags) );
	return ppErrorUnknown;
}

__PP_FUNC_INSTANCE( ppInit, (unsigned int Flags) );

ppError PPAPI ppDriverGetVersion(int* driverVersion)
{
	__PP_FUNC( DriverGetVersion(driverVersion) );
	return ppErrorUnknown;
}
ppError PPAPI ppGetDevice(int* device)
{
	__PP_CTXT_FUNC( GetDevice(device) );
	return ppErrorUnknown;
}
ppError PPAPI ppGetDeviceCount(int* count)
{
	__PP_FUNC1( DeviceGetCount(count), GetDeviceCount(count) );
	return ppErrorUnknown;
}
ppError PPAPI ppGetDeviceProperties(ppDeviceProp* props, int deviceId)
{
	if( s_api == API_CUDA )
	{
		cudaDeviceProp p;
		cudaError_t e = cudaGetDeviceProperties( &p, deviceId );
		if (e != CUDA_SUCCESS)
			return ppErrorUnknown;
		char name[128];
		strcpy( props->name, p.name );
		strcpy( props->gcnArchName, "" );
		props->totalGlobalMem = p.totalGlobalMem;
		memcpy(props->maxThreadsDim, p.maxThreadsDim, 3*sizeof(int));
		memcpy(props->maxGridSize, p.maxGridSize, 3*sizeof(int));
		props->maxThreadsPerBlock = p.maxThreadsPerBlock;
		return ppSuccess;
	}
	return hip2pp( hipGetDeviceProperties( (hipDeviceProp_t*)props, deviceId ) );
}
ppError PPAPI ppDeviceGet(ppDevice* device, int ordinal)
{
	__PP_FUNC( DeviceGet(device, ordinal) );
	return ppErrorUnknown;
}
ppError PPAPI ppDeviceGetName(char* name, int len, ppDevice dev)
{
	__PP_FUNC( DeviceGetName(name, len, dev) );
	return ppErrorUnknown;
}

ppError PPAPI ppDeviceGetAttribute(int* pi, ppDeviceAttribute attrib, ppDevice dev)
{
	__PP_FUNC1(DeviceGetAttribute(pi, (CUdevice_attribute)attrib, dev),
		DeviceGetAttribute(pi, (hipDeviceAttribute_t)attrib, dev));
	return ppErrorUnknown;
}

ppError PPAPI ppDeviceComputeCapability(int* major, int* minor, ppDevice dev)
{
	return ppErrorUnknown;
}
ppError PPAPI ppDevicePrimaryCtxRetain(ppCtx* pctx, ppDevice dev)
{
	return ppErrorUnknown;
}
ppError PPAPI ppDevicePrimaryCtxRelease(ppDevice dev)
{
	return ppErrorUnknown;
}
ppError PPAPI ppDevicePrimaryCtxSetFlags(ppDevice dev, unsigned int flags)
{
	return ppErrorUnknown;
}
ppError PPAPI ppDevicePrimaryCtxGetState(ppDevice dev, unsigned int* flags, int* active)
{
	return ppErrorUnknown;
}
ppError PPAPI ppDevicePrimaryCtxReset(ppDevice dev)
{
	return ppErrorUnknown;
}
ppError PPAPI ppCtxCreate(ppCtx* pctx, unsigned int flags, ppDevice dev)
{
	__PP_FUNC1( CtxCreate(ppCtx2cu(pctx),flags,dev), CtxCreate(ppCtx2hip(pctx),flags,dev) );
	return ppErrorUnknown;
}
ppError PPAPI ppCtxDestroy(ppCtx ctx)
{
	__PP_FUNC1( CtxDestroy( *ppCtx2cu(&ctx) ), CtxDestroy( *ppCtx2hip(&ctx) ) );
	return ppErrorUnknown;
}
/*
ppError PPAPI ppCtxPushCurrent(ppCtx ctx);
ppError PPAPI ppCtxPopCurrent(ppCtx* pctx);
*/
ppError PPAPI ppCtxSetCurrent(ppCtx ctx)
{
	__PP_FUNC1( CtxSetCurrent( *ppCtx2cu(&ctx) ), CtxSetCurrent( *ppCtx2hip(&ctx) ) );
	return ppErrorUnknown;
}
ppError PPAPI ppCtxGetCurrent(ppCtx* pctx)
{
	__PP_FUNC1( CtxGetCurrent( ppCtx2cu(pctx) ), CtxGetCurrent( ppCtx2hip(pctx) ) );
	return ppErrorUnknown;
}
/*
ppError PPAPI ppCtxGetDevice(ppDevice* device);
ppError PPAPI ppCtxGetFlags(unsigned int* flags);
*/
ppError PPAPI ppCtxSynchronize(void)
{
	__PP_FUNC( CtxSynchronize() );
	return ppErrorUnknown;
}
ppError PPAPI ppDeviceSynchronize(void)
{
	__PP_FUNC1( CtxSynchronize(), DeviceSynchronize() );
	return ppErrorUnknown;
}
//ppError PPAPI ppCtxGetCacheConfig(hipFuncCache_t* pconfig);
//ppError PPAPI ppCtxSetCacheConfig(hipFuncCache_t config);
//ppError PPAPI ppCtxGetSharedMemConfig(hipSharedMemConfig* pConfig);
//ppError PPAPI ppCtxSetSharedMemConfig(hipSharedMemConfig config);
ppError PPAPI ppCtxGetApiVersion(ppCtx ctx, unsigned int* version)
{
	__PP_FUNC1( CtxGetApiVersion(*ppCtx2cu(&ctx), version ), CtxGetApiVersion(*ppCtx2hip(&ctx), version ) );
	return ppErrorUnknown;
}
ppError PPAPI ppModuleLoad(ppModule* module, const char* fname)
{
	__PP_FUNC1( ModuleLoad( (CUmodule*)module, fname ), ModuleLoad( (hipModule_t*)module, fname ) );
	return ppErrorUnknown;
}
ppError PPAPI ppModuleLoadData(ppModule* module, const void* image)
{
	__PP_FUNC1( ModuleLoadData( (CUmodule*)module, image ), ModuleLoadData( (hipModule_t*)module, image ) );
	return ppErrorUnknown;
}
ppError PPAPI ppModuleLoadDataEx(ppModule* module, const void* image, unsigned int numOptions, ppJitOption* options, void** optionValues)
{
	__PP_FUNC1( ModuleLoadDataEx( (CUmodule*)module, image, numOptions, (CUjit_option*)options, optionValues ),
		ModuleLoadDataEx( (hipModule_t*)module, image, numOptions, (hipJitOption*)options, optionValues ) );
	return ppErrorUnknown;
}
ppError PPAPI ppModuleUnload(ppModule module)
{
	__PP_FUNC1( ModuleUnload( (CUmodule)module ), ModuleUnload( (hipModule_t)module ) );
	return ppErrorUnknown;
}
ppError PPAPI ppModuleGetFunction(ppFunction* hfunc, ppModule hmod, const char* name)
{
	__PP_FUNC1( ModuleGetFunction( (CUfunction*)hfunc, (CUmodule)hmod, name ), 
		ModuleGetFunction( (hipFunction_t*)hfunc, (hipModule_t)hmod, name ) );
	return ppErrorUnknown;
}
ppError PPAPI ppModuleGetGlobal(ppDeviceptr* dptr, size_t* bytes, ppModule hmod, const char* name)
{
	__PP_FUNC1( ModuleGetGlobal( dptr, bytes, (CUmodule)hmod, name ), 
		ModuleGetGlobal( dptr, bytes, (hipModule_t)hmod, name ) );
	return ppErrorUnknown;
}
//ppError PPAPI ppModuleGetTexRef(textureReference** pTexRef, ppModule hmod, const char* name);
ppError PPAPI ppMemGetInfo(size_t* free, size_t* total)
{
	return ppErrorUnknown;
}
ppError PPAPI ppMalloc(ppDeviceptr* dptr, size_t bytesize)
{
	__PP_FUNC1( MemAlloc(dptr, bytesize), Malloc( dptr, bytesize ) );
	return ppErrorUnknown;
}
ppError PPAPI ppMalloc2(ppDeviceptr* dptr, size_t bytesize)
{
	__PP_FUNC2( Malloc((CUdeviceptr*)dptr, bytesize), Malloc(dptr, bytesize) );
	return ppErrorUnknown;
}
ppError PPAPI ppMemAllocPitch(ppDeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, unsigned int ElementSizeBytes)
{
	return ppErrorUnknown;
}
ppError PPAPI ppFree(ppDeviceptr dptr)
{
	__PP_FUNC1( MemFree( dptr ), Free( dptr ) );
	return ppErrorUnknown;
}
ppError PPAPI ppFree2(ppDeviceptr dptr)
{
	__PP_FUNC2( Free((CUdeviceptr)dptr), Free(dptr) );
	return ppErrorUnknown;
}

//-------------------
ppError PPAPI ppMemcpy(void *dstDevice, void* srcHost, size_t ByteCount, ppMemcpyKind kind)
{
	__PP_FUNC2( Memcpy(dstDevice, srcHost, ByteCount, (cudaMemcpyKind)kind),
		Memcpy(dstDevice, srcHost, ByteCount, (hipMemcpyKind)kind) );
	return ppErrorUnknown;
}

ppError PPAPI ppMemcpyHtoD(ppDeviceptr dstDevice, void* srcHost, size_t ByteCount)
{
	__PP_FUNC1( MemcpyHtoD( dstDevice, srcHost, ByteCount ),
		MemcpyHtoD( dstDevice, srcHost, ByteCount ) );
	return ppErrorUnknown;
}
ppError PPAPI ppMemcpyDtoH(void* dstHost, ppDeviceptr srcDevice, size_t ByteCount)
{
	__PP_FUNC1( MemcpyDtoH( dstHost, srcDevice, ByteCount ),
		MemcpyDtoH( dstHost, srcDevice, ByteCount ) );
	return ppErrorUnknown;
}
ppError PPAPI ppMemcpyDtoD(ppDeviceptr dstDevice, ppDeviceptr srcDevice, size_t ByteCount)
{
	__PP_FUNC( MemcpyDtoD( dstDevice, srcDevice, ByteCount ) );
	return ppErrorUnknown;
}

ppError PPAPI ppMemset(ppDeviceptr dstDevice, unsigned int ui, size_t N)
{
	__PP_FUNC1( MemsetD8( (CUdeviceptr)dstDevice, ui, N ), Memset((void*)dstDevice, ui, N));
	return ppErrorUnknown;
}

ppError PPAPI ppMemsetD8(ppDeviceptr dstDevice, unsigned char ui, size_t N)
{
	__PP_FUNC(MemsetD8(dstDevice, ui, N));
	return ppErrorUnknown;
}
ppError PPAPI ppMemsetD16(ppDeviceptr dstDevice, unsigned short ui, size_t N)
{
	__PP_FUNC(MemsetD16(dstDevice, ui, N));
	return ppErrorUnknown;
}
ppError PPAPI ppMemsetD32(ppDeviceptr dstDevice, unsigned int ui, size_t N)
{
	__PP_FUNC(MemsetD32(dstDevice, ui, N));
	return ppErrorUnknown;
}

//-------------------
ppError PPAPI ppModuleLaunchKernel(ppFunction f, unsigned int gridDimX, unsigned int gridDimY, unsigned int gridDimZ, unsigned int blockDimX, unsigned int blockDimY, unsigned int blockDimZ, unsigned int sharedMemBytes, ppStream hStream, void** kernelParams, void** extra)
{
	__PP_FUNC1( LaunchKernel( (CUfunction)f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, (CUstream)hStream, kernelParams, extra ),
		ModuleLaunchKernel( (hipFunction_t)f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, (hipStream_t)hStream, kernelParams, extra ) );
	return ppErrorUnknown;
}
ppError PPAPI ppGetLastError(ppError pp_error)
{
	__PP_FUNC2(GetLastError((cudaError_t)pp_error),
		GetLastError((hipError_t)pp_error));
	return ppErrorUnknown;
}
//-------------------
pprtcResult PPAPI pprtcGetErrorString(pprtcResult result)
{
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcAddNameExpression(pprtcProgram prog, const char* name_expression)
{
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcCompileProgram(pprtcProgram prog, int numOptions, const char** options)
{
	__PPRTC_FUNC1( CompileProgram( (nvrtcProgram)prog, numOptions, options ),
		CompileProgram( (hiprtcProgram)prog, numOptions, options ) );
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcCreateProgram(pprtcProgram* prog, const char* src, const char* name, int numHeaders, const char** headers, const char** includeNames)
{
	__PPRTC_FUNC1( CreateProgram( (nvrtcProgram*)prog, src, name, numHeaders, headers, includeNames ), 
		CreateProgram( (hiprtcProgram*)prog, src, name, numHeaders, headers, includeNames ) );
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcDestroyProgram(pprtcProgram* prog)
{
	__PPRTC_FUNC1( DestroyProgram( (nvrtcProgram*)prog), 
		DestroyProgram( (hiprtcProgram*)prog ) );
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcGetLoweredName(pprtcProgram prog, const char* name_expression, const char** lowered_name)
{
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcGetProgramLog(pprtcProgram prog, char* log)
{
	__PPRTC_FUNC1( GetProgramLog( (nvrtcProgram)prog, log ), 
		GetProgramLog( (hiprtcProgram)prog, log ) );
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcGetProgramLogSize(pprtcProgram prog, size_t* logSizeRet)
{
	__PPRTC_FUNC1( GetProgramLogSize( (nvrtcProgram)prog, logSizeRet), 
		GetProgramLogSize( (hiprtcProgram)prog, logSizeRet ) );
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcGetCode(pprtcProgram prog, char* code)
{
	__PPRTC_FUNC1( GetPTX( (nvrtcProgram)prog, code ), 
		GetCode( (hiprtcProgram)prog, code ) );
	return PPRTC_ERROR_INTERNAL_ERROR;
}
pprtcResult PPAPI pprtcGetCodeSize(pprtcProgram prog, size_t* codeSizeRet)
{
	__PPRTC_FUNC1( GetPTXSize( (nvrtcProgram)prog, codeSizeRet ), 
		GetCodeSize( (hiprtcProgram)prog, codeSizeRet ) );
	return PPRTC_ERROR_INTERNAL_ERROR;
}

//-------------------

// Implementation of ppPointerGetAttributes is hacky due to differences between CUDA and HIP
ppError PPAPI ppPointerGetAttributes(ppPointerAttribute* attr, ppDeviceptr dptr)
{
	if (s_api == API_CUDA)
	{
		unsigned int data;
		return cu2pp(cuPointerGetAttribute(&data, CU_POINTER_ATTRIBUTE_MEMORY_TYPE, dptr));
	}
	if (s_api == API_HIP) 
		return hip2pp(hipPointerGetAttributes((hipPointerAttribute_t*)attr, (void*)dptr));

	return ppErrorUnknown;
}

//-----------------
ppError PPAPI ppStreamCreate(ppStream* stream)
{
	__PP_FUNC2(StreamCreate((cudaStream_t*)stream),
		StreamCreate((hipStream_t*)stream));

	return ppErrorUnknown;
}



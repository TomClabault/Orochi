//
// Copyright (c) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
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

#include "basicTests.h"
#include "common.h"

TEST_F( OroTestBase, init )
{ 

}

TEST_F( OroTestBase, checkCUEW )
{
	// this unit test is just to inform if CUEW is disabled.
	// if it fails, this means that you should install the CUDA SDK, add its include path to this project, and enable OROCHI_ENABLE_CUEW.
	// ( if the CUDA SDK is installed, the premake script should automatically enable CUEW )
	#ifndef OROCHI_ENABLE_CUEW
	printf("This build of Orochi is not able to run on CUDA.\n");
	ASSERT_TRUE( false );
	#endif
}

TEST_F( OroTestBase, deviceprops )
{
	{
		oroDeviceProp props;
		OROCHECK( oroGetDeviceProperties( &props, m_device ) );
		printf( "executing on %s (%s)\n", props.name, props.gcnArchName );
		printf( "%d multiProcessors\n", props.multiProcessorCount );
	}
}

TEST_F(OroTestBase, deviceGetSet)
{
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;

	// TODO: this unit test doesn't work on CUDA,
	//       because Orochi needs to add support of "Cuda Runtime" for function like: cudaGetDevice/cudaSetDevice
	if ( !isAmd )
		return;

	int deviceIndex = 0;
	OROCHECK(oroSetDevice(deviceIndex));
	deviceIndex = -1;
	OROCHECK(oroGetDevice(&deviceIndex));
	OROASSERT(deviceIndex == 0);
	deviceIndex = -1;
	OROCHECK(oroCtxGetDevice(&deviceIndex));
	OROASSERT(deviceIndex == 0);
}

TEST_F( OroTestBase, kernelExec ) 
{
	OrochiUtils o;
	int a_host = -1;
	int* a_device = nullptr;
	OROCHECK( oroMalloc( (oroDeviceptr*)&a_device, sizeof( int ) ) );
	OROCHECK( oroMemset( (oroDeviceptr)a_device, 0, sizeof( int ) ) );
	oroFunction kernel = o.getFunctionFromFile( m_device, "../UnitTest/testKernel.h", "testKernel", 0 ); 
	if ( !kernel )
	{
		printf("ERROR: kernel file failed to load.");
		ASSERT_TRUE(false);
		return;
	}
	int blockCount = 0;
	OROCHECK( oroModuleOccupancyMaxActiveBlocksPerMultiprocessor( &blockCount, kernel, 128, 0 ) );
	printf( "%d blocks per multiprocessor\n", blockCount );
	OROASSERT( 0 < blockCount );
	const void* args[] = { &a_device };
	OrochiUtils::launch1D( kernel, 64, args, 64 );
	OrochiUtils::waitForCompletion();
	OROCHECK( oroMemcpyDtoH( &a_host, (oroDeviceptr)a_device, sizeof( int ) ) );
	OROASSERT( a_host == 2016 );
	OROCHECK( oroFree( (oroDeviceptr)a_device ) );
	o.unloadKernelCache();
}

TEST_F( OroTestBase, GpuMemoryTest )
{
	OrochiUtils o;

	Oro::GpuMemory<int> device_memory;
	device_memory.resize( 1 );
	OROASSERT( device_memory.size() == 1ULL );

	device_memory.reset();

	auto kernel = o.getFunctionFromFile( m_device, "../UnitTest/testKernel.h", "testKernel", 0 );
	if ( !kernel )
	{
		printf("ERROR: kernel file failed to load.");
		ASSERT_TRUE(false);
		return;
	}

	const void* args[] = { Oro::arg_cast( device_memory.address() ) };

	OrochiUtils::launch1D( kernel, 64, args, 64 );
	OrochiUtils::waitForCompletion();

	const auto val = device_memory.getSingle();
	OROASSERT( val == 2016 );

	const auto values = device_memory.getData();
	OROASSERT( std::size( values ) == 1ULL );
	OROASSERT( values[0] == 2016 );

	const auto test_value = 123;
	const std::vector<int> test_data = { test_value, test_value, test_value };
	device_memory.copyFromHost( std::data( test_data ), std::size( test_data ) );

	OROASSERT( device_memory.size() == std::size( test_data ) );

	const auto output_data = device_memory.getData();

	for( auto&& out : output_data )
	{
		OROASSERT( out == test_value );
	}
	o.unloadKernelCache();
}

TEST_F( OroTestBase, Event )
{
	OrochiUtils o;
	int a_host = -1;
	int* a_device = nullptr;
	OROCHECK( oroMalloc( (oroDeviceptr*)&a_device, sizeof( int ) ) );
	OROCHECK( oroMemset( (oroDeviceptr)a_device, 0, sizeof( int ) ) );

	OroStopwatch sw( m_stream );

	oroFunction kernel = o.getFunctionFromFile( m_device, "../UnitTest/testKernel.h", "testKernel", 0 );
	if ( !kernel )
	{
		printf("ERROR: kernel file failed to load.");
		ASSERT_TRUE(false);
		return;
	}

	const void* args[] = { &a_device };
	sw.start();
	OrochiUtils::launch1D( kernel, 64, args, 64, 0, m_stream );
	sw.stop();

	OrochiUtils::waitForCompletion( m_stream );
	OROCHECK( oroMemcpyDtoH( &a_host, (oroDeviceptr)a_device, sizeof( int ) ) );
	OROASSERT( a_host == 2016 );
	OROCHECK( oroFree( (oroDeviceptr)a_device ) );

	float ms = sw.getMs();
	printf( "kernelExec: %3.2fms\n", ms );
	o.unloadKernelCache();
}

// Load a Binary file and put content to std::vector
void loadFile( const char* path, std::vector<char>& dst ) 
{
	std::fstream f( path, std::ios::binary | std::ios::in );
	if( f.is_open() )
	{
		size_t sizeFile;
		f.seekg( 0, std::fstream::end );
		size_t size = sizeFile = (size_t)f.tellg();
		dst.resize( size );
		f.seekg( 0, std::fstream::beg );
		f.read( dst.data(), size );
		f.close();
	}
	else
	{
		printf("WARNING: failed to open file %s\n", path);
	}
}
#if 0
TEST_F( OroTestBase, linkBc )
{
	oroDeviceProp props;
	OROCHECK( oroGetDeviceProperties( &props, m_device ) );
	int v;
	oroDriverGetVersion( &v );
	std::vector<char> data0;
	std::vector<char> data1;
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;
	std::string archName(props.gcnArchName);
	archName = archName.substr( 0, archName.find( ':' ));
	// todo - generate cubin for NVIDIA GPUs (skip on CUDA for now)
	{
		std::string bcFile = isAmd ? ( "../UnitTest/bitcodes/moduleTestFunc-hip-amdgcn-amd-amdhsa-" + archName + ".bc" ) : "../UnitTest/bitcodes/moduleTestFunc.cubin";
		loadFile( bcFile.c_str(), data1 );
	}
	{
		std::string bcFile = isAmd ? ( "../UnitTest/bitcodes/moduleTestKernel-hip-amdgcn-amd-amdhsa-" + archName + ".bc" ) : "../UnitTest/bitcodes/moduleTestKernel.cubin";
		loadFile( bcFile.c_str(), data0 );
	}

	{
		orortcLinkState rtc_link_state;
		orortcJIT_option options[6];
		void* option_vals[6];
		float wall_time;

		unsigned int log_size = 8192;
		char error_log[8192];
		char info_log[8192];
		size_t out_size;
		void* cuOut;

		options[0] = ORORTC_JIT_WALL_TIME;
		option_vals[0] = (void*)( &wall_time );

		options[1] = ORORTC_JIT_INFO_LOG_BUFFER;
		option_vals[1] = info_log;

		options[2] = ORORTC_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
		option_vals[2] = (void*)( log_size );

		options[3] = ORORTC_JIT_ERROR_LOG_BUFFER;
		option_vals[3] = error_log;

		options[4] = ORORTC_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
		option_vals[4] = (void*)( log_size );//todo. behavior difference

		options[5] = ORORTC_JIT_LOG_VERBOSE;
		option_vals[5] = (void*)m_jitLogVerbose;;

		void* binary;
		size_t binarySize = 0;
		orortcJITInputType type = isAmd ? ORORTC_JIT_INPUT_LLVM_BITCODE : ORORTC_JIT_INPUT_CUBIN;
		ORORTCCHECK( orortcLinkCreate( 6, options, option_vals, &rtc_link_state ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data1.data(), data1.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data0.data(), data0.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkComplete( rtc_link_state, &binary, &binarySize ) );

		oroFunction function;
		oroModule module;
		oroError ee = oroModuleLoadData( &module, binary );
		ee = oroModuleGetFunction( &function, module, "testKernel" );
		int x_host = -1;
		int* x_device = nullptr;
		OROCHECK( oroMalloc( (oroDeviceptr*)&x_device, sizeof( int ) ) );
		OROCHECK( oroMemset( (oroDeviceptr)x_device, 0, sizeof( int ) ) );
		const void* args[] = { &x_device };

		OrochiUtils::launch1D( function, 64, args, 64 );
		OrochiUtils::waitForCompletion();
		OROCHECK( oroMemcpyDtoH( &x_host, (oroDeviceptr)x_device, sizeof( int ) ) );
		OROASSERT( x_host == 2016 );
		OROCHECK( oroFree( (oroDeviceptr)x_device ) );
		ORORTCCHECK( orortcLinkDestroy( rtc_link_state ) );
		ORORTCCHECK( oroModuleUnload( module ) );
	}
}
#endif
TEST_F( OroTestBase, link ) 
{
	oroDeviceProp props;
	OROCHECK( oroGetDeviceProperties( &props, m_device ) );
	std::vector<char> data0;
	std::vector<char> data1;
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;

	std::string arch = "-arch=sm_" + std::to_string( props.major ) + std::string( "0" );

	std::vector<const char*> opts = isAmd ? std::vector<const char *>({ "-fgpu-rdc", "-c", "--cuda-device-only" })
											:  std::vector<const char *>({ "--device-c", arch.c_str() });
	{
		// known issue: On Linux, rocm 6, orortcCompileProgram will report a fail, because using an 'extern' function.
		// but the bitcode is correctly generated and executed. the output of the execution is also checked by this unit test, and is correct.
		std::string code;
		OrochiUtils::readSourceCode( "../UnitTest/moduleTestKernel.h", code );
		OrochiUtils::getData( m_device, code.c_str(), "../UnitTest/moduleTestKernel.h", &opts, data1 );
	}
	{
		std::string code;
		OrochiUtils::readSourceCode( "../UnitTest/moduleTestFunc.h", code );
		OrochiUtils::getData( m_device, code.c_str(), "../UnitTest/moduleTestFunc.h", &opts, data0 );
	}

	{
		orortcLinkState rtc_link_state;
		orortcJIT_option options[6];
		void* option_vals[6];
		float wall_time;
		unsigned int log_size = 8192;
		char error_log[8192];
		char info_log[8192];

		options[0] = ORORTC_JIT_WALL_TIME;
		option_vals[0] = static_cast<void*>( &wall_time );

		options[1] = ORORTC_JIT_INFO_LOG_BUFFER;
		option_vals[1] = info_log;

		options[2] = ORORTC_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
		option_vals[2] = static_cast<void*>( &log_size );

		options[3] = ORORTC_JIT_ERROR_LOG_BUFFER;
		option_vals[3] = error_log;

		options[4] = ORORTC_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
		option_vals[4] = static_cast<void*>( &log_size );//todo. behavior difference

		options[5] = ORORTC_JIT_LOG_VERBOSE;
		option_vals[5] = (void*)m_jitLogVerbose;

		void* binary = nullptr;
		size_t binarySize = 0;

		orortcJITInputType type = isAmd ? ORORTC_JIT_INPUT_LLVM_BITCODE : ORORTC_JIT_INPUT_CUBIN;

		ORORTCCHECK( orortcLinkCreate( 6, options, option_vals, &rtc_link_state ) );
		
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data1.data(), data1.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data0.data(), data0.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkComplete( rtc_link_state, &binary, &binarySize ) );
		OROASSERT(binary != nullptr);
		OROASSERT(binarySize != 0);

		oroFunction function = static_cast<oroFunction>(nullptr);
		oroModule module;
		oroError ee = oroModuleLoadData( &module, binary );
		OrochiUtils::waitForCompletion();
		ee = oroModuleGetFunction( &function, module, "testKernel" );
		OROASSERT(function != nullptr);

		int x_host = -1;
		int* x_device = nullptr;
		OROCHECK( oroMalloc( (oroDeviceptr*)&x_device, sizeof( int ) ) );
		OROCHECK( oroMemset( (oroDeviceptr)x_device, 0, sizeof( int ) ) );
		const void* args[] = { &x_device };

		OrochiUtils::launch1D( function, 64, args, 64 );
		OrochiUtils::waitForCompletion();
		OROCHECK( oroMemcpyDtoH( &x_host, (oroDeviceptr)x_device, sizeof( int ) ) );
		OROASSERT( x_host == 2016 );
		OROCHECK( oroFree( (oroDeviceptr)x_device ) );
		ORORTCCHECK( orortcLinkDestroy( rtc_link_state ) );
		ORORTCCHECK( oroModuleUnload( module ) );
	}
}
#if 0
TEST_F( OroTestBase, link_addFile )
{
	oroDeviceProp props;
	OROCHECK( oroGetDeviceProperties( &props, m_device ) );
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;
	std::string archName( props.gcnArchName );
	archName = archName.substr( 0, archName.find( ':' ) );

	std::string bcFile1 = isAmd ? ( "../UnitTest/bitcodes/moduleTestFunc-hip-amdgcn-amd-amdhsa-" + archName + ".bc" )
		: "../UnitTest/bitcodes/moduleTestFunc.cubin";
	std::string bcFile2 = isAmd ? ( "../UnitTest/bitcodes/moduleTestKernel-hip-amdgcn-amd-amdhsa-" + archName + ".bc" )
		: "../UnitTest/bitcodes/moduleTestKernel.cubin";

	{
		orortcLinkState rtc_link_state;
		orortcJIT_option options[6];
		void* option_vals[6];
		float wall_time;
		unsigned int log_size = 8192;
		char error_log[8192];
		char info_log[8192];
		size_t out_size;
		void* cuOut;

		options[0] = ORORTC_JIT_WALL_TIME;
		option_vals[0] = (void*)( &wall_time );

		options[1] = ORORTC_JIT_INFO_LOG_BUFFER;
		option_vals[1] = info_log;

		options[2] = ORORTC_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
		option_vals[2] = (void*)( log_size );

		options[3] = ORORTC_JIT_ERROR_LOG_BUFFER;
		option_vals[3] = error_log;

		options[4] = ORORTC_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
		option_vals[4] = (void*)( log_size );//todo. behavior difference

		options[5] = ORORTC_JIT_LOG_VERBOSE;
		option_vals[5] = (void*)m_jitLogVerbose;;

		void* binary;
		size_t binarySize;

		orortcJITInputType type = isAmd ? ORORTC_JIT_INPUT_LLVM_BITCODE : ORORTC_JIT_INPUT_CUBIN;

		ORORTCCHECK( orortcLinkCreate( 6, options, option_vals, &rtc_link_state ) );

		ORORTCCHECK( orortcLinkAddFile( rtc_link_state, type, bcFile1.c_str(), 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkAddFile( rtc_link_state, type, bcFile2.c_str(), 0, 0, 0 ) );
		// getting HIPRTC_ERROR_BUILTIN_OPERATION_FAILURE here
		ORORTCCHECK( orortcLinkComplete( rtc_link_state, &binary, &binarySize ) );

		oroFunction function;
		oroModule module;
		oroError ee = oroModuleLoadData( &module, binary );
		OrochiUtils::waitForCompletion();
		ee = oroModuleGetFunction( &function, module, "testKernel" );
		int x_host = -1;
		int* x_device = nullptr;
		OROCHECK( oroMalloc( (oroDeviceptr*)&x_device, sizeof( int ) ) );
		OROCHECK( oroMemset( (oroDeviceptr)x_device, 0, sizeof( int ) ) );
		const void* args[] = { &x_device };

		OrochiUtils::launch1D( function, 64, args, 64 );
		OrochiUtils::waitForCompletion();
		OROCHECK( oroMemcpyDtoH( &x_host, (oroDeviceptr)x_device, sizeof( int ) ) );
		OROASSERT( x_host == 2016 );
		OROCHECK( oroFree( (oroDeviceptr)x_device ) );
		ORORTCCHECK( orortcLinkDestroy( rtc_link_state ) );
		ORORTCCHECK( oroModuleUnload( module ) );
	}
}
#endif

TEST_F( OroTestBase, link_null_name ) 
{
	oroDeviceProp props;
	OROCHECK( oroGetDeviceProperties( &props, m_device ) );
	std::vector<char> data0;
	std::vector<char> data1;
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;
	std::string arch = "-arch=sm_" + std::to_string( props.major ) + std::string( "0" );
	std::vector<const char*> opts = isAmd ? std::vector<const char *>({ "-fgpu-rdc", "-c", "--cuda-device-only" })
											:  std::vector<const char *>({ "--device-c", arch.c_str() });
	{
		// known issue: On Linux, rocm 6, orortcCompileProgram will report a fail, because using an 'extern' function.
		// but the bitcode is correctly generated and executed. the output of the execution is also checked by this unit test, and is correct.
		std::string code;
		OrochiUtils::readSourceCode( "../UnitTest/moduleTestKernel.h", code );
		OrochiUtils::getData( m_device, code.c_str(), "../UnitTest/moduleTestKernel.h", &opts, data1 );
	}
	{
		std::string code;
		OrochiUtils::readSourceCode( "../UnitTest/moduleTestFunc.h", code );
		OrochiUtils::getData( m_device, code.c_str(), "../UnitTest/moduleTestFunc.h", &opts, data0 );
	}

	{
		orortcLinkState rtc_link_state;

		void* binary = nullptr;
		size_t binarySize = 0;
		orortcJITInputType type = isAmd ? ORORTC_JIT_INPUT_LLVM_BITCODE : ORORTC_JIT_INPUT_CUBIN;

		ORORTCCHECK( orortcLinkCreate( 0, 0, 0, &rtc_link_state ) );
		
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data1.data(), data1.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data0.data(), data0.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkComplete( rtc_link_state, &binary, &binarySize ) );
		OROASSERT(binary != nullptr);
		OROASSERT(binarySize != 0);

		oroFunction function = static_cast<oroFunction>(nullptr);
		oroModule module;
		oroError ee = oroModuleLoadData( &module, binary );
		ee = oroModuleGetFunction( &function, module, "testKernel" );
		OROASSERT(function != nullptr);

		int x_host = -1;
		int* x_device = nullptr;
		OROCHECK( oroMalloc( (oroDeviceptr*)&x_device, sizeof( int ) ) );
		OROCHECK( oroMemset( (oroDeviceptr)x_device, 0, sizeof( int ) ) );
		const void* args[] = { &x_device };

		OrochiUtils::launch1D( function, 64, args, 64 );
		OrochiUtils::waitForCompletion();
		OROCHECK( oroMemcpyDtoH( &x_host, (oroDeviceptr)x_device, sizeof( int ) ) );
		OROASSERT( x_host == 2016 );
		OROCHECK( oroFree( (oroDeviceptr)x_device ) );
		ORORTCCHECK( orortcLinkDestroy( rtc_link_state ) );
		ORORTCCHECK( oroModuleUnload( module ) );
	}
}

TEST_F( OroTestBase, link_bundledBc )
{
	oroDeviceProp props;
	OROCHECK( oroGetDeviceProperties( &props, m_device ) );
	int v;
	oroDriverGetVersion( &v );
	std::vector<char> data0;
	std::vector<char> data1;
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;

	{
		std::string bcFile = isAmd ? "../UnitTest/bitcodes/moduleTestFunc-hip-amdgcn-amd-amdhsa.bc" : "../UnitTest/bitcodes/moduleTestFunc.fatbin";
		loadFile( bcFile.c_str(), data1 );
	}
	{
		std::string bcFile = isAmd ? "../UnitTest/bitcodes/moduleTestKernel-hip-amdgcn-amd-amdhsa.bc" : "../UnitTest/bitcodes/moduleTestKernel.fatbin";
		loadFile( bcFile.c_str(), data0 );
	}

	{
		orortcLinkState rtc_link_state;
		orortcJIT_option options[7];
		void* option_vals[7];
		float wall_time;

		unsigned int log_size = 8192;
		char error_log[8192];
		char info_log[8192];

		options[0] = ORORTC_JIT_WALL_TIME;
		option_vals[0] =  static_cast<void*>( &wall_time );

		options[1] = ORORTC_JIT_INFO_LOG_BUFFER;
		option_vals[1] = info_log;

		options[2] = ORORTC_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
		option_vals[2] =  static_cast<void*>( &log_size );

		options[3] = ORORTC_JIT_ERROR_LOG_BUFFER;
		option_vals[3] = error_log;

		options[4] = ORORTC_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
		option_vals[4] =  static_cast<void*>( &log_size ); // todo. behavior difference

		options[5] = ORORTC_JIT_LOG_VERBOSE;
		option_vals[5] = (void*)m_jitLogVerbose;;

		void* binary = nullptr;
		size_t binarySize = 0;
		const orortcJITInputType type = isAmd ? ORORTC_JIT_INPUT_LLVM_BUNDLED_BITCODE : ORORTC_JIT_INPUT_FATBINARY;
		ORORTCCHECK( orortcLinkCreate( 6, options, option_vals, &rtc_link_state ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data1.data(), data1.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type, data0.data(), data0.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkComplete( rtc_link_state, &binary, &binarySize ) );
		OROASSERT(binary != nullptr);
		OROASSERT(binarySize != 0);

		oroFunction function = static_cast<oroFunction>(nullptr);
		oroModule module;
		oroError ee = oroModuleLoadData( &module, binary );
		ORORTCCHECK(oroModuleGetFunction( &function, module, "testKernel" ));
		OROASSERT(function != nullptr);

		int x_host = -1;
		int* x_device = nullptr;
		OROCHECK( oroMalloc( (oroDeviceptr*)&x_device, sizeof( int ) ) );
		OROCHECK( oroMemset( (oroDeviceptr)x_device, 0, sizeof( int ) ) );
		const void* args[] = { &x_device };

		OrochiUtils::launch1D( function, 64, args, 64 );
		OrochiUtils::waitForCompletion();
		OROCHECK( oroMemcpyDtoH( &x_host, (oroDeviceptr)x_device, sizeof( int ) ) );
		OROASSERT( x_host == 2016 );
		OROCHECK( oroFree( (oroDeviceptr)x_device ) );
		ORORTCCHECK( orortcLinkDestroy( rtc_link_state ) );
		ORORTCCHECK( oroModuleUnload( module ) );
	}
}

TEST_F( OroTestBase, link_bundledBc_with_bc )
{
	oroDeviceProp props;
	OROCHECK( oroGetDeviceProperties( &props, m_device ) );
	int v;
	oroDriverGetVersion( &v );
	std::vector<char> data0;
	std::vector<char> data1;
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;
	std::string arch = "-arch=sm_" + std::to_string( props.major ) + std::string( "0" );
	{
		std::string bcFile = isAmd ? "../UnitTest/bitcodes/moduleTestFunc-hip-amdgcn-amd-amdhsa.bc" : "../UnitTest/bitcodes/moduleTestFunc.fatbin";
		loadFile( bcFile.c_str(), data1 );
	}
	{
		std::vector<const char*> opts = isAmd ? std::vector<const char *>({ "-fgpu-rdc", "-c", "--cuda-device-only" })
											:  std::vector<const char *>({ "--device-c", arch.c_str() });
		std::string code;
		OrochiUtils::readSourceCode( "../UnitTest/moduleTestKernel.h", code );
		OrochiUtils::getData( m_device, code.c_str(), "../UnitTest/moduleTestKernel.h", &opts, data0 );
	}

	{
		orortcLinkState rtc_link_state;
		orortcJIT_option options[7];
		void* option_vals[7];
		float wall_time;

		unsigned int log_size = 8192;
		char error_log[8192];
		char info_log[8192];

		options[0] = ORORTC_JIT_WALL_TIME;
		option_vals[0] = static_cast<void*>( &wall_time );

		options[1] = ORORTC_JIT_INFO_LOG_BUFFER;
		option_vals[1] = info_log;

		options[2] = ORORTC_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
		option_vals[2] = static_cast<void*>( &log_size );

		options[3] = ORORTC_JIT_ERROR_LOG_BUFFER;
		option_vals[3] = error_log;

		options[4] = ORORTC_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
		option_vals[4] = static_cast<void*>( &log_size ); // todo. behavior difference

		options[5] = ORORTC_JIT_LOG_VERBOSE;
		option_vals[5] = (void*)m_jitLogVerbose;;

		void* binary = nullptr;
		size_t binarySize = 0;
		const orortcJITInputType type0 = isAmd ? ORORTC_JIT_INPUT_LLVM_BITCODE : ORORTC_JIT_INPUT_CUBIN;
		const orortcJITInputType type1 = isAmd ? ORORTC_JIT_INPUT_LLVM_BUNDLED_BITCODE : ORORTC_JIT_INPUT_FATBINARY;
		ORORTCCHECK( orortcLinkCreate( 6, options, option_vals, &rtc_link_state ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type1, data1.data(), data1.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type0, data0.data(), data0.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkComplete( rtc_link_state, &binary, &binarySize ) );
		OROASSERT(binary != nullptr);
		OROASSERT(binarySize != 0);

		oroFunction function = static_cast<oroFunction>(nullptr);
		oroModule module;
		oroError ee = oroModuleLoadData( &module, binary );
		ORORTCCHECK(oroModuleGetFunction( &function, module, "testKernel" ));
		OROASSERT(function != nullptr);

		int x_host = -1;
		int* x_device = nullptr;
		OROCHECK( oroMalloc( (oroDeviceptr*)&x_device, sizeof( int ) ) );
		OROCHECK( oroMemset( (oroDeviceptr)x_device, 0, sizeof( int ) ) );
		const void* args[] = { &x_device };

		OrochiUtils::launch1D( function, 64, args, 64 );
		OrochiUtils::waitForCompletion();
		OROCHECK( oroMemcpyDtoH( &x_host, (oroDeviceptr)x_device, sizeof( int ) ) );
		OROASSERT( x_host == 2016 );
		OROCHECK( oroFree( (oroDeviceptr)x_device ) );
		ORORTCCHECK( orortcLinkDestroy( rtc_link_state ) );
		ORORTCCHECK( oroModuleUnload( module ) );
	}
}

TEST_F( OroTestBase, link_bundledBc_with_bc_loweredName )
{
	oroDeviceProp props;
	OROCHECK( oroGetDeviceProperties( &props, m_device ) );
	int v;
	oroDriverGetVersion( &v );
	std::vector<char> data0;
	std::vector<char> data1;
	const bool isAmd = oroGetCurAPI( 0 ) == ORO_API_HIP;
	std::string arch = "-arch=sm_" + std::to_string( props.major ) + std::string( "0" );
	const char* funcName = "testKernel<0>";
	std::string loweredNameStr;
	orortcProgram prog;

	{
		std::string bcFile = isAmd ? "../UnitTest/bitcodes/moduleTestFunc-hip-amdgcn-amd-amdhsa.bc" : "../UnitTest/bitcodes/moduleTestFunc.fatbin";
		loadFile( bcFile.c_str(), data1 );
	}
	{
		std::vector<const char*> opts = isAmd ? std::vector<const char*>( { "-fgpu-rdc", "-c", "--cuda-device-only" } ) 
			: std::vector<const char*>( { "--device-c", arch.c_str() } );
		std::string code;

		OrochiUtils::readSourceCode( "../UnitTest/moduleTestKernel_loweredName.h", code );
		OrochiUtils::getProgram( m_device, code.c_str(), "../UnitTest/moduleTestKernel_loweredName.h", &opts, funcName, &prog );
		const char* loweredName = nullptr;
		ORORTCCHECK( orortcGetLoweredName( prog, funcName, &loweredName ) );
		OROASSERT(loweredName != nullptr);
		loweredNameStr = std::string( loweredName );

		size_t codeSize = 0;
		ORORTCCHECK( orortcGetBitcodeSize( prog, &codeSize ) );
		OROASSERT(codeSize != 0);
		data0.resize( codeSize );
		ORORTCCHECK( orortcGetBitcode( prog, data0.data() ) );
		ORORTCCHECK( orortcDestroyProgram( &prog ) );
	}

	{
		orortcLinkState rtc_link_state;
		orortcJIT_option options[7];
		void* option_vals[7];
		float wall_time;

		unsigned int log_size = 8192;
		char error_log[8192];
		char info_log[8192];

		options[0] = ORORTC_JIT_WALL_TIME;
		option_vals[0] = static_cast<void*>( &wall_time );

		options[1] = ORORTC_JIT_INFO_LOG_BUFFER;
		option_vals[1] = info_log;

		options[2] = ORORTC_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
		option_vals[2] = static_cast<void*>( &log_size );

		options[3] = ORORTC_JIT_ERROR_LOG_BUFFER;
		option_vals[3] = error_log;

		options[4] = ORORTC_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
		option_vals[4] = static_cast<void*>( &log_size ); // todo. behavior difference

		options[5] = ORORTC_JIT_LOG_VERBOSE;
		option_vals[5] = (void*)m_jitLogVerbose;;

		void* binary = nullptr;
		size_t binarySize = 0;
		const orortcJITInputType type0 = isAmd ? ORORTC_JIT_INPUT_LLVM_BITCODE : ORORTC_JIT_INPUT_CUBIN;
		const orortcJITInputType type1 = isAmd ? ORORTC_JIT_INPUT_LLVM_BUNDLED_BITCODE : ORORTC_JIT_INPUT_FATBINARY;
		ORORTCCHECK( orortcLinkCreate( 6, options, option_vals, &rtc_link_state ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type1, data1.data(), data1.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkAddData( rtc_link_state, type0, data0.data(), data0.size(), 0, 0, 0, 0 ) );
		ORORTCCHECK( orortcLinkComplete( rtc_link_state, &binary, &binarySize ) );
		OROASSERT(binary != nullptr);
		OROASSERT(binarySize != 0);

		oroFunction function = static_cast<oroFunction>(nullptr);;
		oroModule module;
		oroError ee = oroModuleLoadData( &module, binary );
		oroError e =  oroModuleGetFunction( &function, module, loweredNameStr.c_str() );
		OROASSERT(function != nullptr);

		int x_host = -1;
		int* x_device = nullptr;
		OROCHECK( oroMalloc( (oroDeviceptr*)&x_device, sizeof( int ) ) );
		OROCHECK( oroMemset( (oroDeviceptr)x_device, 0, sizeof( int ) ) );
		const void* args[] = { &x_device };

		OrochiUtils::launch1D( function, 64, args, 64 );
		OrochiUtils::waitForCompletion();
		OROCHECK( oroMemcpyDtoH( &x_host, (oroDeviceptr)x_device, sizeof( int ) ) );
		OROASSERT( x_host == 2016 );
		OROCHECK( oroFree( (oroDeviceptr)x_device ) );
		ORORTCCHECK( orortcLinkDestroy( rtc_link_state ) );
		ORORTCCHECK( oroModuleUnload( module ) );
	}
}

TEST_F( OroTestBase, getErrorString )
{
	oroError error = oroErrorInvalidValue;
	const char* str = nullptr;
	OROCHECK( oroGetErrorString( error, &str ) );
	const oroApi api = oroGetCurAPI( 0 );
	if( api == ORO_API_CUDADRIVER )
	{
		OROASSERT( str != nullptr );
	}
	else if( api == ORO_API_HIP )
	{
		// the 'str' will look like "invalid argument". But it may change with the versions and the system language.
		OROASSERT( str != nullptr );
	}
	else
	{
		// Unsupported api. This should not happen.
		OROASSERT( false );
	}
}

TEST_F( OroTestBase, funcPointer )
{
	OrochiUtils o;
	int a_host = -1;
	int* a_device = nullptr;
	OROCHECK( oroMalloc( (oroDeviceptr*)&a_device, sizeof( int ) ) );
	OROCHECK( oroMemset( (oroDeviceptr)a_device, 0, sizeof( int ) ) );
	oroFunction kernel;
	char* deviceBuffer = nullptr;
	oroModule module = nullptr;
	{
		std::string code;
		const char* path = "../UnitTest/testKernel.h";
		bool readSrc = OrochiUtils::readSourceCode( path, code );
		OROASSERT( readSrc );
		o.getModule( m_device, code.c_str(), path, 0, "testFuncPointerKernel", &module );
		OROCHECK( oroModuleGetFunction( &kernel, module, "testFuncPointerKernel" ) );
		{
			oroDeviceptr dFuncPtr = 0;
			size_t numBytes = 0;
			OROCHECK( oroModuleGetGlobal( &dFuncPtr, &numBytes, module, "gFuncPointer" ) );
			o.malloc( deviceBuffer, numBytes );
			o.copyDtoD( deviceBuffer, (char*)dFuncPtr, numBytes );
		}
	}
	const void* args[] = { &a_device, &deviceBuffer };
	OrochiUtils::launch1D( kernel, 64, args, 64 );
	OrochiUtils::waitForCompletion();
	OROCHECK( oroMemcpyDtoH( &a_host, (oroDeviceptr)a_device, sizeof( int ) ) );
	ASSERT_EQ(a_host, 7);
	ORORTCCHECK( oroModuleUnload( module ) );
	OROCHECK( oroFree( (oroDeviceptr)a_device ) );
	o.free( deviceBuffer );
	o.unloadKernelCache();
}

TEST_F( OroTestBase, ManagedMemory )
{
	OroStopwatch sw( m_stream );
	OrochiUtils o;
	constexpr auto streamSize = 64000000; //64 MB
	float* data = nullptr;
	float* output = nullptr;
	const float value = 10.0f;
	size_t n = streamSize / sizeof(float);
	enum TimerEvents { ManagedMemory, NonManagedMemory };

	{
		{
			sw.start();
			o.mallocManaged(data, n, oroManagedMemoryAttachFlags::oroMemAttachGlobal);
			OROASSERT(data != nullptr);
			o.mallocManaged(output, n, oroManagedMemoryAttachFlags::oroMemAttachGlobal);
			OROASSERT(output != nullptr);
			sw.stop();
			printf( "Managed Malloc Time: %3.2fms\n", sw.getMs() );
		}

		{
			auto kernel = o.getFunctionFromFile(m_device, "../UnitTest/testKernel.h", "streamData", 0);
			if ( !kernel )
			{
				printf("ERROR: kernel file failed to load.");
				ASSERT_TRUE(false);
				return;
			}
			const void* args[] = { &data, &n, &output, &value };

			sw.start();
			OrochiUtils::launch1D(kernel, 4096, args, 64);;
			sw.stop();
			OrochiUtils::waitForCompletion();
			printf( "Managed Memory kernelExec1: %3.2fms\n", sw.getMs() );
		}

		{
			sw.start();
			float* dataPtr = (float*)malloc(streamSize);
			o.copyDtoH(dataPtr, data, n);
			float* outputPtr = (float*)malloc(streamSize);
			o.copyDtoH(outputPtr, output, n);
			OrochiUtils::waitForCompletion();
			sw.stop();
			printf( "Host Copy Managed Exec: %3.2fms\n", sw.getMs() );

			for (size_t i = 0; i < n; i++)
			{
				dataPtr[i] += outputPtr[i];
			}

			o.copyHtoD(data, dataPtr, n);
			o.copyHtoD(output, outputPtr, n);
			
		}

		{
			auto kernel = o.getFunctionFromFile(m_device, "../UnitTest/testKernel.h", "streamData", 0);
			if ( !kernel )
			{
				printf("ERROR: kernel file failed to load.");
				ASSERT_TRUE(false);
				return;
			}
			const void* args[] = { &output, &n, &data, &value };

			sw.start();
			OrochiUtils::launch1D(kernel, 4096, args, 64);;
			sw.stop();
			OrochiUtils::waitForCompletion();
			printf( "Managed Memory kernelExec2: %3.2fms\n", sw.getMs() );
		}

		o.free(data);
		data = nullptr;
		o.free(output);
		output = nullptr;
		
	}

	{
		{
			sw.start();
			o.malloc(data, n);
			OROASSERT(data != nullptr);
			o.malloc(output, n);
			OROASSERT(output != nullptr);
			sw.stop();
			printf( "Malloc Time: %3.2fms\n", sw.getMs() );

		}

		{
			auto kernel = o.getFunctionFromFile(m_device, "../UnitTest/testKernel.h", "streamData", 0);
			if ( !kernel )
			{
				printf("ERROR: kernel file failed to load.");
				ASSERT_TRUE(false);
				return;
			}
			const void* args[] = { &data, &n, &output, &value };

			sw.start();
			OrochiUtils::launch1D(kernel, 4096, args, 64);
			sw.stop();
			OrochiUtils::waitForCompletion();
			printf( "Non Managed Memory kernelExec1: %3.2fms\n", sw.getMs() );
		}

		{
			sw.start();
			float* dataPtr = (float*)malloc(streamSize);
			o.copyDtoH(dataPtr, data, n);
			float* outputPtr = (float*)malloc(streamSize);
			o.copyDtoH(outputPtr, output, n);
			OrochiUtils::waitForCompletion();
			sw.stop();
			printf( "Host Copy Non Managed Exec: %3.2fms\n", sw.getMs() );
			for (size_t i = 0; i < n; i++)
			{
				dataPtr[i] += outputPtr[i];
			}

			o.copyHtoD(data, dataPtr, n);
			o.copyHtoD(output, outputPtr, n);
		}

		{
			auto kernel = o.getFunctionFromFile(m_device, "../UnitTest/testKernel.h", "streamData", 0);
			if ( !kernel )
			{
				printf("ERROR: kernel file failed to load.");
				ASSERT_TRUE(false);
				return;
			}
			const void* args[] = { &output, &n, &data, &value };

			sw.start();
			OrochiUtils::launch1D(kernel, 4096, args, 64);
			sw.stop();
			OrochiUtils::waitForCompletion();
			printf( "Non Managed Memory kernelExec2: %3.2fms\n", sw.getMs() );
		}

		o.free(data);
		data = nullptr;
		o.free(output);
		output = nullptr;
		
	}
	o.unloadKernelCache();
}




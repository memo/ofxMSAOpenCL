#pragma once

#include "ofMain.h"
#ifdef TARGET_OSX
	#include <OpenCL/Opencl.h>
#else
	#include <CL/opencl.h>
	#include <assert.h>
#endif

#include "MSAOpenCLKernel.h"
#include "MSAOpenCLProgram.h"
#include "MSAOpenCLBuffer.h"
#include "MSAOpenCLBufferManagedT.h"
#include "MSAOpenCLTypes.h"
#include "MSAOpenCLImage.h"
//#include "MSAOpenCLImagePingPong.h"



namespace msa {
	
	class OpenCL {
	public:
		OpenCL();
		~OpenCL();
		
		static OpenCL *currentOpenCL;

        // fills the deviceInfo vector and returns number of devices found
        // later you can pass these device numbers into setup or setupFromOpenGL
        int     getDeviceInfos(int clDeviceType = CL_DEVICE_TYPE_GPU);
        int     getNumDevices() { return deviceInfo.size(); }
		

        // initializes openCL with the passed in device number
        // use zero-based device number (0, 1, 2 etc) NOT clDeviceId
        // use getDeviceInfos to see what devices are available
        // or use deviceNumber = 0 to use first device found
        // or use deviceNumber = -1 to use last device found (best for Macs with multiple GPUs)
        // TODO: allow passing in multiple devices
		void	setup(int clDeviceType = CL_DEVICE_TYPE_GPU, int deviceNumber = -1);
		void	setupFromOpenGL(int deviceNumber = -1);
        
        
		cl_device_id&		getDevice();
		cl_context&			getContext();
		cl_command_queue&	getQueue();
		
		
		// doesn't return until all commands in the queue have been sent
		void	flush();
		
		
		// doesn't return until all commands in the queue have been sent and finished executing
		void	finish();	
		
		
		// load a program (contains a bunch of kernels)
		// returns pointer to the program should you need it (for most operations you won't need this)
		OpenCLProgramPtr 	loadProgramFromFile(string filename, bool isBinary = false);
		OpenCLProgramPtr 	loadProgramFromSource(string programSource);
		
		// specify a kernel to load from the specified program
		// if you leave the program parameter blank it will use the last loaded program
		// returns pointer to the kernel should you need it (for most operations you won't need this)
		OpenCLKernelPtr	loadKernel(string kernelName, OpenCLProgramPtr program = OpenCLProgramPtr());
		
		
		
		// create OpenCL buffer memory objects
		// if dataPtr parameter is passed in, data is uploaded immediately
		// parameters with default values can be omited
		OpenCLBuffer*	createBuffer(int numberOfBytes,
									 cl_mem_flags memFlags = CL_MEM_READ_WRITE,
									 void *dataPtr = NULL,
									 bool blockingWrite = CL_FALSE);
		
		// create buffer from the GL Object - e.g. VBO (they share memory space on device)
		// parameters with default values can be omited
		OpenCLBuffer*	createBufferFromGLObject(GLuint glBufferObject,
												 cl_mem_flags memFlags = CL_MEM_READ_WRITE);
		
		
		
		// create OpenCL image memory objects
		// if dataPtr parameter is passed in, data is uploaded immediately
		
		// create a 2D Image with given properties
		// Image is not linked to an OpenGL texture
		// parameters with default values can be omited
		OpenCLImage*		createImage2D(int width,
										  int height,
										  cl_channel_order imageChannelOrder = CL_RGBA,
										  cl_channel_type imageChannelDataType = CL_FLOAT,
										  cl_mem_flags memFlags = CL_MEM_READ_WRITE,
										  void *dataPtr = NULL,
										  bool blockingWrite = CL_FALSE);
		
		// create a 2D Image from the ofTexture passed in (they share memory space on device)
		// parameters with default values can be omited
		OpenCLImage*		createImageFromTexture(ofTexture &tex,
												   cl_mem_flags memFlags = CL_MEM_READ_WRITE,
												   int mipLevel = 0);
		
		
		// create both a 2D Image AND an ofTexture at the same time (they share memory space on device)
		// parameters with default values can be omited
		OpenCLImage*		createImageWithTexture(int width,
												   int height,
												   int glType = GL_RGBA,
												   cl_mem_flags memFlags = CL_MEM_READ_WRITE);
		
		
		
		// parameters with default values can be omited
		OpenCLImage*		createImage3D(int width,
										  int height,
										  int depth,
										  cl_channel_order imageChannelOrder = CL_RGBA,
										  cl_channel_type imageChannelDataType = CL_FLOAT,
										  cl_mem_flags memFlags = CL_MEM_READ_WRITE,
										  void *dataPtr = NULL,
										  bool blockingWrite = CL_FALSE);
		
		
		// retrieve kernel so you can run it or setup params etc.
		OpenCLKernelPtr	kernel(string kernelName);
		
		
		const map<string, OpenCLProgramPtr>&	getPrograms() {
			return programs;
		}
		
		// will delete any programs and release their GPU resources & kernels.
		void clearPrograms(){
			programs.clear();
		}
		
		map<string, OpenCLKernelPtr>	getKernels() {
			return kernels;
		}
		
        // get info for i'th device
		string getInfoAsString(int deviceNumber = 0);
		
		struct DeviceInfo {
			cl_platform_id clPlatformId; /// < platform this device belongs to
            cl_device_id clDeviceId;
			cl_char		vendorName[1024];
			cl_char		deviceName[1024];
			cl_char		driverVersion[1024];
			cl_char		deviceVersion[1024];
			cl_uint		maxComputeUnits;
			cl_uint		maxWorkItemDimensions;
			size_t		maxWorkItemSizes[32];
			size_t		maxWorkGroupSize;
			cl_uint		maxClockFrequency;
			cl_ulong	maxMemAllocSize;
			cl_bool		imageSupport;
			cl_uint		maxReadImageArgs;
			cl_uint		maxWriteImageArgs;
			size_t		image2dMaxWidth;
			size_t		image2dMaxHeight;
			size_t		image3dMaxWidth;
			size_t		image3dMaxHeight;
			size_t		image3dMaxDepth;
			cl_uint		maxSamplers;
			size_t		maxParameterSize;
			cl_ulong	globalMemCacheSize;
			cl_ulong	globalMemSize;
			cl_ulong	maxConstantBufferSize;
			cl_uint		maxConstantArgs;
			cl_ulong	localMemSize;
			cl_bool		errorCorrectionSupport;
			size_t		profilingTimerResolution;
			cl_bool		endianLittle;
			cl_char		profile[1024];
			cl_char		extensions[1024];		
		};
        
        vector<DeviceInfo> deviceInfo;
		
		
	protected:
		cl_device_id					clDevice;
		/*cl_platform_id					mPlatformID;*/
		cl_context						clContext;
		cl_command_queue				clQueue;
		
		map<string, OpenCLProgramPtr> programs;
		map<string, OpenCLKernelPtr>	 kernels;
		vector<OpenCLMemoryObject*>					 memObjects;
		bool										 isSetup;
		
		void createQueue();
	};
	
}
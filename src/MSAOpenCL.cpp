#include "MSAOpenCL.h"
#include "ofAppGLFWWindow.h"

namespace msa {

	OpenCL *OpenCL::currentOpenCL = NULL;


	OpenCL::OpenCL() {
		ofLog(OF_LOG_VERBOSE, "OpenCL::OpenCL");
		isSetup		= false;
		clContext	= NULL;
		clDevice	= NULL;
		clQueue		= NULL;
		mPlatformID = 0;
	}

	OpenCL::~OpenCL() {
		ofLog(OF_LOG_VERBOSE, "OpenCL::~OpenCL");

		clFinish(clQueue);

		for(int i=0; i<memObjects.size(); i++) delete memObjects[i];	// FIX
		for(auto it = kernels.begin(); it !=kernels.end(); ++it) it->second.reset();
		for(int i=0; i<programs.size(); i++) programs[i].reset();

		clReleaseCommandQueue(clQueue);
		clReleaseContext(clContext);
	}


	void OpenCL::setup(int clDeviceType, int numDevices) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::setup " + ofToString(clDeviceType) + ", " + ofToString(numDevices));

		if(isSetup) {
			ofLog(OF_LOG_VERBOSE, "... already setup. returning");
			return;
		}

		cl_int err;

		int numDevicesToUse = createDevice(clDeviceType, numDevices);
		clContext = clCreateContext(NULL, numDevicesToUse, &clDevice, NULL, NULL, &err);
		if(clContext == NULL) {
			ofLog(OF_LOG_ERROR, "Error creating clContext.");
			assert(err != CL_INVALID_PLATFORM);
			assert(err != CL_INVALID_VALUE);
			assert(err != CL_INVALID_DEVICE);
			assert(err != CL_INVALID_DEVICE_TYPE);
			assert(err != CL_DEVICE_NOT_AVAILABLE);
			assert(err != CL_DEVICE_NOT_FOUND);
			assert(err != CL_OUT_OF_HOST_MEMORY);
			assert(false);
		}


		createQueue();
	}	


	void OpenCL::setupFromOpenGL() {
		ofLog(OF_LOG_VERBOSE, "OpenCL::setupFromOpenGL ");

		if(isSetup) {
			ofLog(OF_LOG_VERBOSE, "... already setup. returning");
			return;
		}

		cl_int err;

		createDevice(CL_DEVICE_TYPE_GPU, 1);

#ifdef TARGET_OSX	
		CGLContextObj kCGLContext = CGLGetCurrentContext();
		CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
		cl_context_properties properties[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0 };
		clContext = clCreateContext(properties, 0, 0, NULL, NULL, &err);
#elif defined _WIN32
		//aqcuire shared context on windows.
		{
			// TODO: we want to be more specific about the platform,
			// at the moment only the first successful platform is selected. 
			// so platform defaults to 0

			HGLRC ctxt = static_cast<ofAppGLFWWindow*>(ofGetWindowPtr())->getWGLContext();

			HGLRC ctxt2 = wglGetCurrentContext();
			HDC dc = wglGetCurrentDC();

			cl_context_properties properties[] = 
			{
				CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
				CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
				CL_CONTEXT_PLATFORM, (cl_context_properties)mPlatformID, 
				0
			};
			ofLogNotice() << mPlatformID;
			cl_device_id id = getDevice();
			
			clContext = clCreateContext(properties, 1, &id, NULL, NULL, &err);
			ofLogNotice() << err;
		}

#endif
		if(clContext == NULL) {
			ofLog(OF_LOG_ERROR, "Error creating clContext.");
			assert(err != CL_INVALID_PLATFORM);
			assert(err != CL_INVALID_VALUE);
			assert(err != CL_INVALID_DEVICE);
			assert(err != CL_INVALID_DEVICE_TYPE);
			assert(err != CL_DEVICE_NOT_AVAILABLE);
			assert(err != CL_DEVICE_NOT_FOUND);
			assert(err != CL_OUT_OF_HOST_MEMORY);
			assert(false);
		}

		createQueue();
	}	


	cl_device_id& OpenCL::getDevice() {
		return clDevice;
	}


	cl_context& OpenCL::getContext() {
		return clContext;
	}

	cl_command_queue& OpenCL::getQueue() {
		return clQueue;
	}



	std::shared_ptr<OpenCLProgram>  OpenCL::loadProgramFromFile(string filename, bool isBinary) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::loadProgramFromFile");
		std::shared_ptr<OpenCLProgram> p = std::shared_ptr<OpenCLProgram> (new OpenCLProgram());
		p->loadFromFile(filename, isBinary);
		programs.push_back(p);
		return p;
	}


	std::shared_ptr<OpenCLProgram>  OpenCL::loadProgramFromSource(string source) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::loadProgramFromSource");
		std::shared_ptr<OpenCLProgram> p = std::shared_ptr<OpenCLProgram> (new OpenCLProgram());
		p->loadFromSource(source);
		programs.push_back(p);
		return p;
	} 


	std::shared_ptr<OpenCLKernel> OpenCL::loadKernel(string kernelName, std::shared_ptr<OpenCLProgram> program) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::loadKernel " + kernelName + ", " + ofToString((int)program.get()));
		if(program.get() == NULL) program = programs[programs.size() - 1];
		std::shared_ptr<OpenCLKernel> k = program->loadKernel(kernelName);
		kernels[kernelName] = k;
		return k;
	}


	OpenCLBuffer* OpenCL::createBuffer(int numberOfBytes, cl_mem_flags memFlags, void *dataPtr, bool blockingWrite) {
		OpenCLBuffer *clBuffer = new OpenCLBuffer();
		clBuffer->initBuffer(numberOfBytes, memFlags, dataPtr, blockingWrite);
		memObjects.push_back(clBuffer);
		return clBuffer;
	}


	OpenCLBuffer* OpenCL::createBufferFromGLObject(GLuint glBufferObject, cl_mem_flags memFlags) {
		OpenCLBuffer *clBuffer = new OpenCLBuffer();
		clBuffer->initFromGLObject(glBufferObject, memFlags);
		memObjects.push_back(clBuffer);
		return clBuffer;
	}


	OpenCLImage* OpenCL::createImage2D(int width, int height, cl_channel_order imageChannelOrder, cl_channel_type imageChannelDataType, cl_mem_flags memFlags, void *dataPtr, bool blockingWrite) {
		return createImage3D(width, height, 1, imageChannelOrder, imageChannelDataType, memFlags, dataPtr, blockingWrite);
	}


	OpenCLImage* OpenCL::createImageFromTexture(ofTexture &tex, cl_mem_flags memFlags, int mipLevel) {
		OpenCLImage *clImage = new OpenCLImage();
		clImage->initFromTexture(tex, memFlags, mipLevel);
		memObjects.push_back(clImage);
		return clImage;
	}

	OpenCLImage* OpenCL::createImageWithTexture(int width, int height, int glType, cl_mem_flags memFlags) {
		OpenCLImage *clImage = new OpenCLImage();
		clImage->initWithTexture(width, height, glType, memFlags);
		memObjects.push_back(clImage);
		return clImage;
	}


	OpenCLImage* OpenCL::createImage3D(int width, int height, int depth, cl_channel_order imageChannelOrder, cl_channel_type imageChannelDataType, cl_mem_flags memFlags, void *dataPtr, bool blockingWrite) {
		OpenCLImage *clImage = new OpenCLImage();
		clImage->initWithoutTexture(width, height, depth, imageChannelOrder, imageChannelDataType, memFlags, dataPtr, blockingWrite);
		memObjects.push_back(clImage);
		return clImage;
	}



	std::shared_ptr<OpenCLKernel> OpenCL::kernel(string kernelName) {
		// todo: check if kernel could be found.
		if (kernels.find(kernelName) != kernels.end()) {
			return kernels[kernelName];
		} else {
			ofLogError() << "Could not find kernel with name: " << kernelName;
			return std::shared_ptr<OpenCLKernel>();	// return empty shared ptr.
		}
	}

	void OpenCL::flush() {
		clFlush(clQueue);
	}


	void OpenCL::finish() {
		clFinish(clQueue);
	}



	int OpenCL::createDevice(int clDeviceType, int numDevices) {
		cl_int err;
		cl_uint numDevicesFound;

		cl_platform_id platformIdBuffer[100];
		cl_uint numPlatforms=0;

		//	windows AMD sdk/ati radeon driver implementation doesn't accept NULL as a platform ID, so fetch it first
		err = clGetPlatformIDs(	sizeof(platformIdBuffer)/sizeof(platformIdBuffer[0]), platformIdBuffer, &numPlatforms );

		//	error fetching platforms... try NULL anyway
		if ( err != CL_SUCCESS || numPlatforms == 0 )
		{
			platformIdBuffer[0] = NULL;
			numPlatforms = 1;
		}

		//	find first successfull platform
		for ( int p=0;	p<numPlatforms;	p++ )
		{
			mPlatformID = platformIdBuffer[p];
			err = clGetDeviceIDs(mPlatformID, clDeviceType, numDevices, &clDevice, &numDevicesFound);
			if ( err != CL_SUCCESS )
				continue;
		}

		ofLog(OF_LOG_VERBOSE, ofToString(numDevicesFound, 0) + " devices found, on " + ofToString(numPlatforms, 0) + " platforms\n");

		//	no platforms worked
		if ( err != CL_SUCCESS )
		{
			ofLog(OF_LOG_ERROR, "Error creating clDevice.");
			assert(false);
			return 0;
		}	

		int numDevicesToUse = min((int)numDevicesFound, numDevices);

		for(int i=0; i<numDevicesToUse; i++) {
			size_t	size;
			cl_device_id &d = (&clDevice)[i];
			err = clGetDeviceInfo(d, CL_DEVICE_VENDOR, sizeof(info.vendorName), info.vendorName, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_NAME, sizeof(info.deviceName), info.deviceName, &size);
			err |= clGetDeviceInfo(d, CL_DRIVER_VERSION, sizeof(info.driverVersion), info.driverVersion, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_VERSION, sizeof(info.deviceVersion), info.deviceVersion, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(info.maxComputeUnits), &info.maxComputeUnits, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(info.maxWorkItemDimensions), &info.maxWorkItemDimensions, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(info.maxWorkItemSizes), &info.maxWorkItemSizes, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(info.maxWorkGroupSize), &info.maxWorkGroupSize, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(info.maxClockFrequency), &info.maxClockFrequency, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(info.maxMemAllocSize), &info.maxMemAllocSize, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_IMAGE_SUPPORT, sizeof(info.imageSupport), &info.imageSupport, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(info.maxReadImageArgs), &info.maxReadImageArgs, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(info.maxWriteImageArgs), &info.maxWriteImageArgs, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(info.image2dMaxWidth), &info.image2dMaxWidth, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(info.image2dMaxHeight), &info.image2dMaxHeight, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(info.image3dMaxWidth), &info.image3dMaxWidth, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(info.image3dMaxHeight), &info.image3dMaxHeight, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(info.image3dMaxDepth), &info.image3dMaxDepth, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_SAMPLERS, sizeof(info.maxSamplers), &info.maxSamplers, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(info.maxParameterSize), &info.maxParameterSize, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(info.globalMemCacheSize), &info.globalMemCacheSize, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(info.globalMemSize), &info.globalMemSize, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(info.maxConstantBufferSize), &info.maxConstantBufferSize, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(info.maxConstantArgs), &info.maxConstantArgs, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(info.localMemSize), &info.localMemSize, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(info.errorCorrectionSupport), &info.errorCorrectionSupport, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(info.profilingTimerResolution), &info.profilingTimerResolution, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_ENDIAN_LITTLE, sizeof(info.endianLittle), &info.endianLittle, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_PROFILE, sizeof(info.profile), info.profile, &size);
			err |= clGetDeviceInfo(d, CL_DEVICE_EXTENSIONS, sizeof(info.extensions), info.extensions, &size);

			if(err != CL_SUCCESS) {
				ofLog(OF_LOG_ERROR, "Error getting clDevice information.");
				assert(false);
			}

			ofLog(OF_LOG_VERBOSE, getInfoAsString());
		}


		return numDevicesToUse;
	}

	string OpenCL::getInfoAsString() {
		return string("\n\n*********\nOpenCL Device information:") + 
			"\n vendorName.................." + string((char*)info.vendorName) + 
			"\n deviceName.................." + string((char*)info.deviceName) + 
			"\n driverVersion..............." + string((char*)info.driverVersion) +
			"\n deviceVersion..............." + string((char*)info.deviceVersion) +
			"\n maxComputeUnits............." + ofToString(info.maxComputeUnits, 0) +
			"\n maxWorkItemDimensions......." + ofToString(info.maxWorkItemDimensions, 0) +
			"\n maxWorkItemSizes[0]........." + ofToString(info.maxWorkItemSizes[0], 0) + 
			"\n maxWorkGroupSize............" + ofToString(info.maxWorkGroupSize, 0) +
			"\n maxClockFrequency..........." + ofToString(info.maxClockFrequency, 0) +
			"\n maxMemAllocSize............." + ofToString(info.maxMemAllocSize/1024.0f/1024.0f, 3) + " MB" + 
			"\n imageSupport................" + (info.imageSupport ? "YES" : "NO") +
			"\n maxReadImageArgs............" + ofToString(info.maxReadImageArgs, 0) +
			"\n maxWriteImageArgs..........." + ofToString(info.maxWriteImageArgs, 0) +
			"\n image2dMaxWidth............." + ofToString(info.image2dMaxWidth, 0) +
			"\n image2dMaxHeight............" + ofToString(info.image2dMaxHeight, 0) +
			"\n image3dMaxWidth............." + ofToString(info.image3dMaxWidth, 0) +
			"\n image3dMaxHeight............" + ofToString(info.image3dMaxHeight, 0) +
			"\n image3dMaxDepth............." + ofToString(info.image3dMaxDepth, 0) +
			"\n maxSamplers................." + ofToString(info.maxSamplers, 0) +
			"\n maxParameterSize............" + ofToString(info.maxParameterSize, 0) +
			"\n globalMemCacheSize.........." + ofToString(info.globalMemCacheSize/1024.0f/1024.0f, 3) + " MB" + 
			"\n globalMemSize..............." + ofToString(info.globalMemSize/1024.0f/1024.0f, 3) + " MB" +
			"\n maxConstantBufferSize......." + ofToString(info.maxConstantBufferSize/1024.0f, 3) + " KB"
			"\n maxConstantArgs............." + ofToString(info.maxConstantArgs, 0) +
			"\n localMemSize................" + ofToString(info.localMemSize/1024.0f, 3) + " KB"
			"\n errorCorrectionSupport......" + (info.errorCorrectionSupport ? "YES" : "NO") +
			"\n profilingTimerResolution...." + ofToString(info.profilingTimerResolution, 0) +
			"\n endianLittle................" + ofToString(info.endianLittle, 0) +
			"\n profile....................." + string((char*)info.profile) +
			"\n extensions.................." + string((char*)info.extensions) +
			"\n*********\n\n";
	}


	void OpenCL::createQueue() {
		int err = 0;
		clQueue = clCreateCommandQueue(clContext, clDevice, 0, &err);
		if(clQueue == NULL || err != CL_SUCCESS ) {
			ofLog(OF_LOG_ERROR, "Error creating command queue.");

		}

		isSetup = true;
		currentOpenCL = this;
	}

}

#include "MSAOpenCL.h"
#ifdef TARGET_OSX
#include <OpenCL/Opencl.h>
#include <OpenGL/OpenGL.h>
#endif

#include "MSAOpenCLProgram.h"

namespace msa {

	OpenCL *OpenCL::currentOpenCL = NULL;


	OpenCL::OpenCL() {
		ofLog(OF_LOG_VERBOSE, "OpenCL::OpenCL");
		isSetup		= false;
		clContext	= NULL;
		clDevice	= NULL;
		clQueue		= NULL;
	}

	OpenCL::~OpenCL() {
		clFinish(clQueue);

		for(int i=0; i<memObjects.size(); i++) delete memObjects[i];	// FIX
		kernels.clear();
		clearPrograms();

		clReleaseCommandQueue(clQueue);
		clReleaseContext(clContext);
		ofLog(OF_LOG_VERBOSE, "OpenCL::~OpenCL");
	}


	void OpenCL::setup(int clDeviceType_, int deviceNumber) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::setup " + ofToString(clDeviceType_));

		if(isSetup) {
			ofLog(OF_LOG_VERBOSE, "... already setup. returning");
			return;
		}

		if(deviceInfo.size() == 0) getDeviceInfos(clDeviceType_);
		deviceNumber = (deviceNumber + getNumDevices()) % getNumDevices();
		clDevice = deviceInfo[deviceNumber].clDeviceId;

		cl_int err;
		clContext = clCreateContext(NULL, 1, &clDevice, NULL, NULL, &err);
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


	void OpenCL::setupFromOpenGL(int deviceNumber) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::setupFromOpenGL ");

		if(isSetup) {
			ofLog(OF_LOG_VERBOSE, "... already setup. returning");
			return;
		}

		if(deviceInfo.size() == 0) getDeviceInfos(CL_DEVICE_TYPE_GPU);
		deviceNumber = (deviceNumber + getNumDevices()) % getNumDevices();
		clDevice = deviceInfo[deviceNumber].clDeviceId;
		cl_platform_id clPlatformID = deviceInfo[deviceNumber].clPlatformId;

		cl_int err;

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

			cl_context_properties properties[] = 
			{
				CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
				CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
				CL_CONTEXT_PLATFORM, (cl_context_properties)clPlatformID, 
				0
			};
			ofLogNotice() << "Using OpenCL Platform: 0x" << std::hex << clPlatformID;
			ofLogNotice() << "Using OpenCL Device: 0x" << std::hex << clDevice;

			clContext = clCreateContext(properties, 1, &clDevice, NULL, NULL, &err);
			ofLogNotice() <<  "Created OpenCL context: " << (err == CL_SUCCESS ? "SUCCESS" : "ERROR");
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



	OpenCLProgramPtr  OpenCL::loadProgramFromFile(string filename, bool isBinary) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::loadProgramFromFile");
		OpenCLProgramPtr p = OpenCLProgramPtr (new OpenCLProgram());
		p->loadFromFile(filename, isBinary);
		programs[filename] = p;
		return p;
	}


	OpenCLProgramPtr  OpenCL::loadProgramFromSource(string source) {
		static int program_counter = 0;
		/// TODO: maybe md5hash source to get a more reliable identifier for program.
		ofLog(OF_LOG_VERBOSE, "OpenCL::loadProgramFromSource");
		OpenCLProgramPtr p = OpenCLProgramPtr (new OpenCLProgram());
		p->loadFromSource(source);
		programs["#from_source_" + ofToString(program_counter++)] = p;
		return p;
	} 

	OpenCLKernelPtr OpenCL::loadKernel(string kernelName, OpenCLProgramPtr program) {
		ofLog(OF_LOG_VERBOSE, "OpenCL::loadKernel " + kernelName + ", " + ofToString((int)program.get()));
		if(program.get() == NULL) program = (programs.begin()->second);
		OpenCLKernelPtr k = program->loadKernel(kernelName);
		kernels[kernelName] = k;
		return k;
	}


	OpenCLBuffer* OpenCL::createBuffer(int numberOfBytes, cl_mem_flags memFlags, void *dataPtr, bool blockingWrite) {
		OpenCLBuffer *clBuffer = new OpenCLBuffer();
		clBuffer->initBuffer(numberOfBytes, memFlags, dataPtr);
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



	OpenCLKernelPtr OpenCL::kernel(string kernelName) {
		// todo: check if kernel could be found.
		if (kernels.find(kernelName) != kernels.end()) {
			return kernels[kernelName];
		} else {
			ofLogError() << "Could not find kernel with name: " << kernelName;
			return OpenCLKernelPtr();	// return empty shared ptr.
		}
	}

	void OpenCL::flush() {
		clFlush(clQueue);
	}


	void OpenCL::finish() {
		clFinish(clQueue);
	}



	int OpenCL::getDeviceInfos(int clDeviceType) {
		cl_int err;

		cl_uint numPlatforms=0;
		err = clGetPlatformIDs( NULL, NULL, &numPlatforms ); ///< first, only fetch number of platforms.
		vector<cl_platform_id> platformIdBuffer;
		platformIdBuffer.resize(numPlatforms); /// resize to correct size

		//	windows AMD sdk/ati radeon driver implementation doesn't accept NULL as a platform ID, so fetch it first
		err = clGetPlatformIDs(	numPlatforms, platformIdBuffer.data(), NULL);
		platformIdBuffer.resize(numPlatforms);

		//	error fetching platforms... try NULL anyway
		if ( err != CL_SUCCESS || numPlatforms == 0 )
		{
			platformIdBuffer[0] = NULL;
			numPlatforms = 1;
		}

		/// a map over all platforms and devices.
		map<cl_platform_id, vector<cl_device_id> > devicesPerPlatform;

		int totalDevicesFound = 0;
		//	find first successfull platform
		// TODO: what if there is more than one platform?
		for ( int p=0;	p < numPlatforms;	p++ ) {
			cl_platform_id platformId = platformIdBuffer[p];

			// first only retrieve the number of devices.
			cl_uint numDevices=0;
			err = clGetDeviceIDs(platformId, clDeviceType, NULL, NULL, &numDevices);
			if ( err == CL_SUCCESS ) {
				//--------! invariant: numDevices now holds the numer of devices in this particular platform.
				vector<cl_device_id> deviceIds;
				deviceIds.resize(numDevices); // make sure there's enough space in there.
				/// now retrieve any device ids from the OpenCL platform into the deviceIds vector.
				err = clGetDeviceIDs(platformId, clDeviceType, numDevices, deviceIds.data(), NULL);
				if ( err == CL_SUCCESS ) {
					/// now store all found devices into the map.
					devicesPerPlatform[platformId] = deviceIds;
					totalDevicesFound += deviceIds.size();
				}
			}
		}

		// reset err.
		err = 0;

		ofLog(OF_LOG_VERBOSE, ofToString(totalDevicesFound, 0) + " devices found, on " + ofToString(numPlatforms, 0) + " platforms\n");

		//	no platforms worked
		if ( totalDevicesFound == 0) {
			ofLog(OF_LOG_ERROR, "Error finding clDevices.");
			assert(false);
			return 0;
		}	


		deviceInfo.clear();

		// now we map over all platforms and devices and collect all data we need.

		for ( map<cl_platform_id, vector<cl_device_id> >::iterator it = devicesPerPlatform.begin(); it != devicesPerPlatform.end(); it++)
		{
			vector<cl_device_id>& devices = it->second;

			for (int i = 0; i<devices.size(); i++){ 
				DeviceInfo info;

				info.clDeviceId = devices[i];  ///< store deviceID
				info.clPlatformId = it->first; ///< store platformID with device

				err = clGetDeviceInfo(info.clDeviceId, CL_DEVICE_VENDOR, sizeof(info.vendorName), info.vendorName, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_NAME, sizeof(info.deviceName), info.deviceName, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DRIVER_VERSION, sizeof(info.driverVersion), info.driverVersion, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_VERSION, sizeof(info.deviceVersion), info.deviceVersion, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(info.maxComputeUnits), &info.maxComputeUnits, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(info.maxWorkItemDimensions), &info.maxWorkItemDimensions, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(info.maxWorkItemSizes), &info.maxWorkItemSizes, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(info.maxWorkGroupSize), &info.maxWorkGroupSize, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(info.maxClockFrequency), &info.maxClockFrequency, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(info.maxMemAllocSize), &info.maxMemAllocSize, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_IMAGE_SUPPORT, sizeof(info.imageSupport), &info.imageSupport, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(info.maxReadImageArgs), &info.maxReadImageArgs, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(info.maxWriteImageArgs), &info.maxWriteImageArgs, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(info.image2dMaxWidth), &info.image2dMaxWidth, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(info.image2dMaxHeight), &info.image2dMaxHeight, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(info.image3dMaxWidth), &info.image3dMaxWidth, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(info.image3dMaxHeight), &info.image3dMaxHeight, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(info.image3dMaxDepth), &info.image3dMaxDepth, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_SAMPLERS, sizeof(info.maxSamplers), &info.maxSamplers, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(info.maxParameterSize), &info.maxParameterSize, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(info.globalMemCacheSize), &info.globalMemCacheSize, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(info.globalMemSize), &info.globalMemSize, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(info.maxConstantBufferSize), &info.maxConstantBufferSize, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(info.maxConstantArgs), &info.maxConstantArgs, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(info.localMemSize), &info.localMemSize, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(info.errorCorrectionSupport), &info.errorCorrectionSupport, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(info.profilingTimerResolution), &info.profilingTimerResolution, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_ENDIAN_LITTLE, sizeof(info.endianLittle), &info.endianLittle, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_PROFILE, sizeof(info.profile), info.profile, NULL);
				err |= clGetDeviceInfo(info.clDeviceId, CL_DEVICE_EXTENSIONS, sizeof(info.extensions), info.extensions, NULL);

				deviceInfo.push_back(info);

				if(err != CL_SUCCESS) {
					ofLog(OF_LOG_ERROR, "Error getting clDevice information.");
					assert(false);
				}

				ofLog(OF_LOG_VERBOSE, getInfoAsString(i));
			}
		}

		return deviceInfo.size();
	}

	string OpenCL::getInfoAsString(int deviceNumber) {
		deviceNumber = (deviceNumber + getNumDevices()) % getNumDevices();

		DeviceInfo &info = deviceInfo[deviceNumber];
		return string("\n\n*********\nOpenCL Device information for device #") + ofToString(deviceNumber) +
			"\n cl_device_id................" + ofToString(info.clDeviceId) +
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

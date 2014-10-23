#include "MSAOpenCL.h"
#include "MSAOpenCLKernel.h"

namespace msa { 

	OpenCLKernel::OpenCLKernel(OpenCL* pOpenCL, cl_kernel clKernel, string name) {
		ofLog(OF_LOG_VERBOSE, "OpenCLKernel::OpenCLKernel " + ofToString((int)pOpenCL) + ", " + name);
		this->pOpenCL	= pOpenCL;
		this->name		= name;
		this->clKernel	= clKernel;
	}

	// ----------------------------------------------------------------------

	OpenCLKernel::~OpenCLKernel() {
		ofLog(OF_LOG_VERBOSE, "OpenCLKernel::~OpenCLKernel " + name);
		clReleaseKernel(clKernel);
	}

	// ----------------------------------------------------------------------


	const string getCLErrorString(cl_int error)
	{
		static const string errorString[] = {
			"CL_SUCCESS",
			"CL_DEVICE_NOT_FOUND",
			"CL_DEVICE_NOT_AVAILABLE",
			"CL_COMPILER_NOT_AVAILABLE",
			"CL_MEM_OBJECT_ALLOCATION_FAILURE",
			"CL_OUT_OF_RESOURCES",
			"CL_OUT_OF_HOST_MEMORY",
			"CL_PROFILING_INFO_NOT_AVAILABLE",
			"CL_MEM_COPY_OVERLAP",
			"CL_IMAGE_FORMAT_MISMATCH",
			"CL_IMAGE_FORMAT_NOT_SUPPORTED",
			"CL_BUILD_PROGRAM_FAILURE",
			"CL_MAP_FAILURE",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"CL_INVALID_VALUE",
			"CL_INVALID_DEVICE_TYPE",
			"CL_INVALID_PLATFORM",
			"CL_INVALID_DEVICE",
			"CL_INVALID_CONTEXT",
			"CL_INVALID_QUEUE_PROPERTIES",
			"CL_INVALID_COMMAND_QUEUE",
			"CL_INVALID_HOST_PTR",
			"CL_INVALID_MEM_OBJECT",
			"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
			"CL_INVALID_IMAGE_SIZE",
			"CL_INVALID_SAMPLER",
			"CL_INVALID_BINARY",
			"CL_INVALID_BUILD_OPTIONS",
			"CL_INVALID_PROGRAM",
			"CL_INVALID_PROGRAM_EXECUTABLE",
			"CL_INVALID_KERNEL_NAME",
			"CL_INVALID_KERNEL_DEFINITION",
			"CL_INVALID_KERNEL",
			"CL_INVALID_ARG_INDEX",
			"CL_INVALID_ARG_VALUE",
			"CL_INVALID_ARG_SIZE",
			"CL_INVALID_KERNEL_ARGS",
			"CL_INVALID_WORK_DIMENSION",
			"CL_INVALID_WORK_GROUP_SIZE",
			"CL_INVALID_WORK_ITEM_SIZE",
			"CL_INVALID_GLOBAL_OFFSET",
			"CL_INVALID_EVENT_WAIT_LIST",
			"CL_INVALID_EVENT",
			"CL_INVALID_OPERATION",
			"CL_INVALID_GL_OBJECT",
			"CL_INVALID_BUFFER_SIZE",
			"CL_INVALID_MIP_LEVEL",
			"CL_INVALID_GLOBAL_WORK_SIZE",
		};

		const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

		const int index = -error;

		return (index >= 0 && index < errorCount) ? errorString[index] : "Unspecified Error: " + ofToString(error);
	}

	// ----------------------------------------------------------------------

	cl_int OpenCLKernel::bindOpenGLInterOp(){
		cl_int err = CL_SUCCESS;
		if (!mOpenGLInteropArguments.empty()){
			// we have to acquire our opengl interop objects first.
			err = clEnqueueAcquireGLObjects(pOpenCL->getQueue(), mOpenGLInteropArguments.size() , mOpenGLInteropArguments.data(), 0, NULL, NULL);
		}
		return err;
	}

	cl_int OpenCLKernel::unbindOpenGLInterOp(){
		cl_int err = CL_SUCCESS;
		if (!mOpenGLInteropArguments.empty()){
			// we have to release our opengl interop objects, if we acquired them earlier.
			err = clEnqueueReleaseGLObjects(pOpenCL->getQueue(), mOpenGLInteropArguments.size() , mOpenGLInteropArguments.data(), 0, NULL, NULL);
		}
		return err;
	}

	// ----------------------------------------------------------------------

	void OpenCLKernel::run(int numDimensions, size_t *globalSize, size_t *localSize, cl_uint eventsInWaitList_, const cl_event* eventWaitList_, cl_event* runEvent_) {
		if (clKernel== NULL) return;
		cl_int err=CL_SUCCESS;
		bindOpenGLInterOp();
		err = clEnqueueNDRangeKernel(pOpenCL->getQueue(), clKernel, numDimensions, NULL, globalSize, localSize, eventsInWaitList_, eventWaitList_, runEvent_);
		if (err != CL_SUCCESS) {
			ofLogNotice() << getCLErrorString(err);
		}
		unbindOpenGLInterOp();
	}

	// ----------------------------------------------------------------------

	inline size_t roundToNextMultipleOf(size_t num_, size_t divisor_){
		if (divisor_ == 0) {
			ofLogError() << __FUNCTION__ << "division by zero!";
			// we return the full divisor, since this
			return divisor_;
		}
		return std::min(num_ % divisor_, size_t(1)) * divisor_ + (num_ / divisor_) * divisor_;
	}

	// ----------------------------------------------------------------------

	void OpenCLKernel::run1D(size_t globalSize, size_t localSize) {
		size_t globalSizes[1];

//		if (localSize > pOpenCL->info.maxWorkGroupSize) {
//			ofLogError() << "Could not run OpenCL 1D kernel at workgroup size: " << localSize << ". Max supported local (=workgroup) size: " << pOpenCL->info.maxWorkGroupSize;
//			return;
//		} 
		if(localSize > 0) {
			// tig: make sure localSize <= maxWorkGroupSize
			size_t localSizes[1];
			localSizes[0] = localSize;
			globalSizes[0] = roundToNextMultipleOf(globalSize,localSize);	// make sure global size is a multiple of local size
			run(1, globalSizes, localSizes);
		} else {
			globalSizes[0] = globalSize;
			run(1, globalSizes, NULL);
		}
	}

	// ----------------------------------------------------------------------

	void OpenCLKernel::run2D(size_t globalSizeX, size_t globalSizeY, size_t localSizeX, size_t localSizeY, cl_uint eventsInWaitList_ , const cl_event* eventWaitList_, cl_event* runEvent_ ) {
		// tig: make sure localSizeX * localSizeY <= maxWorkGroupSize
//		if (localSizeX * localSizeY > pOpenCL->info.maxWorkGroupSize) {
//			ofLogError() << "Could not run OpenCL 2D kernel at workgroup size: " << localSizeY * localSizeX << ". Max supported local (=workgroup) size: " << pOpenCL->info.maxWorkGroupSize;
//			return;
//		}
		size_t globalSizes[2] = { 0, 0 };
		if(localSizeY && localSizeX) {
			globalSizes[0] = roundToNextMultipleOf(globalSizeX,localSizeX);	// make sure global size is a multiple of local size, if localsize specified
			globalSizes[1] = roundToNextMultipleOf(globalSizeY,localSizeY);
			size_t localSizes[2];
			localSizes[0] = localSizeX;
			localSizes[1] = localSizeY;
			run(2, globalSizes, localSizes, eventsInWaitList_ , eventWaitList_, runEvent_);
		} else {
			// no local size specified - let driver figure out how to break up workload.
			globalSizes[0] = globalSizeX;
			globalSizes[1] = globalSizeY;
			run(2, globalSizes, NULL, eventsInWaitList_ , eventWaitList_, runEvent_);
		}
		/// note: if runEvent_ contains a cl_event pointer other than NULL,
		/// runEvent_ will be set to a unique identifier to this kernel execution instance
		/// as a side-effect.
	}

	// ----------------------------------------------------------------------

	void OpenCLKernel::run3D(size_t globalSizeX, size_t globalSizeY, size_t globalSizeZ, size_t localSizeX, size_t localSizeY, size_t localSizeZ) {
		// tig: make sure localSizeX * localSizeY * localSizeZ <= maxWorkGroupSize
//		if (localSizeX * localSizeY * localSizeZ > pOpenCL->info.maxWorkGroupSize) {
//			ofLogError() << "Could not run OpenCL 3D kernel at workgroup size: " << localSizeZ * localSizeY * localSizeX << ". Max supported local (=workgroup) size: " << pOpenCL->info.maxWorkGroupSize;
//			return;
//		}
		size_t globalSizes[3];
		globalSizes[0] = roundToNextMultipleOf(globalSizeX,localSizeX);	// make sure global sizes are a multiple of local size
		globalSizes[1] = roundToNextMultipleOf(globalSizeY,localSizeY);
		globalSizes[2] = roundToNextMultipleOf(globalSizeZ,localSizeZ);
		if(localSizeZ && localSizeY && localSizeX) {
			size_t localSizes[3];
			localSizes[0] = localSizeX;
			localSizes[1] = localSizeY;
			localSizes[2] = localSizeZ;
			run(3, globalSizes, localSizes);
		} else {
			run(3, globalSizes, NULL);
		}
	}

	// ----------------------------------------------------------------------

	cl_kernel& OpenCLKernel::getCLKernel() {
		return clKernel;
	}

	// ----------------------------------------------------------------------

	string OpenCLKernel::getName() {
		return name;
	}

	// ----------------------------------------------------------------------

	bool OpenCLKernel::setArg(int argNumber, void* argp_, size_t size_){
		if ( !clKernel ) return false;
		// ----------| invariant: we have a valid kernel.

		cl_int err = clSetKernelArg(clKernel, argNumber, size_, argp_);

		if (err != CL_SUCCESS) {
			ofLogNotice() << getCLErrorString(err);
		}
		return (err == CL_SUCCESS);
	}

	// ----------------------------------------------------------------------

	// assign buffer to arguments
	//	void setArg(int argNumber, cl_mem clMem);
//	bool  OpenCLKernel::setArg(int argNumber, float f) {
//		return setArg(argNumber, &f, sizeof(float));
//	}
//
//	// ----------------------------------------------------------------------
//
//	bool OpenCLKernel::setArg(int argNumber, int i){
//		return setArg(argNumber, &i, sizeof(int));
//	}

	// ----------------------------------------------------------------------

	bool OpenCLKernel::setArg(int argNumber, msa::OpenCLMemoryObject& memObject){
		// tig: if the buffer has a corresponding gl object, we need to flag it,
		// so that it can be bound and then unbound upon run().
		if ( !clKernel ) return false;
		// ----------| invariant: we have a valid kernel.

		// if this object has an openGL representation it needs to be flagged as blocked
		// whenever openCL runs on it. 
		// we'll do this automatically in run(), but we first have to register all objects 
		// that have such dependencies.
		if (memObject.hasCorrespondingGLObject){
			// make sure we haven't stored this object before.
			if (find(mOpenGLInteropArguments.begin(),mOpenGLInteropArguments.end(), memObject.clMemObject) == mOpenGLInteropArguments.end()){
				mOpenGLInteropArguments.push_back(memObject.clMemObject);
			}
		}
		bool result = setArg(argNumber, &memObject.clMemObject,  sizeof(&memObject.clMemObject));

		return result;
	}

	// ----------------------------------------------------------------------


	
}


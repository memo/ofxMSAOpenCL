#include "MSAOpenCL.h"
#include "MSAOpenCLKernel.h"

namespace msa { 
	
	OpenCLKernel::OpenCLKernel(OpenCL* pOpenCL, cl_kernel clKernel, string name) {
		ofLog(OF_LOG_VERBOSE, "OpenCLKernel::OpenCLKernel " + ofToString((int)pOpenCL) + ", " + name);
		this->pOpenCL	= pOpenCL;
		this->name		= name;
		this->clKernel	= clKernel;
	}
	
	
	OpenCLKernel::~OpenCLKernel() {
		ofLog(OF_LOG_VERBOSE, "OpenCLKernel::~OpenCLKernel " + name);
		clReleaseKernel(clKernel);
	}
	
	/*
	 void OpenCLKernel::setArg(int argNumber, cl_mem clMem) {
	 ofLog(OF_LOG_VERBOSE, "OpenCLKernel::setArg " + name + ": " + ofToString(argNumber));	
	 
	 assert(clKernel);
	 
	 cl_int err  = clSetKernelArg(clKernel, argNumber, sizeof(cl_mem), &clMem);
	 assert(err == CL_SUCCESS);
	 }*/
	
	void OpenCLKernel::run(int numDimensions, size_t *globalSize, size_t *localSize) {
		assert(clKernel);
		
		cl_int err;
		
		//	size_t localSize = MIN(n, info.maxWorkGroupSize);
		
		err = clEnqueueNDRangeKernel(pOpenCL->getQueue(), clKernel, numDimensions, NULL, globalSize, localSize, 0, NULL, NULL);
		assert(err == CL_SUCCESS);
	}


	inline size_t roundToNextMultipleOf(size_t num_, size_t divisor_){
		return std::min(num_ % divisor_, size_t(1)) * divisor_ + (num_ / divisor_) * divisor_;
	}
	
	void OpenCLKernel::run1D(size_t globalSize, size_t localSize) {
		size_t globalSizes[1];
		// tig: make sure localSize <= maxWorkGroupSize
		if (localSize > pOpenCL->info.maxWorkGroupSize) {
			ofLogError() << "Could not run OpenCL 1D kernel at workgroup size: " << localSize << ". Max supported local (=workgroup) size: " << pOpenCL->info.maxWorkGroupSize;
			return;
		}
		globalSizes[0] = roundToNextMultipleOf(globalSize,localSize);	// make sure global size is a multiple of local size
		if(localSize) {
			size_t localSizes[1];
			localSizes[0] = localSize;
			run(1, globalSizes, localSizes);
		} else {
			run(1, globalSizes, NULL);
		}
	}
	
	void OpenCLKernel::run2D(size_t globalSizeX, size_t globalSizeY, size_t localSizeX, size_t localSizeY) {
		// tig: make sure localSizeX * localSizeY <= maxWorkGroupSize
		if (localSizeX * localSizeY > pOpenCL->info.maxWorkGroupSize) {
			ofLogError() << "Could not run OpenCL 2D kernel at workgroup size: " << localSizeY * localSizeX << ". Max supported local (=workgroup) size: " << pOpenCL->info.maxWorkGroupSize;
			return;
		}
		size_t globalSizes[2];
		globalSizes[0] = roundToNextMultipleOf(globalSizeX,localSizeX);	// make sure global size is a multiple of local size
		globalSizes[1] = roundToNextMultipleOf(globalSizeY,localSizeY);
		if(localSizeY && localSizeX) {
			size_t localSizes[2];
			localSizes[0] = localSizeX;
			localSizes[1] = localSizeY;
			run(2, globalSizes, localSizes);
		} else {
			run(2, globalSizes, NULL);
		}
	}
	
	void OpenCLKernel::run3D(size_t globalSizeX, size_t globalSizeY, size_t globalSizeZ, size_t localSizeX, size_t localSizeY, size_t localSizeZ) {
		// tig: make sure localSizeX * localSizeY * localSizeZ <= maxWorkGroupSize
		if (localSizeX * localSizeY * localSizeZ > pOpenCL->info.maxWorkGroupSize) {
			ofLogError() << "Could not run OpenCL 3D kernel at workgroup size: " << localSizeZ * localSizeY * localSizeX << ". Max supported local (=workgroup) size: " << pOpenCL->info.maxWorkGroupSize;
			return;
		}
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
	
	
	cl_kernel& OpenCLKernel::getCLKernel() {
		return clKernel;
	}
	
	string OpenCLKernel::getName() {
		return name;
	}
}
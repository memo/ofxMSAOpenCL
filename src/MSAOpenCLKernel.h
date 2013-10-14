/***********************************************************************
 
 OpenCL Kernel
 Do not instanciate this directly.
 Instead use OpenCL::loadKernel or OpenCLProgram::loadKernel
 
 ************************************************************************/ 


#pragma once

#include "ofMain.h"
#include <OpenCL/Opencl.h>
#include "MSAOpenCLMemoryObject.h"


namespace msa { 
	class OpenCL;
	
	class OpenCLKernel {
		friend class OpenCLProgram;
		
	public:
		
		~OpenCLKernel();
		
		// assign buffer to arguments
		//	void setArg(int argNumber, cl_mem clMem);
		//	void setArg(int argNumber, float f);
		//	void setArg(int argNumber, int i);
		
		template<class T>
		bool setArg(int argNumber, T &arg){
			//		ofLog(OF_LOG_VERBOSE, "OpenCLKernel::setArg " + name + ": " + ofToString(argNumber));	
			assert(clKernel);
			if ( !clKernel )
				return false;
			
			cl_int err  = clSetKernelArg(clKernel, argNumber, sizeof(T), &arg);
			assert(err != CL_INVALID_KERNEL);
			assert(err != CL_INVALID_ARG_INDEX);
			assert(err != CL_INVALID_ARG_VALUE);
			assert(err != CL_INVALID_MEM_OBJECT);
			assert(err != CL_INVALID_SAMPLER);
			assert(err != CL_INVALID_ARG_SIZE);
			assert(err == CL_SUCCESS);
			return (err==CL_SUCCESS);
		}
		
		// run the kernel
		// globalSize and localSize should be int arrays with same number of dimensions as numDimensions
		// leave localSize blank to let OpenCL determine optimum
		void	run(int numDimensions, size_t *globalSize, size_t *localSize = NULL);
		
		// some wrappers for above to create the size arrays on the run
		void	run1D(size_t globalSize, size_t localSize = 0);
		void	run2D(size_t globalSizeX, size_t globalSizeY, size_t localSizeX = 0, size_t localSizeY = 0);
		void	run3D(size_t globalSizeX, size_t globalSizeY, size_t globalSizeZ, size_t localSizeX = 0, size_t localSizeY = 0, size_t localSizeZ = 0);
		
		cl_kernel& getCLKernel();
		string getName();
		
	protected:
		string			name;
		OpenCL*		pOpenCL;
		cl_kernel		clKernel;
		
		OpenCLKernel(OpenCL *pOpenCL, cl_kernel clKernel, string name);
	};
}

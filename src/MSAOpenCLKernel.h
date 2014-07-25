/***********************************************************************

OpenCL Kernel
Do not instanciate this directly.
Instead use OpenCL::loadKernel or OpenCLProgram::loadKernel

************************************************************************/ 


#pragma once

#include "ofMain.h"
#ifdef __APPLE__
#include <OpenCL/Opencl.h>
#else
#include <CL/opencl.h>
#endif
#include "MSAOpenCLMemoryObject.h"


namespace msa { 
	// ff decl
	class OpenCL;
	class OpenCLBuffer;

	// ----------

	class OpenCLKernel {
		friend class OpenCLProgram;

	public:

		~OpenCLKernel();

		// assign buffer to arguments
		//	void setArg(int argNumber, cl_mem clMem);
		bool setArg(int argNumber, float f);
		bool setArg(int argNumber, int i);
		bool setArg(int argNumber, OpenCLMemoryObject& buf);

		bool setArg(int argNumber, void* argp_, size_t size_);

		// run the kernel
		// globalSize and localSize should be int arrays with same number of dimensions as numDimensions
		// leave localSize blank to let OpenCL determine optimum
		cl_int  bindOpenGLInterOp();
		void	run(int numDimensions, size_t *globalSize, size_t *localSize = NULL, cl_uint eventsInWaitList_ = 0, const cl_event* eventWaitList_ = NULL, cl_event* runEvent_ = NULL);
		cl_int  unbindOpenGLInterOp();

		// some wrappers for above to create the size arrays on the run
		void	run1D(size_t globalSize, size_t localSize = 0);
		void	run2D(size_t globalSizeX, size_t globalSizeY, size_t localSizeX = 0, size_t localSizeY = 0, cl_uint eventsInWaitList_ = 0, const cl_event* eventWaitList_ = NULL, cl_event* runEvent_ = NULL);
		void	run3D(size_t globalSizeX, size_t globalSizeY, size_t globalSizeZ, size_t localSizeX = 0, size_t localSizeY = 0, size_t localSizeZ = 0);


		cl_kernel& getCLKernel();
		string getName();

	protected:
		string			name;
		OpenCL*		pOpenCL;
		cl_kernel		clKernel;

		OpenCLKernel(OpenCL *pOpenCL, cl_kernel clKernel, string name);
	private:
		/// holds a list of all objects we need to flag up to openGL through 
		/// clEnqueueAcquireGLObjects before we run.
		vector<cl_mem>	mOpenGLInteropArguments;
	};
}

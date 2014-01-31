/***********************************************************************
 
 OpenCL Memory Object base class for Images and Buffers
 Do not instantiate this class
 
 ************************************************************************/

#pragma once

#include "ofMain.h"
	#ifdef __APPLE__
#include <OpenCL/Opencl.h>
#else
	#include <CL/opencl.h>
#endif


namespace msa {
	class OpenCL;
	
	class OpenCLMemoryObject {
		friend class OpenCLKernel;
		
	public:
		virtual ~OpenCLMemoryObject();
		
		cl_mem	&getCLMem();
		
		operator cl_mem&() {
			return getCLMem();
		}
		OpenCLMemoryObject();
		
		/// Takes ownership of corresponding OpenGL object, if any
		bool lockGLObject();
		/// Releases ownership of corresponding OpenGL object, if any
		bool unlockGLObject();
		
	protected:
		
		cl_mem		clMemObject;
		OpenCL*		pOpenCL;
		void memoryObjectInit();

		bool hasCorrespondingGLObject;
		bool hasGLObjectOwnership;
		
	};
}
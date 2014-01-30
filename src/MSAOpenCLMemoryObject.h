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
		
	public:
		virtual ~OpenCLMemoryObject();
		
		cl_mem	&getCLMem();
		
		operator cl_mem&() {
			return getCLMem();
		}
		
		
		
	protected:
		OpenCLMemoryObject();
		cl_mem		clMemObject;
		OpenCL*		pOpenCL;
		
		void memoryObjectInit();
	};
}
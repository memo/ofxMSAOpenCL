/***********************************************************************
 
 OpenCL Memory Object base class for Images and Buffers
 Do not instantiate this class

 ************************************************************************/

#pragma once

#include "ofMain.h"
#include <OpenCL/Opencl.h>

namespace msa { 
	class OpenCL;
	
	class OpenCLMemoryObject {
		
	public:
		virtual ~OpenCLMemoryObject();
		
        cl_mem& getCLMem() { return clMemObject; }
		operator cl_mem&() { return getCLMem(); }
		
	protected:
		OpenCLMemoryObject();
		cl_mem		clMemObject;
		OpenCL*		pOpenCL;
		
		void memoryObjectInit();
	};
}
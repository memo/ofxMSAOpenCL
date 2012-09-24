#include "MSAOpenCL.h"
#include "MSAOpenCLMemoryObject.h"

namespace msa { 
	
	OpenCLMemoryObject::OpenCLMemoryObject() {
		ofLog(OF_LOG_VERBOSE, "OpenCLMemoryObject::OpenCLMemoryObject");
		pOpenCL = NULL;
		clMemObject = NULL;
	}
	
	
	OpenCLMemoryObject::~OpenCLMemoryObject() {
		ofLog(OF_LOG_VERBOSE, "OpenCLMemoryObject::~OpenCLMemoryObject");
		if(clMemObject) clReleaseMemObject(clMemObject);
	}
	
	
	cl_mem &OpenCLMemoryObject::getCLMem() {
		return clMemObject;
	}
	
	void OpenCLMemoryObject::memoryObjectInit() {
		ofLog(OF_LOG_VERBOSE, "OpenCLMemoryObject::memoryObjectInit");
		pOpenCL = OpenCL::currentOpenCL;
	}
}
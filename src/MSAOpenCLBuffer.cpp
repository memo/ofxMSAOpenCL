#include "MSAOpenCL.h"
#include "MSAOpenCLBuffer.h"

namespace msa {
	
	OpenCLBuffer::OpenCLBuffer() {
		ofLog(OF_LOG_VERBOSE, "OpenCLBuffer::OpenCLBuffer");
	}
	
	void OpenCLBuffer::initBuffer(int numberOfBytes,
								  cl_mem_flags memFlags,
								  void *dataPtr)
	{
		
		ofLog(OF_LOG_VERBOSE, "OpenCLBuffer::initBuffer");
		
		init();
		
		cl_int err;
		clMemObject = clCreateBuffer(pOpenCL->getContext(), memFlags, numberOfBytes, memFlags & CL_MEM_USE_HOST_PTR ? dataPtr : NULL, &err);
		assert(err == CL_SUCCESS);
		assert(clMemObject);
		
		if(dataPtr) write(dataPtr, 0, numberOfBytes);
	}
	
	
	void OpenCLBuffer::initFromGLObject(GLuint glBufferObject,
										cl_mem_flags memFlags)
	{	
		ofLog(OF_LOG_VERBOSE, "OpenCLBuffer::initFromGLObject");
		
		init();
		
		cl_int err;
		clMemObject= clCreateFromGLBuffer(pOpenCL->getContext(), memFlags, glBufferObject, &err);
		assert(err != CL_INVALID_CONTEXT);
		assert(err != CL_INVALID_VALUE);
		assert(err != CL_INVALID_GL_OBJECT);
		assert(err != CL_OUT_OF_HOST_MEMORY);
		assert(err == CL_SUCCESS);
		assert(clMemObject);	

		hasCorrespondingGLObject = true;
	}
	
	
	void OpenCLBuffer::read(void *dataPtr, int startOffsetBytes, int numberOfBytes, bool blockingRead) {
		if (hasCorrespondingGLObject) lockGLObject();
		cl_int err = clEnqueueReadBuffer(pOpenCL->getQueue(), clMemObject, blockingRead, startOffsetBytes, numberOfBytes, dataPtr, 0, NULL, NULL);
		if (hasCorrespondingGLObject) unlockGLObject();
		assert(err == CL_SUCCESS);
	}
	
	
	void OpenCLBuffer::write(void *dataPtr, int startOffsetBytes, int numberOfBytes) {
		if (hasCorrespondingGLObject) lockGLObject();
		cl_int err = clEnqueueWriteBuffer(pOpenCL->getQueue(), clMemObject, CL_TRUE, startOffsetBytes, numberOfBytes, dataPtr, 0, NULL, NULL);
		if (hasCorrespondingGLObject) unlockGLObject();
		assert(err == CL_SUCCESS);
	}

	void OpenCLBuffer::writeAsync(void *dataPtr, int startOffsetBytes, int numberOfBytes, cl_event& writeEvent_) {
		if (hasCorrespondingGLObject) lockGLObject();
		cl_int err = clEnqueueWriteBuffer(pOpenCL->getQueue(), clMemObject, CL_FALSE, startOffsetBytes, numberOfBytes, dataPtr, 0, NULL, &writeEvent_);
		if (hasCorrespondingGLObject) unlockGLObject();
		assert(err == CL_SUCCESS);
	}
	
	void OpenCLBuffer::copyFrom(OpenCLBuffer &srcBuffer, int srcOffsetBytes, int dstOffsetBytes, int numberOfBytes) {
		if (hasCorrespondingGLObject) lockGLObject();
		cl_int err = clEnqueueCopyBuffer(pOpenCL->getQueue(), srcBuffer.getCLMem(), clMemObject, srcOffsetBytes, dstOffsetBytes, numberOfBytes, 0, NULL, NULL);
		if (hasCorrespondingGLObject) unlockGLObject();
		assert(err == CL_SUCCESS);
	}
	
	
	void OpenCLBuffer::init() {
		memoryObjectInit();
	}
}

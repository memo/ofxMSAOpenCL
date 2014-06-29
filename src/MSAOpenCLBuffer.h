/***********************************************************************
 
 OpenCL Buffer Memory Object Low level wrapper
 Allocates and manages only device side buffer
 You can either instantiate this class directly. e.g.:
 OpenCLBuffer myBuffer;
 myBuffer.initFromGLObject(myVbo);
 
 or create it via Instead use OpenCL::createBufferXXXX. e.g.:
 OpenCLBuffer *myBuffer = openCL.createBufferFromGLObject(myVBO);
 (this method is here for backwards compatibility with previous versions of OpenCL)
 
 ************************************************************************/

#pragma once

#include "ofMain.h"
#include <OpenCL/Opencl.h>
#include "MSAOpenCLMemoryObject.h"

namespace msa {
 	
	class OpenCLBuffer : public OpenCLMemoryObject {
	public:
		
		OpenCLBuffer();
		
		// if dataPtr parameter is passed in, data is uploaded immediately
		// parameters with default values can be omited
		void initBuffer(	int numberOfBytes,
						cl_mem_flags memFlags = CL_MEM_READ_WRITE,
						void *dataPtr = NULL,
						bool blockingWrite = CL_FALSE);
		
		
		// create buffer from the GL Object - e.g. VBO (they share memory space on device)
		// parameters with default values can be omited
		void initFromGLObject(	GLuint glBufferObject,
							  cl_mem_flags memFlags = CL_MEM_READ_WRITE);
		
		
		// read from device memory, into main memory (into dataPtr)
        // note: offset and count is in bytes
		void read(void *dataPtr,
				  int startOffsetBytes,
				  int numberOfBytes,
				  bool blockingRead = CL_TRUE);
		
		// write from main memory (dataPtr), into device memory
        // note: offset and count is in bytes
		void write(void *dataPtr,
				   int startOffsetBytes,
				   int numberOfBytes,
				   bool blockingWrite = CL_FALSE);
		
		
		// copy data from another object on device memory
		void copyFrom(OpenCLBuffer &srcBuffer,
					  int srcOffsetBytes,
					  int dstOffsetBytes,
					  int numberOfBytes);
		
	protected:
		//	int numberOfBytes;		//dont know how big it is if we pass in globject ?
		
		void init();
	};
}

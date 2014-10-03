/***********************************************************************

OpenCL Buffer Memory Object High level wrapper
Allocates and manages both device and host buffer
Instantiate this class directly. e.g.:
OpenCLBufferManagerT myBuffer<Particle>;
myBuffer.initBuffer(20);

************************************************************************/

#pragma once

#include "ofMain.h"
#ifdef TARGET_OSX
#include <OpenCL/Opencl.h>
#else
#include <CL/opencl.h>
#include <assert.h>
#endif
#include "MSAOpenCLBuffer.h"
#include "MSAOpenCLMemoryObject.h"

namespace msa {

	template <typename T>
	class OpenCLBufferManagedT {
	public:

		//		OpenCLBufferManagedT();

		// allocates
		// parameters with default values can be omitted
		void initBuffer(int numberOfItems, void *dataPtr = NULL, cl_mem_flags memFlags = CL_MEM_READ_WRITE) {
			initData(numberOfItems, dataPtr);
			_clBuffer.initBuffer(sizeof(T) * size(), memFlags, &_data[0]);
		};

		// create buffer from the GL Object - e.g. VBO (they share memory space on device)
		// parameters with default values can be omited
		void initFromGLObject(GLuint glBufferObject, int numberOfItems, void *dataPtr = NULL, cl_mem_flags memFlags = CL_MEM_READ_WRITE) {
			initData(numberOfItems, dataPtr);
			_clBuffer.initFromGLObject(glBufferObject, memFlags);
		};


		// read from device memory, into main memory
		// note: offset and count is in ITEM COUNT not bytes
		void readFromDevice(int startOffsetItems = 0, int numberOfItems = 0, bool blockingRead = CL_TRUE) {
			if(numberOfItems == 0) numberOfItems = size();
			_clBuffer.read(&_data[0], startOffsetItems * sizeof(T), numberOfItems * sizeof(T), blockingRead);
		};

		// write from main memory into device memory
		// note: offset and count is in ITEM COUNT not bytes
		void writeToDevice(int startOffsetItems = 0, int numberOfItems = 0, bool blockingWrite = CL_FALSE) {
			if(numberOfItems == 0) numberOfItems = size();
			_clBuffer.write(&_data[0], startOffsetItems * sizeof(T), numberOfItems * sizeof(T), blockingWrite);
		};

		T& operator[](int i) { return _data[i]; }
		int size() const { return _data.size(); }

		OpenCLBuffer& getCLBuffer() { return _clBuffer; }
		cl_mem& getCLMem() { return _clBuffer.getCLMem(); }
		operator cl_mem&() { return getCLMem(); }



	protected:
		vector<T> _data;
		OpenCLBuffer _clBuffer;

		void initData(int numberOfItems, void *dataPtr){
			_data.resize(numberOfItems);
			if(dataPtr) memcpy(&_data[0], dataPtr, sizeof(T) * size());
		};
	};

	//    template <typename T>
	//    void OpenCLBufferManagedT<T>::OpenCLBufferManagedT::copyFrom(OpenCLBufferManagedT &srcBuffer, int srcOffsetBytes, int dstOffsetBytes, int numberOfBytes) {
	//        cl_int err = clEnqueueCopyBuffer(pOpenCL->getQueue(), srcBuffer.getCLMem(), clMemObject, srcOffsetBytes, dstOffsetBytes, numberOfBytes, 0, NULL, NULL);
	//        assert(err == CL_SUCCESS);
	//    }
	//    


}

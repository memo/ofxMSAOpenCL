#include "MSAOpenCL.h"
#include "MSAOpenCLMemoryObject.h"

namespace msa { 
	
	OpenCLMemoryObject::OpenCLMemoryObject()
	: pOpenCL(NULL)
	, clMemObject(NULL)
	, hasCorrespondingGLObject(false)
	, hasGLObjectOwnership(false)
	{
	}
	
	// ----------------------------------------------------------------------
	
	OpenCLMemoryObject::~OpenCLMemoryObject() {
		ofLog(OF_LOG_VERBOSE, "OpenCLMemoryObject::~OpenCLMemoryObject");
		if(clMemObject) clReleaseMemObject(clMemObject);
	}
	
	// ----------------------------------------------------------------------
	
	void OpenCLMemoryObject::memoryObjectInit() {
		ofLog(OF_LOG_VERBOSE, "OpenCLMemoryObject::memoryObjectInit");
		pOpenCL = OpenCL::currentOpenCL;
	}
	
	// ----------------------------------------------------------------------
	
	bool OpenCLMemoryObject::lockGLObject(){

		/// we'll always return true if there's no OpenGL object to lock.
		if (!hasCorrespondingGLObject) return true;
		
		
		if (hasGLObjectOwnership) {
			ofLogNotice() << "Attempt to lock same openGL object twice for OpenCL. OpenCL already has ownership.";
			return false;
		}
		
			cl_int err = clEnqueueAcquireGLObjects(pOpenCL->getQueue(), 1 , &clMemObject, 0, NULL, NULL);

		if (err != CL_SUCCESS){
			ofLogNotice() << "could not acquire gl object";
			return false;
		}
		//----------| invariant: we were successful in acquiring our object.
		hasGLObjectOwnership = true;
		return true;
	}
	
	// ----------------------------------------------------------------------
	
	bool OpenCLMemoryObject::unlockGLObject(){
		// we'll always return true if there's no OpenGL object to unlock.
		if (!hasCorrespondingGLObject) return true;
		
		
		if ( !hasGLObjectOwnership) {
			ofLogNotice() << "Attempt to unlock openGL object that was not in OpenCL ownership.";
			return false;
		}
		
		cl_int err = clEnqueueReleaseGLObjects(pOpenCL->getQueue(), 1 , &clMemObject, 0, NULL, NULL);
		
		if (err != CL_SUCCESS){
			ofLogNotice() << "could not acquire gl object";
			return false;
		}
		//----------| invariant: we were successful in releasing our object.
		hasGLObjectOwnership = false;
		return true;
	}
}

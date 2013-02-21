#pragma once

#include "ofMain.h"
#include "MSAPingPong.h"
#include "MSAOpenCLImage.h"

namespace msa {
	
	class OpenCLImagePingPong : public PingPong<OpenCLImage> {
	public:
		
		void initWithTexture(int width, int height, int glType) {
			objects[0].initWithTexture(width, height, glType);
			objects[1].initWithTexture(width, height, glType);
		}
		
		cl_mem &getCLMem() {
			return getTarget().getCLMem();
		}
		
		operator OpenCLImage&() {
			return getTarget();
		}
		
		
		void draw(int x, int y) {
			getTarget().draw(x, y);
		}
		
		void draw(int x, int y, int w, int h) {
			getTarget().draw(x, y, w, h);
		}
		
		int getWidth() {
			return getTarget().getWidth();
		}
		
		int getHeight() {
			return getTarget().getHeight();
		}
		
		int getDepth() {
			return getTarget().getDepth();
		}
		
		void reset() {
			objects[0].reset();
			objects[1].reset();
		}
		
		
		// read from device memory, into main memoy (into dataPtr)
		// if origin and/or region is NULL, entire image is read
		void read(void *dataPtr,
				  bool blockingRead = CL_TRUE,
				  size_t *pOrigin = NULL,
				  size_t *pRegion = NULL,
				  size_t rowPitch = 0,
				  size_t slicePitch = 0) {
			getTarget().read(dataPtr, blockingRead, pOrigin, pRegion, rowPitch, slicePitch);
		}
		
		// write from main memory (dataPtr), into device memory
		// if origin and/or region is NULL, entire image is written
		void write(void *dataPtr,
				   bool blockingWrite = CL_FALSE,
				   size_t *pOrigin = NULL,
				   size_t *pRegion = NULL,
				   size_t rowPitch = 0,
				   size_t slicePitch = 0) {
			getTarget().write(dataPtr, blockingWrite, pOrigin, pRegion, rowPitch, slicePitch);
		}
		
		
		// copy data from another image
		// if origin and/or region is NULL, entire image is written
		void copyFrom(OpenCLImage &srcImage,
					  size_t *pSrcOrigin = NULL, 
					  size_t *pDstOrigin = NULL, 
					  size_t *pRegion = NULL) {
			getTarget().copyFrom(srcImage, pSrcOrigin, pDstOrigin, pRegion);
		}
	};
	
}
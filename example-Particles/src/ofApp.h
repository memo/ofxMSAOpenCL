/******
 
 This example updates 1M particles on the GPU using OpenCL
 The OpenCL kernel writes position data directly to a VBO stored in the OpenGL device memory
 so now data transfer between host and device during runtime
 
 
 Kernel based on Rui's ofxOpenCL particle example opencl particles 001b.zip
 at http://code.google.com/p/ruisource/
 *****/






#pragma once


#include "ofMain.h"


class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
};





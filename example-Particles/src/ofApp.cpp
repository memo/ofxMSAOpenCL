/******
 
 This example updates 1M particles on the GPU using OpenCL
 The OpenCL kernel writes position data directly to a VBO stored in the OpenGL device memory
 so now data transfer between host and device during runtime
 
 
 Kernel based on Rui's ofxOpenCL particle example opencl particles 001b.zip 
 at http://code.google.com/p/ruisource/ 
 *****/


#include "ofApp.h"
#include "MSAOpenCL.h"

#define NUM_PARTICLES (1000000)


typedef struct{
	float2 vel;
	float mass;
	float dummy;		// need this to make sure the float2 vel is aligned to a 16 byte boundary
} Particle;


float2				mousePos;
float2				dimensions;


msa::OpenCL			opencl;

msa::OpenCLBufferManagedT<Particle>	particles; // vector of Particles on host and corresponding clBuffer on device

float2				particlesPos[NUM_PARTICLES];
msa::OpenCLBufferManagedT<float2> particlePos; // vector of particle positions on host and corresponding clBuffer, and vbo on device

GLuint				vbo;


//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0, 0, 0);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);
	
	opencl.setupFromOpenGL();
    
    // create vbo
    glGenBuffersARB(1, &vbo);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float2) * NUM_PARTICLES, 0, GL_DYNAMIC_COPY_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    // init host and CL buffers
    particles.initBuffer(NUM_PARTICLES);
    particlePos.initFromGLObject(vbo, NUM_PARTICLES);

    // init data
	
	for(int i=0; i<NUM_PARTICLES; i++) {
		Particle &p = particles[i];
		p.vel.set(0, 0);
		p.mass = ofRandom(0.5, 1);		
		particlePos[i].set(ofRandomWidth(), ofRandomHeight());
	}
	
    particles.writeToDevice();
    particlePos.writeToDevice(); ///< uploads buffer data to shared CL/GL memory, so the vbo and the cl buffer are written in one go, since they occupy the same memory locations.
	
	opencl.loadProgramFromFile("MSAOpenCL/Particle.cl");
	opencl.loadKernel("updateParticle");

	opencl.kernel("updateParticle")->setArg(0, particles);
	opencl.kernel("updateParticle")->setArg(1, particlePos);
    opencl.kernel("updateParticle")->setArg(2, mousePos);//.getPtr(), sizeof(float2));
    opencl.kernel("updateParticle")->setArg(3, dimensions);//.getPtr(), sizeof(float2));
	
	glPointSize(1);
}




//--------------------------------------------------------------
void ofApp::update(){
	
	mousePos.x = ofGetMouseX();
	mousePos.y = ofGetMouseY();
	dimensions.x = ofGetWidth();
	dimensions.y = ofGetHeight();
	
    opencl.kernel("updateParticle")->setArg(2, mousePos);//.getPtr(), sizeof(float2));
    opencl.kernel("updateParticle")->setArg(3, dimensions);//.getPtr(), sizeof(float2) );
	glFlush();
	
	opencl.kernel("updateParticle")->run1D(NUM_PARTICLES);
}


//--------------------------------------------------------------
void ofApp::draw(){
	opencl.finish();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);

	opencl.finish();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	
	
	glColor3f(1, 1, 1);
	string info = "fps: " + ofToString(ofGetFrameRate()) + "\nnumber of particles: " + ofToString(NUM_PARTICLES);
	ofDrawBitmapString(info, 20, 20);
}

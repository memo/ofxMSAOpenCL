/******
 
 This example updates 1M particles on the GPU using OpenCL
 The OpenCL kernel writes position data directly to a VBO stored in the OpenGL device memory
 so now data transfer between host and device during runtime
 
 Based on Rui's ofxOpenCL particle example opencl particles 001b.zip 
 at http://code.google.com/p/ruisource/ 
 *****/


#include "ofApp.h"
#include "MSAOpenCL.h"


#define USE_OPENGL_CONTEXT


#define NUM_PARTICLES (1000000)



typedef struct{
	float2 vel;
	float mass;
	float dummy;		// need this to make sure the float2 vel is aligned to a 16 byte boundary
} Particle;


float2				mousePos;
float2				dimensions;

msa::OpenCL			opencl;
std::shared_ptr<msa::OpenCLKernel>	kernelUpdate;


Particle			particles[NUM_PARTICLES];
msa::OpenCLBuffer	clMemParticles;		// stores above data


float2				particlesPos[NUM_PARTICLES];
msa::OpenCLBuffer	clMemPosVBO;		// stores above data

GLuint				vbo[1];


//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0, 0, 0);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);
	
#ifdef USE_OPENGL_CONTEXT
	opencl.setupFromOpenGL();
#else	
	opencl.setup(CL_DEVICE_TYPE_CPU, 2);
#endif	
	
	for(int i=0; i<NUM_PARTICLES; i++) {
		Particle &p = particles[i];
		p.vel.set(0, 0);
		p.mass = ofRandom(0.5, 1);		
		particlesPos[i].set(ofRandomWidth(), ofRandomHeight());
	}
	
	glGenBuffersARB(1, vbo);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float2) * NUM_PARTICLES, particlesPos, GL_DYNAMIC_COPY_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	
	
	opencl.loadProgramFromFile("MSAOpenCL/Particle.cl");
	kernelUpdate = opencl.loadKernel("updateParticle");
	
	
	clMemParticles.initBuffer(sizeof(Particle) * NUM_PARTICLES, CL_MEM_READ_WRITE, particles);
	clMemPosVBO.initFromGLObject(vbo[0]);
	
	kernelUpdate->setArg(0, clMemParticles);
	kernelUpdate->setArg(1, clMemPosVBO);
	kernelUpdate->setArg(2, &mousePos, sizeof(float2));
	kernelUpdate->setArg(3, &dimensions, sizeof(float2));
	
	glPointSize(1);
}


//--------------------------------------------------------------
void ofApp::update(){

	
	mousePos.x = ofGetMouseX();
	mousePos.y = ofGetMouseY();
	dimensions.x = ofGetWidth();
	dimensions.y = ofGetHeight();
	
	kernelUpdate->setArg(2, &mousePos, sizeof(float2));
	kernelUpdate->setArg(3, &dimensions, sizeof(float2));
	glFlush();
	
	//ofLog() << err;
	kernelUpdate->run1D(NUM_PARTICLES);
	
	//ofLog() << err;

}

//--------------------------------------------------------------
void ofApp::draw(){
	opencl.finish();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	opencl.finish();
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	
	
	glColor3f(1, 1, 1);
	string info = "fps: " + ofToString(ofGetFrameRate()) + "\nnumber of particles: " + ofToString(NUM_PARTICLES);
	ofDrawBitmapString(info, 20, 20);
}

//--------------------------------------------------------------
void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	ofToggleFullscreen();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	
}


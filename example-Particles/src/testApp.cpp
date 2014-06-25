/******
 
 This example updates 1M particles on the GPU using OpenCL
 The OpenCL kernel writes position data directly to a VBO stored in the OpenGL device memory
 so now data transfer between host and device during runtime
 
 Based on Rui's ofxOpenCL particle example opencl particles 001b.zip 
 at http://code.google.com/p/ruisource/ 
 *****/


#include "testApp.h"
#include "MSAOpenCL.h"



#define NUM_PARTICLES (1000*1000)



typedef struct{
	float2 vel;
	float mass;
	float dummy;		// need this to make sure the float2 vel is aligned to a 16 byte boundary
} Particle;


float2				mousePos;
float2				dimensions;

msa::OpenCL			opencl;

Particle			particles[NUM_PARTICLES];
msa::OpenCLBuffer	clMemParticles;		// stores above data


float2				particlesPos[NUM_PARTICLES];
msa::OpenCLBuffer	clMemPosBuffer;		// stores above data

GLuint				vbo[1];


//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0, 0, 0);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);
	
	opencl.setupFromOpenGL();

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
	opencl.loadKernel("updateParticle");

	
	clMemParticles.initBuffer(sizeof(Particle) * NUM_PARTICLES, CL_MEM_READ_WRITE, particles);

	clMemPosBuffer.initFromGLObject(vbo[0]);
	
	opencl.kernel("updateParticle")->setArg(0, clMemParticles.getCLMem());
	opencl.kernel("updateParticle")->setArg(1, clMemPosBuffer.getCLMem());
	opencl.kernel("updateParticle")->setArg(2, mousePos);
	opencl.kernel("updateParticle")->setArg(3, dimensions);
	
	glPointSize(1);
}


//--------------------------------------------------------------
void testApp::update(){
	mousePos.x = ofGetMouseX();
	mousePos.y = ofGetMouseY();
	dimensions.x = ofGetWidth();
	dimensions.y = ofGetHeight();
	
	opencl.kernel("updateParticle")->setArg(2, mousePos);
	opencl.kernel("updateParticle")->setArg(3, dimensions);
	opencl.kernel("updateParticle")->run1D(NUM_PARTICLES);
}

//--------------------------------------------------------------
void testApp::draw(){
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
void testApp::exit() {
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}


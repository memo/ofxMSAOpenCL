#include "testApp.h"

#include "MSAOpenCL.h"
#include "MSATimer.h"

#define SIZE	(2048*2048)
#define REPS	500

msa::OpenCL			openCL;
float				buf[2][SIZE];
msa::OpenCLBuffer	clBuf[2];
float				scalerMultipler;

float				testBuffer[SIZE];

msa::Timer			timer;

//--------------------------------------------------------------
void testApp::setup(){
	
	// dump everything to console
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// initialize input data
	for(int i=0;i<SIZE; i++){
		buf[0][i] = i+1;
	}
	
		
	// setup openCL, load program and init kernels
	openCL.setup(CL_DEVICE_TYPE_GPU, 2);
	openCL.loadProgramFromFile("MSAOpenCL/HelloWorld.cl", false);
	msa::OpenCLKernel *kernelSquare	= openCL.loadKernel("square");
	msa::OpenCLKernel *kernelInverse	= openCL.loadKernel("inverse");
	msa::OpenCLKernel *kernelMultiplyScaler = openCL.loadKernel("multiplyScaler");
	
	
	// create openCL buffers and upload initial data
	printf("\nPlease wait while preparing buffers...");
	timer.start();
	clBuf[0].initBuffer(SIZE * sizeof(float), CL_MEM_READ_WRITE, buf[0]);
	clBuf[1].initBuffer(SIZE * sizeof(float), CL_MEM_READ_WRITE);

	kernelSquare->setArg(0, clBuf[0].getCLMem());
	kernelSquare->setArg(1, clBuf[1].getCLMem());
	
	kernelInverse->setArg(0, clBuf[1].getCLMem());
	kernelInverse->setArg(1, clBuf[0].getCLMem());
	
	kernelMultiplyScaler->setArg(0, clBuf[0].getCLMem());
	kernelMultiplyScaler->setArg(1, scalerMultipler);
	kernelMultiplyScaler->setArg(2, clBuf[1].getCLMem());
	openCL.finish();	// not normally needed, but here for more precise time measurement

	timer.stop();
	printf("took %f seconds\n", timer.getSeconds());
	
	
	
	// run kernels
	printf("\nPlease wait while running kernels...");
	timer.start();
	size_t sizes[1] = { SIZE };
	for(int r=0; r<REPS; r++) {
		// run these kernels passing in a sizes array
		kernelSquare->run(1, sizes);
		kernelInverse->run(1, sizes);
		
		// running this one with the run1D wrapper, just to show how it works
		// actualy it does the same as the above run method (except it internally creates the array everytime its run)
		kernelMultiplyScaler->run1D(SIZE);		
	}
	openCL.finish();	// not normally needed, but here for more precise time measurement
	timer.stop();
	printf("took %f seconds\n", timer.getSeconds());

	
	
	// read results back from openCL device
	printf("\nPlease wait while reading back buffer...");
	timer.start();
	clBuf[0].read(buf[1], 0, SIZE * sizeof(float));
	timer.stop();
	printf("took %f seconds\n", timer.getSeconds());

	
	
	// perform operation on CPU as well to compare results
	printf("\nPlease wait while running on CPU for comparison...");
	timer.start();
	for(int r=0; r<REPS; r++) {
		for(int i=0; i<SIZE; i++) testBuffer[i] = (buf[0][i] * buf[0][i]);
		for(int i=0; i<SIZE; i++) testBuffer[i] = 1.0f/testBuffer[i];
		for(int i=0; i<SIZE; i++) testBuffer[i] = testBuffer[i] * scalerMultipler;
	}
	openCL.finish();	// not normally needed, but here for more precise time measurement
	timer.stop();
	printf("took %f seconds\n", timer.getSeconds());

	
	
	// compare results
	float diffSum = 0;
	for(int i=0; i<SIZE; i++) {
		float diff = testBuffer[i] - buf[1][i];
//		printf("input:%f outputCL:%f outputTest:%f diff:%f\n", buf[0][i], buf[1][i], testBuffer[i], diff);
	}
	printf("\n\noutput diffSum: %f\n\n", diffSum);
	
	std::exit(0);
}

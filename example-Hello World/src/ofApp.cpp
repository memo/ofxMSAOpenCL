#include "ofApp.h"


#include "MSAOpenCL.h"
//#include "MSATimer.h"


#define SIZE	1000000
#define REPS	500


msa::OpenCL			openCL;

// contains initialization data
float initData[SIZE];

// 4 buffers, one for each kernel output (alternatively could do ping pong)
msa::OpenCLBufferManagedT<float> buf[4];

// buffer for CPU comparison
float				control[SIZE];
float				scalerMultipler = ofRandom(100000000);
float				testBuffer[SIZE];


//--------------------------------------------------------------
void ofApp::setup(){
	// dump everything to console
	ofSetLogLevel(OF_LOG_VERBOSE);
	

	// initialize input data
    for(int i=0;i<SIZE; i++){
		initData[i] = ofRandomf() * 10000;
	}


	// setup openCL, load program and init kernels
	openCL.setup(CL_DEVICE_TYPE_GPU);
	openCL.loadProgramFromFile("MSAOpenCL/HelloWorld.cl", false);
	shared_ptr<msa::OpenCLKernel> kernelSquare	= openCL.loadKernel("square");
	shared_ptr<msa::OpenCLKernel> kernelInverse	= openCL.loadKernel("inverse");
	shared_ptr<msa::OpenCLKernel> kernelMultiplyScaler = openCL.loadKernel("multiplyScaler");
	

	// create openCL buffers and upload initial data
	printf("\nPlease wait while preparing buffers...");
//	timer.start();
	
    buf[0].initBuffer(SIZE, initData); // contains orig data
    buf[1].initBuffer(SIZE);    // will contain squared data
    buf[2].initBuffer(SIZE);    // will contain inverted data
    buf[3].initBuffer(SIZE);    // will contain scaled data
    
	kernelSquare->setArg(0, buf[0]); // read from buf0
	kernelSquare->setArg(1, buf[1]); // write to buf1

	kernelInverse->setArg(0, buf[1]);    // read from buf1
	kernelInverse->setArg(1, buf[2]);    // write to buf2
	
	kernelMultiplyScaler->setArg(0, buf[2]); // read from buf2
	kernelMultiplyScaler->setArg(1, scalerMultipler);   // multiplier
	kernelMultiplyScaler->setArg(2, buf[3]); // write to buf3
//
	openCL.finish();	// not normally needed, but here for more precise time measurement

//	timer.stop();

	printf("done");
	

	// run kernels
	printf("\nPlease wait while running kernels...");
//	timer.start();
	size_t sizes[1] = { SIZE };
	for(int r=0; r<REPS; r++) {
		// run these kernels passing in a sizes array
		kernelSquare->run(1, sizes);
		kernelInverse->run(1, sizes);
		

		// running this one with the run1D wrapper, just to show how it works
		// actually it does the same as the above run method (except it internally creates the array everytime its run)
		kernelMultiplyScaler->run1D(SIZE);		
	}
	openCL.finish();	// not normally needed, but here for more precise time measurement
//	timer.stop();
//	printf("took %f seconds\n", timer.getSeconds());

	
	


	// read results back from openCL device
	printf("\nPlease wait while reading back buffer...");
//	timer.start();
	buf[3].readFromDevice();
//	timer.stop();
	printf("done\n");


	// perform operation on CPU as well to compare results
	printf("\nPlease wait while running on CPU for comparison...");
//	timer.start();
	for(int r=0; r<REPS; r++) {
		for(int i=0; i<SIZE; i++) control[i] = (initData[i] * initData[i]);
		for(int i=0; i<SIZE; i++) control[i] = 1.0f/control[i];
		for(int i=0; i<SIZE; i++) control[i] = control[i] * scalerMultipler;
	}
	openCL.finish();	// not normally needed, but here for more precise time measurement
//	timer.stop();
	printf("done\n");

	
	

	// compare results
	float diffSum = 0;
	for(int i=0; i<SIZE; i++) {
		float diff = control[i] - buf[3][i];
//		printf("init:%f | buf3:%f | control:%f | diff:%f\n", initData[i], buf[3][i], control[i], diff);
	}
	printf("\n\noutput diffSum: %f\n\n", diffSum);
	

	std::exit(0);
}

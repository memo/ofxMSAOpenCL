#include "ofApp.h"

#include "MSAOpenCL.h"

ofVideoGrabber		videoGrabber;
int					vidWidth;
int					vidHeight;
float				captureFPS;				// video capture fps


msa::OpenCL			openCL;
msa::OpenCLImage	clImage[2];				// two OpenCL images
int					activeImageIndex = 0;

unsigned char		*pixels;				// temp buffer


// parameters
bool				doBlur		= true;
int					blurAmount	= 5;
bool				doFlipX		= true;
bool				doFlipY		= false;
bool				doGreyscale	= true;
bool				doInvert	= true;
bool				doThreshold	= false;
cl_float			threshLevel	= 0.5;




//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(50, 50, 50);
	
	
	// dump everything to console
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	// disable vsync (to allow >60fps)
	ofSetVerticalSync(false);
	
	
	// init grabber
	videoGrabber.initGrabber(640, 480);	
	vidWidth	= videoGrabber.getWidth();
	vidHeight	= videoGrabber.getHeight();
	

	// allocate temp buffer
	pixels		= new unsigned char[vidWidth * vidHeight * 4];
	

	// init OpenCL from OpenGL context to enable GL-CL data sharing
	openCL.setupFromOpenGL();
	
	
	// create OpenCL textures and related OpenGL textures
	clImage[0].initWithTexture(vidWidth, vidHeight, GL_RGBA);
	clImage[1].initWithTexture(vidWidth, vidHeight, GL_RGBA);
	

	// load and compile OpenCL program
	openCL.loadProgramFromFile("MSAOpenCL/ImageProcessing.cl");
	
	
	// load kernels
	openCL.loadKernel("msa_boxblur");
	openCL.loadKernel("msa_flipx");
	openCL.loadKernel("msa_flipy");
	openCL.loadKernel("msa_greyscale");
	openCL.loadKernel("msa_invert");
	openCL.loadKernel("msa_threshold");
}

//--------------------------------------------------------------
void ofApp::update(){
	
	// grab new frame
	videoGrabber.update();
	
	// if there is a new frame....
	if(videoGrabber.isFrameNew()) {
		
		// RGB textures don't seem to work well. so need to copy the vidgrabber data into a RGBA texture
		int pixelIndex = 0;
		for(int i=0; i<vidWidth; i++) {
			for(int j=0; j<vidHeight; j++) {
				int indexRGB	= pixelIndex * 3;
				int indexRGBA	= pixelIndex * 4;
				
				pixels[indexRGBA  ] = videoGrabber.getPixels()[indexRGB  ];
				pixels[indexRGBA+1] = videoGrabber.getPixels()[indexRGB+1];
				pixels[indexRGBA+2] = videoGrabber.getPixels()[indexRGB+2];
				pixels[indexRGBA+3] = 255;
				pixelIndex++;
			}
		}
		

		// write the new pixel data into the OpenCL Image (and thus the OpenGL texture)
		clImage[activeImageIndex].write(pixels);
		
		
		if(doBlur) {
			msa::OpenCLKernel *kernel = openCL.kernel("msa_boxblur");
			for(int i=0; i<blurAmount; i++) {
				cl_int offset = i * i / 2 + 1;
				kernel->setArg(0, clImage[activeImageIndex].getCLMem());
				kernel->setArg(1, clImage[1-activeImageIndex].getCLMem());
				kernel->setArg(2, offset);
				kernel->run2D(vidWidth, vidHeight);
				activeImageIndex = 1 - activeImageIndex;
			}
		}
		
		if(doFlipX) {
			msa::OpenCLKernel *kernel = openCL.kernel("msa_flipx");
			kernel->setArg(0, clImage[activeImageIndex].getCLMem());
			kernel->setArg(1, clImage[1-activeImageIndex].getCLMem());
			kernel->run2D(vidWidth, vidHeight);
			activeImageIndex = 1 - activeImageIndex;
		}
		
	
		if(doFlipY) {
			msa::OpenCLKernel *kernel = openCL.kernel("msa_flipy");
			kernel->setArg(0, clImage[activeImageIndex].getCLMem());
			kernel->setArg(1, clImage[1-activeImageIndex].getCLMem());
			kernel->run2D(vidWidth, vidHeight);
			activeImageIndex = 1 - activeImageIndex;
		}
		
		if(doGreyscale) {
			msa::OpenCLKernel *kernel = openCL.kernel("msa_greyscale");
			kernel->setArg(0, clImage[activeImageIndex].getCLMem());
			kernel->setArg(1, clImage[1-activeImageIndex].getCLMem());
			kernel->run2D(vidWidth, vidHeight);
			activeImageIndex = 1 - activeImageIndex;
		}
		
		if(doInvert) {
			msa::OpenCLKernel *kernel = openCL.kernel("msa_invert");
			kernel->setArg(0, clImage[activeImageIndex].getCLMem());
			kernel->setArg(1, clImage[1-activeImageIndex].getCLMem());
			kernel->run2D(vidWidth, vidHeight);
			activeImageIndex = 1 - activeImageIndex;
		}
	
		if(doThreshold) {
			msa::OpenCLKernel *kernel = openCL.kernel("msa_threshold");
			kernel->setArg(0, clImage[activeImageIndex].getCLMem());
			kernel->setArg(1, clImage[1-activeImageIndex].getCLMem());
			kernel->setArg(2, threshLevel);
			kernel->run2D(vidWidth, vidHeight);
			activeImageIndex = 1 - activeImageIndex;
		}
		
		
		// calculate capture fps
		static float lastTime = 0;
		float nowTime = ofGetElapsedTimef();
		float timeDiff = nowTime - lastTime;
		if(timeDiff > 0 ) captureFPS = 0.9f * captureFPS + 0.1f / timeDiff;
		lastTime = nowTime;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	videoGrabber.draw(0, 200);

	// make sure all OpenCL kernels have finished executing before drawing
	openCL.finish();
	
	// draw the OpenGL texture (which was mapped to the OpenCL image)
	clImage[activeImageIndex].getTexture().draw(vidWidth, 200);
	
	
	ofDrawBitmapString(    " app FPS            : " + ofToString(ofGetFrameRate(), 2)
					   + "\n capture FPS        : " + ofToString(captureFPS, 2)
					   + "\n"
					   + "\n doBlur (b)         : " + (doBlur ? "X" : "")
					   + "\n   blurAmount (1-9) : " + ofToString(blurAmount)
					   + "\n doFlipX (x)        : " + (doFlipX ? "X" : "")
					   + "\n doFlipY (y)        : " + (doFlipY ? "X" : "")
					   + "\n doGreyscale (g)    : " + (doGreyscale ? "X" : "")
					   + "\n doInvert (i)       : " + (doInvert ? "X" : "")
					   + "\n doThreshold (t)    : " + (doThreshold ? "X" : "")
					   + "\n   threshLevel ([]) : " + ofToString(threshLevel, 2)
					   , 30, 30);
}

//--------------------------------------------------------------
void ofApp::exit() {
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key) {
		case 'b':
			doBlur ^= true;
			break;
		
		case 'x':
			doFlipX ^= true;
			break;
		
		case 'y':
			doFlipY ^= true;
			break;
		
		case 'g':
			doGreyscale ^= true;
			break;
		
		case 'i':
			doInvert ^= true;
			break;
		
		case 't':
			doThreshold ^= true;
			break;
		
		case 's':
			videoGrabber.videoSettings();
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			blurAmount = key - '0';
			break;
			
		case '[':
			threshLevel -= 0.05;
			if(threshLevel<0) threshLevel = 0;
			break;
	
		case ']':
			threshLevel += 0.05;
			if(threshLevel>1) threshLevel = 1;
			break;
	}

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


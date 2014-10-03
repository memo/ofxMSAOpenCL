#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "MSAOpenCL.h"

class ofApp : public ofBaseApp{
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

public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
};

#endif

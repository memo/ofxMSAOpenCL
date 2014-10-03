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
	int					activeImageIndex;
	
	unsigned char		*pixels;				// temp buffer
	
	
	// parameters
	bool				doBlur;
	int					blurAmount;
	bool				doFlipX;
	bool				doFlipY;
	bool				doGreyscale;
	bool				doInvert;
	bool				doThreshold;
	cl_float			threshLevel;

public:

	ofApp()
	: doBlur(true)
	, activeImageIndex(0)
	, blurAmount(5)
	, doFlipX(true)
	, doFlipY(false)
	, doGreyscale(true)
	, doInvert(true)
	, doThreshold(false)
	, threshLevel(0.5)
	{};

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

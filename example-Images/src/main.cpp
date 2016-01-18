#include "ofMain.h"
#include "ofApp.h"


//========================================================================
int main( ){

    ofGLWindowSettings settings;
    settings.setGLVersion(3, 2);
    settings.width = 1300;
    settings.height = 768;
    ofCreateWindow(settings);
	ofRunApp(new ofApp());

}

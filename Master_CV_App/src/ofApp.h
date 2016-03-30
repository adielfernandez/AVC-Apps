#pragma once

#include "ofMain.h"
#include "Osc.h"
#include "Camera.h"
#include "ofxCv.h"
#include "Button.hpp"
#include "NavPanel.hpp"

#pragma once



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
    

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    

    
    

    bool bScaleDown;
    
    
    //-----CAMERAS-----
    const int numFeeds = 14;
    vector<shared_ptr<Camera>> cameras;
    
    const int numScreens = 17;
    
    
    //-----UI-----
    int viewMode;
    ofTrueTypeFont titleFont;
    ofTrueTypeFont smallerFont;
    
    int feedWidth, feedHeight;
    int leftMargin;
    int topMargin;
    int centerMargin;
    
    NavPanel panel;
    
    //positions for aesthetics
    ofVec2f rawImagePos, cvImagePos;
    
    
    //-----Data-----
    Osc oscHandler;
    int dataPerSec;
    unsigned long long lastSendTime;
    
    
};
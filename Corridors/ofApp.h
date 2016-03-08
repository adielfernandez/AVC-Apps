#pragma once

#include "ofMain.h"

#include "Osc.h"
#include "Gui.h"

#include "Corridor.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
		
    
    //-----EXPERIMENTAL-----
    //scales down the CV data
    //most of the way through
    bool bScaleDown;
    //----------------------
    
    
    //-----CORRIDORS-----
    const int numFeeds = 4;
    vector<string> addresses;
    vector<shared_ptr<Corridor>> corridors;


    Corridor corridor2;
    Corridor corridor3;
    Corridor corridor4;
    Corridor corridor5;
    
    //-----UI-----
    int viewMode;
    ofTrueTypeFont titleFont;
    ofTrueTypeFont smallerFont;
    
    int feedWidth, feedHeight;
    int leftMargin;
    int topMargin;
    int centerMargin;
    
    //positions for aesthetics
    ofVec2f rawImagePos, cvImagePos;
    
    
    
    //-----Data-----
    Osc oscHandler;
    int dataPerSec;
    unsigned long long lastSendTime;
    
    
};
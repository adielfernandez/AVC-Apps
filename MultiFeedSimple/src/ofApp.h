#pragma once

#include "ofMain.h"
#include "ofGstVideoPlayer.h"

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
		
    
    ofGstVideoUtils gst1, gst2, gst3, gst4, gst5;
    
    float feedHeight, feedWidth;
    
    ofImage img1, img2, img3, img4, img5;
    
    unsigned long n1, n2, n3, n4, n5;
    
    int staggerTime, wait;
    bool started1, started2, started3, started4, started5;
    
    vector<string> addresses;
    
    
};

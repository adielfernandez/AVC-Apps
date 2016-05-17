#pragma once

#include "ofMain.h"
#include "Osc.h"
#include "Camera.h"
#include "ofxCv.h"
#include "Button.hpp"
#include "NavPanel.hpp"
#include "Aggregator.hpp"

#include "PlaybackBar.hpp"

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
		
    

    PlaybackBar playback;
    

    bool bScaleDown;
    bool useLiveFeed;
    
    //-----CAMERAS-----
    const int numFeeds = 14;
    vector<shared_ptr<Camera>> cameras;
    
    //different views within the app
    const int numScreens = 18;
    
    
    //Aggregates
    Aggregator Lobby1Aggregate;
    Aggregator Lobby2Aggregate;
    
    
    
    //-----UI-----
    int viewMode;
    ofTrueTypeFont titleFont;
    ofTrueTypeFont mediumFont;
    ofTrueTypeFont smallerFont;
    
    int feedWidth, feedHeight;
    int scaledWidth, scaledHeight;
    int leftMargin;
    int topMargin;
    
    NavPanel panel;
    
    unsigned long long saveTime;
    bool saveTrigger;
    
    
    //positions for aesthetics
    ofVec2f mainContentPos, secondaryContentPos;
    
    //positions for all cameras view
    vector<ofVec2f> allCamsPos;
    vector<ofVec2f> allLabelsPos;
    int frameWidth, frameHeight;
    
    
    //-----OSC Data-----
    Osc oscHandler;
    int dataPerSec;
    unsigned long long lastPGSSendTime, lastAudioSendTime;
    
    bool audioSent, pgsSent;
    
    int oscBoxWidth;
    int oscBoxHeight;
    

    
    ofVec2f oscVizOrigin;

    
    float oscVizHeight;
    float oscVizWidth;
    
    ofFbo pgsViz, audioViz;
    
};








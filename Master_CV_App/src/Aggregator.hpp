//
//  Aggregator.hpp
//  Stitching Test
//
//  Created by Adiel Fernandez on 3/3/16.
//
//

#ifndef Aggregator_hpp
#define Aggregator_hpp

#include <stdio.h>

#endif /* Aggregator_hpp */


#include "ofMain.h"
#include "ofxCv.h"
#include "Camera.h"
#include "ofxOsc.h"
#include "ThreadedCV.hpp"

#pragma once


class Aggregator{
    
public:
    
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void mouseEntered(ofMouseEventArgs & args);
    void mouseExited(ofMouseEventArgs & args);
    
    //=========================    
    
    Aggregator();
    void setup(string _name, int _numCams, vector<shared_ptr<Camera>> _cams, bool _bScaleDown);
    void setupViewControl(int _thisView, int *_viewMode, ofVec2f _adjOrigin);
    void update();
    
    void drawRaw(int x, int y);
    void drawCV(int x, int y);
    void drawMain();

    void saveAllSettings();
    
    //view control
    int *viewMode;
    int thisView;
    
    
    //decides if we're tweaking the
    //mapping (0) or masking (1)
    int manipulationMode;
    
    
    //General info and UI
    string name;
    int numCams;
    bool bScaleDown;
    int feedWidth, feedHeight;
    int scaledWidth, scaledHeight;
    ofColor backgroundInCol, backgroundOutCol;
    ofTrueTypeFont *font;
    
    
    //Where to get the feeds
    vector<shared_ptr<Camera>> cams;
    
    //a helper vector to hold the indices of the cameras we're using
    //corridor one: cams 0, 1, 2, 3, 4, 5
    //corridor six: cams 10, 11, 12, 13
    vector<int> indices;
    
    //where all the feeds will be
    //pasted into the aggregate
    vector<ofVec2f> positions;
    
    //disable changing of the positions
    //if we're using BG Diff
    bool disablePositioning;
    
    
    //CV Objects for aggregate
    ofPixels masterPix;
    ofPixels blurredMaster;
    ofPixels threshMaster;
    
    ofImage threshMasterImg;
    int masterWidth, oldMasterWidth;
    int masterHeight, oldMasterHeight;
    
    //if the overall height/width of the
    //CV image changes we need to reset the BG
    bool bgResetNeeded;
    bool waitingToBgDiff;
    
    
    ofxCv::ContourFinder contours;
    
    
    //threaded CV processor
    ThreadedCV aggregateProcessor;
    ofPixels threadOutputPix;
    unsigned long long lastAnalyzeTime;
    
    //-----Mouse Handling-----
    
    //This holds the upper left corner
    //of the content area where the
    //camera/quad is drawn on screen.
    ofVec2f adjustedOrigin;
    
    //This holds the adjusted mouse so clicks
    //still make sense in the quad's reference frame
    ofVec2f adjustedMouse;
    
    vector<bool> posMouseLock;
    float mouseHandleRad;
    ofColor handleCol, grabbedCol, disableCol;
    
    
    //mouse stuff for masking
    int maskToolSize;
    bool maskPressed;
    string maskFileName;
    ofColor maskingCol;
    
    
    
    //Masking stuff
    ofPixels maskPix;
    ofImage maskImg;
    bool maskChanged;
    ofVec2f maskBoundStart, maskBoundEnd;
    
    
    //-----OSC Data gathering-----
    void gatherOscStats();

        //Blob Stats
    ofVec2f avgDir;
    ofVec2f avgPos;
    ofVec2f avgVel;
    float avgSpeed;
    float density;
    
        //osc bundles to prepare
    ofxOscMessage corridorStats;
    ofxOscMessage corridorStartFlag;
    ofxOscBundle corridorBundle;
    ofxOscBundle blobsBundle;
    
    
    
    //-----Gui-----
    void drawGui(int x, int y);
    void loadSettings();
    void saveSettings();
    
    string guiName;
    ofxPanel gui;
    ofxLabel manipulationLabel;
    ofxIntSlider blurAmountSlider;
    ofxIntSlider numErosionsSlider;
    ofxIntSlider numDilationsSlider;
    
    ofxLabel bgDiffLabel;
    ofxIntSlider learningTime;
    ofxButton resetBG;
    ofxToggle useBgDiff;
    ofxIntSlider thresholdSlider;
    ofxToggle drawThresholdToggle;
    
    ofxLabel contoursLabel;
    ofxIntSlider minBlobAreaSlider;
    ofxIntSlider maxBlobAreaSlider;
    ofxIntSlider persistenceSlider;
    ofxIntSlider maxDistanceSlider;
    ofxToggle drawContoursToggle;
    ofxToggle showInfoToggle;
    
    ofxLabel maskingLabel;
    ofxToggle useMask;
    ofxToggle drawOrErase;
    ofxButton clearMask;
    ofxButton saveMask;
    ofxButton loadMask;
    
    ofxFloatSlider contrastSlider;
    ofxFloatSlider contrastPhaseSlider;

    ofxLabel positionsLabel;
    ofxButton trimPixels;
    ofxVec2Slider camPos1;
    ofxVec2Slider camPos2;
    ofxVec2Slider camPos3;
    ofxVec2Slider camPos4;
    ofxVec2Slider camPos5;
    ofxVec2Slider camPos6;
    
    
};
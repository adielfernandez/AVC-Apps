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
    
    
    //view control
    int *viewMode;
    int thisView;
    
    
    //General info and UI
    string name;
    int numCams;
    bool bScaleDown;
    int feedWidth, feedHeight;
    int scaledWidth, scaledHeight;
    
    //master ofPixels object for aggregate
    ofPixels masterPix;
    ofPixels blurredMaster;
    ofPixels threshMaster;
    
    ofxCv::ContourFinder contours;
    
    
    ofImage masterImg;
    int overallWidth;
    int overallHeight;
    
    //Image Data
    //control
    vector<shared_ptr<Camera>> cams;
    vector<int> indices;
    

    vector<ofVec2f> positions;
    
    //This holds the upper left corner
    //of the content area where the camera/quad
    //is drawn on screen.
    ofVec2f adjustedOrigin;
    
    //This holds the adjusted mouse so clicks
    //still make sense in the quad's reference frame
    ofVec2f adjustedMouse;
    
    vector<bool> posMouseLock;
    float mouseHandleRad;
    ofColor handleCol, grabbedCol;
    
    
    //Blob Stats
    ofVec2f avgDir;
    ofVec2f avgPos;
    ofVec2f avgVel;
    float avgSpeed;
    
    //osc bundles to prepare
    ofxOscMessage corridorStats;
    ofxOscBundle corridorBundle;
    ofxOscBundle blobsBundle;
    
    //Gui
    void drawGui(int x, int y);
    void loadSettings();
    void saveSettings();
    
    string guiName;
    
    ofxPanel gui;
    
    ofxIntSlider thresholdSlider;
    ofxIntSlider blurAmountSlider;
    ofxIntSlider numErosionsSlider;
    ofxIntSlider numDilationsSlider;
    ofxIntSlider minBlobAreaSlider;
    ofxIntSlider maxBlobAreaSlider;
    ofxIntSlider persistenceSlider;
    ofxIntSlider maxDistanceSlider;
    ofxToggle drawThresholdToggle;
    ofxToggle drawContoursToggle;
    ofxToggle showInfoToggle;
    ofxVec2Slider camPos1;
    ofxVec2Slider camPos2;
    ofxVec2Slider camPos3;
    ofxVec2Slider camPos4;
    ofxVec2Slider camPos5;
    ofxVec2Slider camPos6;
    
    
};



#include <stdio.h>
#include "ofxGui.h"

#pragma once

class CamGui {


public:
    
    
    // Constructor
    CamGui();
    
    // Methods
    void setup(string name, bool solo);
    void draw(int x, int y);
    void loadSettings();
    void saveSettings();
    
    string guiName;
    
    ofxPanel gui;
    ofxIntSlider thresholdSlider;
    ofxIntSlider blurAmountSlider;
    ofxIntSlider numErosionsSlider;
    ofxIntSlider numDilationsSlider;
    ofxToggle drawThresholdToggle;
    
    ofxIntSlider minBlobAreaSlider;
    ofxIntSlider maxBlobAreaSlider;
    ofxIntSlider persistenceSlider;
    ofxIntSlider maxDistanceSlider;
    ofxToggle drawContoursToggle;
    ofxToggle showInfoToggle;
    
    ofxVec2Slider mapPt0;
    ofxVec2Slider mapPt1;
    ofxVec2Slider mapPt2;
    ofxVec2Slider mapPt3;
    ofxVec2Slider mapPt4;
    ofxVec2Slider mapPt5;
    ofxVec2Slider mapPt6;
    ofxVec2Slider mapPt7;
    ofxVec2Slider mapPt8;
    
    ofxVec2Slider cropStart;
    ofxVec2Slider cropEnd;
    
};



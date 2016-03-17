

#include <stdio.h>
#include "ofxGui.h"

#pragma once

class Gui {


public:
    
    
    // Constructor
    Gui();
    
    // Methods
    void setup(string name);
    void draw(int x, int y);
    void load(string filePath);
    
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

    ofxToggle backgroundSubtract;
    ofxFloatSlider backgroundLearnRate;
    ofxIntSlider varThreshold;
    
    ofxVec2Slider quadPt0;
    ofxVec2Slider quadPt1;
    ofxVec2Slider quadPt2;
    ofxVec2Slider quadPt3;
    
};





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
    ofxToggle useThreshold;
    
    ofxLabel contoursLabel;
    ofxIntSlider minBlobAreaSlider;
    ofxIntSlider maxBlobAreaSlider;
    ofxIntSlider persistenceSlider;
    ofxIntSlider maxDistanceSlider;
    ofxToggle drawContoursToggle;
    ofxToggle showInfoToggle;
    ofxToggle useBlobFilter;
    ofxToggle drawFilteredBlobs;
    ofxIntSlider filterRadiusSlider;
    ofxIntSlider stillTimeSlider;
    ofxFloatSlider speedThresholdSlider;
    
    ofxLabel maskingLabel;
    ofxToggle useMask;
    ofxToggle drawOrErase;
    ofxButton clearMask;
    ofxButton saveMask;
    ofxButton loadMask;

    ofxFloatSlider contrastSlider;
    ofxFloatSlider contrastPhaseSlider;

    
    ofxLabel croppingLabel;
    ofxButton resetCrop;
    
    
    ofxLabel mappingLabel;
    ofxButton resetMap;
    ofxButton rotate180;
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



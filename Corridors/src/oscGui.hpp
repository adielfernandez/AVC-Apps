//
//  oscGui.hpp
//  Corridors
//
//  Created by Adiel Fernandez on 3/8/16.
//
//

#ifndef oscGui_hpp
#define oscGui_hpp

#include <stdio.h>

#endif /* oscGui_hpp */


#include "ofxGui.h"

#pragma once

class oscGui {
    
    
public:
    
    
    // Constructor
    oscGui();
    
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


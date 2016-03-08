//
//  AggregateGui.hpp
//  Stitching Test
//
//  Created by Adiel Fernandez on 3/3/16.
//
//

#ifndef AggregateGui_hpp
#define AggregateGui_hpp

#include <stdio.h>

#endif /* AggregateGui_hpp */

#include "ofxGui.h"

#pragma once

class AggregateGui {
    
    
public:
    
    
    // Constructor
    AggregateGui();
    
    // Methods
    void setup(string name);
    void draw(int x, int y);
    void load();
    
    string guiName;
    
    ofxPanel gui;

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


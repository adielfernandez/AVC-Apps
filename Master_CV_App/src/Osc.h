
#ifndef osc_h
#define osc_h

#include <stdio.h>

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"

#pragma once

class Osc {
public:

    Osc();
    
    // Methods
    void setup(string _pgsIP, string _audioIP);
    void update();
    void drawGui(int x, int y);

    void saveSettings();
    void loadSettings();
    
    ofxOscSender pgsSender;
    int pgsPort;
    string pgsIP;
    
    ofxOscSender audioSender;
    int audioPort;
    string audioIP;
    
    ofxPanel baseGui;
    
    ofxLabel oscLabel;
    ofxToggle sendOsc;
    ofxIntSlider oscSendRate;
    ofxToggle sendCorridor1;
    ofxToggle sendCorridor2;
    ofxToggle sendCorridor3;
    ofxToggle sendCorridor4;
    ofxToggle sendCorridor5;
    ofxToggle sendCorridor6;
    
    ofxToggle sendAudioOsc;
    
    string fileName;
    
};
#endif /* osc_h */

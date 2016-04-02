
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
    void setupAudio(string _IP, int _port);
    void setupPGS(string _IP, int _port);
    void setupHeartbeat(string _IP, int _port);
    void setupGui();
    
    void update();
    void drawGui(int x, int y);

    void saveSettings();
    void loadSettings();
    
    ofxOscSender pgsSender;
    string pgsIP;
    int pgsPort;
    
    ofxOscSender audioSender;
    string audioIP;
    int audioPort;

    ofxOscSender heartbeatSender;
    string heartbeatIP;
    int heartbeatPort;
    
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

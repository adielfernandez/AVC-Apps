
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
    void setupPGS1(string _IP, int _port);
    void setupPGS2(string _IP, int _port);
    void setupPGSBackup(string _IP, int _port);
    void setupPrevizDev(string _IP, int _port);
    
    void setupHeartbeat(string _IP, int _port);
    void setupGui();
    
    void update();
    void drawGui(int x, int y);

    void saveSettings();
    void loadSettings();
    
    ofxOscSender pgs1Sender;
    string pgs1IP;
    int pgs1Port;

    ofxOscSender pgs2Sender;
    string pgs2IP;
    int pgs2Port;
    
    ofxOscSender pgsBackupSender;
    string pgsBackupIP;
    int pgsBackupPort;
    
    ofxOscSender previzDevSender;
    string previzDevIP;
    int previzDevPort;
    
    ofxOscSender audioSender;
    string audioIP;
    int audioPort;

    ofxOscSender heartbeatSender;
    string heartbeatIP;
    int heartbeatPort;
    
    ofxPanel baseGui;
    
    ofxLabel oscLabel;
    ofxLabel corridorLabel;
    ofxLabel masterToggleLabel;
    ofxToggle sendAllOsc;
    ofxToggle sendCorridor1;
    ofxToggle sendCorridor2;
    ofxToggle sendCorridor3;
    ofxToggle sendCorridor4;
    ofxToggle sendCorridor5;
    ofxToggle sendCorridor6;
    
    ofxLabel receiversLabel;
    ofxToggle sendPgs1;
    ofxToggle sendPgs2;
    ofxToggle sendPgsBackup;
    ofxToggle sendPrevizDev;
    ofxToggle sendAudioOsc;
    ofxToggle sendHeartbeat;
    
    ofxIntSlider pgsSendRate;
    ofxIntSlider audioSendRate;
    
    string fileName;
    
};
#endif /* osc_h */

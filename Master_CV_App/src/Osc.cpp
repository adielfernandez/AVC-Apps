
#include "Osc.h"

Osc::Osc(){
    
    
}

void Osc::setup(string _pgsIP, string _audioIP){

    pgsPort = 12345;
    audioPort = 12345;
    
    pgsIP = _pgsIP;
    audioIP = _audioIP;
    
    pgsSender.setup(pgsIP, pgsPort);
    audioSender.setup(audioIP, audioPort);
    
    
    fileName = "oscSettings";
    
    baseGui.setup(fileName, fileName + ".xml", 0, 0);
    
    baseGui.add(oscLabel.setup("OSC DATA", ""));
    baseGui.add(sendOsc.setup("Send OSC", true));
    baseGui.add(oscSendRate.setup("Millis bet. sends", 500, 20, 2000));
    
    baseGui.add(sendCorridor1.setup("Send Corridor 1 Data", true));
    baseGui.add(sendCorridor2.setup("Send Corridor 2 Data", true));
    baseGui.add(sendCorridor3.setup("Send Corridor 3 Data", true));
    baseGui.add(sendCorridor4.setup("Send Corridor 4 Data", true));
    baseGui.add(sendCorridor5.setup("Send Corridor 5 Data", true));
    baseGui.add(sendCorridor6.setup("Send Corridor 6 Data", true));
    
    baseGui.add(sendAudioOsc.setup("Send Audio Data", false));
    

    
}

void Osc::update(){
    
    
    
}

void Osc::drawGui(int x, int y){
    
    baseGui.setPosition(x, y);
    baseGui.draw();
    
}

void Osc::saveSettings(){
    
    baseGui.saveToFile(fileName + ".xml");
    
}

void Osc::loadSettings(){
    
    baseGui.loadFromFile(fileName + ".xml");
    
}








#include "Osc.h"

Osc::Osc(){
    
    
}

void Osc::setupPGS1(string _IP, int _port){
    
    pgs1IP = _IP;
    pgs1Port = _port;
    pgs1Sender.setup(pgs1IP, pgs1Port);
    
}

void Osc::setupPGS2(string _IP, int _port){
    
    pgs2IP = _IP;
    pgs2Port = _port;
    pgs2Sender.setup(pgs2IP, pgs2Port);
    
}

void Osc::setupPreviz(string _IP, int _port){
    
    previzIP = _IP;
    previzPort = _port;
    previzSender.setup(previzIP, previzPort);
    
}

void Osc::setupPrevizDev(string _IP, int _port){
    
    previzDevIP = _IP;
    previzDevPort = _port;
    previzDevSender.setup(previzDevIP, previzDevPort);
    
}

void Osc::setupAudio(string _IP, int _port){

    audioIP = _IP;
    audioPort = _port;
    audioSender.setup(audioIP, audioPort);
    
}

void Osc::setupHeartbeat(string _IP, int _port){
    
    heartbeatIP = _IP;
    heartbeatPort = _port;
    heartbeatSender.setup(heartbeatIP, heartbeatPort);

}


void Osc::setupGui(){
    
    fileName = "oscSettings";
    
    baseGui.setup(fileName, fileName + ".xml", 0, 0);
    
    baseGui.add(oscLabel.setup("OSC DATA", ""));

    
    baseGui.add(sendCorridor1.setup("Send Corridor 1 Data", true));
    baseGui.add(sendCorridor2.setup("Send Corridor 2 Data", true));
    baseGui.add(sendCorridor3.setup("Send Corridor 3 Data", true));
    baseGui.add(sendCorridor4.setup("Send Corridor 4 Data", true));
    baseGui.add(sendCorridor5.setup("Send Corridor 5 Data", true));
    baseGui.add(sendCorridor6.setup("Send Corridor 6 Data", true));
    
    baseGui.add(receiversLabel.setup("Send to...", ""));
    baseGui.add(sendPgs1.setup("Send to PGS-1", true));
    baseGui.add(sendPgs2.setup("Send to PGS-2", true));
    baseGui.add(sendPreviz.setup("Send to Previz", true));
    baseGui.add(sendPrevizDev.setup("Send to PrevizDev", true));
    baseGui.add(sendAudioOsc.setup("Send Audio Data", true));
    baseGui.add(sendHeartbeat.setup("Send Heartbeat", true));
    baseGui.add(oscSendRate.setup("Millis bet. sends", 500, 20, 500));
    baseGui.add(sendAllOsc.setup("Send OSC", true));
    
    receiversLabel.setDefaultTextColor(ofColor(255));
    
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







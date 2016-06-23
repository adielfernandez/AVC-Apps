
#include "Osc.h"

Osc::Osc(){
    
    
}

void Osc::setup(){
    
    fileName = "oscSettings";
    
    baseGui.setup(fileName, fileName + ".xml", 0, 0);
    
    baseGui.add(oscLabel.setup("OSC DATA", ""));
    
    baseGui.add(corridorLabel.setup("Corridors to include", ""));
    baseGui.add(sendCorridor1.setup("Lobby 1 Data", true));
    baseGui.add(sendCorridor2.setup("Corridor 2 Data", true));
    baseGui.add(sendCorridor3.setup("Corridor 3 Data", true));
    baseGui.add(sendCorridor4.setup("Corridor 4 Data", true));
    baseGui.add(sendCorridor5.setup("Corridor 5 Data", true));
    baseGui.add(sendCorridor6.setup("Lobby 2 Data", true));
    
    baseGui.add(receiversLabel.setup("Send to...", ""));
    baseGui.add(sendPgs1.setup("Send to PGS-1", true));
    baseGui.add(sendPgs2.setup("Send to PGS-2", true));
    baseGui.add(sendPgsBackup.setup("Send to PGS-Backup", true));
    baseGui.add(sendPrevizDev.setup("Send to PrevizDev", true));
    baseGui.add(sendHeartbeat.setup("Send Heartbeat", false));
    baseGui.add(pgsSendRate.setup("PGS Rate (in ms)", 40, 15, 100));
    
    baseGui.add(sendAudioOsc.setup("Send Audio Data", true));
    baseGui.add(sendAudioBackupOsc.setup("Send Audio Backup Data", true));
    baseGui.add(audioSendRate.setup("Audio Rate (in ms)", 250, 15, 500));
    
    baseGui.add(masterToggleLabel.setup("Master OSC Toggle", ""));
    baseGui.add(sendAllOsc.setup("Send OSC", true));
    
    oscLabel.setBackgroundColor(ofColor(255));
    receiversLabel.setBackgroundColor(ofColor(255));
    corridorLabel.setBackgroundColor(ofColor(255));
    masterToggleLabel.setBackgroundColor(ofColor(255));
    

    
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

void Osc::setupPGSBackup(string _IP, int _port){
    
    pgsBackupIP = _IP;
    pgsBackupPort = _port;
    pgsBackupSender.setup(pgsBackupIP, pgsBackupPort);
    
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

void Osc::setupAudioBackup(string _IP, int _port){
    
    audioBackupIP = _IP;
    audioBackupPort = _port;
    audioBackupSender.setup(audioBackupIP, audioBackupPort);
    
}

void Osc::setupHeartbeat(string _IP, int _port){
    
    heartbeatIP = _IP;
    heartbeatPort = _port;
    heartbeatSender.setup(heartbeatIP, heartbeatPort);

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







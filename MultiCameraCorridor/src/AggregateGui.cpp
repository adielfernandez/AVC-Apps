//
//  AggregateGui.cpp
//  Stitching Test
//
//  Created by Adiel Fernandez on 3/3/16.
//
//

#include "AggregateGui.hpp"
#include "ofMain.h"
#include "ofApp.h"



AggregateGui::AggregateGui(){
    
}

void AggregateGui::setup(string name){
    
    guiName = name;
    
    //param 1 = gui title, param 2 = filename, then 3&4 = startposition
    gui.setup(guiName, guiName + ".xml", 0, 0);
    

    gui.add(minBlobAreaSlider.setup("Min Blob Area", 0, 0, 1000));
    gui.add(maxBlobAreaSlider.setup("Max Blob Area", 50000, 0, 100000));
    gui.add(persistenceSlider.setup("Persistence", 15, 0, 100));
    gui.add(maxDistanceSlider.setup("Max Distance", 32, 0, 100));
    gui.add(drawThresholdToggle.setup("Draw threshold", false));
    gui.add(drawContoursToggle.setup("Draw Contours", true));
    gui.add(showInfoToggle.setup("Info", false));

    gui.add(camPos1.setup("Cam1 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos2.setup("Cam2 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos3.setup("Cam3 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos4.setup("Cam4 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos5.setup("Cam5 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos6.setup("Cam6 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    
}

void AggregateGui::load(){
    
    gui.loadFromFile(guiName + ".xml");
    
}

void AggregateGui::draw(int x, int y){
    
    gui.setPosition(x, y);
    gui.draw();
    
}
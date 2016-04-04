
#include "CamGui.h"
#include "ofMain.h"
#include "ofApp.h"



CamGui::CamGui(){
    
}

void CamGui::setup(string name, bool solo){

    guiName = name;
    
    //param 1 = gui title, param 2 = filename, then 3&4 = startposition
    gui.setup(guiName, guiName + ".xml", 0, 0);
    
    gui.add(thresholdSlider.setup("Threshold", 0, 0, 255));
    gui.add(blurAmountSlider.setup("Blur", 1, 0, 50));
    gui.add(numErosionsSlider.setup("Number of erosions", 0, 0, 10));
    gui.add(numDilationsSlider.setup("Number of dilations", 0, 0, 10));
    gui.add(drawThresholdToggle.setup("Draw threshold", false));
    
    if(solo){
        gui.add(minBlobAreaSlider.setup("Min Blob Area", 0, 0, 1000));
        gui.add(maxBlobAreaSlider.setup("Max Blob Area", 50000, 0, 100000));
        gui.add(persistenceSlider.setup("Persistence", 15, 0, 100));
        gui.add(maxDistanceSlider.setup("Max Distance", 32, 0, 100));
        gui.add(drawContoursToggle.setup("Draw Contours", true));
        gui.add(showInfoToggle.setup("Info", false));
    }
    
    gui.add(quadPt0.setup("Point 0", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(quadPt1.setup("Point 1", ofVec2f(640, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(quadPt2.setup("Point 2", ofVec2f(640, 512), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(quadPt3.setup("Point 3", ofVec2f(0, 512), ofVec2f(0, 0), ofVec2f(640, 512)));

    if(!solo){
        gui.add(cropStart.setup("Crop Start (norm)", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(1.0, 1.0)));
        gui.add(cropEnd.setup("Crop End (norm)", ofVec2f(1.0, 1.0), ofVec2f(0, 0), ofVec2f(1.0, 1.0)));
    }
}

void CamGui::load(){
    
    gui.loadFromFile(guiName + ".xml");
    
}

void CamGui::draw(int x, int y){
    
    gui.setPosition(x, y);
    gui.draw();
    
}


#include "CamGui.h"
#include "ofMain.h"
#include "ofApp.h"



CamGui::CamGui(){
    
}

void CamGui::setup(string name, bool solo){

    guiName = name;
    
    //param 1 = gui title, param 2 = filename, then 3&4 = startposition
    gui.setup(guiName, guiName + ".xml", 0, 0);
    
    gui.add(manipulationLabel.setup("   IMAGE DISTORTION", ""));
    gui.add(blurAmountSlider.setup("Blur", 1, 0, 40));
    gui.add(thresholdSlider.setup("Threshold", 0, 0, 255));
    gui.add(drawThresholdToggle.setup("Draw threshold", false));

    if(solo){
        gui.add(contrastSlider.setup("Contrast Exponent", 1.0, 1.0, 8.0));
        gui.add(contrastPhaseSlider.setup("Contrast Phase", 0.0, 0.0, 0.4));

        gui.add(numErosionsSlider.setup("Number of erosions", 0, 0, 10));
        gui.add(numDilationsSlider.setup("Number of dilations", 0, 0, 10));
        
        gui.add(bgDiffLabel.setup("   BG SUBTRACTION", ""));
        gui.add(useBgDiff.setup("Use BG Diff", false));
        gui.add(learningTime.setup("Frames to learn BG", 100, 0, 2000));
        gui.add(resetBG.setup("Reset Background"));
        
        gui.add(contoursLabel.setup("   CONTOUR FINDING", ""));
        gui.add(minBlobAreaSlider.setup("Min Blob Area", 0, 0, 1000));
        gui.add(maxBlobAreaSlider.setup("Max Blob Area", 1000, 0, 50000));
        gui.add(persistenceSlider.setup("Persistence", 15, 0, 100));
        gui.add(maxDistanceSlider.setup("Max Distance", 32, 0, 100));
        gui.add(drawContoursToggle.setup("Draw Contours", true));
        gui.add(showInfoToggle.setup("Info", false));
        
        gui.add(maskingLabel.setup("   MASKING", ""));
        gui.add(useMask.setup("Use Mask", true));
        gui.add(drawOrErase.setup("Draw or Erase", true));
        gui.add(clearMask.setup("Clear Mask"));
        gui.add(saveMask.setup("Save Mask"));
        gui.add(loadMask.setup("Load Mask"));
        
        
        
    } else {
        
        gui.add(useThreshold.setup("Pre-Thread Thresh", false));

        
        gui.add(croppingLabel.setup("   CROPPING", ""));
        gui.add(resetCrop.setup("Reset Cropping Points"));
        gui.add(cropStart.setup("Crop Start (norm)", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(1.0, 1.0)));
        gui.add(cropEnd.setup("Crop End (norm)", ofVec2f(1.0, 1.0), ofVec2f(0, 0), ofVec2f(1.0, 1.0)));
    }

    int feedWidth = 640;
    int feedHeight = 512;
    ofVec2f feedDim(feedWidth, feedHeight);
    
    gui.add(mappingLabel.setup("   MAPPING", ""));
    gui.add(resetMap.setup("Reset Map Points"));
    gui.add(rotate180.setup("Set 180 Rotation"));
    gui.add(mapPt0.setup("Point 0", ofVec2f(0, 0), ofVec2f(0, 0), feedDim));
    gui.add(mapPt1.setup("Point 1", ofVec2f(feedWidth/2, 0), ofVec2f(0, 0), feedDim));
    gui.add(mapPt2.setup("Point 2", ofVec2f(feedWidth, 0), ofVec2f(0, 0), feedDim));
    gui.add(mapPt3.setup("Point 3", ofVec2f(0, feedHeight/2), ofVec2f(0, 0), feedDim));
    gui.add(mapPt4.setup("Point 4", ofVec2f(feedWidth/2, feedHeight/2), ofVec2f(0, 0), feedDim));
    gui.add(mapPt5.setup("Point 5", ofVec2f(feedWidth, feedHeight/2), ofVec2f(0, 0), feedDim));
    gui.add(mapPt6.setup("Point 6", ofVec2f(0, feedHeight), ofVec2f(0, 0), feedDim));
    gui.add(mapPt7.setup("Point 7", ofVec2f(feedWidth/2, feedHeight), ofVec2f(0, 0), feedDim));
    gui.add(mapPt8.setup("Point 8", ofVec2f(feedWidth, feedHeight), ofVec2f(0, 0), feedDim));
    

    gui.setHeaderBackgroundColor(ofColor(255));
    
    //color applies to gui title only
    gui.setTextColor(ofColor(0));
    
    maskingLabel.setBackgroundColor(ofColor(255));
    bgDiffLabel.setBackgroundColor(ofColor(255));
    manipulationLabel.setBackgroundColor(ofColor(255));
    mappingLabel.setBackgroundColor(ofColor(255));
    croppingLabel.setBackgroundColor(ofColor(255));
    contoursLabel.setBackgroundColor(ofColor(255));
    
    //this changes the color of all the labels
    contoursLabel.setDefaultTextColor(ofColor(255));
    
}

void CamGui::loadSettings(){
    
    gui.loadFromFile(guiName + ".xml");
    
}

void CamGui::saveSettings(){
    
    gui.saveToFile(guiName + ".xml");
    
}

void CamGui::draw(int x, int y){
    
    gui.setPosition(x, y);
    gui.draw();
    
}

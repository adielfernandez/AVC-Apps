#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(200);
    ofSetVerticalSync(false);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    numCorridors = 1;
//    corridors.resize(numCorridors);
//    corridors[0].setup("192.168.1.5", "Corridor 2");
//    corridors[1].setup("192.168.1.5", "Corridor 3");
//    corridors[2].setup("192.168.1.5", "Corridor 4");
//    corridors[3].setup("192.168.1.5", "Corridor 5");
    
    
    bScaleDown = true;
    
    testCorridor.bScaleDown = bScaleDown;
    testCorridor.setup("192.168.1.5", "Corridor 2");
    
    feedWidth = 640;
    feedHeight = 512;
    
    
    
    
    //-----UI-----
    leftMargin = 230;
    topMargin = 70;
    centerMargin = 20;
    
    rawImagePos.set(leftMargin, topMargin);
    cvImagePos.set(leftMargin + feedWidth + centerMargin, topMargin);
    
    /*
     viewMode: What we're seeing
     0 = View corridor 2
     1 = View corridor 3
     2 = View corridor 4
     3 = View corridor 5
     4 = View CV for all corridors
    */
    viewMode = 0;
    
    
    
    titleFont.load("font/Aller_Rg.ttf", 50, true);
    
    //-----Data-----
    oscHandler.setup("localhost");
    
    
}

//--------------------------------------------------------------
void ofApp::update(){

    
    testCorridor.update();

    if(viewMode == 0){
        testCorridor.bEnableQuadMapping = true;
        testCorridor.adjustedQuadOrigin = rawImagePos;
    } else {
        //make sure we're not checking the mouse
        //if the corridor isn't the active one
        testCorridor.bEnableQuadMapping = false;
    }
    
    
    //update all corridors regardless of view
//    for(int i = 0; i < corridors.size(); i++){
//        corridors[i].update();
//    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

//    ofColor one(0, 23, 69);
//    ofColor two(0, 12, 36);

    ofColor one(80, 0, 0);
    ofColor two(30, 0, 0);
    
    ofBackgroundGradient(one, two);

    
//    if(viewMode >= 0 && viewMode <= 3){
//        
//        //add 2 to viewMode to get actual Corridor Name
//        ofSetColor(255);
//        titleFont.drawString(corridors[viewMode].name, rawImagePos.x, rawImagePos.y - 5);
//        
//        
//        corridors[viewMode].drawRaw(rawImagePos);
//        corridors[viewMode].drawCV(cvImagePos);
//        corridors[viewMode].drawGui();
//        
//        
//        
//        
//    } else {
//        
//        //draw all CV images
//        
//        
//    }
    
    
    if(viewMode == 0){
    
        ofSetColor(255);
        titleFont.drawString(testCorridor.name, rawImagePos.x, rawImagePos.y - 10);
        
        testCorridor.drawRaw(rawImagePos);

        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        }
        
        testCorridor.drawCV(cvImagePos, scale);
        testCorridor.drawGui(15, topMargin);

    }
    
    ofSetColor(255);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
    ofDrawBitmapString("Current Feed FPS: " + ofToString(testCorridor.cameraFPS), 15, 45);
    
    string msg;
    if(bScaleDown){
        msg = "CV Scaled Down";
    } else {
        msg = "CV NOT Scaled Down";
    }
    
    msg += "\nCurrent CV resolution: " + ofToString(testCorridor.threshPix.getWidth()) + " x " + ofToString(testCorridor.threshPix.getHeight());
    
    ofDrawBitmapString(msg, 800, 30);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}



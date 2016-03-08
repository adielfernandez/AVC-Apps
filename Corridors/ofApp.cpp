#include "ofApp.h"

using namespace ofxCv;


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(200);
    ofSetVerticalSync(false);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    
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
    viewMode = 4;
    
    
    titleFont.load("fonts/Aller_Rg.ttf", 50, true);
    smallerFont.load("fonts/Aller_Rg.ttf", 20, true);
    

    
    
    //if we're using image scaling for faster processing
    bScaleDown = true;
    
    int high = 80;
    int low = 30;
    
    

    
    //IP array
//    string addresses[numFeeds];
    
    //these will eventually be different
    addresses.resize(numFeeds);

    addresses[0] = "192.168.1.4";
    addresses[1] = "192.168.1.4";
    addresses[2] = "192.168.1.4";
    addresses[3] = "192.168.1.4";
    
    for(int i = 0; i < numFeeds; i++){
        
        auto cor = std::make_shared<Corridor>();
        cor -> setup(addresses[i], "corridor " + ofToString(i), false);
        corridors.emplace_back(new Corridor);
        
    }
    
    string IP = "192.168.1.4";

    
    corridor2.bScaleDown = bScaleDown;
    corridor2.setup(IP, "Corridor 2", false);
    corridor2.adjustedQuadOrigin = rawImagePos;
    corridor2.backgroundIn.set(high, 0, 0);
    corridor2.backgroundOut.set(low, 0, 0);
    
    corridor3.bScaleDown = bScaleDown;
    corridor3.setup(IP, "Corridor 3", false);
    corridor3.adjustedQuadOrigin = rawImagePos;
    corridor3.backgroundIn.set(high, high, 0);
    corridor3.backgroundOut.set(low, low, 0);
    
    corridor4.bScaleDown = bScaleDown;
    corridor4.setup(IP, "Corridor 4", false);
    corridor4.adjustedQuadOrigin = rawImagePos;
    corridor4.backgroundIn.set(0, high, 0);
    corridor4.backgroundOut.set(0, low, 0);
    
    corridor5.bScaleDown = bScaleDown;
    corridor5.setup(IP, "Corridor 5", false);
    corridor5.adjustedQuadOrigin = rawImagePos;
    corridor5.backgroundIn.set(0, 0, high);
    corridor5.backgroundOut.set(0, 0, low);
    

    
    
    
    //-----Data-----
    oscHandler.setup("localhost");
    
    //time between sending system snapshots (in ms)
    dataPerSec = 3;
    lastSendTime = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){

    


    if(viewMode == 0){
        
        
        corridor2.bEnableQuadMapping = true;
        corridor3.bEnableQuadMapping = false;
        corridor4.bEnableQuadMapping = false;
        corridor5.bEnableQuadMapping = false;
        
        
    } else if(viewMode == 1){
        
        
        corridor2.bEnableQuadMapping = false;
        corridor3.bEnableQuadMapping = true;
        corridor4.bEnableQuadMapping = false;
        corridor5.bEnableQuadMapping = false;
        
    } else if(viewMode == 2){
        
        
        corridor2.bEnableQuadMapping = false;
        corridor3.bEnableQuadMapping = false;
        corridor4.bEnableQuadMapping = true;
        corridor5.bEnableQuadMapping = false;
        
    } else if(viewMode == 3){
        
        
        corridor2.bEnableQuadMapping = false;
        corridor3.bEnableQuadMapping = false;
        corridor4.bEnableQuadMapping = false;
        corridor5.bEnableQuadMapping = true;
        
    } else {
        
        //make sure we're not checking the mouse
        //if the corridor isn't the active one
        

        
        
        

        
    }
    
    
    corridor2.update();
    corridor3.update();
    corridor4.update();
    corridor5.update();
    

    
}

//--------------------------------------------------------------
void ofApp::draw(){



    
    //draw the right corridor
    if(viewMode == 0){
    
        ofBackgroundGradient(corridor2.backgroundIn, corridor2.backgroundOut);
        
        ofSetColor(255);
        titleFont.drawString(corridor2.name, rawImagePos.x, rawImagePos.y - 10);
        
        corridor2.drawRaw(rawImagePos);

        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        }
        
        corridor2.drawCV(cvImagePos, scale);
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(corridor2.contours.size()), cvImagePos.x, cvImagePos.y + feedHeight/2 + 20);
        
        corridor2.drawGui(15, topMargin);
        
        ofSetColor(255);
        ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
        ofDrawBitmapString("Current Feed FPS: " + ofToString(corridor2.cameraFPS), 15, 45);
        
        string msg;
        if(bScaleDown){
            msg = "CV Scaled Down";
        } else {
            msg = "CV NOT Scaled Down";
        }
        
        msg += "\nCurrent CV resolution: " + ofToString(corridor2.threshPix.getWidth()) + " x " + ofToString(corridor2.threshPix.getHeight());
        
        ofDrawBitmapString(msg, 800, 30);
        
        

    } else if(viewMode == 1) {
        
        ofBackgroundGradient(corridor3.backgroundIn, corridor3.backgroundOut);
        
        ofSetColor(255);
        titleFont.drawString(corridor3.name, rawImagePos.x, rawImagePos.y - 10);
        
        corridor3.drawRaw(rawImagePos);
        
        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        }
        
        corridor3.drawCV(cvImagePos, scale);
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(corridor3.contours.size()), cvImagePos.x, cvImagePos.y + feedHeight/2 * scale + 20);
        
        corridor3.drawGui(15, topMargin);
        
        ofSetColor(255);
        ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
        ofDrawBitmapString("Current Feed FPS: " + ofToString(corridor3.cameraFPS), 15, 45);
        
        string msg;
        if(bScaleDown){
            msg = "CV Scaled Down";
        } else {
            msg = "CV NOT Scaled Down";
        }
        
        msg += "\nCurrent CV resolution: " + ofToString(corridor3.threshPix.getWidth()) + " x " + ofToString(corridor3.threshPix.getHeight());
        
        ofDrawBitmapString(msg, 800, 30);
        
    } else if(viewMode == 2) {
        
        ofBackgroundGradient(corridor4.backgroundIn, corridor4.backgroundOut);
        
        ofSetColor(255);
        titleFont.drawString(corridor4.name, rawImagePos.x, rawImagePos.y - 10);
        
        corridor4.drawRaw(rawImagePos);
        
        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        }
        
        corridor4.drawCV(cvImagePos, scale);
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(corridor4.contours.size()), cvImagePos.x, cvImagePos.y + feedHeight/2 * scale + 20);
        
        corridor4.drawGui(15, topMargin);
        
        ofSetColor(255);
        ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
        ofDrawBitmapString("Current Feed FPS: " + ofToString(corridor4.cameraFPS), 15, 45);
        
        string msg;
        if(bScaleDown){
            msg = "CV Scaled Down";
        } else {
            msg = "CV NOT Scaled Down";
        }
        
        msg += "\nCurrent CV resolution: " + ofToString(corridor4.threshPix.getWidth()) + " x " + ofToString(corridor4.threshPix.getHeight());
        
        ofDrawBitmapString(msg, 800, 30);
        
    } else if(viewMode == 3) {
        
        ofBackgroundGradient(corridor5.backgroundIn, corridor5.backgroundOut);
        
        ofSetColor(255);
        titleFont.drawString(corridor5.name, rawImagePos.x, rawImagePos.y - 10);
        
        corridor5.drawRaw(rawImagePos);
        
        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        }
        
        corridor5.drawCV(cvImagePos, scale);
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(corridor5.contours.size()), cvImagePos.x, cvImagePos.y + feedHeight/2 * scale + 20);
        
        corridor5.drawGui(15, topMargin);
        
        ofSetColor(255);
        ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
        ofDrawBitmapString("Current Feed FPS: " + ofToString(corridor5.cameraFPS), 15, 45);
        
        string msg;
        if(bScaleDown){
            msg = "CV Scaled Down";
        } else {
            msg = "CV NOT Scaled Down";
        }
        
        msg += "\nCurrent CV resolution: " + ofToString(corridor5.threshPix.getWidth()) + " x " + ofToString(corridor5.threshPix.getHeight());
        
        ofDrawBitmapString(msg, 800, 30);
        
    } else {
        
        
        //draw all the corridors
        
        ofBackgroundGradient(80, 30);

        
//        float spacing = 15;
//        float frameWidth = ofGetWidth()/2 - leftMargin - spacing * 2;
//        float frameHeight = frameWidth * feedHeight/feedWidth;
//        float guiSpacer = leftMargin;
//
        //position 4 screens around the window
//        ofVec2f p1, p2, p3, p4;
//        p1 = rawImagePos;
//        p2.set(rawImagePos.x + frameWidth + spacing + guiSpacer, rawImagePos.y);
//        p3.set(rawImagePos.x, rawImagePos.y + spacing + frameHeight);
//        p4.set(rawImagePos.x + frameWidth + spacing + guiSpacer, rawImagePos.y + spacing + frameHeight);

        float spacing = 10;
        float topSpacing = 30;
        float frameWidth = ofGetWidth()/4 - spacing*2;
        float frameHeight = frameWidth * feedHeight/feedWidth;

        
        ofVec2f p1, p2, p3, p4;
        p1.set(spacing, topSpacing);
        p2.set(p1.x + ofGetWidth()/4, topSpacing);
        p3.set(p1.x + ofGetWidth()/2, topSpacing);
        p4.set(p1.x + ofGetWidth()* 3/4, topSpacing);
        
        
        float frameScaling;
        
        if(bScaleDown){
            frameScaling = frameWidth/float(feedWidth/2);
        } else {
            frameScaling = frameWidth/float(feedWidth);
        }
        
        //draw color coded rects behind everything
        float trans = 100;
        ofFill();
        ofSetColor(corridor2.backgroundIn.r, corridor2.backgroundIn.g, corridor2.backgroundIn.b, trans);
        ofDrawRectangle(0, 0, ofGetWidth()/4, ofGetHeight());
        ofSetColor(corridor3.backgroundIn.r, corridor3.backgroundIn.g, corridor3.backgroundIn.b, trans);
        ofDrawRectangle(ofGetWidth()/4, 0, ofGetWidth()/4, ofGetHeight());
        ofSetColor(corridor4.backgroundIn.r, corridor4.backgroundIn.g, corridor4.backgroundIn.b, trans);
        ofDrawRectangle(ofGetWidth()/2, 0, ofGetWidth()/4, ofGetHeight());
        ofSetColor(corridor5.backgroundIn.r, corridor5.backgroundIn.g, corridor5.backgroundIn.b, trans);
        ofDrawRectangle(ofGetWidth()* 3/4, 0, ofGetWidth()/4, ofGetHeight());
        
        
        //draw CV
        corridor2.drawCV(p1, frameScaling);
        corridor3.drawCV(p2, frameScaling);
        corridor4.drawCV(p3, frameScaling);
        corridor5.drawCV(p4, frameScaling);
        
        //draw GUIs
        corridor2.drawGui(p1.x, p1.y + frameHeight + spacing);
        corridor3.drawGui(p2.x, p2.y + frameHeight + spacing);
        corridor4.drawGui(p3.x, p3.y + frameHeight + spacing);
        corridor5.drawGui(p4.x, p4.y + frameHeight + spacing);
        
        float guiSpacer = 210;
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs:\n" + ofToString(corridor2.contours.size()), p1.x + guiSpacer, p1.y + frameHeight + spacing*2);
        ofDrawBitmapString("Num Blobs:\n" + ofToString(corridor3.contours.size()), p2.x + guiSpacer, p2.y + frameHeight + spacing*2);
        ofDrawBitmapString("Num Blobs:\n" + ofToString(corridor4.contours.size()), p3.x + guiSpacer, p3.y + frameHeight + spacing*2);
        ofDrawBitmapString("Num Blobs:\n" + ofToString(corridor5.contours.size()), p4.x + guiSpacer, p4.y + frameHeight + spacing*2);
        
        float fontSpacing = 5;
        //draw titles
        ofSetColor(255);
        smallerFont.drawString(corridor2.name, p1.x, p1.y - fontSpacing);
        smallerFont.drawString(corridor3.name, p2.x, p2.y - fontSpacing);
        smallerFont.drawString(corridor4.name, p3.x, p3.y - fontSpacing);
        smallerFont.drawString(corridor5.name, p4.x, p4.y - fontSpacing);

        ofSetColor(255);
        ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), spacing, ofGetHeight() - 15);
    }

    
    
    
    //OSC Data sending
    //only send according to sendInterval (200 ms, i.e. 5X per second)
    if(ofGetElapsedTimeMillis() - lastSendTime > (1000/dataPerSec)){
    
        //need to go through all the contour finders
        //corridor 2
        for(int i = 0; i < corridor2.contours.size(); i++){
            
            ofPoint center = toOf(corridor2.contours.getCenter(i));
            int label = corridor2.contours.getLabel(i);
            ofVec2f velocity = toOf(corridor2.contours.getVelocity(i));
            
            oscHandler.sendBlob(2, label, center, velocity);
        }
        
        //corridor 3
        for(int i = 0; i < corridor3.contours.size(); i++){
            
            ofPoint center = toOf(corridor3.contours.getCenter(i));
            int label = corridor3.contours.getLabel(i);
            ofVec2f velocity = toOf(corridor3.contours.getVelocity(i));
            
            oscHandler.sendBlob(3, label, center, velocity);
        }
        
        //corridor 4
        for(int i = 0; i < corridor4.contours.size(); i++){
            
            ofPoint center = toOf(corridor4.contours.getCenter(i));
            int label = corridor4.contours.getLabel(i);
            ofVec2f velocity = toOf(corridor4.contours.getVelocity(i));
            
            oscHandler.sendBlob(4, label, center, velocity);
        }
        
        //corridor 5
        for(int i = 0; i < corridor5.contours.size(); i++){
            
            ofPoint center = toOf(corridor5.contours.getCenter(i));
            int label = corridor5.contours.getLabel(i);
            ofVec2f velocity = toOf(corridor5.contours.getVelocity(i));
            
            oscHandler.sendBlob(5, label, center, velocity);
        }
    
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    //view mode
    if(key == '1'){
        viewMode = 0;
    } else if(key == '2'){
        viewMode = 1;
    } else if(key == '3'){
        viewMode = 2;
    } else if(key == '4'){
        viewMode = 3;
    } else if(key == '0'){
        viewMode = 4;
    }
    
    //cycle right
    if(key == OF_KEY_RIGHT){
        viewMode++;
        if(viewMode > 4) viewMode = 0;
    }
    
    //cycle left
    if(key == OF_KEY_LEFT){
        viewMode--;
        if(viewMode < 0) viewMode = 4;
    }
    
    //save all
    if(key == 's'){
        
        corridor2.corridorGui.gui.saveToFile(corridor2.name + ".xml");
        corridor3.corridorGui.gui.saveToFile(corridor3.name + ".xml");
        corridor4.corridorGui.gui.saveToFile(corridor4.name + ".xml");
        corridor5.corridorGui.gui.saveToFile(corridor5.name + ".xml");
        
    }
    
    
    
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



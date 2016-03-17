//
//  Aggregator.cpp
//  Stitching Test
//
//  Created by Adiel Fernandez on 3/3/16.
//
//

#include "Aggregator.hpp"


Aggregator::Aggregator(){
    
    
}


void Aggregator::setup(int _numFeeds, int w, int h){
    
    numFeeds = _numFeeds;
    feedWidth = w;
    feedHeight = h;
    mouseHandleRad = 8;
    
    //initialize positions in a tile pattern
    //they'll be moved later
    for(int i = 0; i < numFeeds; i++){
        
        int h;
        
        if(i > 2){
            h = feedHeight;
        } else {
            h = 0;
        }
        
        ofVec2f p((i % 3) * feedWidth, h);
        positions.push_back(p);
        
    
        posMouseLock.push_back(false);
        
    }
    
    //need to find the max dimensions of the aggregate
    //depeding on how much overlap there is

    
    if(numFeeds > 3){
        overallWidth = 3 * feedWidth;
        overallHeight = 2 * feedHeight;
    } else {
        overallWidth = numFeeds * feedWidth;
        overallHeight = feedHeight;
    }
    
    
    handleCol.set(255, 0, 0);
    grabbedCol.set(0, 255, 0);
    
    
    
    //-----Gui-----
    string name = "Main Lobby Aggregate";
    aGui.setup(name);
    aGui.load();
    
    bEnableMouse = false;
    
    
}


void Aggregator::getNewData(vector<shared_ptr<ofTexture>> rawTex, vector<shared_ptr<ofPixels>> threshPix){

    cameraRawTexs = rawTex;
    cameraThreshPixels = threshPix;
    

    
    
}


void Aggregator::update(){

    //set positions to gui values
    positions[0].set(aGui.camPos1 -> x, aGui.camPos1 -> y);
    positions[1].set(aGui.camPos2 -> x, aGui.camPos2 -> y);
    positions[2].set(aGui.camPos3 -> x, aGui.camPos3 -> y);
    positions[3].set(aGui.camPos4 -> x, aGui.camPos4 -> y);
    positions[4].set(aGui.camPos5 -> x, aGui.camPos5 -> y);
    positions[5].set(aGui.camPos6 -> x, aGui.camPos6 -> y);
    
    
    //handle mouse interaction with map pts
    adjustedMouse.set(ofGetMouseX() - adjustedOrigin.x, ofGetMouseY() - adjustedOrigin.y);
    
    
    //update the positions iwth the mouse if we've locked on to them
    //from the mousePressed method
    for(int i = 0; i < positions.size(); i++){
        
        if(posMouseLock[i]){
            positions[i].x = ofClamp(adjustedMouse.x, 0, feedWidth * 3);
            positions[i].y = ofClamp(adjustedMouse.y, 0, feedHeight * 2);
            
            //update the gui values so they can be saved
            if(i == 0){
                aGui.camPos1 = positions[i];
            } else if(i == 1){
                aGui.camPos2 = positions[i];
            } else if(i == 2){
                aGui.camPos3 = positions[i];
            } else if(i == 3){
                aGui.camPos4 = positions[i];
            } else if(i == 4){
                aGui.camPos5 = positions[i];
            } else if(i == 5){
                aGui.camPos6 = positions[i];
            }
            
        }
        
        
        
    }
    
    
    //update the positions vector to where the feeds have been moved
    int furthestLeft = 0;
    int furthestDown = 0;
    
    for(int i = 0; i < positions.size(); i++){
        if(positions[i].x > furthestLeft) furthestLeft = positions[i].x;
        if(positions[i].y > furthestDown) furthestDown = positions[i].y;
    }
    
    overallWidth = furthestLeft + feedWidth;
    overallHeight = furthestDown + feedHeight;
    
    
    //clear it
    masterPix.clear();
    
    //recalculate the overall size of the master pixels object
    //based on the positions of the frames within it
    masterPix.allocate(overallWidth, overallHeight, 1);
    masterPix.setColor(ofColor(0));
    
    //paste the other ofPixels into masterPix
    for(int i = 0; i < numFeeds; i++){
        
        cameraThreshPixels[i] -> blendInto(masterPix, positions[i].x, positions[i].y);
        
    }
    
    if(aGui.drawThresholdToggle){
        masterImg.allocate(masterPix.getWidth(), masterPix.getHeight(), OF_IMAGE_GRAYSCALE);
        masterImg.setFromPixels(masterPix);
    }
    
    
    //for mouse interaction
    if(bEnableMouse){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }

    
}

void Aggregator::drawRaw(int x, int y){
    
    ofPushMatrix();
    ofTranslate(x, y);
    
    for(int i = 0; i < cameraRawTexs.size(); i++){
        
        ofSetColor(255, 150);
        cameraRawTexs[i] -> draw(positions[i]);
        ofSetColor(255);
        ofDrawBitmapString("Cam " + ofToString(i + 1), positions[i].x + 5, positions[i].y + 12);
        
        
    }
    
    
    
    ofPopMatrix();
    
    
    
    
}

void Aggregator::drawCV(int x, int y){
    
        
    ofPushMatrix();
    ofTranslate(x, y);

    
    
    if(aGui.drawThresholdToggle){
        


        ofSetColor(255);
        masterImg.draw(0, 0);
        
        
        

        
    }
    
    
    ofNoFill();
    
    //draw mouse handles
    for(int i = 0; i < positions.size(); i++){

        ofColor c;
        
        if(posMouseLock[i]){
            c = grabbedCol;
        } else {
            c = handleCol;
        }
        
        ofSetColor(c);
        ofDrawCircle(positions[i], mouseHandleRad);
        
        //draw camera frames to show overlap regions
        ofSetColor(c, 100);
        ofDrawRectangle(positions[i], feedWidth, feedHeight);
        
    }
    
    
    
    ofPopMatrix();
    
    
    
    
}



void Aggregator::mousePressed(ofMouseEventArgs &args){

    
    if(bEnableMouse){

    
        //handle mouse interaction with quadMap points
        for(int i = 0; i < positions.size(); i++){
            
            //need to adjust mouse position since we're
            //translating the raw camera view from the origin with pushmatrix
            float dist = ofDist(adjustedMouse.x, adjustedMouse.y, positions[i].x, positions[i].y);
            
            if(dist < mouseHandleRad){
                posMouseLock[i] = true;
            } else {
                posMouseLock[i] = false;
            }
            
            
        }
        
    }
}

void Aggregator::mouseReleased(ofMouseEventArgs &args){
    
    for(int i = 0; i < posMouseLock.size(); i++){
        posMouseLock[i] = false;
    }
    
}

void Aggregator::mouseMoved(ofMouseEventArgs &args){}
void Aggregator::mouseDragged(ofMouseEventArgs & args){}
void Aggregator::mouseScrolled(ofMouseEventArgs & args){}
void Aggregator::mouseEntered(ofMouseEventArgs & args){}
void Aggregator::mouseExited(ofMouseEventArgs & args){}


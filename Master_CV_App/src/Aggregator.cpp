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

void Aggregator::setupViewControl(int _thisView, int *_viewMode, ofVec2f _adjOrigin){
    
    thisView = _thisView;
    viewMode = _viewMode;
    adjustedOrigin = _adjOrigin;
    
}

void Aggregator::setup(string _name, int _numCams, vector<shared_ptr<Camera>> _cams, bool _bScaleDown){
    
    name = _name;
    numCams = _numCams;
    feedWidth = 640;
    feedHeight = 512;
    
    cams = _cams;
    bScaleDown = _bScaleDown;
    
    if(bScaleDown){
        scaledWidth = feedWidth/2;
        scaledHeight = feedHeight/2;
    } else {
        scaledWidth = feedWidth;
        scaledHeight = feedHeight;
    }
    
    
    mouseHandleRad = 8;
    
    //initialize positions in a tile pattern
    //they'll be moved later
    for(int i = 0; i < numCams; i++){
        
        int h;
        
        if(i > 2){
            h = scaledHeight;
        } else {
            h = 0;
        }
        
        ofVec2f p((i % 3) * scaledWidth, h);
        positions.push_back(p);
        
    
        posMouseLock.push_back(false);
        
    }
    
    
    
    //a helper vector to hold the indices of the cameras we're using
    
    indices.resize(numCams);
    for(int i = 0; i < numCams; i++){
        
        //corridor one: cams 0, 1, 2, 3, 4, 5
        //corridor six: cams 10, 11, 12, 13
        indices[i] = (numCams == 6 ? 0 : 10) + i;
        
    }
    
    
    
    
    
    
    //need to find the max dimensions of the aggregate
    //depeding on how much overlap there is

    
    if(numCams > 3){
        overallWidth = 3 * scaledWidth;
        overallHeight = 2 * scaledHeight;
    } else {
        overallWidth = numCams * scaledWidth;
        overallHeight = scaledHeight;
    }
    
    
    handleCol.set(255);
    grabbedCol.set(0, 255, 0);
    
    
    
    //-----Gui-----
    guiName = name;
    
    //param 1 = gui title, param 2 = filename, then 3&4 = startposition
    gui.setup(guiName, guiName + ".xml", 0, 0);
    
    gui.add(blurAmountSlider.setup("Blur", 1, 0, 30));
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
    
    if(numCams > 4){
        gui.add(camPos5.setup("Cam5 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
        gui.add(camPos6.setup("Cam6 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    }
        
    loadSettings();
    
    
    
}




void Aggregator::update(){

    //set positions to gui values
    positions[0].set(camPos1 -> x, camPos1 -> y);
    positions[1].set(camPos2 -> x, camPos2 -> y);
    positions[2].set(camPos3 -> x, camPos3 -> y);
    positions[3].set(camPos4 -> x, camPos4 -> y);
    if(numCams > 4){
        positions[4].set(camPos5 -> x, camPos5 -> y);
        positions[5].set(camPos6 -> x, camPos6 -> y);
    }
    
    //handle mouse interaction with map pts
    adjustedMouse.set(ofGetMouseX() - adjustedOrigin.x, ofGetMouseY() - adjustedOrigin.y);
    
    
    //update the positions with the mouse if we've locked on to them
    //from the mousePressed method
    for(int i = 0; i < positions.size(); i++){
        
        if(posMouseLock[i]){
            positions[i].x = ofClamp(adjustedMouse.x, 0, scaledWidth * 3);
            positions[i].y = ofClamp(adjustedMouse.y, 0, scaledHeight * 2);
            
            //update the gui values so they can be saved
            if(i == 0){
                camPos1 = positions[i];
            } else if(i == 1){
                camPos2 = positions[i];
            } else if(i == 2){
                camPos3 = positions[i];
            } else if(i == 3){
                camPos4 = positions[i];
            } else if(i == 4){
                camPos5 = positions[i];
            } else if(i == 5){
                camPos6 = positions[i];
            }
            
        }
        
        
        
    }
    
    
    //get the furthest left and down parts of the aggregated image
    //so the masterPix object can be resized appropriately
    int furthestLeft = 0;
    int furthestDown = 0;
    
    for(int i = 0; i < positions.size(); i++){
        
        int thisMaxLeft = positions[i].x + cams[indices[i]] -> threadOutput.getWidth();
        if(thisMaxLeft > furthestLeft){
            furthestLeft = thisMaxLeft;
        }
        
        int thisMaxRight = positions[i].y + cams[indices[i]] -> threadOutput.getHeight();
        if(thisMaxRight > furthestDown){
            furthestDown = thisMaxRight;
        }
        
    }
    
    overallWidth = furthestLeft;
    overallHeight = furthestDown;
    
    
    //clear it
    masterPix.clear();
    
    //recalculate the overall size of the master pixels object
    //based on the positions of the frames within it
    masterPix.allocate(overallWidth, overallHeight, 1);
    masterPix.setColor(ofColor(0));
    
    //paste the other ofPixels into masterPix
    for(int i = 0; i < numCams; i++){
        
        cams[indices[i]] -> threadOutput.blendInto(masterPix, positions[i].x, positions[i].y);
        
    }
    
    //now take the masterPix object and do some CV on it
    
    
    

    
    //load pixels into an image just for drawing
    if(drawThresholdToggle){
        masterImg.allocate(masterPix.getWidth(), masterPix.getHeight(), OF_IMAGE_GRAYSCALE);
        masterImg.setFromPixels(masterPix);
    }
    
    
    //if we're in the right view
    if((*viewMode) == thisView){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }
    

}

void Aggregator::drawRaw(int x, int y){
    
    ofPushMatrix();
    ofTranslate(x, y);
    
    for(int i = 0; i < numCams; i++){
        
        ofSetColor(255, 200);
        cams[indices[i]] -> drawCroppedTex(positions[i]);
        
        ofSetColor(255);
        ofDrawBitmapString("Cam " + ofToString(indices[i] + 1), positions[i].x + 5, positions[i].y + 12);
        
        
    }
    
    
    
    ofPopMatrix();
    
    
    
    
}

void Aggregator::drawCV(int x, int y){
    
        
    ofPushMatrix();
    ofTranslate(x, y);

    
    
    if(drawThresholdToggle){
        


        ofSetColor(255);
        masterImg.draw(0, 0);
        
    }
    
    
    ofNoFill();
    
    ofSetLineWidth(0.5);
    
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
        ofDrawRectangle(positions[i], cams[indices[i]] -> threadOutput.getWidth(), cams[indices[i]] -> threadOutput.getHeight());
        
    }
    
    ofFill();
    
    ofPopMatrix();
    
    
    
    
}


void Aggregator::drawGui(int x, int y){
    
    gui.setPosition(x, y);
    gui.draw();

}

void Aggregator::loadSettings(){
    
    gui.loadFromFile(guiName + ".xml");
    
}



void Aggregator::mousePressed(ofMouseEventArgs &args){
    
    //handle mouse interaction with quadMap points
    for(int i = 0; i < positions.size(); i++){
        
        //need to adjust mouse position since we're
        //translating the raw camera view from the origin with pushmatrix
        float dist = ofDist(adjustedMouse.x, adjustedMouse.y, positions[i].x, positions[i].y);
        
        if(dist < mouseHandleRad){
            posMouseLock[i] = true;
            
            //exit the for loop, prevents locking
            //onto multiple handles at once
            break;
            
        } else {
            posMouseLock[i] = false;
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


//
//  Aggregator.cpp
//  Stitching Test
//
//  Created by Adiel Fernandez on 3/3/16.
//
//

#include "Aggregator.hpp"

using namespace ofxCv;

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
    
    
    handleCol.set(0, 255, 0);
    grabbedCol.set(0, 150, 255);
    
    
    
    //-----Gui-----
    guiName = name;
    
    //param 1 = gui title, param 2 = filename, then 3&4 = startposition
    gui.setup(guiName, guiName + ".xml", 0, 0);
    
    gui.add(thresholdSlider.setup("Threshold", 0, 0, 255));
    gui.add(blurAmountSlider.setup("Blur", 1, 0, 50));
    gui.add(numErosionsSlider.setup("Number of erosions", 0, 0, 10));
    gui.add(numDilationsSlider.setup("Number of dilations", 0, 0, 10));
    gui.add(minBlobAreaSlider.setup("Min Blob Area", 0, 0, 500));
    gui.add(maxBlobAreaSlider.setup("Max Blob Area", 25000, 0, 50000));
    gui.add(persistenceSlider.setup("Persistence", 15, 0, 100));
    gui.add(maxDistanceSlider.setup("Max Distance", 32, 0, 100));
    gui.add(drawBlurredToggle.setup("Draw blurred", false));
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
        
        //if we're the second camera of the 4 camera lobby (lobby 2)
        //rotate the ofPixels to paste it in correctly
        if(numCams == 4 && i == 1){
            
            ofPixels rotated;
            cams[indices[i]] -> threadOutput.rotate90To(rotated, 1);
            
            rotated.blendInto(masterPix, positions[i].x, positions[i].y);
            
        } else {
            
            //all other cameras get pasted in normally
            cams[indices[i]] -> threadOutput.blendInto(masterPix, positions[i].x, positions[i].y);
            
        }
        
    }
    
    
    //now take the masterPix object and do some CV on it
    
    //blur it
    GaussianBlur(masterPix, blurredMaster, blurAmountSlider);
    
    //threshold it
    threshold(blurredMaster, threshMaster, thresholdSlider);
    
    //ERODE it
    for(int e = 0; e < numErosionsSlider; e++){
        erode(threshMaster);
    }
    
    //DILATE it
    for(int d = 0; d < numDilationsSlider; d++){
        dilate(threshMaster);
    }
    
    
    
    
    
    //-----Done with image altering-----
    //------Now do contour finding------
    
    
    //Define contour finder
    contours.setMinArea(minBlobAreaSlider);
    contours.setMaxArea(maxBlobAreaSlider);
    contours.setThreshold(254);  //only detect white
    
    // wait for half a frame before forgetting something
    contours.getTracker().setPersistence(persistenceSlider);
    
    // an object can move up to ___ pixels per frame
    contours.getTracker().setMaximumDistance(maxDistanceSlider);
    
    //find dem blob
    contours.findContours(threshMaster);
    

    
    //load pixels into an image just for drawing
    if(drawBlurredToggle){
        blurredMasterImg.allocate(blurredMaster.getWidth(), blurredMaster.getHeight(), OF_IMAGE_GRAYSCALE);
        blurredMasterImg.setFromPixels(blurredMaster);
    }
    
    if(drawThresholdToggle){
        threshMasterImg.allocate(threshMaster.getWidth(), threshMaster.getHeight(), OF_IMAGE_GRAYSCALE);
        threshMasterImg.setFromPixels(threshMaster);
    }
    
    
    //if we're in the right view
    if((*viewMode) == thisView){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }
    
    
    
    
    //get together all the average blob data for this corridor
    avgDir.set(0);
    avgVel.set(0);
    avgPos.set(0);
    avgSpeed = 0;
    
    corridorBundle.clear();
    corridorStats.clear();
    blobsBundle.clear();
    
    
    //which Corridor is it
    int oneOrSix;
    
    if(numCams == 6){
        oneOrSix = 1;
    } else {
        oneOrSix = 6;
    }
    
    
    
    //go through the blobs and start collecting data
    for(int i = 0; i < contours.size(); i++){
        
        //get data from contour
        int label = contours.getLabel(i);
        ofPoint center = toOf(contours.getCenter(i));
        ofPoint centerNormalized = center/ofVec2f(scaledWidth, scaledHeight);
        ofVec2f velocity = toOf(contours.getVelocity(i));
        
        //add it to our averages
        avgVel += velocity;
        avgSpeed += velocity.length();
        avgPos += centerNormalized;
        
        //prepare a message for this blob then
        //push it into the blobMessages vector
        ofxOscMessage m;
        
        m.setAddress("/corridor_" + ofToString(oneOrSix) + "/blob_data");
        m.addIntArg(label);
        m.addFloatArg(centerNormalized.x);
        m.addFloatArg(centerNormalized.y);
        m.addFloatArg(velocity.x);
        m.addFloatArg(velocity.x);
        
        blobsBundle.addMessage(m);
        
    }
    
    //average out all the data for this corridor
    avgSpeed = avgSpeed/float(contours.size());
    avgVel = avgVel/float(contours.size());
    avgDir = avgVel.getNormalized();
    avgPos = avgPos/float(contours.size());
    

    
    //prepare the corridor stats message
    corridorStats.setAddress("/corridor_" + ofToString(oneOrSix) + "_stats");
    corridorStats.addIntArg(contours.size());
    corridorStats.addFloatArg(avgPos.x);
    corridorStats.addFloatArg(avgPos.y);
    corridorStats.addFloatArg(avgDir.x);
    corridorStats.addFloatArg(avgDir.y);
    corridorStats.addFloatArg(avgSpeed);
    
    //now assemble the corridor bundle from the stats message and all the blobs
    corridorBundle.addMessage(corridorStats);
    corridorBundle.addBundle(blobsBundle);
    

}

void Aggregator::drawRaw(int x, int y){
    
    ofPushMatrix();
    ofTranslate(x, y);
    
    for(int i = 0; i < numCams; i++){
        
        ofSetColor(255, 200);
        
        if(numCams == 4 && i == 1){

            //draw rotated 90 degrees clockwise
            ofPushMatrix();
            ofTranslate(positions[i].x, positions[i].y);
            ofRotate(90);
            ofTranslate(0, -cams[indices[i]] -> croppedHeight);
            
            
            cams[indices[i]] -> drawCroppedTex(ofVec2f(0, 0));
            
            ofPopMatrix();
            
        } else {
            //draw normally
            cams[indices[i]] -> drawCroppedTex(positions[i]);
        }
        
        ofSetColor(255);
        ofDrawBitmapString("Cam " + ofToString(indices[i] + 1), positions[i].x + 5, positions[i].y + 12);
        
    }
    
    
    
    ofPopMatrix();
    
    
    
    
}

void Aggregator::drawCV(int x, int y){
    
        
    ofPushMatrix();
    ofTranslate(x, y);

    
    if(drawBlurredToggle){
        
        ofSetColor(255);
        blurredMasterImg.draw(0, 0);
        
    }
    
    if(drawThresholdToggle){
        
        ofSetColor(255);
        threshMasterImg.draw(0, 0);
        
    }
    
    
    if(drawContoursToggle){
        
        ofSetColor(255, 0, 0);
        ofPushMatrix();
        ofSetLineWidth(1);
        
        contours.draw();
        
        for(int i = 0; i < contours.size(); i++) {
            ofPoint center = toOf(contours.getCenter(i));
            ofPushMatrix();
            ofTranslate(center.x, center.y);
            int label = contours.getLabel(i);
            ofVec2f velocity = toOf(contours.getVelocity(i));
            
            string msg = ofToString(label);
            
            if(showInfoToggle){
               
//                msg = ofToString(label) + " : " + ofToString(center.x) + "," + ofToString(center.y) + " : " + ofToString(velocity.x) + "," + ofToString(velocity.y);

                ofSetColor(0, 100, 255);
                ofDrawBitmapString(msg, 0, 0);
            }
            
        }
        ofPopMatrix();
        
    }
    
    
    
    ofNoFill();
    
    
    
    //draw mouse handles
    for(int i = 0; i < positions.size(); i++){

        ofColor c;
        
        if(posMouseLock[i]){
            c = grabbedCol;
            ofSetLineWidth(2);
        } else {
            c = handleCol;
            ofSetLineWidth(0.5);
        }
        
        ofSetColor(c);
        ofDrawCircle(positions[i], mouseHandleRad);
        
        //draw camera frames to show overlap regions
        ofSetColor(c, 100);
        //if we're camera 2 in corridor 6 draw it rotated
        if(numCams == 4 && i == 1){
            
            //draw with width/height switched
            ofDrawRectangle(positions[i], cams[indices[i]] -> threadOutput.getHeight(), cams[indices[i]] -> threadOutput.getWidth());
            
        } else {
            
            //draw normally
            ofDrawRectangle(positions[i], cams[indices[i]] -> threadOutput.getWidth(), cams[indices[i]] -> threadOutput.getHeight());
        }
        
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

void Aggregator::saveSettings(){
    
    gui.saveToFile(guiName + ".xml");
    
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


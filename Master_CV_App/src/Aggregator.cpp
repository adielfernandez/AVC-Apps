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
    //depeding on how much overlap there is. This will
    //overwritten by settings/manipulation later
    if(numCams > 3){
        masterWidth = 3 * scaledWidth;
        masterHeight = 2 * scaledHeight;
    } else {
        masterWidth = numCams * scaledWidth;
        masterHeight = scaledHeight;
    }
    
    
    handleCol.set(0, 255, 0);
    grabbedCol.set(0, 150, 255);
    disableCol.set(255, 100);
    
    
    //-----Gui-----
    guiName = name;
    
    //param 1 = gui title, param 2 = filename, then 3&4 = startposition
    gui.setup(guiName, guiName + ".xml", 0, 0);
    
    gui.add(manipulationLabel.setup("   IMAGE MANIPULATION", ""));
    gui.add(blurAmountSlider.setup("Blur", 1, 0, 50));
    gui.add(numErosionsSlider.setup("Number of erosions", 0, 0, 10));
    gui.add(numDilationsSlider.setup("Number of dilations", 0, 0, 10));
    gui.add(thresholdSlider.setup("Threshold", 0, 0, 255));
    gui.add(drawThresholdToggle.setup("Draw threshold", false));
    
    gui.add(bgDiffLabel.setup("   BG SUBTRACTION", ""));
    gui.add(useBgDiff.setup("Use BG Diff", false));
    gui.add(learningTime.setup("Frames to learn BG", 100, 0, 1000));
    gui.add(resetBG.setup("Reset Background"));
    
    gui.add(contoursLabel.setup("   CONTOUR FINDING", ""));
    gui.add(minBlobAreaSlider.setup("Min Blob Area", 0, 0, 500));
    gui.add(maxBlobAreaSlider.setup("Max Blob Area", 12000, 0, 5000));
    gui.add(persistenceSlider.setup("Persistence", 15, 0, 100));
    gui.add(maxDistanceSlider.setup("Max Distance", 32, 0, 100));
    gui.add(drawContoursToggle.setup("Draw Contours", true));
    gui.add(showInfoToggle.setup("Info", false));
    
    gui.add(positionsLabel.setup("   FEED POSITIONS", ""));
    gui.add(trimPixels.setup("Trim Dead Space", false));
    gui.add(camPos1.setup("Cam1 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos2.setup("Cam2 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos3.setup("Cam3 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    gui.add(camPos4.setup("Cam4 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    
    if(numCams > 4){
        gui.add(camPos5.setup("Cam5 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
        gui.add(camPos6.setup("Cam6 Pos", ofVec2f(0, 0), ofVec2f(0, 0), ofVec2f(640, 512)));
    }
        
    loadSettings();
    
    
    aggregateProcessor.setup(&threadOutputPix, &contours);

    //background colors (red means thread has crashed)
    backgroundInCol.set(80);
    backgroundOutCol.set(0);
    
    waitingToBgDiff = true;
    
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
    
    //disable positioning if we're doing BG Diff
    disablePositioning = useBgDiff;
    
    
    //update the positions with the mouse if we've locked on to them
    //from the mousePressed method
    if(!disablePositioning){
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
    }

    //get the furthest Right and down parts of the aggregated image
    //so the masterPix object can be resized appropriately
    int furthestRight = 0;
    int furthestDown = 0;
    int furthestLeft = 10000;
    int furthestUp = 10000;
    
    for(int i = 0; i < positions.size(); i++){
        
        int thisMaxRight = positions[i].x + cams[indices[i]] -> threadOutput.getWidth();
        if(thisMaxRight > furthestRight){
            furthestRight = thisMaxRight;
        }
        
        int thisMaxBottom = positions[i].y + cams[indices[i]] -> threadOutput.getHeight();
        if(thisMaxBottom > furthestDown){
            furthestDown = thisMaxBottom;
        }
        
        
        if(positions[i].x < furthestLeft) furthestLeft = positions[i].x;
        if(positions[i].y < furthestUp) furthestUp = positions[i].y;
        
    }
    
    masterWidth = furthestRight;
    masterHeight = furthestDown;
    
    //trim out the space above and to the left in masterPix
    //but only if we're not current background differencing
    //or the thread will crash
    if(trimPixels && !useBgDiff){

        //subtract from all the camera positions
        for(int i = 0; i < positions.size(); i++){
            positions[i].x -= furthestLeft;
            positions[i].y -= furthestUp;
        }
            
        //and update the gui to store the values
        camPos1 = positions[0];
        camPos2 = positions[1];
        camPos3 = positions[2];
        camPos4 = positions[3];
        camPos5 = positions[4];
        camPos6 = positions[5];
        
    }
    
    
    //clear it
    masterPix.clear();
    
    //recalculate the overall size of the master pixels object
    //based on the positions of the frames within it
    masterPix.allocate(masterWidth, masterHeight, 1);
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
    
    //now package and send stuff to the threadedCV object
    //but only do it about 30 fps since the app will run faster
    //than we'll be getting frames
    
    int desiredFPS = 30;
    int delayTime = 1000/(float)desiredFPS;
    
    if(ofGetElapsedTimeMillis() - lastAnalyzeTime > delayTime){
        
     
        bool useBg;
        if(!waitingToBgDiff){
            
            //if we're not waiting, get the value from the gui
            useBg = useBgDiff;
        } else {
            
            //if we are waiting, then send a false to the aggregate thread
            useBg = false;
        }
        
        
        //construct a vector of ints with all the settings
        vector<int> settings;
        settings.resize(11);
        
        settings[0] = blurAmountSlider;
        settings[1] = numErosionsSlider;
        settings[2] = numDilationsSlider;
        
        settings[3] = learningTime;
        settings[4] = useBg;      //bool casted as int into vector
        settings[5] = resetBG;        //bool casted as int into vector
        settings[6] = thresholdSlider;
        settings[7] = minBlobAreaSlider;
        settings[8] = maxBlobAreaSlider;
        settings[9] = persistenceSlider;
        settings[10] = maxDistanceSlider;
        
        masterPix.setImageType(OF_IMAGE_GRAYSCALE);
    
        aggregateProcessor.analyze(masterPix, settings);
        
        lastAnalyzeTime = ofGetElapsedTimeMillis();

        
    }
    
    
    
    //update the thread more to receive its data asap
    aggregateProcessor.update();
    
    //don't actually start the background until a few seconds in
    //when settings are loaded and cameras have started
    if(ofGetElapsedTimeMillis() < 4000){
        waitingToBgDiff = true;
    } else {
        waitingToBgDiff = false;
    }

    
    
    
    
    //if we're in the right view listen to the mouse
    if((*viewMode) == thisView){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }
    
    //Get together all the average blob data for this corridor.
    //This puts all the data into bundles but does not send
    //Sending of all the system data happens at once in ofApp.
    gatherOscStats();
    
    //if we haven't received anything from the thread in over a second
    //assume the thread crashed and change the background color to red as a warning
    if(!waitingToBgDiff && ofGetElapsedTimeMillis() - aggregateProcessor.lastDataSendTime > 1000){
        backgroundInCol.lerp(ofColor(100, 0, 0), 0.08);
        backgroundOutCol.lerp(ofColor(10, 0, 0), 0.08);
    }

    
    
}


void Aggregator::drawMain(){
    
    
    drawRaw(adjustedOrigin.x, adjustedOrigin.y);
    
    drawCV(adjustedOrigin.x, adjustedOrigin.y);
    
    drawGui(15, adjustedOrigin.y);
    
    
    
}





void Aggregator::drawRaw(int x, int y){
    
    ofPushMatrix();
    ofTranslate(x, y);
    
    //draw border around entire masterPix object
    ofSetColor(180, 0, 0, 100);
    ofDrawRectangle(0, 0, masterWidth, masterHeight);
    ofNoFill();
    ofSetColor(255);
    ofSetLineWidth(1.0);
    ofDrawRectangle(0, 0, masterWidth, masterHeight);
    ofFill();
    
    for(int i = 0; i < numCams; i++){
        
        ofSetColor(255, 255);
        
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
    
    ofSetColor(200);
    ofDrawBitmapString("W: " + ofToString(masterWidth) + "\nH: " + ofToString(masterHeight) , masterWidth + 5, masterHeight + 15);
    
    
    //Blob data
    
    string blobData = "";
    
    blobData += "Num Blobs: " + ofToString(contours.size());
    blobData += "\nAvg. Pos X (norm): " + ofToString(avgPos.x);
    blobData += "\nAvg. Pos Y (norm): " + ofToString(avgPos.y);
    blobData += "\nAvg. Heading X (norm): " + ofToString(avgDir.x);
    blobData += "\nAvg. Heading Y (norm): " + ofToString(avgDir.y);
    blobData += "\nAvg. Speed (raw): " + ofToString(avgSpeed);
    
    ofSetColor(255);
    ofDrawBitmapString(blobData, masterWidth + 20, 10);
    
    
    ofPopMatrix();
    
    
}

void Aggregator::drawCV(int x, int y){
    
        
    ofPushMatrix();
    ofTranslate(x, y);
    
    if(drawThresholdToggle){
        
        threshMasterImg.allocate(threadOutputPix.getWidth(), threadOutputPix.getHeight(), OF_IMAGE_GRAYSCALE);
        
        threshMasterImg.setFromPixels(threadOutputPix);

        ofSetColor(255, 180);
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
        
        if(disablePositioning){
            c = disableCol;
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
    
    ofSetColor(255);
    ofDrawBitmapString("CV Thread ID: " + ofToString(aggregateProcessor.getThreadId()), 800, 30);
    ofDrawBitmapString("Last Data from Thread: " + ofToString(ofGetElapsedTimeMillis() - aggregateProcessor.lastDataSendTime) + " \tms ago", 800, 45);
    
    if(useBgDiff){
        if(waitingToBgDiff){
            ofSetColor(255, 0, 0);
            ofDrawBitmapString("Background Differencing Inactive. System not ready", adjustedOrigin.x, adjustedOrigin.y + masterHeight + 15);
        } else {
            ofSetColor(0, 255, 0);
            ofDrawBitmapString("Background Differencing Active", adjustedOrigin.x, adjustedOrigin.y + masterHeight + 15);
        }
    }
    
    if(disablePositioning){
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Disable BG differencing to adjust positions and trim dead space", adjustedOrigin.x, adjustedOrigin.y + masterHeight + 30);
    }
    
    
    
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
    
    if(!disablePositioning){
    
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


void Aggregator::gatherOscStats(){
    
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
        ofPoint centerNormalized = center/ofVec2f(masterWidth, masterHeight);
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



#include "ofApp.h"

using namespace ofxCv;


//--------------------------------------------------------------
void ofApp::exit(){
    
    for(int i = 0; i < corridors.size(); i++){
        corridors[i] -> closeFeed();
        
        cout << "Closing Feed: " + ofToString(i) << endl;
    }
    
}



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
    viewMode = 0;
    
    
    titleFont.load("fonts/Aller_Rg.ttf", 50, true);
    smallerFont.load("fonts/Aller_Rg.ttf", 20, true);
    

    
    
    //if we're using image scaling for faster processing
    bScaleDown = true;

    

    //--------------------------------------------------------
    //---------------------CORRIDOR SETUP---------------------
    //--------------------------------------------------------
    //---        A few local arrays so attributes can      ---
    //---              all be set in the same place        ---
    //--------------------------------------------------------
    
    //IP Addresses
    //these will eventually be different
    vector<string> addresses;
    addresses.resize(4);

    addresses[0] = "192.168.187.35";
    addresses[1] = "192.168.187.37";
    addresses[2] = "192.168.187.39";
    addresses[3] = "192.168.187.34";
    
    
    
    //Background Colors
    //holds a dark then a light color for the gradient background
    vector<ofColor> backgroundCols;
    backgroundCols.resize(4 * 2);
    
    int high = 80;  //the high value of the primary colors
    int low = 30;   //the low value
    
    //first corridor is red
    backgroundCols[0].set(high, 0, 0);
    backgroundCols[1].set(low, 0, 0);
    //then yellow
    backgroundCols[2].set(high, high, 0);
    backgroundCols[3].set(low, low, 0);
    //then green
    backgroundCols[4].set(0, high, 0);
    backgroundCols[5].set(0, low, 0);
    //then blue
    backgroundCols[6].set(0, 0, high);
    backgroundCols[7].set(0, 0, low);
    
    
    //Which cameras will be live or with a video for debugging
    vector<bool> useLiveorMovie;
    useLiveorMovie.resize(4);
    
    useLiveorMovie[0] = true;
    useLiveorMovie[1] = true;
    useLiveorMovie[2] = true;
    useLiveorMovie[3] = true;
    
    
    //set up the corridors
    for(int i = 0; i < numFeeds; i++){
        
        //create shared_ptr to a corridor
        auto cor = std::make_shared<Corridor>();

        //corridor 2 = corridors[0] so add 2 to i to get the right corridor name
        
        cor -> setup(addresses[i], "Corridor " + ofToString(i + 2), bScaleDown, useLiveorMovie[i]);
        cor -> adjustedQuadOrigin = rawImagePos;
        cor -> backgroundIn.set(backgroundCols[i * 2]);
        cor -> backgroundOut.set(backgroundCols[i * 2 + 1]);
        
        corridors.push_back(cor);

        
    }
    

    
    
    //-----Data-----
    sender.setup("localhost");
    
    //time between sending system snapshots (in ms)
    dataPerSec = 3;
    lastSendTime = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){

    


    if(viewMode >= 0 && viewMode <= numFeeds){
        
        
        //only enable quad mapping if we're the one on screen
        for(int i = 0; i < corridors.size(); i++){

            if(viewMode == i){
                corridors[i] -> bEnableQuadMapping = true;
            } else {
                corridors[i] -> bEnableQuadMapping = false;
            }
            
        }
        
        
    } else {
        

        
    }
    
    
    
    //update everything regardless of other functionalities
    for(int i = 0; i < corridors.size(); i++){
        
        corridors[i] -> update();
        
    }

    
    
}

//--------------------------------------------------------------
void ofApp::draw(){



    
    //draw the right corridor
    if(viewMode >= 0 && viewMode < numFeeds){
    
        ofBackgroundGradient(corridors[viewMode] -> backgroundIn, corridors[viewMode] -> backgroundOut);
        
        ofSetColor(255);
        titleFont.drawString(corridors[viewMode] -> name, rawImagePos.x, rawImagePos.y - 10);
        
        corridors[viewMode] -> drawRaw(rawImagePos);

        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        }
        
        corridors[viewMode] -> drawCV(cvImagePos, scale);
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(corridors[viewMode] -> contours.size()), cvImagePos.x, cvImagePos.y + feedHeight/2 + 20);
        
        corridors[viewMode] -> drawGui(15, topMargin);
        
        ofSetColor(255);
        ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
        ofDrawBitmapString("Current Feed FPS: " + ofToString(corridors[viewMode] -> cameraFPS), 15, 45);
        
        string msg;
        if(bScaleDown){
            msg = "CV Scaled Down";
        } else {
            msg = "CV NOT Scaled Down";
        }
        
        msg += "\nCurrent CV resolution: " + ofToString(corridors[viewMode] -> threshPix.getWidth()) + " x " + ofToString(corridors[viewMode] -> threshPix.getHeight());
        
        ofDrawBitmapString(msg, 800, 30);
        
        ofVec2f m;
        
        m = ofVec2f(mouseX, mouseY) - rawImagePos;
        
        if(m.x > 0 && m.x < feedWidth && m.y > 0 && m.y < feedHeight){
            
            ofColor pixVal = corridors[viewMode] -> gst.getPixels().getColor(m.x, m.y);
            
            float bright = pixVal.getBrightness();
            
            ofDrawBitmapString(ofToString(bright), rawImagePos.x + m.x + 5, rawImagePos.y + m.y - 5);
            
        }
        
        
        
        
    } else {
        
        
        //draw all the corridors
        ofBackgroundGradient(80, 30);

        //positions to make everything look pretty
        float spacing = 10;
        float topSpacing = 30;
        float frameWidth = ofGetWidth()/4 - spacing*2;
        float frameHeight = frameWidth * feedHeight/feedWidth;
        float guiSpacer = 210;
        float fontSpacing = 5;
        
        //positions of each of the corridor "frames"
        vector<ofVec2f> pts;
        pts.reserve(numFeeds);
        for(int i = 0; i < numFeeds; i++){
            pts[i].set(spacing + i * ofGetWidth()/4, topSpacing);
        }
        
        float frameScaling;
        
        if(bScaleDown){
            frameScaling = frameWidth/float(feedWidth/2);
        } else {
            frameScaling = frameWidth/float(feedWidth);
        }
        
        //draw color coded rects behind everything
        float trans = 100;
        
        for(int i = 0; i < corridors.size(); i++){
            
            //draw color coded background rect
            ofFill();
            ofSetColor(corridors[i] -> backgroundIn, trans);
            ofDrawRectangle(i * ofGetWidth()/4, 0, ofGetWidth()/4, ofGetHeight());
            
            //draw CV
            corridors[i] -> drawCV(pts[i], frameScaling);
            
            //draw GUIs
            corridors[i] -> drawGui(pts[i].x, pts[i].y + frameHeight + spacing);
        
            //draw num blobs under CV image
            ofSetColor(255);
            ofDrawBitmapString("Num Blobs:\n" + ofToString(corridors[i] -> contours.size()), pts[i].x + guiSpacer, pts[i].y + frameHeight + spacing*2);
            
            //draw titles
            ofSetColor(255);
            smallerFont.drawString(corridors[i] -> name, pts[i].x, pts[i].y - fontSpacing);

        }
        

        ofSetColor(255);
        ofDrawBitmapString("Overall Framerate: " + ofToString(ofGetFrameRate()), spacing, ofGetHeight() - 15);
        
    }

    
    
    
    //OSC Data sending
    //only send according to sendInterval (200 ms, i.e. 5X per second)
    if(ofGetElapsedTimeMillis() - lastSendTime > (1000/dataPerSec)){
    
        
        // go through each corridor
        for(int i = 0; i < corridors.size(); i++){
            
            //the go through each blob
            for(int j = 0; j < corridors[i] -> contours.size(); j++){
            
                //get relevant data
                ofPoint center = toOf(corridors[i] -> contours.getCenter(j));
                int label = corridors[i] -> contours.getLabel(j);
                ofVec2f velocity = toOf(corridors[i] -> contours.getVelocity(j));
                
                //then send the blob
                sender.sendBlob(2, label, center, velocity);
                
            
            }
            
        }
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
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
        
        for(int i = 0; i < corridors.size(); i++){
            corridors[i] -> corridorGui.gui.saveToFile(corridors[i] -> name + ".xml");
        }
        
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



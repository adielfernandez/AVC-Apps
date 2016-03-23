#include "ofApp.h"

using namespace ofxCv;


//--------------------------------------------------------------
void ofApp::exit(){
    
    for(int i = 0; i < cameras.size(); i++){
        cameras[i] -> closeFeed();
        
        cout << "Closing Feed: " + ofToString(i) << endl;
    }
    
}

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(false);
    ofSetFrameRate(200);
    
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
     0 = View corridor 1
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
    
    //IP Addresses
    //these will eventually be different
    vector<string> addresses;
    addresses.resize(6);
    
    addresses[0] = "192.168.187.39";
    addresses[1] = "192.168.187.37";
    addresses[2] = "192.168.187.39";
    addresses[3] = "192.168.187.34";
    addresses[4] = "192.168.187.39";
    addresses[5] = "192.168.187.39";
    
    
    
    //Background Colors
    //holds a dark then a light color for the gradient background
    vector<ofColor> backgroundCols;
    backgroundCols.resize(6 * 2);
    
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
    //then cyan
    backgroundCols[6].set(0, high + low, high + low);
    backgroundCols[7].set(0, low + low, low + low);
    //then blue
    backgroundCols[8].set(0, 0, high);
    backgroundCols[9].set(0, 0, low);
    //then magenta
    backgroundCols[10].set(high, 0, high);
    backgroundCols[11].set(low, 0, low);
    
    
    //Which cameras will be live or with a video for debugging
    vector<bool> useLiveorMovie;
    useLiveorMovie.resize(6);
    
    useLiveorMovie[0] = false;
    useLiveorMovie[1] = false;
    useLiveorMovie[2] = false;
    useLiveorMovie[3] = false;
    useLiveorMovie[4] = false;
    useLiveorMovie[5] = false;
    
    
    //file names for test movies
    vector<string> movieFiles;
    movieFiles.resize(6);
    
//    movieFiles[0] = "movies/2016-03-17-11-05-27_FLIR_1001.mov";
//    movieFiles[1] = "movies/2016-03-17-11-05-29_FLIR_1002.mov";
//    movieFiles[2] = "movies/2016-03-17-11-05-31_FLIR_1003.mov";
//    movieFiles[3] = "movies/2016-03-17-11-05-33_FLIR_1005.mov";
//    movieFiles[4] = "movies/2016-03-17-11-05-35_FLIR_1004.mov";
    movieFiles[0] = "movies/flir_gray.mov";
    
    
    
    //set up actual feeds
    int stagger = 500;
    
    //set up the cameras
    for(int i = 0; i < numFeeds; i++){
        
        //create shared_ptr to a corridor
        auto cor = std::make_shared<Camera>();
        
        cor -> setMovieFile(movieFiles[i]);
        cor -> setup(addresses[i], "Lobby 1 Cam " + ofToString(i + 1), bScaleDown, useLiveorMovie[i]);
        cor -> adjustedQuadOrigin = rawImagePos;
        cor -> backgroundIn.set(backgroundCols[i * 2]);
        cor -> backgroundOut.set(backgroundCols[i * 2 + 1]);
        cor -> staggerTime = stagger * i;
        
        cameras.push_back(cor);
        
        
    }
    
    
    
    
    //-----Data-----
    oscHandler.setup("localhost");
    
    //time between sending system snapshots (in ms)
    dataPerSec = 3;
    lastSendTime = 0;
    
    

    
    
}

//--------------------------------------------------------------
void ofApp::update(){

    
    
    
    if(viewMode >= 0 && viewMode < numFeeds){
        
        
        //only enable quad mapping if we're the one on screen
        for(int i = 0; i < cameras.size(); i++){
            
            if(viewMode == i){
                cameras[i] -> bEnableQuadMapping = true;
            } else {
                cameras[i] -> bEnableQuadMapping = false;
            }
            
        }
        
    }
    
    
    
    //update everything regardless of other functionalities
    for(int i = 0; i < cameras.size(); i++){
        
        cameras[i] -> update();
        
    }
    
    
    //put together a vector of pointers to the thresholded  objects
    //ofPixel within the cameras vector. Then feed them into the aggregator
    vector<shared_ptr<ofPixels>> camPixels;
    vector<shared_ptr<ofTexture>> camTexs;
    
    for(int i = 0; i < numFeeds; i++){
        
        //This gets the thresholded ofPixels instance of each camera
        auto thisCam = make_shared<ofPixels>(cameras[i] -> threshPix);
        camPixels.push_back(thisCam);
        
        //This gets the texture of the raw image instance of each camera
        auto thisTex = make_shared<ofTexture>(cameras[i] -> subTex);
        camTexs.push_back(thisTex);
        
    }

    

    
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    
    ofSetWindowTitle("Main Lobby Aggregator");
    
    
    
    //draw the right corridor
    if(viewMode >= 0 && viewMode < numFeeds){
        
        ofBackgroundGradient(cameras[viewMode] -> backgroundIn, cameras[viewMode] -> backgroundOut);
        
        ofSetColor(255);
        titleFont.drawString(cameras[viewMode] -> name, rawImagePos.x, rawImagePos.y - 10);
        
        cameras[viewMode] -> drawRaw(rawImagePos);
        
        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        }
        
        cameras[viewMode] -> drawCV(cvImagePos, scale);
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(cameras[viewMode] -> contours.size()), cvImagePos.x, cvImagePos.y + feedHeight/2 + 20);
        
        cameras[viewMode] -> drawGui(15, topMargin);
        
        ofSetColor(255);
        ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
        ofDrawBitmapString("Current Feed FPS: " + ofToString(cameras[viewMode] -> cameraFPS), 15, 45);
        
        string msg;
        if(bScaleDown){
            msg = "CV Scaled Down";
        } else {
            msg = "CV NOT Scaled Down";
        }
        
        msg += "\nCurrent CV resolution: " + ofToString(cameras[viewMode] -> threshPix.getWidth()) + " x " + ofToString(cameras[viewMode] -> threshPix.getHeight());
        
        ofDrawBitmapString(msg, 800, 30);
        
        
        
    }
    

    
    
    //OSC Data sending
    //only send according to sendInterval (200 ms, i.e. 5X per second)
    if(ofGetElapsedTimeMillis() - lastSendTime > (1000/dataPerSec)){
        
        
        // go through each corridor
        for(int i = 0; i < cameras.size(); i++){
            
            //the go through each blob
            for(int j = 0; j < cameras[i] -> contours.size(); j++){
                
                //get relevant data
                ofPoint center = toOf(cameras[i] -> contours.getCenter(j));
                int label = cameras[i] -> contours.getLabel(j);
                ofVec2f velocity = toOf(cameras[i] -> contours.getVelocity(j));
                
                //then send the blob
                oscHandler.sendBlob(2, label, center, velocity);
                
                
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
    } else if(key == '5'){
        viewMode = 4;
    } else if(key == '6'){
        viewMode = 5;
    } else if(key == ' '){
        viewMode = 6;
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
        
        for(int i = 0; i < cameras.size(); i++){
            cameras[i] -> cameraGui.gui.saveToFile(cameras[i] -> name + ".xml");
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

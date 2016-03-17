#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    feedHeight = 512;
    feedWidth = 640;
    
    
    //address IPs
    addresses.resize(14);
    
    addresses[0] = "192.168.187.35";
    addresses[1] = "192.168.187.37";
    addresses[2] = "192.168.187.39";
    addresses[3] = "192.168.187.34";
    addresses[4] = "192.168.187.38";
    
    n1 = 0;
    n2 = 0;
    n3 = 0;
    n4 = 0;
    n5 = 0;
    
    started1 = false;
    started2 = false;
    started3 = false;
    started4 = false;
    started5 = false;
    
    wait = 100;
    staggerTime = 200;
}

//--------------------------------------------------------------
void ofApp::update(){

    
    
    if(!started1 && ofGetElapsedTimeMillis() > wait){
        
        started1 = true;
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Setting up Feed 1" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        
        gst1.setPipeline("rtspsrc location=rtsp://admin:admin@" + addresses[0] + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! queue2 max-size-buffers=2 ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);

        cout << "\n\n\n\n\n" << endl;
        cout << "Starting Feed 1 Pipeline" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst1.startPipeline();
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Playing Feed 1" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst1.play();
        
    }

    if(!started2 && ofGetElapsedTimeMillis() > wait + staggerTime * 1){
        
        started2 = true;
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Setting up Feed 2" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        
        gst2.setPipeline("rtspsrc location=rtsp://admin:admin@" + addresses[1] + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! queue2 max-size-buffers=2 ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);

        cout << "\n\n\n\n\n" << endl;
        cout << "Starting Feed 2 Pipeline" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst2.startPipeline();
        
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Playing Feed 2" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst2.play();
        
    }
    
    if(!started3 && ofGetElapsedTimeMillis() > wait + staggerTime * 2){
        
        started3 = true;
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Setting up Feed 3" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst3.setPipeline("rtspsrc location=rtsp://admin:admin@" + addresses[2] + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! queue2 max-size-buffers=2 ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Starting Feed 3 Pipeline" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst3.startPipeline();
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Playing Feed 3" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst3.play();
        
    }
    
    if(!started4 && ofGetElapsedTimeMillis() > wait + staggerTime * 3){
        
        started4 = true;
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Setting up Feed 4" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        
        gst4.setPipeline("rtspsrc location=rtsp://admin:admin@" + addresses[3] + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! queue2 max-size-buffers=2 ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Starting Feed 4 Pipeline" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst4.startPipeline();
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Playing Feed 4" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst4.play();
        
    }
    
    if(!started5 && ofGetElapsedTimeMillis() > wait + staggerTime * 4){
        
        started5 = true;
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Setting up Feed 5" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        
        gst5.setPipeline("rtspsrc location=rtsp://admin:admin@" + addresses[4] + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! queue2 max-size-buffers=2 ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Starting Feed 5 Pipeline" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst5.startPipeline();
        
        cout << "\n\n\n\n\n" << endl;
        cout << "Playing Feed 5" << endl;
        cout << "\n\n\n\n\n" << endl;
        
        gst5.play();
        
    }
    
    
    if(started1) gst1.update();
    if(started2) gst2.update();
    if(started3) gst3.update();
    if(started4) gst4.update();
    if(started5) gst5.update();
    
    
    if(gst1.isFrameNew()){
        n1++;
    }

    if(gst2.isFrameNew()){
        n2++;
    }
    
    if(gst3.isFrameNew()){
        n3++;
    }
    
    if(gst4.isFrameNew()){
        n4++;
    }
    
    if(gst5.isFrameNew()){
        n5++;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    
    
    float aspect = feedHeight/feedHeight;
    float w = 200;
    float h = w * aspect;
    
    int topMargin = 12;
    int leftMargin = 12;
    
    if(started1){
        img1.setFromPixels(gst1.getPixels());
        img1.draw(leftMargin, topMargin, w, h);
    }
    if(started2){
        img2.setFromPixels(gst2.getPixels());
        img2.draw(leftMargin + w*1, topMargin, w, h);
    }
    if(started3){
        img3.setFromPixels(gst3.getPixels());
        img3.draw(leftMargin + w*2, topMargin, w, h);
    }
    if(started4){
        img4.setFromPixels(gst4.getPixels());
        img4.draw(leftMargin + w*3, topMargin, w, h);
    }
    if(started5){
        img5.setFromPixels(gst5.getPixels());
        img5.draw(leftMargin + w*4, topMargin, w, h);
    }
    
    ofSetColor(255);
    ofDrawBitmapString("Frames: " + ofToString(n1), leftMargin + 5, topMargin + 15);
    ofDrawBitmapString("Frames: " + ofToString(n2), leftMargin + w*1 + 5, topMargin + 15);
    ofDrawBitmapString("Frames: " + ofToString(n3), leftMargin + w*2 + 5, topMargin + 15);
    ofDrawBitmapString("Frames: " + ofToString(n4), leftMargin + w*3 + 5, topMargin + 15);
    ofDrawBitmapString("Frames: " + ofToString(n5), leftMargin + w*4 + 5, topMargin + 15);
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

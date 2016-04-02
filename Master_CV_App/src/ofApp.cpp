#include "ofApp.h"

using namespace ofxCv;


//--------------------------------------------------------------
void ofApp::exit(){
    
    for(int i = 0; i < cameras.size(); i++){
        cameras[i] -> closeFeed();
    }
    
}

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(false);
    ofSetFrameRate(200);
    
//    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetWindowTitle("AV&C Terrell Place Sensor Processor");
    
    feedWidth = 640;
    feedHeight = 512;
    
    //-----UI-----
    leftMargin = 230;
    topMargin = 70;
    centerMargin = 20;
    
    mainContentPos.set(leftMargin, topMargin);
    secondaryContentPos.set(leftMargin + feedWidth + centerMargin, topMargin);
    
    /*
     viewMode: What we're seeing
     0 = View cam 1
     1 = View cam 2
     2 = View cam 3
     3 = View cam 4
     
     etc...
     
     14 = Lobby 1 aggregation
     15 = Lobby 2 aggregation
     16 = OSC Page
     17 = all cameras
     
     */
    viewMode = 0;
    
    
    titleFont.load("fonts/Aller_Rg.ttf", 45, true);
    
    smallerFont.load("fonts/Aller_Rg.ttf", 16, true);
    
    
    
    //if we're using image scaling for faster processing
    bScaleDown = true;
    
    //address IPs
    vector<string> addresses;
    addresses.resize(14);
    
    //address IPs
    vector<string> names;
    names.resize(14);
    
    names[0] = "Cam-1";
    addresses[0] = "192.168.1.6";
    
    names[1] = "Cam-2";
    addresses[1] = "192.168.1.6";
    
    names[2] = "Cam-3";
    addresses[2] = "192.168.1.6";
    
    names[3] = "Cam-4";
    addresses[3] = "192.168.1.6";
    
    names[4] = "Cam-5";
    addresses[4] = "192.168.1.6";
    
    names[5] = "Cam-6";
    addresses[5] = "192.168.1.6";
    
    names[6] = "Cam-7";
    addresses[6] = "192.168.1.6";
    
    names[7] = "Cam-8";
    addresses[7] = "192.168.1.6";
    
    names[8] = "Cam-9";
    addresses[8] = "192.168.1.6";
    
    names[9] = "Cam-10";
    addresses[9] = "192.168.1.6";
    
    names[10] = "Cam-11";
    addresses[10] = "192.168.1.6";
    
    names[11] = "Cam-12";
    addresses[11] = "192.168.1.6";
    
    names[12] = "Cam-13";
    addresses[12] = "192.168.1.6";
    
    names[13] = "Cam-14";
    addresses[13] = "192.168.1.6";
    
    
    
    
    //file names for test movies
    vector<string> movieFiles;
    movieFiles.resize(6);
    
    movieFiles[0] = "movies/flir_gray.mov";
//    movieFiles[0] = "movies/2016-03-17-11-05-27_FLIR_1001.mov";
//    movieFiles[1] = "movies/2016-03-17-11-05-29_FLIR_1002.mov";
//    movieFiles[2] = "movies/2016-03-17-11-05-31_FLIR_1003.mov";
//    movieFiles[3] = "movies/2016-03-17-11-05-33_FLIR_1005.mov";
//    movieFiles[4] = "movies/2016-03-17-11-05-35_FLIR_1004.mov";
    
    
    //master control of live vs video
    useLiveFeed = true;
    
    //set up actual feeds
    int stagger = 250;
    
    //set up the cameras
    for(int i = 0; i < numFeeds; i++){
        
        //create shared_ptr to a corridor
        auto cor = std::make_shared<Camera>();
        
        cor -> setMovieFile(movieFiles[0]);
        cor -> setupViewControl(i, &viewMode, mainContentPos);
        cor -> setup(addresses[i], names[i], bScaleDown, useLiveFeed);
        
        cor -> staggerTime = stagger * i;
        
        cameras.push_back(cor);
        
        
    }
    
    
    //----------AGGREGATORS----------
    Lobby1Aggregate.setup("Lobby1Aggregate", 6, cameras, bScaleDown);
    Lobby1Aggregate.setupViewControl(14, &viewMode, mainContentPos);
    Lobby2Aggregate.setup("Lobby2Aggregate", 4, cameras, bScaleDown);
    Lobby2Aggregate.setupViewControl(15, &viewMode, mainContentPos);
    
    
    //-----Data-----
    
    //get the IP from file
    ofBuffer buffer = ofBufferFromFile("ip.txt");
    
    string pgsIP, audioIP, heartbeatIP;
    int pgsPort, audioPort, heartbeatPort;

    cout << "Getting OSC Configuration from File" << endl;
    
    if(buffer.size()) {
        
        int lineNum = 0;
        
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            
            string line = *it;
            
            if(lineNum == 7){
                pgsIP = line;
            } else if(lineNum == 8){
                pgsPort = ofToInt(line);
            } else if(lineNum == 9){
                audioIP = line;
            } else if(lineNum == 10){
                audioPort = ofToInt(line);
            } else if(lineNum == 11){
                heartbeatIP = line;
            } else if(lineNum == 12){
                heartbeatPort = ofToInt(line);
            }
            
            lineNum++;
        }
        
    }
    
    cout << "PGS IP: " << pgsIP << endl;
    cout << "PGS Port: " << pgsPort << endl;
    cout << "Audio IP: " << audioIP << endl;
    cout << "Audio Port: " << audioPort << endl;
    cout << "Heartbeat IP: " << heartbeatIP << endl;
    cout << "Heartbeat Port: " << heartbeatPort << endl;
    
    
    
    oscHandler.setupPGS(pgsIP, pgsPort);
    oscHandler.setupAudio(audioIP, audioPort);
    oscHandler.setupHeartbeat(heartbeatIP, heartbeatPort);
    
    oscHandler.setupGui();
    
    oscHandler.loadSettings();

    //for visualizing message sending
    lastSendTime = 0;
    sendAlertTrans = 0;
    sendTrans = 0;
    
    
    
    
    
    
    
    
    
    //-----create buttons for UI navigation-----
    panel.setup(&viewMode);
    
    //video playback
    if(!useLiveFeed){
        
        ofVec2f p;
        p.set(panel.pos.x + 10, panel.pos.y - playback.bgHeight - 10);
        
        cout << "[ofApp] cam vector address: " << &cameras << endl;
        cout << "[ofApp] cam vector first cam address: " << cameras[0] << endl;
        playback.setup(p, cameras, &smallerFont);
        
    }
    
    //make room for the positions of all the cameras
    for(int i = 0; i < 14; i++){
        ofVec2f p(0,0);
        allCamsPos.push_back(p);
    }
    
    //and corridor labels too
    for(int i = 0; i < 6; i++){
        ofVec2f p(0,0);
        allLabelsPos.push_back(p);
    }
    
    float aspectRatio = 512.0f/640.0f;
    frameWidth = 190;
    frameHeight = frameWidth * aspectRatio;
    
    int leftPadding = 20;
    int frameMargin = 30;
    int topPos = topMargin + 20;
    
    //first 6 cameras in 1st corridor
    allCamsPos[0].set(leftPadding, topPos);
    allCamsPos[1].set(leftPadding + frameWidth, topPos);
    allCamsPos[2].set(leftPadding + frameWidth * 2, topPos);
    allCamsPos[3].set(leftPadding, topPos + frameHeight);
    allCamsPos[4].set(leftPadding + frameWidth, topPos + frameHeight);
    allCamsPos[5].set(leftPadding + frameWidth * 2, topPos + frameHeight);
    
    //next 4 cameras are corridors 2, 3, 4 and 5
    //in a row underneath corridor 1
    float nextRowY = topPos + frameHeight * 2 + frameMargin;
    allCamsPos[6].set(leftPadding, nextRowY);
    allCamsPos[7].set(leftPadding + frameWidth + frameMargin * 2, nextRowY);
    allCamsPos[8].set(leftPadding + frameWidth * 2 + frameMargin * 4, nextRowY);
    allCamsPos[9].set(leftPadding + frameWidth * 3 + frameMargin * 6, nextRowY);

    //last 4 are to the right of corridor 1
    float cor6leftEdge = leftPadding + frameWidth * 3 + frameMargin;
    allCamsPos[10].set(cor6leftEdge, topPos);
    allCamsPos[11].set(cor6leftEdge + frameWidth, topPos);
    allCamsPos[12].set(cor6leftEdge + frameWidth * 2, topPos);
    allCamsPos[13].set(cor6leftEdge, topPos + frameHeight);
    
    allLabelsPos[0] = allCamsPos[0];
    allLabelsPos[1] = allCamsPos[6];
    allLabelsPos[2] = allCamsPos[7];
    allLabelsPos[3] = allCamsPos[8];
    allLabelsPos[4] = allCamsPos[9];
    allLabelsPos[5] = allCamsPos[10];
    
}

//===============================================================//
//---------------------------------------------------------------//
//                _  _ ___  ___  ____ ___ ____                   //
//                |  | |__] |  \ |__|  |  |___                   //
//                |__| |    |__/ |  |  |  |___                   //
//                                                               //
//---------------------------------------------------------------//
//===============================================================//


void ofApp::update(){

    //button panel
    panel.update();
    
    
    //update the playback bar if we're not live
    if(!useLiveFeed) playback.update();
    
    
    
    //Cameras
    for(int i = 0; i < cameras.size(); i++){
        cameras[i] -> update();
    }
    
    //Lobby 1
    Lobby1Aggregate.update();
    

    //Lobby 2
    Lobby2Aggregate.update();
    

    
    //OSC

    //Indicator box
    //trans of the red rect frame
    if(sendAlertTrans > 0){
        sendAlertTrans *= 0.92;
    }
    
    //trans of the white rect itself
    if(sendTrans > 0){
        sendTrans *= 0.96;
    }

    
    
}


//===============================================================//
//---------------------------------------------------------------//
//                    ___  ____ ____ _ _ _                       //
//                    |  \ |__/ |__| | | |                       //
//                    |__/ |  \ |  | |_|_|                       //
//                                                               //
//---------------------------------------------------------------//
//===============================================================//

void ofApp::draw(){

    
    
    
    ofBackgroundGradient(200, 0);
    
    //draw the raw cameras
    if(viewMode >= 0 && viewMode < numFeeds){
        
//        ofBackgroundGradient(cameras[viewMode] -> backgroundIn, cameras[viewMode] -> backgroundOut);
        
        ofSetColor(255);
        ofDrawBitmapString("Current Feed FPS: " + ofToString(cameras[viewMode] -> cameraFPS), 15, 45);
        titleFont.drawString(cameras[viewMode] -> name, mainContentPos.x, mainContentPos.y - 10);
        
        cameras[viewMode] -> drawRaw(mainContentPos);
        
        float scale;
        if(bScaleDown){
            scale = 1.0;
        } else {
            scale = 0.5;
        
        }
        cameras[viewMode] -> drawCV(secondaryContentPos, scale);
        
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(cameras[viewMode] -> contours.size()), secondaryContentPos.x, secondaryContentPos.y + feedHeight/2 + 20);
        
        cameras[viewMode] -> drawGui(15, topMargin);
        
        
        
        string msg;
        if(bScaleDown){
            msg = "CV Scaled Down";
        } else {
            msg = "CV NOT Scaled Down";
        }
        

        
        ofDrawBitmapString(msg, 800, 30);
        
        
        
    } else if(viewMode == 14){
        
        //CORRIDOR 1 AGGREGATION

        
        ofSetColor(255);
        titleFont.drawString("Corridor 1 Aggregate", mainContentPos.x, mainContentPos.y - 10);
        
        Lobby1Aggregate.drawRaw(mainContentPos.x, mainContentPos.y);
        Lobby1Aggregate.drawCV(mainContentPos.x, mainContentPos.y);
        
        Lobby1Aggregate.drawGui(15, topMargin);
        
        
        
        
        
        
        
        
    } else if(viewMode == 15){
        
        //CORRIDOR 1 AGGREGATION
        
        ofSetColor(255);
        titleFont.drawString("Corridor 6 Aggregate", mainContentPos.x, mainContentPos.y - 10);
        
        Lobby2Aggregate.drawRaw(mainContentPos.x, mainContentPos.y);
        Lobby2Aggregate.drawCV(mainContentPos.x, mainContentPos.y);
        
        Lobby2Aggregate.drawGui(15, topMargin);
        
        
        
        
        
        
        
        
    } else if(viewMode == 16){
        
        //OSC VIEWER
        
        ofSetColor(255);
        titleFont.drawString("OSC Info", mainContentPos.x, mainContentPos.y - 10);
        
        oscHandler.drawGui(15, topMargin);
        
        
        //draw OSC data to screen
        string oscDataFormat;
        
        oscDataFormat += "OSC Data to PGS\n";
        oscDataFormat += "---------------\n";
        oscDataFormat += "Sending to \"" + oscHandler.pgsIP + "\"\n";
        oscDataFormat += "on port 12345\n";
        oscDataFormat += "\n";
        oscDataFormat += "DATA FORMAT\n";
        oscDataFormat += "---------------\n";
        oscDataFormat += "ALL SYSTEM DATA (Top Level Bundle):\n";
        oscDataFormat += "\n";
        oscDataFormat += "    System Message:\n";
        oscDataFormat += "        Address: /system_stats\n";
        oscDataFormat += "        int32 Arg: total blobs in system\n";
        oscDataFormat += "\n";
        oscDataFormat += "    Corridor 1 Bundle:\n";
        oscDataFormat += "        Corridor Message:\n";
        oscDataFormat += "            Address: /corridor_[num]_stats\n";
        oscDataFormat += "            int32 Arg: numBlobs in corridor\n";
        oscDataFormat += "            Float Arg: avg blob position in X\n";
        oscDataFormat += "            Float Arg: avg blob position in Y\n";
        oscDataFormat += "            Float Arg: avg velocity position in X\n";
        oscDataFormat += "            Float Arg: avg velocity position in Y\n";
        oscDataFormat += "            Float Arg: avg speed\n";
        oscDataFormat += "\n";
        oscDataFormat += "        Blob Bundle:\n";
        oscDataFormat += "            Individual Blob Message:\n";
        oscDataFormat += "            Address: /corridor_[num]/blob_data\n";
        oscDataFormat += "            int32 Arg: Blob ID\n";
        oscDataFormat += "            Float Arg: blob position in X\n";
        oscDataFormat += "            Float Arg: blob position in Y\n";
        oscDataFormat += "            Float Arg: avg velocity position in X\n";
        oscDataFormat += "            Float Arg: avg velocity position in Y\n";
        oscDataFormat += "\n";
        oscDataFormat += "    Corridor 2 Bundle (same format)\n";
        oscDataFormat += "    Corridor 3 Bundle (same format)\n";
        oscDataFormat += "    Etc. for other corridors\n";

        
        int oscBoxWidth = 450;
        int oscBoxHeight = 500;
        int oscGap = 20;
        int textX = 15;
        int textY = 25;
        
        //draw transparent box
        ofSetColor(0, 200);
        ofDrawRectangle(leftMargin, topMargin, oscBoxWidth, oscBoxHeight);
        
        
        ofSetColor(255);
        ofDrawBitmapString(oscDataFormat, leftMargin + textX, topMargin + textY);
        
        
        
        string audioDataFormat;
        
        audioDataFormat += "OSC Data to Audio System\n";
        audioDataFormat += "---------------\n";
        audioDataFormat += "Sending to \"" + oscHandler.audioIP + "\"\n";
        audioDataFormat += "on port 12345\n";
        audioDataFormat += "\n";
        audioDataFormat += "DATA FORMAT\n";
        audioDataFormat += "---------------\n";
        audioDataFormat += "Bundle:\n";
        audioDataFormat += "--Message:\n";
        audioDataFormat += "    Address: /Corridor_1\n";
        audioDataFormat += "    Float Arg: DENSITY\n";
        audioDataFormat += "    Float Arg: SPEED\n";
        audioDataFormat += "--Message:\n";
        audioDataFormat += "    Address: /Corridor_2\n";
        audioDataFormat += "    Float Arg: DENSITY\n";
        audioDataFormat += "    Float Arg: SPEED\n";
        audioDataFormat += "Etc. for other corridors\n";
        audioDataFormat += "\n";
        audioDataFormat += "\n";
        audioDataFormat += "\n";
        audioDataFormat += "\n";
        audioDataFormat += "\n";
        audioDataFormat += "\n";
        audioDataFormat += "DENSITY value is number\n";
        audioDataFormat += "of blobs per camera.\n";
        audioDataFormat += "\n";
        audioDataFormat += "\n";
        audioDataFormat += "--------NOTES--------\n";
        audioDataFormat += "All positions are normalized from 0-1:\n";
        audioDataFormat += "\n";
        audioDataFormat += "X direction is along the LED wall within the corridor\n";
        audioDataFormat += "0 at left corner, 1 at the right corner.\n";
        audioDataFormat += "\n";
        audioDataFormat += "Y direction is to/away from the LED wall\n";
        audioDataFormat += "0 is at the wall, 1 far from the wall.\n";
        
        //draw transparent box
        ofSetColor(0, 200);
        ofDrawRectangle(leftMargin + oscBoxWidth + oscGap, topMargin, oscBoxWidth, oscBoxHeight);
        
        
        ofSetColor(255);
        ofDrawBitmapString(audioDataFormat, leftMargin + oscBoxWidth + oscGap + textX, topMargin + textY);
        
        
        //-----OSC Indicator Box-----
        float rectWidth = 140;
        float rectHeight = 40;
        float spacing = 10;
        
        ofVec2f boxPos;
        boxPos.set(ofGetWidth() - rectWidth - spacing, panel.pos.y - rectHeight - spacing);
        
        ofPushStyle();
        ofSetColor(255, sendTrans);
        ofFill();
        ofDrawRectangle(boxPos, rectWidth, rectHeight);
        
        ofNoFill();
        ofSetColor(255, 0, 0, sendAlertTrans);
        ofSetLineWidth(5);
        ofDrawRectangle(boxPos, rectWidth, rectHeight);
        
        ofSetColor(0, sendTrans);
        ofDrawBitmapString("MESSAGE SENT", boxPos.x + 20, boxPos.y + rectHeight/2 + 3);
        ofPopStyle();
        
        
    } else if(viewMode == 17){
        
        //All cameras
        
        ofSetColor(255);
        titleFont.drawString("All Cameras", mainContentPos.x, mainContentPos.y - 10);
        
        for(int i = 0; i < numFeeds; i++){
            
            float scale = frameWidth/(float)cameras[i] -> scaledWidth;
            
            cameras[i] -> drawCV(allCamsPos[i], scale);
            
            //draw camera label
            ofSetColor(180);
            ofDrawBitmapString(cameras[i] -> name, allCamsPos[i].x + 5, allCamsPos[i].y + 13);
            
        }
        
        //draw corridor labels
        for(int i = 0; i < allLabelsPos.size(); i++){
            ofSetColor(255);
            smallerFont.drawString("Corridor " + ofToString(i + 1), allLabelsPos[i].x, allLabelsPos[i].y - 5);
        }
        
        
    }
    
    
    
    
    //--------------------OSC DATA--------------------
    
    if(oscHandler.sendOsc && oscHandler.oscSendRate > 0 && (playback.isPlaying || useLiveFeed)){
        

        if(ofGetElapsedTimeMillis() - lastSendTime > oscHandler.oscSendRate){
            
            //==============================
            //-----DATA TO AUDIO SYSTEM-----
            //==============================
            
            
//            ofxOscBundle bundle;
//            
//            int numCorridors = 6;
//            
//            for(int i = 0; i < numCorridors; i++){
//                
//                ofxOscMessage m;
//                m.setAddress("/Corridor " + ofToString(i + 1));
//                m.addFloatArg(blobs.size());
//                m.addFloatArg(avgSpeed);
//                
//                bundle.addMessage(m);
//                
//            }
//            
//            sender.sendBundle(bundle);
            
            
            //=====================
            //-----DATA TO PGS-----
            //=====================
            
            //we need to go through all the corridors and pull together
            //the data then send it at the very end.
            //first prepare the bundles and messages to be filled as we go along
            
            
            //ONE Total system bundle
//            ofxOscBundle allSystemData;
//            //ONE message for system-wide data
//            ofxOscMessage systemMessage;
//            //SIX bundles, one for each corridor
//            vector<ofxOscBundle> corridorBundles;
//            //SIX messages, one for each corridor
//            vector<ofxOscMessage> corridorMessages;
//
//            //SIX bundles for each vector of blobs
//            vector<ofxOscBundle> blobBundles;
//
//
//
//            //averaged data
//            int totalNumBlobs = 0;
//
//
//            //pretend we have more corridors to send more "complete" data mock up
//            int numCorridors = 6;
//
//            //go through all corridors
//            for(int i = 0; i < numCorridors; i++){
//
//                //get data for this corridor
//                string corridorAddress = "/corridor_" + ofToString(i + 1) + "_stats";
//
//
//                //reset stats to get averages
//                int numBlobs;
//                float avgSpeed;
//                ofVec2f avgPos;
//                ofVec2f avgDir;
//                ofVec2f avgVel;
//
//                numBlobs = blobs.size();
//                avgSpeed = 0;
//                avgPos.set(0);
//                avgVel.set(0);
//                avgDir.set(0);
//
//                totalNumBlobs += numBlobs;
//
//                //go through all blobs within corridor
//                //corridor Index will change when more cameras are actually used
//
//                ofxOscBundle thisBlobBundle;
//                blobBundles.push_back(thisBlobBundle);
//
//                vector<ofxOscMessage> thisCorridorBlobs;
//
//                for(int j = 0; j < blobs.size(); j++){
//
//                    int blobID = blobs[j].ID;
//                    ofPoint center = blobs[j].pos;
//                    ofVec2f velocity = blobs[j].vel;
//
//
//                    //assemble the message for this blob
//                    ofxOscMessage thisBlob;
//                    thisBlob.setAddress("/corridor_" + ofToString(i + 1) + "/blob_data");
//                    thisBlob.addIntArg(blobID + 10000 * i);
//                    thisBlob.addFloatArg(center.x/corridorDim.x);   //divide by cam dimensions
//                    thisBlob.addFloatArg(center.y/corridorDim.y);  //to normalize
//                    thisBlob.addFloatArg(velocity.x);
//                    thisBlob.addFloatArg(velocity.y);
//
//                    //add it to the right blob bundle
//                    blobBundles[i].addMessage(thisBlob);
//
//
//
//                    //add to the averages
//                    avgSpeed += velocity.length();
//                    avgVel += velocity;
//                    avgPos += center;
//                }
//
//
//
//
//                //average out all the data for this corridor
//                avgSpeed = avgSpeed/float(numBlobs);
//                avgVel = avgVel/float(numBlobs);
//                avgDir = avgVel.getNormalized();
//                avgPos = avgPos/float(numBlobs);
//
//
//                ofxOscMessage thisCorridorData;
//                thisCorridorData.setAddress(corridorAddress);
//                thisCorridorData.addIntArg(numBlobs);
//                thisCorridorData.addFloatArg(avgPos.x);
//                thisCorridorData.addFloatArg(avgPos.y);
//                thisCorridorData.addFloatArg(avgDir.x);
//                thisCorridorData.addFloatArg(avgDir.y);
//                thisCorridorData.addFloatArg(avgSpeed);
//
//                //store this corridor's stats
//                corridorMessages.push_back(thisCorridorData);
//
//                //add a wrapper for this bundle
//                ofxOscBundle thisCorridorBundle;
//                corridorBundles.push_back(thisCorridorBundle);
//
//
//
//
//            }
//
//
//            //start assembling messages and bundles
//            systemMessage.setAddress("/system_stats");
//            systemMessage.addIntArg(totalNumBlobs);
//
//            // all system data message
//            allSystemData.addMessage(systemMessage);
//
//            //now corridor data
//            //loop through 6 corridors and assemble
//            //all the data into bundles
//            for(int i = 0; i < numCorridors; i++){
//
//                //corridor wrapper gets a corridor message...
//                corridorBundles[i].addMessage(corridorMessages[i]);
//
//                //and a blob bundle (this was pre-populated with messages
//                //above when we went through the
//                corridorBundles[i].addBundle(blobBundles[i]);
//
//                allSystemData.addBundle(corridorBundles[i]);
//            }
//
//
//
//            //Now send it all at once
//            sender.sendBundle(allSystemData);
            
            
            
            lastSendTime = ofGetElapsedTimeMillis();
            
            sendAlertTrans = 255;
            sendTrans = 255;
            
        }
        
    }

    

    
    
    
    
    
    
    
    

    ofSetColor(255);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
    

    
    //UI panel for buttons to switch between views
    panel.draw();

    if(!useLiveFeed) playback.draw();
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    //cycle right
    if(key == OF_KEY_RIGHT){
        viewMode++;
        if(viewMode > numScreens - 1) viewMode = 0;
    }
    
    //cycle left
    if(key == OF_KEY_LEFT){
        viewMode--;
        if(viewMode < 0) viewMode = numScreens - 1;
    }
    
    //save all
    if(key == 's'){
        
        for(int i = 0; i < cameras.size(); i++){
            cameras[i] -> cameraGui.gui.saveToFile(cameras[i] -> name + ".xml");
        }
        
        oscHandler.saveSettings();
        
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

    //if we're in the navpanel...
    if(y > panel.pos.y){
        
        panel.checkForClicks(x, y);

    }
    
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

#include "ofApp.h"

using namespace ofxCv;


//--------------------------------------------------------------
void ofApp::exit(){
    
    for(int i = 0; i < cameras.size(); i++){
        cameras[i] -> closeFeed();
        cameras[i] -> imageProcessor.stopThread();
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
    int centerMargin = 20;
    
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
     16 = All Blobs
     17 = All cameras
     18 = OSC Page
     
     */
    viewMode = 14;
    
    
    titleFont.load("fonts/Aller_Rg.ttf", 40, true);
    mediumFont.load("fonts/Aller_Rg.ttf", 18, true);
    smallerFont.load("fonts/Aller_Rg.ttf", 14, true);
    
    
    
    //if we're using image scaling for faster processing
    bScaleDown = true;
    
    if(bScaleDown){
        scaledWidth = feedWidth/2;
        scaledHeight = feedHeight/2;
    } else {
        scaledWidth = feedWidth;
        scaledHeight = feedHeight;
    }
    
    
    
    //address IPs
    vector<string> addresses;
    addresses.resize(14);
    
    //address IPs
    vector<string> names;
    names.resize(14);
    
    
    //TERRELL PLACE IP Setup
    names[0] = "Cam-1";
    addresses[0] = "192.168.187.47";
    
    names[1] = "Cam-2";
    addresses[1] = "192.168.187.45";
    
    names[2] = "Cam-3";
    addresses[2] = "192.168.187.44";
    
    names[3] = "Cam-4";
    addresses[3] = "192.168.187.35";
    
    names[4] = "Cam-5";
    addresses[4] = "192.168.187.37";
    
    names[5] = "Cam-6";
    addresses[5] = "192.168.187.43";
    
    names[6] = "Cam-7";
    addresses[6] = "192.168.187.39";
    
    names[7] = "Cam-8";
    addresses[7] = "192.168.187.42";
    
    names[8] = "Cam-9";
    addresses[8] = "192.168.187.46";
    
    names[9] = "Cam-10";
    addresses[9] = "192.168.187.36";
    
    names[10] = "Cam-11";
    addresses[10] = "192.168.187.41";
    
    names[11] = "Cam-12";
    addresses[11] = "192.168.187.38";
    
    names[12] = "Cam-13";
    addresses[12] = "192.168.187.40";
    
    names[13] = "Cam-14";
    addresses[13] = "192.168.187.34";
    
    
    
    
    //file names for test movies
    vector<string> movieFiles;
    movieFiles.resize(14);
    
    
    
    for(int i = 0; i < 14; i++){
        
        movieFiles[i] = "videos/Cam_" + ofToString(i + 1) + ".mov";
        
    }
    
    
    //master control of live vs video
    useLiveFeed = true;
    
    //set up actual feeds
    int stagger = 100;
    
    //set up the cameras
    for(int i = 0; i < numFeeds; i++){
        
        //create shared_ptr to a corridor
        auto cor = std::make_shared<Camera>();
        
        //do these first
        cor -> setMovieFile(movieFiles[i]);
        cor -> setFont(&mediumFont);
        
        //params: the view of this camera, the view control variable,
        //the position of the main content area
        cor -> setupViewControl(i, &viewMode, mainContentPos);
        
        //if it's a soloCam (corridors 2-5 i.e. cameras 6-9),
        //no need for cropping. if not, no need for contours
        bool solo;
        if(i >= 6 && i <= 9){
            solo = true;
        } else {
            solo = false;
        }
        
        cor -> soloCam = solo;
        
        //and stagger
        cor -> staggerTime = stagger * i;

        //then set up everything else
        //single cam at AVC
//        cor -> setup("192.168.187.36", names[i], bScaleDown, useLiveFeed);

        //proper addresses at TERRELL PLACE
        cor -> setup(addresses[i], names[i], bScaleDown, useLiveFeed);
        
        cameras.push_back(cor);
        
        
    }
    
    
    //----------AGGREGATORS----------
    Lobby1Aggregate.setup("Lobby1Aggregate", 6, cameras, bScaleDown);
    Lobby1Aggregate.setupViewControl(14, &viewMode, mainContentPos);
    Lobby2Aggregate.setup("Lobby2Aggregate", 4, cameras, bScaleDown);
    Lobby2Aggregate.setupViewControl(15, &viewMode, mainContentPos);
    
    Lobby1Aggregate.font = &mediumFont;
    Lobby2Aggregate.font = &mediumFont;
    
    //-----Data-----
    
    //get the IP from file
    ofBuffer buffer = ofBufferFromFile("ip.txt");
    
    string pgs1IP = "";
    string pgs2IP = "";
    string pgsBackupIP = "";
    string previzDevIP = "";
    string audioIP = "";
    string audioBackupIP = "";
    string heartbeatIP = "";
    
    
    int pgs1Port = 0;
    int pgs2Port = 0;
    int pgsBackupPort = 0;
    int previzDevPort = 0;
    int audioPort = 0;
    int audioBackupPort = 0;
    int heartbeatPort = 0;

    cout << "Getting OSC Configuration from File" << endl;
    
    if(buffer.size()) {
        
        int lineNum = 0;
        
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            
            string line = *it;
            
            if(lineNum == 1){
                pgs1IP = line;
            } else if(lineNum == 4){
                pgs1Port = ofToInt(line);
            } else if(lineNum == 7){
                pgs2IP = line;
            } else if(lineNum == 10){
                pgs2Port = ofToInt(line);
            } else if(lineNum == 13){
                pgsBackupIP = line;
            } else if(lineNum == 16){
                pgsBackupPort = ofToInt(line);
            } else if(lineNum == 19){
                previzDevIP = line;
            } else if(lineNum == 22){
                previzDevPort = ofToInt(line);
            } else if(lineNum == 25){
                audioIP = line;
            } else if(lineNum == 28){
                audioPort = ofToInt(line);
            } else if(lineNum == 31){
                audioBackupIP = line;
            } else if(lineNum == 34){
                audioBackupPort = ofToInt(line);
            } else if(lineNum == 37){
                heartbeatIP = line;
            } else if(lineNum == 40){
                heartbeatPort = ofToInt(line);
            }
            
            lineNum++;
        }
        
    }
    
    cout << "PGS1 IP: " << pgs1IP << endl;
    cout << "PGS1 Port: " << pgs1Port << endl;
    cout << "PGS2 IP: " << pgs2IP << endl;
    cout << "PGS2 Port: " << pgs2Port << endl;
    cout << "PgsBackupIP IP: " << pgsBackupIP << endl;
    cout << "PgsBackupPort Port: " << pgsBackupPort << endl;
    cout << "PrevizDev IP: " << previzDevIP << endl;
    cout << "PrevizDev Port: " << previzDevPort << endl;
    cout << "Audio IP: " << audioIP << endl;
    cout << "Audio Port: " << audioPort << endl;
    cout << "Audio Backup IP: " << audioBackupIP << endl;
    cout << "Audio Backup Port: " << audioBackupPort << endl;
    cout << "Heartbeat IP: " << heartbeatIP << endl;
    cout << "Heartbeat Port: " << heartbeatPort << endl;
    
    
    
    oscHandler.setupPGS1(pgs1IP, pgs1Port);
    oscHandler.setupPGS2(pgs2IP, pgs2Port);
    oscHandler.setupPGSBackup(pgsBackupIP, pgsBackupPort);
    oscHandler.setupPrevizDev(previzDevIP, previzDevPort);
    oscHandler.setupAudio(audioIP, audioPort);
    oscHandler.setupAudioBackup(audioBackupIP, audioBackupPort);
    oscHandler.setupHeartbeat(heartbeatIP, heartbeatPort);
    
    oscHandler.setup();
    
    oscHandler.loadSettings();

    //-----for visualizing message sending-----
    lastPGSSendTime = 0;
    lastAudioSendTime = 0;

    pgsSent = false;
    audioSent = false;
    
    
    oscBoxWidth = 450;
    oscBoxHeight = 600;
    oscVizOrigin.set(leftMargin, topMargin + oscBoxHeight);

    oscVizHeight = 80;
    oscVizWidth = oscBoxWidth;
    
    pgsViz.allocate(oscVizWidth, oscVizHeight);
    pgsViz.begin();
    ofClear(0, 0, 0);
    ofSetColor(0);
    ofDrawRectangle(0, 0, oscVizWidth, oscVizHeight);
    pgsViz.end();
    
    audioViz.allocate(oscVizWidth, oscVizHeight);
    audioViz.begin();
    ofClear(0, 0, 0);
    ofSetColor(0);
    ofDrawRectangle(0, 0, oscVizWidth, oscVizHeight);
    audioViz.end();
    
    
    
    
    
    //-----create buttons for UI navigation-----
    //pass it all the things it needs to control
    panel.setup(&viewMode, cameras, &Lobby1Aggregate, &Lobby2Aggregate);
    
    //video playback
    if(!useLiveFeed){
        
        ofVec2f p;
        p.set(mainContentPos.x, panel.pos.y - playback.bgHeight - 10);
        
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
    
    
    
    //UI Viewability
    //Go through all GUIs and minimize all parameters
    //so long guis dont interfere with visuals
    
    //cameras
    for(int i = 0; i < cameras.size(); i++){
        cameras[i] -> cameraGui.gui.minimizeAll();
    }
    //aggregators
    Lobby1Aggregate.gui.minimizeAll();
    Lobby2Aggregate.gui.minimizeAll();
    
    
    
    //-----ERROR LOGGING-----
    logFileName = "logs/log_for_app_started_at_" + ofGetTimestampString() + ".txt";
    lastLogTime = 0;
    
    timeBetweenLogs = 30;
    numLogsToSave = 100;
    
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

    //Update the UI Panel
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

    
    
    
    //-----ERROR LOGGING-----
    
    //store a new log string into the vector every so often
    if(ofGetElapsedTimeMillis() - lastLogTime > timeBetweenLogs){
        
        //if we've reached 10 past logs, start replacing them...
        if(lastXLogs.size() >= numLogsToSave){
            
            //Delete the first entry
            lastXLogs.erase(lastXLogs.begin());
            
        }
        
        string l = getLogString();
        lastXLogs.push_back(l);
        
        //compose a buffer to save to file
        ofBuffer stats;
        
        //clear buffer garbage
        stats.clear();
        
        stats.append("Summary of last " + ofToString(lastXLogs.size()) + " snapshots\n");
        stats.append("Time between snapshots: " + ofToString(timeBetweenLogs) + " ms\n");
        stats.append("\n");
        stats.append("Entries sorted chronologically (newest last)\n");
        stats.append("\n\n\n");
        
        //now go through all the logs stored and put them in the buffer
        for(int i = 0; i < lastXLogs.size(); i++){
            
            stats.append("==================================================\n");
            stats.append("===================SNAPSHOT # " + ofToString(i + 1) + "==================\n");
            stats.append("==================================================\n");
            
            stats.append(lastXLogs[i]);
            
        }
        
        
        //overwrite file with last stats message
        ofBufferToFile(logFileName, stats);
        
        cout << "Log saved" << endl;

        
        lastLogTime = ofGetElapsedTimeMillis();
    }
    
    

    
    
}


string ofApp::getLogString(){
    
    string log;
    
    unsigned long long now = ofGetElapsedTimeMillis();
    //heading
    

    log += "System snapshot composed...\n";
    log += "Date (MM/DD/YYYY): " + ofToString(ofGetMonth()) + "/" + ofToString(ofGetDay()) + "/" + ofToString(ofGetYear()) + "\n";
    log += "Time: " + ofToString(ofGetHours()) + ":" + ofToString(ofGetMinutes()) + ":" + ofToString(ofGetSeconds()) + "\n";
    log += "\n";
    log += "Last Framerate: " + ofToString(ofGetFrameRate()) + "\n";
    log += "App age: " + ofToString(ofGetElapsedTimef()) + " seconds";
    log += "\n";
    log += "\n";
    log += "----------LOBBY 1----------\n";
    log += "Num Raw Blobs: " + ofToString(Lobby1Aggregate.contours.size()) + "\n";
    log += "Num Processed Blobs: " + ofToString(Lobby1Aggregate.filteredContours.size()) + "\n";
    log += "Last Data Received from threaded processor: " + ofToString(now - Lobby1Aggregate.aggregateProcessor.lastDataSendTime) + " ms ago\n";
    log += "\n";
    
    //go through middle corridors
    for(int i = 6; i <= 9; i++){
        
        log += "----------CORRIDOR " + ofToString(i - 4) + "----------\n";
        log += "Num Raw Blobs: " + ofToString(cameras[i] -> contours.size()) + "\n";
        log += "Num Processed Blobs: " + ofToString(cameras[i] -> filteredContours.size()) + "\n";
        log += "Last Data Received from threaded processor: " + ofToString(now - cameras[i] -> imageProcessor.lastDataSendTime) + " ms ago\n";
        log += "\n";
        
    }
    
    log += "----------LOBBY 2----------\n";
    log += "Num Raw Blobs: " + ofToString(Lobby2Aggregate.contours.size()) + "\n";
    log += "Num Processed Blobs: " + ofToString(Lobby2Aggregate.filteredContours.size()) + "\n";
    log += "Last Data Received from threaded processor: " + ofToString(now - Lobby2Aggregate.aggregateProcessor.lastDataSendTime) + " ms ago\n";
    
    log += "\n";
    log += "\n";
    
    log += "----------RAW CAMERA INFO----------\n";
    log += "\n";
    
    for(int i = 0; i < numFeeds; i++){
        
        log += "-----CAM " + ofToString(i + 1) + "-----\n";
        log += "Num frames received: " + ofToString(cameras[i] -> numFramesRec) + "\n";
        log += "Time since last frame: " + ofToString(now - cameras[i] -> lastFrameTime) + "\n";
        log += "\n";
        
    }
    
    log += "\n\n\n\n";
    
    return log;
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
    
    string currentViewTitle = "";
    
    //draw the raw cameras
    if(viewMode >= 0 && viewMode < numFeeds){
        
        //camera background
        ofBackgroundGradient(cameras[viewMode] -> backgroundInCol, cameras[viewMode] -> backgroundOutCol);
        
        //title
        currentViewTitle = cameras[viewMode] ->  name;
        
        //camera visuals
        cameras[viewMode] -> drawMain();
        
        
        
    } else if(viewMode == 14){
        
        //CORRIDOR 1 AGGREGATION
        ofBackgroundGradient(Lobby1Aggregate.backgroundInCol, Lobby1Aggregate.backgroundOutCol);
        
        //Title
        currentViewTitle = "Lobby 1 Aggregate";
        
        //aggregator visuals
        Lobby1Aggregate.drawMain();
        
        
        
    } else if(viewMode == 15){
        
        //CORRIDOR 2 AGGREGATION
        ofBackgroundGradient(Lobby2Aggregate.backgroundInCol, Lobby2Aggregate.backgroundOutCol);

        //Title
        currentViewTitle = "Lobby 2 Aggregate";
        
        //Aggregator visuals
        Lobby2Aggregate.drawMain();
        
        
        
    } else if(viewMode == 16){
        
        //All Blob Data Mode
        
        currentViewTitle = "All Blob Data";
        
        /*
         *  Shows Data for whole system:
         *  Num blobs
         *  Avg Pos, avg Vel, avg dir, avg speed
         */
        
        int numCorridors = 6;
        int spacer = 15;
        int infoPanelWidth = (ofGetWidth() - spacer * 7)/numCorridors;
        int infoPanelHeight = 400;

        vector<ofVec2f> infoPanelPos;
        infoPanelPos.resize(6);
        for(int i = 0; i < numCorridors; i++){
            infoPanelPos[i].set(spacer + i * (infoPanelWidth + spacer), topMargin);
        
            ofSetColor(0, 200);
            ofDrawRectangle(infoPanelPos[i], infoPanelWidth, infoPanelHeight);
    
            ofSetColor(255);
            
            string name;
            
            if(i == 0){
                name = "Lobby 1";
            } else if(i == 5){
                name = "Lobby 2";
            } else {
                name = "Corridor " + ofToString(i + 1);
            }
            
            smallerFont.drawString(name, infoPanelPos[i].x + spacer, infoPanelPos[i].y + spacer + smallerFont.stringHeight(name));
        }
        
        

        
        
        //Lobby 1
        string Lobby1Blobs = "";
        
        Lobby1Blobs += "Num Blobs:\n" + ofToString(Lobby1Aggregate.contours.size());
        Lobby1Blobs += "\n\nAvg. Pos X (norm):\n" + ofToString(Lobby1Aggregate.avgPos.x);
        Lobby1Blobs += "\n\nAvg. Pos Y (norm):\n" + ofToString(Lobby1Aggregate.avgPos.y);
        Lobby1Blobs += "\n\nAvg. Heading X (norm):\n" + ofToString(Lobby1Aggregate.avgDir.x);
        Lobby1Blobs += "\n\nAvg. Heading Y (norm):\n" + ofToString(Lobby1Aggregate.avgDir.y);
        Lobby1Blobs += "\n\nAvg. Speed (raw): " + ofToString(Lobby1Aggregate.avgSpeed);
        
        ofSetColor(255);
        ofDrawBitmapString(Lobby1Blobs, infoPanelPos[0].x + spacer, infoPanelPos[0].y + spacer * 4);
        
        
        //Single Camera Corridors (cameras 6, 7, 8, 9)
        for(int i = 6; i <= 9; i++){
            
            auto c = cameras[i];
            
            string blobData = "";
            
            blobData += "Num Blobs:\n" + ofToString(c -> contours.size());
            blobData += "\n\nAvg. Pos X (norm):\n" + ofToString(c -> avgPos.x);
            blobData += "\n\nAvg. Pos Y (norm):\n" + ofToString(c -> avgPos.y);
            blobData += "\n\nAvg. Heading X (norm):\n" + ofToString(c -> avgDir.x);
            blobData += "\n\nAvg. Heading Y (norm):\n" + ofToString(c -> avgDir.y);
            blobData += "\n\nAvg. Speed (raw):\n" + ofToString(c -> avgSpeed);
            
            ofSetColor(255);
            ofDrawBitmapString(blobData, infoPanelPos[i - 5].x + spacer, infoPanelPos[i - 5].y + spacer * 4);
            
        }
        
        
        
        //Lobby 2
        string Lobby2Blobs = "";
        
        Lobby2Blobs += "Num Blobs:\n" + ofToString(Lobby2Aggregate.contours.size());
        Lobby2Blobs += "\n\nAvg. Pos X (norm):\n" + ofToString(Lobby2Aggregate.avgPos.x);
        Lobby2Blobs += "\n\nAvg. Pos Y (norm):\n" + ofToString(Lobby2Aggregate.avgPos.y);
        Lobby2Blobs += "\n\nAvg. Heading X (norm):\n" + ofToString(Lobby2Aggregate.avgDir.x);
        Lobby2Blobs += "\n\nAvg. Heading Y (norm):\n" + ofToString(Lobby2Aggregate.avgDir.y);
        Lobby2Blobs += "\n\nAvg. Speed (raw):\n" + ofToString(Lobby2Aggregate.avgSpeed);
        
        ofSetColor(255);
        ofDrawBitmapString(Lobby2Blobs, infoPanelPos[5].x + spacer, infoPanelPos[5].y + spacer * 4);
        
        
        
        
        
        
    } else if(viewMode == 17){
        
        //All cameras
        
        currentViewTitle = "All Cameras";
        
        for(int i = 0; i < numFeeds; i++){
            
            float scale = frameWidth/(float)cameras[i] -> scaledWidth;
            
            cameras[i] -> drawPostCvWindow(allCamsPos[i].x, allCamsPos[i].y, scale);
            
            //draw camera label
            ofSetColor(180);
            ofDrawBitmapString(cameras[i] -> name, allCamsPos[i].x + 5, allCamsPos[i].y + 13);
            
        }
        
        //draw corridor labels
        for(int i = 0; i < allLabelsPos.size(); i++){
            ofSetColor(255);
            smallerFont.drawString("Corridor " + ofToString(i + 1), allLabelsPos[i].x, allLabelsPos[i].y - 5);
        }
        
        
    } else if(viewMode == 18){
        
        //OSC VIEWER
        currentViewTitle = "OSC Info";
        
        oscHandler.drawGui(15, topMargin);
        
        
        //draw OSC data to screen
        string oscDataFormat;
        
        oscDataFormat += "OSC DATA\n";
        oscDataFormat += "---------------\n";
        oscDataFormat += "PGS-1: " + oscHandler.pgs1IP + " on port: " + ofToString(oscHandler.pgs1Port) + "\n";
        oscDataFormat += "PGS-2: " + oscHandler.pgs2IP + " on port: " + ofToString(oscHandler.pgs2Port) + "\n";
        oscDataFormat += "PGS-Backup: " + oscHandler.pgsBackupIP + " on port: " + ofToString(oscHandler.pgsBackupPort) + "\n";
        oscDataFormat += "PrevizDev: " + oscHandler.previzDevIP + " on port: " + ofToString(oscHandler.previzDevPort) + "\n";
        oscDataFormat += "Heartbeat: " + oscHandler.heartbeatIP + " on port: " + ofToString(oscHandler.heartbeatPort) + "\n";
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
        oscDataFormat += "        Corridor Start Blobs Message:\n";
        oscDataFormat += "            Address: /corridor_[num]/start_blobs\n";
        oscDataFormat += "\n";
        oscDataFormat += "        Blob Bundle:\n";
        oscDataFormat += "            Individual Blob Message:\n";
        oscDataFormat += "            Address: /corridor_[num]/blob_data\n";
        oscDataFormat += "            int32 Arg: Blob ID\n";
        oscDataFormat += "            Float Arg: blob position in X\n";
        oscDataFormat += "            Float Arg: blob position in Y\n";
        oscDataFormat += "            Float Arg: avg velocity position in X\n";
        oscDataFormat += "            Float Arg: avg velocity position in Y\n";
        oscDataFormat += "            Bool Arg: is blob standing still\n";
        oscDataFormat += "\n";
        oscDataFormat += "        Corridor Message:\n";
        oscDataFormat += "            Address: /corridor_[num]/stats\n";
        oscDataFormat += "            int32 Arg: numBlobs in corridor\n";
        oscDataFormat += "            Float Arg: avg blob position in X\n";
        oscDataFormat += "            Float Arg: avg blob position in Y\n";
        oscDataFormat += "            Float Arg: avg velocity position in X\n";
        oscDataFormat += "            Float Arg: avg velocity position in Y\n";
        oscDataFormat += "            Float Arg: avg speed\n";
        oscDataFormat += "\n";
        oscDataFormat += "    Corridor 2 Bundle (same format)\n";
        oscDataFormat += "    Corridor 3 Bundle (same format)\n";
        oscDataFormat += "    Etc. for other corridors\n";
        
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
        audioDataFormat += "Sending to Primary Audio \"" + oscHandler.audioIP + "\"\n";
        audioDataFormat += "on port 12345\n";
        audioDataFormat += "And to Backup Audio \"" + oscHandler.audioBackupIP + "\"\n";
        audioDataFormat += "on port 12345\n";
        audioDataFormat += "\n";
        audioDataFormat += "DATA FORMAT\n";
        audioDataFormat += "---------------\n";
        audioDataFormat += "Bundle:\n";
        audioDataFormat += "--Message:\n";
        audioDataFormat += "    Address: /Corridor 1\n";
        audioDataFormat += "    Float Arg: DENSITY\n";
        audioDataFormat += "    Float Arg: SPEED\n";
        audioDataFormat += "--Message:\n";
        audioDataFormat += "    Address: /Corridor 2\n";
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
        audioDataFormat += "of blobs per corridor.\n";
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
        
        int oscGap = 20;
        
        //draw transparent box
        ofSetColor(0, 200);
        ofDrawRectangle(leftMargin + oscBoxWidth + oscGap, topMargin, oscBoxWidth, oscBoxHeight);
        
        
        ofSetColor(255);
        ofDrawBitmapString(audioDataFormat, leftMargin + oscBoxWidth + oscGap + textX, topMargin + textY);
        
        
        //-----OSC Visualizer Boxes-----
        
        //PGS box

        
        //cursor will scroll across both Fbos and drop a tick mark every time an OSC trigger is received
        float pgsCursorX, audioCursorX;
        
        
        
        //-----PGS viz drawing-----
        
        //how long it takes visualizer cursor to reach the end
        float vizDuration = 2.0f;
        
        pgsCursorX = ofMap(fmod(ofGetElapsedTimef(), vizDuration), 0.0f, vizDuration, 0, oscVizWidth);
        
        pgsViz.begin();
        
        //drop a line where the cursor is when an osc message is sent
        if(pgsSent){
            
            ofSetColor(170, 0, 0);
            ofSetLineWidth(1);
            ofDrawLine(pgsCursorX, 0, pgsCursorX, oscVizHeight);
            
            pgsSent = false;
        }
        
        //clear the fbo when the the cursor gets to the end
        if(pgsCursorX > 0.95 * oscVizWidth){
            ofSetColor(0);
            ofDrawRectangle(0, 0, oscVizWidth, oscVizHeight);
        }
        
        pgsViz.end();
        

        
        float oscVizSpacerY = 20;
        
        ofSetColor(255);
        pgsViz.draw(oscVizOrigin.x, oscVizOrigin.y + oscVizSpacerY);
        ofSetColor(255);
        ofDrawBitmapString("Outgoing PGS Data", oscVizOrigin.x + 5, oscVizOrigin.y + oscVizSpacerY + 12);
        
        //draw white border
        ofNoFill();
        ofSetLineWidth(1);
        ofSetColor(255);
        ofDrawRectangle(oscVizOrigin.x, oscVizOrigin.y + oscVizSpacerY, oscVizWidth, oscVizHeight);
        ofFill();
        
        //draw the cursor
        if(oscHandler.sendAllOsc){
            ofSetColor(255, 0, 0);
            ofSetLineWidth(3);
            ofDrawLine(oscVizOrigin.x + pgsCursorX, oscVizOrigin.y + oscVizSpacerY, oscVizOrigin.x + pgsCursorX, oscVizOrigin.y + oscVizSpacerY + oscVizHeight);
        }
        
        //-----Audio viz drawing-----
        float betweenSpace = oscGap;
        float audioVizX = oscVizOrigin.x + oscVizWidth + betweenSpace;
        
        audioCursorX = ofMap(fmod(ofGetElapsedTimef(), vizDuration), 0.0f, vizDuration, 0, oscVizWidth);
        
        
        audioViz.begin();
        
        //drop a line where the cursor is when an osc message is sent
        if(audioSent){
            
            ofSetColor(170, 0, 0);
            ofSetLineWidth(1);
            ofDrawLine(audioCursorX, 0, audioCursorX, oscVizHeight);
            
            audioSent = false;
        }

        
        //clear the fbo when the the cursor gets to the end
        if(audioCursorX > 0.95 * oscVizWidth){
            ofSetColor(0);
            ofDrawRectangle(0, 0, oscVizWidth, oscVizHeight);
        }
        
        audioViz.end();
        
        
        ofSetColor(255);
        audioViz.draw(audioVizX, oscVizOrigin.y + oscVizSpacerY);
        ofSetColor(255);
        ofDrawBitmapString("Outgoing Audio Data", audioVizX + 5, oscVizOrigin.y + oscVizSpacerY + 12);
        
        //draw white border
        ofNoFill();
        ofSetLineWidth(1);
        ofSetColor(255);
        ofDrawRectangle(audioVizX, oscVizOrigin.y + oscVizSpacerY, oscVizWidth, oscVizHeight);
        ofFill();
        
        //draw the cursor
        if(oscHandler.sendAllOsc){
            ofSetColor(255, 0, 0);
            ofSetLineWidth(3);
            ofDrawLine(audioVizX + audioCursorX, oscVizOrigin.y + oscVizSpacerY, audioVizX + audioCursorX, oscVizOrigin.y + oscVizSpacerY + oscVizHeight);
        }
    }
    
    
    
    
    //--------------------OSC DATA--------------------
    
    //send OSC at all?
    if(oscHandler.sendAllOsc && (playback.isPlaying || useLiveFeed)){
        
        
        //==============================
        //-----DATA TO AUDIO SYSTEM-----
        //==============================

        if(oscHandler.sendAudioOsc && ofGetElapsedTimeMillis() - lastAudioSendTime > oscHandler.audioSendRate){
            
            

            //ROUGH pixels to ft conversion
            //ft/pixel = (corridor width in feet)/(feed width in pixels)
            //different in each corridor
            
            float conversionFactor;
            
            
            ofxOscBundle audioBundle;
            
            int numCorridors = 6;
            
            for(int i = 0; i < numCorridors; i++){
                
                //the message for this corridor
                ofxOscMessage m;
                m.setAddress("/Corridor " + ofToString(i + 1));
                
                //conversion factor for scaling from pixels to "feet" (ish)
                //for the speed value to audio:
                //(feet per pixel, i.e. width of corridor in feet divided by width of corridor in pixels)
                
                switch (i) {
                    case 0:
                        conversionFactor = 71.0/(float)Lobby1Aggregate.masterWidth;
                        break;
                    case 1:
                        conversionFactor = 12.0/(float)scaledWidth;
                        break;
                    case 2:
                        conversionFactor = 16.0/(float)scaledWidth;
                        break;
                    case 3:
                        conversionFactor = 17.0/(float)scaledWidth;
                        break;
                    case 4:
                        conversionFactor = 12.0/(float)scaledWidth;
                        break;
                    case 5:
                        conversionFactor = 39.0/(float)Lobby2Aggregate.masterWidth;
                        break;
                }
                
                //Lobby 1
                if(i == 0){
                    
                    m.addFloatArg(Lobby1Aggregate.density);
                    m.addFloatArg(Lobby1Aggregate.avgSpeed * conversionFactor);
                    
                } else if(i >= 1 && i <= 4){
                    
                    //corridors 2, 3, 4, 5 are:
                    //cameras 6, 7, 8, 9 (zero-indexed)
                    m.addFloatArg(cameras[i + 5] -> density);
                    m.addFloatArg(cameras[i + 5] -> avgSpeed * conversionFactor);
                    
                } else {
                    
                    m.addFloatArg(Lobby2Aggregate.density);
                    m.addFloatArg(Lobby2Aggregate.avgSpeed * conversionFactor);
                    
                }
                
                audioBundle.addMessage(m);
                
            }
            
            oscHandler.audioSender.sendBundle(audioBundle);
            oscHandler.audioBackupSender.sendBundle(audioBundle);
            
            lastAudioSendTime = ofGetElapsedTimeMillis();
            audioSent = true;
            
        }
        
        
        
        
        //======================
        //-----DATA TO PGSs-----
        //======================
        
        if(ofGetElapsedTimeMillis() - lastPGSSendTime > oscHandler.pgsSendRate){
            
            
            //we need to go through all the corridors and pull together
            //the data then send it at the very end.
            //first prepare the bundles and messages to be filled as we go along
            
            
            //ONE Total system bundle
            ofxOscBundle allSystemData;
            //ONE message for system-wide data
            ofxOscMessage systemMessage;
            //SIX bundles, one for each corridor
            vector<ofxOscBundle> corridorBundles;
            //SIX messages, one for each corridor
            vector<ofxOscMessage> corridorMessages;
            
            //SIX bundles for each vector of blobs
            vector<ofxOscBundle> blobBundles;
            
            
            
            //averaged data
            int totalNumBlobs = 0;
            
            totalNumBlobs += Lobby1Aggregate.contours.size();
            totalNumBlobs += cameras[6] -> contours.size();
            totalNumBlobs += cameras[7] -> contours.size();
            totalNumBlobs += cameras[8] -> contours.size();
            totalNumBlobs += cameras[9] -> contours.size();
            totalNumBlobs += Lobby2Aggregate.contours.size();
            
            
            
            allSystemData.clear();
            systemMessage.clear();
            
            //start assembling messages and bundles
            systemMessage.setAddress("/system_stats");
            systemMessage.addIntArg(totalNumBlobs);
            
            // all system data message
            allSystemData.addMessage(systemMessage);
            
            
            //now go through all the corridors and add the
            //corridor bundles to the system bundle
            if(oscHandler.sendCorridor1) allSystemData.addBundle(Lobby1Aggregate.corridorBundle);
            if(oscHandler.sendCorridor2) allSystemData.addBundle(cameras[6] -> corridorBundle);
            if(oscHandler.sendCorridor3) allSystemData.addBundle(cameras[7] -> corridorBundle);
            if(oscHandler.sendCorridor4) allSystemData.addBundle(cameras[8] -> corridorBundle);
            if(oscHandler.sendCorridor5) allSystemData.addBundle(cameras[9] -> corridorBundle);
            if(oscHandler.sendCorridor6) allSystemData.addBundle(Lobby2Aggregate.corridorBundle);
            
            
            //Now send it all at once
            if(oscHandler.sendPgs1) oscHandler.pgs1Sender.sendBundle(allSystemData);
            if(oscHandler.sendPgs2) oscHandler.pgs2Sender.sendBundle(allSystemData);
            if(oscHandler.sendPgsBackup) oscHandler.pgsBackupSender.sendBundle(allSystemData);
            if(oscHandler.sendPrevizDev) oscHandler.previzDevSender.sendBundle(allSystemData);
            
            
            lastPGSSendTime = ofGetElapsedTimeMillis();
            pgsSent = true;
            
        }
        
        
    }
    
    
    

    

    
    
    
    
    
    
    
    

    ofSetColor(255);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 30);
    
    titleFont.drawString(currentViewTitle, mainContentPos.x, mainContentPos.y - 20);
    
    string msg;
    if(bScaleDown){
        msg = "CV Scaled Down";
    } else {
        msg = "CV NOT Scaled Down";
    }
    
    ofDrawBitmapString(msg, 15, 45);
    
    
    //UI panel for buttons to switch between views
    panel.draw();

    if(!useLiveFeed) playback.draw();
    
    
    
    //draw save feedback
    if(saveTrigger){
        
        float duration = 50;
        float fadeOut = 900;
        string saveString = "Settings Saved";
        float saveMargin = 15;
        float boxHeight = titleFont.stringHeight(saveString) + saveMargin * 2;
        float descenderTweak = 6; //descenders in the font don't play right with spacing
        ofColor start(0, 120, 255, 0);
        ofColor end(0, 120, 255, 100);
        ofColor boxCol;
        ofColor textCol(255, 0);
        
        if(ofGetElapsedTimeMillis() - saveTime < duration){

            float pct = ofMap(ofGetElapsedTimeMillis(), saveTime, saveTime + duration, 0.0, 1.0);
            boxCol = start.getLerped(end, pct);
            textCol = ofColor(255, 0).getLerped(ofColor(255, 255), pct);
            
            ofSetColor(boxCol);
            ofDrawRectangle(0, ofGetHeight()/2 - boxHeight/2, ofGetWidth(), boxHeight);
            
            ofSetColor(textCol);
            titleFont.drawString(saveString, ofGetWidth()/2 - titleFont.stringWidth(saveString)/2, ofGetHeight()/2 + titleFont.stringHeight(saveString)/2 - descenderTweak);

            
        } else if(ofGetElapsedTimeMillis() - saveTime < duration + fadeOut) {
            
            float pct = ofMap(ofGetElapsedTimeMillis(), saveTime + duration, saveTime + duration + fadeOut, 0.0, 1.0);
            
            boxCol = end.getLerped(start, pct);
            textCol = ofColor(255, 255).getLerped(ofColor(255, 0), pct);
            
            ofSetColor(boxCol);
            ofDrawRectangle(0, ofGetHeight()/2 - boxHeight/2, ofGetWidth(), boxHeight);
            
            ofSetColor(textCol);
            titleFont.drawString(saveString, ofGetWidth()/2 - titleFont.stringWidth(saveString)/2, ofGetHeight()/2 + titleFont.stringHeight(saveString)/2 - descenderTweak);
            
        } else{
            saveTrigger = false;
        }
        
        
    }

    
    
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
            cameras[i] -> saveAllSettings();
        }
        
        oscHandler.saveSettings();
        Lobby1Aggregate.saveAllSettings();
        Lobby2Aggregate.saveAllSettings();
        
        saveTrigger = true;
        saveTime = ofGetElapsedTimeMillis();
        cout << "Cameras, Aggregates and OSC settings saved" << endl;
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
    panel.checkForClicks(x, y);

    
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

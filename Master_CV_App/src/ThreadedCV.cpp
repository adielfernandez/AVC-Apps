//
//  ThreadedCV.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 4/13/16.
//
//

#include "ThreadedCV.hpp"


using namespace ofxCv;


ThreadedCV::ThreadedCV(){
    
}


ThreadedCV::~ThreadedCV(){
    

    closeAllChannels();
    
    waitForThread(true, 4000);
    
}

void ThreadedCV::closeAllChannels(){
    
    //close thread channels
    quadPixelsIn.close();
    settingsIn.close();
    threshPixOut.close();
    contoursOut.close();
    
}

void ThreadedCV::emptyAllChannels(){
    
    cout << "Emptying all thread channels" << endl;
    
    //close thread channels
    quadPixelsIn.empty();
    settingsIn.empty();
    threshPixOut.empty();
    contoursOut.empty();
    
}

void ThreadedCV::setup(ofPixels *_mainThreadPix, ofxCv::ContourFinder *_mainThreadCons){
    
    mainThreadPix = _mainThreadPix;
    mainThreadContours = _mainThreadCons;
    lastDataSendTime = 0;
    
    needsAutoReset = true;
    
    threadPixels.allocate(320, 256, 1);
    threshPix.allocate(320, 256, 1);
    blurredPix.allocate(320, 256, 1);
    
    isThreadCrashed = false;
    firstAfterCrash = true;
    
    firstStop = true;
    
    lastRestartTime = 0;
    
    startThread();
    
}


void ThreadedCV::analyze(ofPixels & p, vector<int> & settings){
    
    
    //send texture and CV settings to the thread
    quadPixelsIn.send(p);
    settingsIn.send(settings);
    
}


void ThreadedCV::update(){

    
    
    ofPixels t;
    if(threshPixOut.tryReceive(t)){
        *mainThreadPix = t;
        lastDataSendTime = ofGetElapsedTimeMillis();
    }
    
    ofxCv::ContourFinder c;
    if(contoursOut.tryReceive(c)){
        *mainThreadContours = c;
    }
    
    if(ofGetElapsedTimeMillis() - lastDataSendTime > 1000){
        isThreadCrashed = true;
        
        if(firstAfterCrash){
            cout << "Stopping Thread" << endl;
            stopThread();
            
            emptyAllChannels();
            
            background.reset();
            
            firstAfterCrash = false;
        }
        
    } else {
        
        isThreadCrashed = false;
        firstAfterCrash = true;
        
    }
    
    
    //only try to restart the thread every 4 seconds
    if(isThreadCrashed && ofGetElapsedTimeMillis() - lastRestartTime > 10000){
        
        cout << "Attempting to start thread..." << endl;
        startThread();
        
        lastRestartTime = ofGetElapsedTimeMillis();
        firstStop = true;
        
    }
    
    //if it has been 2 seconds since last restart AND we're still crashed
    //then stop and prepare for the next restart
    //make sure to wait longer since we have waitForThread(5000)
    if(isThreadCrashed && ofGetElapsedTimeMillis() - lastRestartTime > 4000 && firstStop){
        
        cout << "Stopping Thread" << endl;
        stopThread();
        
        emptyAllChannels();
        
        background.reset();
        
        firstStop = false;
        
    }
    
    
}




void ThreadedCV::threadedFunction(){
    
    while(isThreadRunning()){
        
        
        //        cout << getThreadName() << " is running..." << endl;
        
        
        //wait until we have our settings loaded and our pixels
        if(settingsIn.receive(settings) && quadPixelsIn.receive(threadPixels)){
            
            //unpack the vector and save it to the values we'll be using
            blurAmt = settings[0];
            numErosions = settings[1];
            numDilations = settings[2];
            learningTime = settings[3];
            useBgDiff = settings[4];        //int casted back to bool
            resetBG = settings[5];
            threshold = settings[6];
            minBlobArea = settings[7];
            maxBlobArea = settings[8];
            persistence = settings[9];
            maxBlobDist = settings[10];
            contrastExp = settings[11]/1000.0f;
            contrastShift = settings[12]/1000.0f;
            

            threadPixels.setImageType(OF_IMAGE_GRAYSCALE);
            
            //EXPERIMENTAL: Contrast
            for(int i = 0; i < threadPixels.getWidth() * threadPixels.getHeight(); i++){
                
                float normPixVal = threadPixels[i]/255.0f;
                
                threadPixels[i] = ofClamp( 255 * pow((normPixVal + contrastShift), contrastExp), 0, 255);
            }
            
            
            //blur it
            ofxCv::GaussianBlur(threadPixels, blurredPix, blurAmt);
            
            //either use BG differencing or just straight up threshold it
            if(useBgDiff){
                
                //if we're going from not using the background to using it again
                //then reset the background (needsAutoReset == true).
                //Also reset it if requested by the GUI (resetBG == true)
                if(needsAutoReset || resetBG){
                    
                    background.reset();
                    background.setDifferenceMode(ofxCv::RunningBackground::BRIGHTER);
                    needsAutoReset = false;
                    
                }

                background.setLearningTime(learningTime);
                background.setThresholdValue(threshold);
                
                background.update(blurredPix, threshPix);

                
                
            } else {
                
                //threshold it
                ofxCv::threshold(blurredPix, threshPix, threshold);
                
                needsAutoReset = true;
               
            }
            
            
            //ERODE it
            for(int e = 0; e < numErosions; e++){
                erode(threshPix);
            }
            
            //DILATE it
            for(int d = 0; d < numDilations; d++){
                dilate(threshPix);
            }
            
            
            //-----Done with image altering-----
            //------Now do contour finding------
            
            
            //Define contour finder
            contours.setMinArea(minBlobArea);
            contours.setMaxArea(maxBlobArea);
            contours.setThreshold(254);  //only detect white
            
            // wait before forgetting something
            contours.getTracker().setPersistence(persistence);
            
            // an object can move up to ___ pixels per frame
            contours.getTracker().setMaximumDistance(maxBlobDist);
            
            //find dem blobs
            contours.findContours(threshPix);
            

            threshPixOut.send(std::move(threshPix));

            contoursOut.send(std::move(contours));
            
        }
        
        
        
    }
    
}

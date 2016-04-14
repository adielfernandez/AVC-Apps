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
    
    //close thread channels
    quadPixelsIn.close();
    settingsIn.close();
    threshPixOut.close();
    contoursOut.close();
    
//    cout << getThreadName() << " closing...\n" << endl;
    
    waitForThread(true, 1000);
}


void ThreadedCV::setup(ofPixels *_mainThreadPix, ofxCv::ContourFinder *_mainThreadCons){
    
    mainThreadPix = _mainThreadPix;
    mainThreadContours = _mainThreadCons;
    lastDataSendTime = 0;
    
    startThread();
//    cout << getThreadName() << " started...\n" << endl;
    
    
    
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
            
            //Make grayscale
            threadPixels.setImageType(OF_IMAGE_GRAYSCALE);
            
            //blur it
            ofxCv::GaussianBlur(threadPixels, blurredPix, blurAmt);
            
            if(resetBG) background.reset();
            
            //either use BG differencing or just straight up threshold it
            if(useBgDiff){
                
                background.setLearningTime(learningTime);
                background.setThresholdValue(threshold);
                
                background.update(blurredPix, threshPix);
                
                
            } else {
                
                //threshold it
                ofxCv::threshold(blurredPix, threshPix, threshold);
               
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

//
//  ThreadedSoloCV.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 4/13/16.
//
//

#include "ThreadedSoloCV.hpp"


using namespace ofxCv;


ThreadedSoloCV::ThreadedSoloCV(){
    
}


ThreadedSoloCV::~ThreadedSoloCV(){
    
    //close thread channels
    quadPixelsIn.close();
    settingsIn.close();
    threshPixOut.close();
    contoursOut.close();
    
    cout << getThreadName() << " closing...\n" << endl;
    
    waitForThread(true, 1000);
}


void ThreadedSoloCV::setup(ofPixels *_mainThreadPix, ofxCv::ContourFinder *_mainThreadCons, bool solo){
    
    mainThreadPix = _mainThreadPix;
    mainThreadContours = _mainThreadCons;
    soloCam = solo;
    lastDataSendTime = 0;
    
    startThread();
    cout << getThreadName() << " started...\n" << endl;
    
    
    
}


void ThreadedSoloCV::analyze(ofPixels & p, vector<int> & settings){
    
    
    //send texture and CV settings to the thread
    quadPixelsIn.send(p);
    settingsIn.send(settings);
    
}


void ThreadedSoloCV::update(){

    
    
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




void ThreadedSoloCV::threadedFunction(){
    
    while(isThreadRunning()){
        
        
        //        cout << getThreadName() << " is running..." << endl;
        
        
        
        //wait until we have our settings loaded and our pixels
        if(settingsIn.receive(settings) && quadPixelsIn.receive(threadPixels)){
            
            //unpack the vector and save it to the values we'll be using
            threshold = settings[0];
            blurAmt = settings[1];
            numErosions = settings[2];
            numDilations = settings[3];
            minBlobArea = settings[4];
            maxBlobArea = settings[5];
            persistence = settings[6];
            maxBlobDist = settings[7];
            cropStartX = settings[8];
            cropStartY = settings[9];
            cropEndX = settings[10];
            cropEndY = settings[11];
            
            //Make grayscale
            threadPixels.setImageType(OF_IMAGE_GRAYSCALE);
            
            //blur it
            ofxCv::GaussianBlur(threadPixels, blurredPix, blurAmt);
            
            //threshold it
            ofxCv::threshold(blurredPix, threshPix, threshold);
            
            
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

//
//  ThreadedCV.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/23/16.
//
//

#include "ThreadedCV.hpp"


using namespace ofxCv;


ThreadedCV::ThreadedCV(){
    
}


ThreadedCV::~ThreadedCV(){
    
    //close thread channels
    quadPixelsIn.close();
    CVsettingsIn.close();
    
    threshPixOut.close();
    contoursOut.close();
    
    
    waitForThread(true);
    
}


void ThreadedCV::setup(bool _scaleDown){
    
    bScaleDown = _scaleDown;
    
    feedWidth = 640;
    feedHeight = 512;
    
    if(bScaleDown){
        scaledWidth = feedWidth/2;
        scaledHeight = feedHeight/2;
    } else {
        scaledWidth = feedWidth;
        scaledHeight = feedHeight;
    }
    

        
    //allocate all the ofPixels we'll be using
    grayPix.allocate(scaledWidth, scaledHeight, 1);
    blurredPix.allocate(scaledWidth, scaledHeight, 1);
    threshPix.allocate(scaledWidth, scaledHeight, 1);
    
    startThread();
    
    
}


void ThreadedCV::analyze(ofPixels &pix, const vector<int> &settings){
    
    //send texture and CV settings to the thread
    quadPixelsIn.send(pix);
    CVsettingsIn.send(settings);

    
}



void ThreadedCV::update(){
    
    //check if the thread is done with threshPix
    //if so upload it to the texture
    while(threshPixOut.tryReceive(threshPix)){
        mainThreadThreshTex -> loadData(threshPix);
    }
    
    //check if the thread is done finding contours
    //then copy the threaded contours to a public version
    while(contoursOut.tryReceive(contours)){
        (*mainThreadContours) = std::move(contours);
    }
    
    
}

void ThreadedCV::ThreadedFunction(){
    
        
    //the texture we'll get from the feed in the GL Thread
    ofPixels quadPix;
    
    //only do anything if we have a new frame
    while(quadPixelsIn.receive(quadPix)){

        
        //vector we're getting from the GL thread
        vector<int> settings;
        
        //Use that vector to populate these
        int threshold;
        int blurAmt;
        int numErosions;
        int numDilations;
        int minBlobArea;
        int maxBlobArea;
        int persistence;
        int maxBlobDist;

    
        //wait until we have our settings loaded
        while(CVsettingsIn.receive(settings)){

            //unpack the vector and save it to the values we'll be using
            threshold = settings[0];
            blurAmt = settings[1];
            numErosions = settings[2];
            numDilations = settings[3];
            minBlobArea = settings[4];
            maxBlobArea = settings[5];
            persistence = settings[6];
            maxBlobDist = settings[7];
            
        }
    
        //we have our pixels and our settings so get to work...
        
        
        //Convert to color
        convertColor(quadPix, grayPix, CV_RGBA2GRAY);
        
        //blur it
        ofxCv::GaussianBlur(grayPix, blurredPix, blurAmt);
        
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
        
        // wait for half a frame before forgetting something
        contours.getTracker().setPersistence(persistence);
        
        // an object can move up to ___ pixels per frame
        contours.getTracker().setMaximumDistance(maxBlobDist);
        
        //find dem blobs
        contours.findContours(threshPix);

        
        
        //now pass all the data back to the GL thread
        threshPixOut.send(std::move(threshPix));
        contoursOut.send(std::move(contours));
        
        
    }
    
    
    
    
    
}
















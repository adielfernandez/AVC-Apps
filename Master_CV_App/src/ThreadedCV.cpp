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
    threshPixOut.close();
    
    
    waitForThread(true);
    
}


void ThreadedCV::setup(ofPixels *_mainThreadPix){

    mainThreadPix = _mainThreadPix;
    
    
    startThread();
    
    
}


void ThreadedCV::analyze(ofPixels & p, vector<int> & settings){

    
    //send texture and CV settings to the thread
    quadPixelsIn.send(p);
    settingsIn.send(settings);
    
}


void ThreadedCV::update(){
    
    newFrame = false;
    while(threshPixOut.tryReceive(threshPix)){
        newFrame = true;
    }
    
    
    if(newFrame){

        *mainThreadPix = threshPix;
    
    }
    
    

}




void ThreadedCV::threadedFunction(){
    
    while(isThreadRunning()){

        
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

        //the texture we'll get from the feed in the GL Thread
        ofPixels threadPixels;
        ofPixels blurredPix;
        ofPixels thresholdedPix;

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



            //blur it
            ofxCv::GaussianBlur(threadPixels, blurredPix, blurAmt);

            //threshold it
            ofxCv::threshold(blurredPix, thresholdedPix, threshold);

            //ERODE it
            for(int e = 0; e < numErosions; e++){
                erode(thresholdedPix);
            }

            //DILATE it
            for(int d = 0; d < numDilations; d++){
                dilate(thresholdedPix);
            }


            //-----Done with image altering-----
            //------Now do contour finding------


            //Define contour finder
//            contours.setMinArea(minBlobArea);
//            contours.setMaxArea(maxBlobArea);
//            contours.setThreshold(254);  //only detect white
//
//            // wait for half a frame before forgetting something
//            contours.getTracker().setPersistence(persistence);
//
//            // an object can move up to ___ pixels per frame
//            contours.getTracker().setMaximumDistance(maxBlobDist);
//
//            //find dem blobs
//            contours.findContours(threshPix);
            
            
            
            threshPixOut.send(std::move(thresholdedPix));

        }
        
    
    
    }

}












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
    settingsIn.close();
    threshPixOut.close();
    contoursOut.close();
    
    waitForThread(true);
    
}


void ThreadedCV::setup(ofPixels *_mainThreadPix, ofxCv::ContourFinder *_mainThreadCons, bool solo){

    mainThreadPix = _mainThreadPix;
    mainThreadContours = _mainThreadCons;
    soloCam = solo;
    
    
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
    }
    
    ofxCv::ContourFinder c;
    if(contoursOut.tryReceive(c)){
        *mainThreadContours = c;
    }
}




void ThreadedCV::threadedFunction(){
    
    while(isThreadRunning()){

//        float FR = 1000.0/((float)ofGetElapsedTimeMillis() - (float)lastFrameTime);
//        cout << "[Thread] Framerate: " << FR << endl;
//        lastFrameTime = ofGetElapsedTimeMillis();
        
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
            
            if(!soloCam){
                int w = cropEndX - cropStartX;
                int h = cropEndY - cropStartY;
                
                threadPixels.crop(cropStartX, cropStartY, w, h);
            }
        
        
            
            
            //blur it
            ofxCv::GaussianBlur(threadPixels, blurredPix, blurAmt);
            
            //only threshold it if we're a soloCam
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
            
            
//            if(soloCam){
                threshPixOut.send(std::move(threshPix));
//            } else {
//                threshPixOut.send(std::move(threadPixels));
//            }
            
            
            contoursOut.send(std::move(contours));
            
        }
        
    
    
    }

}












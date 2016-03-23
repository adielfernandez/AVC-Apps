//
//  ThreadedCV.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/23/16.
//
//

#include "ThreadedCV.hpp"





ThreadedCV::ThreadedCV(bool _scaleDown){
    
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
    
    //allocate all the stuff we'll be using
    //Textures
    quadTex.allocate(scaledWidth, scaledHeight, GL_LUMINANCE);
    
    //FBO
    fbo.allocate(scaledWidth, scaledHeight);
    
    //(also clear it out)
    fbo.begin();
    ofClear(0, 0, 0, 0);
    fbo.end();
    
    //ofPixels
    grayPix.allocate(scaledWidth, scaledHeight, 1);
    fboPix.allocate(scaledWidth, scaledHeight, 1);
    blurredPix.allocate(scaledWidth, scaledHeight, 1);
    threshPix.allocate(scaledWidth, scaledHeight, 1);
    
    //ofImage
    thresholdImg.allocate(scaledWidth, scaledHeight, OF_IMAGE_GRAYSCALE);
    
    //initialize imageQuad vector with 4 points
    //this will be overwritten by xml settings later
    imageQuad.resize(4);
    imageQuad[0].set(0, 0);
    imageQuad[1].set(0, 0);
    imageQuad[2].set(0, 0);
    imageQuad[3].set(0, 0);
    
    
    startThread();
    
}

ThreadedCV::~ThreadedCV(){
    
    //close thread channels
    rawTexIn.close();
    CVsettingsIn.close();
    quadTexOut.close();
    threshTexOut.close();
    contoursOut.close();
    
    
    waitForThread(true);
    
}

void ThreadedCV::analyze(ofTexture &t, const vector<float> &settings){
    
    //send texture and CV settings to the thread
    rawTexIn.send(t);
    CVsettingsIn.send(settings);

    
    
    

    
    
}



void ThreadedCV::update(){
    
    
    
}

void ThreadedCV::ThreadedFunction(){
    
    
    //the texture we'll get from the feed in the GL Thread
    ofTexture rawTex;
    
    //only do anything if we have a new frame
    while(rawTexIn.receive(rawTex)){

        
        //vector we're getting from the GL thread
        vector<float> settings;
        
        //Use that vector to populate these
        int threshold;
        int blurAmt;
        int numErosions;
        int numDilations;
        int minBlobArea;
        int maxBlobArea;
        int persistence;
        int maxBlobDist;
        vector<ofPoint> imageQuad;
        imageQuad.resize(4);
    
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
            
            //quad points
            imageQuad[0].set(settings[8], settings[9]);
            imageQuad[1].set(settings[10], settings[11]);
            imageQuad[2].set(settings[12], settings[13]);
            imageQuad[3].set(settings[14], settings[15]);
            
        }
    
        //we have our texture and our settings so get to work...
        
        
        
        
        
        
        
    }
    
    
    
    
    
}
















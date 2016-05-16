//
//  BlobFilter.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 5/10/16.
//
//

#ifndef BlobFilter_hpp
#define BlobFilter_hpp

#include <stdio.h>

#endif /* BlobFilter_hpp */


#include "ofMain.h"
#include "ofxCv.h"

#pragma once

struct ProcessedBlob{
    
    int ID;
    ofVec2f center, vel;
    
    bool still = false;
    unsigned long long lastMoveTime = 0;
    
    vector<ofVec2f> subBlobs;
    
    
};



class BlobFilter{
    
public:
    
    BlobFilter();
    
    void setup(ofxCv::ContourFinder *_rawContours);
    void update(int _personRadius, int _stillTimeout, float _speedThresh);
    void draw();
    
    //mirror the ofxCv::ContourFinder style
    int size();
    ofPoint getCenter(int i);
    ofVec2f getVelocity(int i);
    int getLabel(int i);
    bool getStill(int i);
    
    
    float personRadius;
    int stillTimeout;
    float speedThresh;
    
    ofxCv::ContourFinder *rawContours;
    
    //vectors to hold processed blobs
    vector<ProcessedBlob> processedBlobs;
    
    vector<ProcessedBlob> tempBlobs;
    
};
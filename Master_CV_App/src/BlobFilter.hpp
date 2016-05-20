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
    ofVec2f center, vel, centerNorm;
    
    bool still = false;
    unsigned long long lastMoveTime = 0;
    
    vector<ofVec2f> subBlobs;
    
    
};



class BlobFilter{
    
public:
    
    BlobFilter();
    
    void setup(ofxCv::ContourFinder *_rawContours, string _corridorName);
    void update(int _personRadius, int _stillTimeout, float _speedThresh, int _width, int _height);
    void draw();
    
    void setDrawInfo(bool draw);
    
    bool drawInfo;
    
    string corridorName;
    
    //mirror the ofxCv::ContourFinder style
    int size();
    ofPoint getCenter(int i);
    ofPoint getCenterNormalized(int i);
    ofVec2f getVelocity(int i);
    int getLabel(int i);
    bool getStill(int i);
    
    int scaledWidth, scaledHeight;
    
    float personRadius;
    int stillTimeout;
    float speedThresh;
    
    ofxCv::ContourFinder *rawContours;
    
    //vectors to hold processed blobs
    vector<ProcessedBlob> processedBlobs;
    vector<ProcessedBlob> tempBlobs;
    
    int numControlPtsX, numControlPtsY;
    vector<float> controlPtsXInput;
    vector<float> controlPtsXOutput;
    
    vector<float> controlPtsYInput;
    vector<float> controlPtsYOutput;
    
};








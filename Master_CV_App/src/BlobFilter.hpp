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
    
    vector<ofVec2f> subBlobs;
    
};



class BlobFilter{
    
public:
    
    BlobFilter();
    
    void setup(ofxCv::ContourFinder *_rawContours);
    void update(int rad);
    void draw();
    
    //mirror te ofxCv::ContourFinder style
    int size();
    ofPoint getCenter(int i);
    ofVec2f getVelocity(int i);
    int getLabel(int i);
    
    
    ofxCv::ContourFinder *rawContours;
    
    //vectors to hold processed blobs
    vector<ProcessedBlob> processedBlobs;
    
    float personRadius;
    
};
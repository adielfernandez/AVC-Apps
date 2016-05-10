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

class BlobFilter{
    
public:
    
    BlobFilter();
    
    void setup(ofxCv::ContourFinder *_rawContours);
    void process();
    
    //mirror te ofxCv::ContourFinder style
    int size();
    ofPoint getCenter(int i);
    ofVec2f getVelocity(int i);
    
    
    ofxCv::ContourFinder *rawContours;
    
    //vectors to hold processed blobs
    vector<int> labels;
    vector<ofVec2f> centers;
    vector<ofVec2f> velocities;
    vector<vector<ofVec2f>> componentBlobs;
    
    float personRadius;
    
};
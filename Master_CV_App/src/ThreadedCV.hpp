//
//  ThreadedCV.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/23/16.
//
//

#ifndef ThreadedCV_hpp
#define ThreadedCV_hpp

#include <stdio.h>

#endif /* ThreadedCV_hpp */

#include "ofMain.h"
#include "ofxCv.h"

#pragma once



/*
 *ThreadedCV:
 *  INPUT:
 *      -Raw texture
 *      -CV variables:
 *          -Blur amt, threshold, etc.
 *
 *  OUTPUT:
 *      -Quad mapped tex
 *      -Thresholded tex
 *      -Contours
 */


class ThreadedCV: public ofThread{
    
public:
    
    ThreadedCV();
    ~ThreadedCV();
    
    void setup(ofPixels *_mainThreadPix, ofxCv::ContourFinder *_mainThreadCons);
    void analyze(ofPixels & pix, vector<int> & settings);
    void update();
    
    
private:
    
    
    //into thread
    ofThreadChannel<ofPixels> quadPixelsIn;
    ofThreadChannel<vector<int>> settingsIn;
    
    //Thread output
    ofThreadChannel<ofPixels> threshPixOut;
    ofThreadChannel<ofxCv::ContourFinder> contoursOut;
    
    ofPixels *mainThreadPix;
    ofxCv::ContourFinder *mainThreadContours;
    
    void threadedFunction();

    ofPixels threshPix;
    ofxCv::ContourFinder contours;
    
    vector<int> settings;
    ofPixels threadPixels;
    ofPixels blurredPix;
    
    int threshold;
    int blurAmt;
    int numErosions;
    int numDilations;
    int minBlobArea;
    int maxBlobArea;
    int persistence;
    int maxBlobDist;
    
    unsigned long lastFrameTime;
};










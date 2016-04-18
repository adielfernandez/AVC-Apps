//
//  ThreadedCV.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 4/13/16.
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
 *      -Raw texture (post quad mapping)
 *      -CV variables:
 *          -Blur amt, threshold, etc.
 *
 *  OUTPUT:
 *      -Thresholded ofPixels
 *      -Contours
 */


class ThreadedCV: public ofThread{
    
public:
    
    ThreadedCV();
    ~ThreadedCV();
    
    void setup(ofPixels *_mainThreadPix, ofxCv::ContourFinder *_mainThreadCons);
    void analyze(ofPixels & pix, vector<int> & settings);
    void update();
    
    unsigned long long lastDataSendTime;
    
    bool isThreadCrashed;
    
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
    
    ofxCv::ContourFinder contours;
    
    vector<int> settings;
    ofPixels threadPixels;
    ofPixels blurredPix;
    ofPixels threshPix;

    
    int blurAmt;
    int numErosions;
    int numDilations;
    int learningTime;
    bool useBgDiff;
    bool resetBG;
    int threshold;
    int minBlobArea;
    int maxBlobArea;
    int persistence;
    int maxBlobDist;
    int cropStartX;
    int cropStartY;
    int cropEndX;
    int cropEndY;
    
    ofxCv::RunningBackground background;
    
    bool useBgNow;
    
};










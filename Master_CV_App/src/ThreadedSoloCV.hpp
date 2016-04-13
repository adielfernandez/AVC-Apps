//
//  ThreadedSoloCV.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 4/13/16.
//
//

#ifndef ThreadedSoloCV_hpp
#define ThreadedSoloCV_hpp

#include <stdio.h>

#endif /* ThreadedSoloCV_hpp */

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
 *      -if soloCam: Thresholded ofPixels
 *      -            & Contours
 *      -if !soloCam: Cropped pixels (no threshold)
 */


class ThreadedSoloCV: public ofThread{
    
public:
    
    ThreadedSoloCV();
    ~ThreadedSoloCV();
    
    void setup(ofPixels *_mainThreadPix, ofxCv::ContourFinder *_mainThreadCons, bool solo);
    void analyze(ofPixels & pix, vector<int> & settings);
    void update();
    
    unsigned long long lastDataSendTime;
    
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
    int cropStartX;
    int cropStartY;
    int cropEndX;
    int cropEndY;
    
    bool soloCam;
    
    
};










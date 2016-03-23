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
    
    void setup(bool _scaleDown);
    void analyze(ofPixels &pix, const vector<int> &settings);
    void update();
    
    ofPixels & getThreshPix();
    ofxCv::ContourFinder & getContours();
    
    
    bool bScaleDown;
    int feedWidth, feedHeight;
    int scaledWidth, scaledHeight;
    

    
    
    
private:
    
    bool newFrame;
    
    //into thread
    ofThreadChannel<ofPixels> quadPixelsIn;
    ofThreadChannel<vector<int>> CVsettingsIn;
    
    //Thread output
    ofThreadChannel<ofPixels> threshPixOut;
    ofThreadChannel<ofxCv::ContourFinder> contoursOut;
    
    
    void ThreadedFunction();
    
    
    ofxCv::ContourFinder contours;
    ofPixels grayPix;
    ofPixels blurredPix;
    ofPixels threshPix;
    
    ofTexture *mainThreadThreshTex;
    ofxCv::ContourFinder *mainThreadContours;


    
};










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
    
    ThreadedCV(bool _scaleDown);
    ~ThreadedCV();
    
    void analyze(ofTexture &t, const vector<float> &settings);
    void update();
    
    bool bScaleDown;
    int feedWidth, feedHeight;
    int scaledWidth, scaledHeight;
    
    
    
    
    
private:
    
    bool newFrame;
    
    //into thread
    ofThreadChannel<ofTexture> rawTexIn;
    ofThreadChannel<vector<float>> CVsettingsIn;
    
    //Thread output
    ofThreadChannel<ofTexture> quadTexOut;
    ofThreadChannel<ofTexture> threshTexOut;
    ofThreadChannel<ofxCv::ContourFinder> contoursOut;
    
    void ThreadedFunction();
    

    ofMesh quadMappedMesh;
    ofTexture quadTex;
    
    ofFbo fbo;
    
    ofxCv::ContourFinder contours;
    ofPixels grayPix;
    ofPixels fboPix;
    ofPixels blurredPix;
    ofPixels threshPix;
    ofImage thresholdImg;
    


    
};










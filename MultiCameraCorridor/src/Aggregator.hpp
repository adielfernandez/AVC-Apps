//
//  Aggregator.hpp
//  Stitching Test
//
//  Created by Adiel Fernandez on 3/3/16.
//
//

#ifndef Aggregator_hpp
#define Aggregator_hpp

#include <stdio.h>

#endif /* Aggregator_hpp */


#include "ofMain.h"
#include "ofxCv.h"
#include "AggregateGui.hpp"

#pragma once


class Aggregator{
    
public:
    
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void mouseEntered(ofMouseEventArgs & args);
    void mouseExited(ofMouseEventArgs & args);
    
    //=========================    
    
    Aggregator();
    void setup(int _numFeeds, int w, int h);
    void getNewData(vector<shared_ptr<ofTexture>> rawTex, vector<shared_ptr<ofPixels>> threshPix);
    void update();
    
    void drawRaw(int x, int y);
    void drawCV(int x, int y);
    void drawGui(int x, int y);
    
    
    //General info and UI
    int numFeeds;
    int feedWidth, feedHeight;
    
    
    //Image Data
    vector<shared_ptr<ofPixels>> cameraThreshPixels;
    vector<shared_ptr<ofTexture>> cameraRawTexs;
    
    
    //Control
    bool bEnableMouse;
    vector<ofVec2f> positions;
    ofVec2f adjustedMouse, adjustedOrigin;
    vector<bool> posMouseLock;
    float mouseHandleRad;
    ofColor handleCol, grabbedCol;
    
    ofPixels masterPix;
    int overallWidth;
    int overallHeight;
    ofImage masterImg;
    
    
    
    //Gui
    AggregateGui aGui;
    
    
};
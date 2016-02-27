//
//  Corridor.h
//  Corridors
//
//  Created by Adiel Fernandez on 2/26/16.
//
//

#ifndef Corridor_h
#define Corridor_h

#include <stdio.h>

#endif /* Corridor_h */


#include "ofMain.h"

#include "ofxGui.h"
#include "Gui.h"
#include "Osc.h"
#include "ofGstVideoPlayer.h"
#include "ofxCv.h"

#pragma once

class Corridor{
    
public:
    
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void mouseEntered(ofMouseEventArgs & args);
    void mouseExited(ofMouseEventArgs & args);
    
    //=========================
    
    Corridor();
    void setup(string IP, string _corridorName);
    void update();
    
    void drawRaw(ofVec2f pos);
    void drawCV(ofVec2f pos, float scale);
    void drawGui(int x, int y);

    
    
    
    //-----Corridor UI-----
    string name;
    ofColor circleCol, circleGrab;
    
    
    //-----Camera Stream-----
    int feedWidth, feedHeight;
    
    ofGstVideoUtils gst;
    
    
    //-----CV && Image Processing-----
    //for remapping a subImage from the feed

    bool bEnableQuadMapping;
    
    vector<ofPoint> imageQuad;
    vector<bool> quadPtMouseLock;
    ofVec2f adjustedQuadOrigin;
    ofVec2f adjustedMouse;
    float mapPtRad;
    
    ofTexture rawTex;
    ofMesh quadMappedMesh;
    ofTexture subTex;
    ofFbo fbo;

    ofxCv::ContourFinder contours;
    ofPixels grayPix;
    ofPixels blurredPix;
    ofPixels threshPix;
    ofImage thresholdImg;
    
    
    //----------EXPERIMENTAL----------
    //bool to toggle scaling down 640x512
    //down to 320x256 for faster processing
    bool bScaleDown;
    
    
    
    //-----Gui-----
    Gui corridorGui;
    bool bShowGui;
    float lastFrameTime;
    int cameraFPS;

    
    
    
};

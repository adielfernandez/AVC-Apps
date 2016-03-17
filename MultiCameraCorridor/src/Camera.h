//
//  Camera.h
//  Corridors
//
//  Created by Adiel Fernandez on 2/26/16.
//
//

#ifndef Corridor_h
#define Corridor_h

#include <stdio.h>

#endif /* Camera_h */


#include "ofMain.h"

#include "ofxGui.h"
#include "CamGui.h"
#include "Osc.h"
#include "ofGstVideoPlayer.h"
#include "ofxCv.h"
#include "ofxOpencv.h"

#pragma once




class Camera{
    
public:
    
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void mouseEntered(ofMouseEventArgs & args);
    void mouseExited(ofMouseEventArgs & args);
    
    //=========================
    
    Camera();
    void setup(string _IP, string _corridorName, bool _scaleDown, bool _useLiveFeed);
    
    void setupFeed();
    void closeFeed();
    void update();
    
    void drawRaw(ofVec2f pos);
    void drawCV(ofVec2f pos, float scale);
    void drawGui(int x, int y);

    
    
    
    //-----Corridor UI-----
    string name;
    string IP;
    ofColor circleCol, circleGrab;
    ofColor backgroundIn, backgroundOut;
    
    //-----Camera Stream-----
    int feedWidth, feedHeight;
    

    ofGstVideoUtils gst;
    ofVideoPlayer movie;
    bool useLiveFeed;
    
    
    int staggerTime;
    bool started;
    unsigned long long numFramesRec;    
    
    
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
    ofPixels fboPix;
    ofPixels blurredPix;
    ofPixels threshPix;
    ofImage thresholdImg;
    
    
    //bool to toggle scaling down 640x512
    //down to 320x256 for faster processing
    bool bScaleDown;
    
    
    
    //-----Gui-----
    CamGui cameraGui;
    bool bShowGui;
    float lastFrameTime;
    int cameraFPS;

    
    
    
};

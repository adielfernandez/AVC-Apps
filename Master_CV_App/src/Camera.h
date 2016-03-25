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
#include "ThreadedCV.hpp"

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
    void setMovieFile(string file);
    
    string fileName;
    
    //-----Corridor UI-----
    string name;
    string IP;
    ofColor circleCol, circleGrab;
    ofColor backgroundIn, backgroundOut;
    
    
    //-----CV && Image Processing-----
    
    /*
     *  CV Workflow
     *  -----------
     *  -Get pixels from camera into texture
     *  -remap the texture over the quad points onto a mesh
     *  -Draw mesh in an FBO
     *  -Get FBO pixels
     *  -Convert to [true] grayscale
     *  -Blur
     *  -Threshold
     *  -Erode
     *  -Dilate
     *  -Find Contours
     *
     */

    
    //-----Camera Stream-----
    int staggerTime;
    bool started;
    unsigned long long numFramesRec;

    int feedWidth, feedHeight;
    int scaledWidth, scaledHeight;
    
    //bool to toggle scaling down 640x512
    //down to 320x256 for faster processing
    bool bScaleDown;
    
    ofGstVideoUtils gst;
    ofVideoPlayer movie;
    bool useLiveFeed;
    
    //raw texture from feed
    ofTexture rawTex;
    
    
    
    //for remapping a subImage from the feed:
    bool bEnableQuadMapping;
    
    vector<ofPoint> imageQuad;
    vector<bool> quadPtMouseLock;
    ofVec2f adjustedQuadOrigin;
    ofVec2f adjustedMouse;
    float mapPtRad;

    //Skewed/keystoned mesh
    ofMesh quadMappedMesh;
    ofTexture quadTex;
    ofFbo fbo;
    ofPixels fboPix;
    
    ofPixels blurredPix;
    ofPixels grayPix;
    ofPixels threshPix;
    ofImage thresholdImg;
    
    //takes in the quad mapped pixels
    //outputs threshold and contours
    ThreadedCV imageProcessor;
    
    //what we'll get from the
    //image processing thread
    ofTexture threshTex;
    ofxCv::ContourFinder contours;

    

    
    
    
    //-----Gui-----
    CamGui cameraGui;
    bool bShowGui;
    float lastFrameTime;
    int cameraFPS;

    
    
    
};

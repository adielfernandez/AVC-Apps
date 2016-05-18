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
#include "BlobFilter.hpp"

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
    void setupViewControl(int _thisView, int *_viewMode, ofVec2f _adjOrigin);
    void setMovieFile(string file);
    void setFont(ofTrueTypeFont *_font);
    
    void saveAllSettings();
    
    void closeFeed();
    void update();
    
    //used by ofApp to show everything
    void drawMain();
    
    //helpers to draw selected bits:

    //raw image (with quad mapping)
    void drawRawWindow(int x, int y);
    
    //Mapped image (with cropping pts)
    void drawCroppingWindow(int x, int y, float scale);
    
    //Mapped image with mask
    void drawMaskingWindow(int x, int y, float scale);
    
    void drawCroppedTex(ofVec2f pos);
    void drawCropSquare();
    
    //draw raw/thresholded & contours
    void drawPostCvWindow(int x, int y, float scale);
    

    
    
    //gui
    void drawGui(int x, int y);
    string fileName;
    
    
    //-----Corridor UI-----
    string name;
    string IP;
    ofColor circleCol, circleGrab;
    ofColor mappingCol, croppingCol, maskingCol;
    ofColor backgroundInCol, backgroundOutCol;
    ofTrueTypeFont *font;
    
    //pointer to the current view in the ofApp
    //need to keep track of when we're active
    //so we only listen to the mouse when needed
    int *viewMode;
    int thisView;
    
    //decides if we're tweaking the
    //soloCam: quad mapping (0) or cropped sub-image (1)
    int manipulationMode;

    
    
    
    
    //-----CV && Image Processing-----
    
    /*
     *  CV Workflow
     *  -----------
     *  -Get pixels from camera into texture
     *  -remap the texture over the quad points onto a mesh
     *  -Draw mesh in an FBO
     *  -Get FBO pixels
     *  -Convert to [true] grayscale
     *  -Crop a subImage 
     *      (only if we're an aggregated camera)
     *  -Blur
     *  -Threshold
     *  -Erode
     *  -Dilate
     *  -Find Contours
     *
     */

    
    //-----Camera Stream-----
    int staggerTime;
    bool pipelineStarted, pipelineSetup;
    bool firstUpdate;
    unsigned long long pipelineSetTime;
    int gstWaitToStart;
    unsigned long long lastFrameTime;
    unsigned long long numFramesRec;
    int cameraFPS;
    int lastCamFPS;
    int avgCamFPS;
    
    const int feedWidth = 640;
    const int feedHeight = 512;
    
    int scaledWidth, scaledHeight;
    int croppedWidth, croppedHeight;
    
    //bool to toggle scaling down 640x512
    //down to 320x256 for faster processing
    bool bScaleDown;
    
    //If camera is solo in corridor
    //No need for cropping
    //If not solo, no need for contours
    bool soloCam;
    
    ofGstVideoUtils gst;
    ofVideoPlayer movie;
    bool useLiveFeed;
    
    //raw texture from feed
    ofTexture rawTex;
    
    
    
    //for remapping a subImage from the feed:
    int numMapPts;
    vector<ofPoint> imageMapPts;
    vector<bool> mapPtMouseLock;
    
    //for cropping a portion of the image
    //just two points: upper left and lower right
    ofPoint cropStart, cropEnd;
    bool cropStartLock, cropEndLock;
    
    
    //This holds the upper left corner
    //of the content area where the camera/quad
    //is drawn on screen.
    ofVec2f adjustedOrigin;
    int titleSpace;
    
    //This holds the adjusted mouse so clicks
    //still make sense in the quad's reference frame
    ofVec2f adjustedMouse;
    float mapPtRad;
    
    //mouse stuff for masking
    int maskToolSize;
    bool maskPressed;
    string maskFileName;
    

    //Masking stuff
    ofPixels maskPix;
    ofImage maskImg;
    bool maskChanged;
    ofVec2f maskBoundStart, maskBoundEnd;

    //Skewed/keystoned mesh
    ofMesh mappedMesh;
    ofTexture mappedTex;
    ofFbo fbo;
    ofPixels fboPix;
    
    //for drawing the image to screen
    ofImage threadOutputImg;
    ofPixels threshPix, blurredPix;
    
    //takes in the mapped pixels
    //outputs threshold and contours
    ThreadedCV imageProcessor;
    
    //what we'll get from the
    //image processing thread
    ofPixels threadOutput;
    ofxCv::ContourFinder contours;
    
    
    //processed blob
    BlobFilter filteredContours;

    //-----OSC Data gathering-----
    void gatherOscStats();
    
    //Blob Stats
    ofVec2f avgDir;
    ofVec2f avgPos;
    ofVec2f avgVel;
    float avgSpeed;
    float density;
    
    //osc bundles to prepare
    ofxOscMessage corridorStats;
    ofxOscMessage corridorStartFlag;
    ofxOscBundle corridorBundle;
    ofxOscBundle blobsBundle;
    
    
    //-----Gui-----
    CamGui cameraGui;
    bool bShowGui;


    //Blackout shapes
    
    
};

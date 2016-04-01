//
//  PlaybackBar.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/30/16.
//
//

#ifndef PlaybackBar_hpp
#define PlaybackBar_hpp

#include <stdio.h>

#endif /* PlaybackBar_hpp */


#include "ofMain.h"
#include "Camera.h"
#pragma once

class PlaybackBar{
    
public:
   
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void mouseEntered(ofMouseEventArgs & args);
    void mouseExited(ofMouseEventArgs & args);
    
    //=========================
    
    
    PlaybackBar();
    
    void setup(ofVec2f _pos, vector<shared_ptr<Camera>> _cams, ofTrueTypeFont *_font);
    void update();
    void draw();
    
    void setAllMoviePosition(float pct);
    void setAllPause(bool pause);

    //control
    vector<shared_ptr<Camera>> cams;
    
    float moviePct;
    
    bool mouseAttach;  //handle control
    bool isPlaying;
    
    
    
    ofTrueTypeFont *font;
    
    
    //background (everything based on bgWidth)
    ofVec2f pos;
    float bgHeight = 60;
    float bgWidth;

    //line followed by playhead
    ofVec2f linePos;
    float lineWidth, lineLength;

    
    //handle
    ofVec2f handlePos;
    float handleWidth, handleHeight;
    ofColor handleCol, clickCol, restCol;
    
    //title
    int titleHeight;
    int playheadHeight;
    float lineMargin;
    
    //play button
    ofVec2f playPos;
    float playButtonWidth, playButtonHeight;
    ofColor playingCol, pausedCol, playButtonCol;
    string playText;
    
    
};
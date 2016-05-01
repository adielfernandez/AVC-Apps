//
//  PlaybackBar.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/30/16.
//
//

#include "PlaybackBar.hpp"



PlaybackBar::PlaybackBar(){
    
    
}


void PlaybackBar::setup(ofVec2f _pos, const vector<shared_ptr<Camera>> _cams, ofTrueTypeFont *_font){
    
    //pointer to the cameras vector
    cams = _cams;
    
    //pointer to a font
    font = _font;
    
    pos = _pos;
    
    bgWidth = 600;
    
    lineMargin = 10;
    
    
    //playback bar height: first 20px for title, next 30px for playhead
    titleHeight = 30;
    playheadHeight = 30;
    linePos.set(pos.x + lineMargin, pos.y + titleHeight + playheadHeight/2);
    lineLength = bgWidth - lineMargin * 2;
    lineWidth = 5;
    
    int playButtonMargin = 2;
    playButtonHeight = titleHeight - playButtonMargin * 2;
    playButtonWidth = 90;
    playPos.set(pos.x + bgWidth - playButtonMargin - playButtonWidth, pos.y + playButtonMargin);
    playText = "Playing";
    
    playingCol.set(0, 120, 255);
    pausedCol.set(255, 0, 0);
    playButtonCol.set(playingCol);
    
    handlePos.set(linePos);
    handleHeight = 30;
    handleWidth = 8;
    
    restCol.set(0, 120, 255);
    clickCol.set(255, 0, 0);
    handleCol.set(restCol);

    
    mouseAttach = false;
    isPlaying = true;
    
}

void PlaybackBar::update(){
    
    float handleX;
    
    
    //if the handle is attached to the mouse, move the handle accordingly
    if(mouseAttach){
        
        handleX = CLAMP(ofGetMouseX(), linePos.x, linePos.x + lineLength);
        handleCol = clickCol;
        
    } else {
        
        //if not attached to mouse move depending on movie position
        moviePct = cams[0] -> movie.getPosition();
        handleX = ofMap(moviePct, 0.0, 1.0, linePos.x, linePos.x + lineLength);
        handleCol = restCol;
    }
    
    handlePos.x = handleX;
    
    ofRegisterMouseEvents(this);
    
    if(!isPlaying){
        handleCol = clickCol;
    }
    
    
    
}

void PlaybackBar::draw(){
    
    //background
    ofSetColor(255, 150);
    ofDrawRectangle(pos, bgWidth, bgHeight);
    //title background
    ofSetColor(0);
    ofDrawRectangle(pos, bgWidth, titleHeight);
    
    //title
    ofSetColor(255);
    string title = "Playback Control";
    font -> drawString(title, pos.x + 5, pos.y + font -> stringHeight(title) + 3);
    
    //play button
    ofSetColor(playButtonCol);
    ofDrawRectangle(playPos, playButtonWidth, playButtonHeight);
    
    ofSetColor(255);
    font -> drawString(playText, playPos.x + playButtonWidth/2 - font -> stringWidth(playText)/2, pos.y + font -> stringHeight("Playing") + 3);    //hard code the height so it doesnt change w/o descenders
    
    
    //line
    ofSetColor(0);
    ofSetLineWidth(lineWidth);
    ofDrawLine(linePos, linePos + ofVec2f(lineLength, 0));

    //handle
    ofSetColor(handleCol);
    ofDrawRectangle(handlePos.x - handleWidth/2, handlePos.y - handleHeight/2, handleWidth, handleHeight);

    //handle outline
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(0);
    ofDrawRectangle(handlePos.x - handleWidth/2, handlePos.y - handleHeight/2, handleWidth, handleHeight);
    ofFill();
    
    
}


void PlaybackBar::setAllMoviePosition(float pct){
    
    for(int i = 0; i < cams.size(); i++){
        
        cams[i] -> movie.setPosition(pct);
        
    }
    
}

void PlaybackBar::setAllPause(bool pause){
    
    for(int i = 0; i < cams.size(); i++){
        
        cams[i] -> movie.setPaused(pause);
        
    }
    
}



void PlaybackBar::mousePressed(ofMouseEventArgs &args){
    
    if(args.x > handlePos.x - handleWidth && args.x < handlePos.x + handleWidth && args.y > handlePos.y - handleHeight && args.y < handlePos.y + handleHeight){
        
        mouseAttach = true;
        
        
    }
    
    
    if(args.x > playPos.x && args.x < playPos.x + playButtonWidth && args.y > playPos.y && args.y < playPos.y + playButtonHeight){
        
        isPlaying = !isPlaying;
        
        if(isPlaying){
            playText = "Playing";
            playButtonCol = playingCol;
            setAllPause(false);
        } else {
            playText = "Paused";
            playButtonCol = pausedCol;
            setAllPause(true);
        }

        
        
    }
    

    
}

void PlaybackBar::mouseReleased(ofMouseEventArgs &args){

    
    if(mouseAttach){
        mouseAttach = false;
        
        //get the pct from the position of the handle
        float pct = ofMap(args.x, linePos.x, linePos.x + lineLength, 0.0, 1.0, true);
        
        setAllMoviePosition(pct);
    }
}

void PlaybackBar::mouseMoved(ofMouseEventArgs &args){}
void PlaybackBar::mouseDragged(ofMouseEventArgs & args){}
void PlaybackBar::mouseScrolled(ofMouseEventArgs & args){}
void PlaybackBar::mouseEntered(ofMouseEventArgs & args){}
void PlaybackBar::mouseExited(ofMouseEventArgs & args){}

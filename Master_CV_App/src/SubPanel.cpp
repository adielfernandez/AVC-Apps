//
//  SubPanel.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/25/16.
//
//

#include "SubPanel.hpp"


SubPanel::SubPanel(){
    
}



void SubPanel::setup(ofVec2f _pos){
    
    pos = _pos;
    
    
    isActive = false;
    

    
    
}

void SubPanel::setFont(ofTrueTypeFont *_font){
    
    font = _font;
    
}

void SubPanel::update(){
    
    for(int i = 0; i < buttons.size(); i++){
        buttons[i].update();
    }
    
}

void SubPanel::draw(){
    
    if(isActive){
        ofSetColor(0);
    } else {
        ofSetColor(20);
    }
    ofDrawRectangle(pos, width, height);
    
//    ofNoFill();
//    ofSetColor(80);
//    ofDrawRectangle(pos, width, height);
//    ofFill();
    
    

    ofSetColor(255);
    font -> drawString(title, pos.x + margin, pos.y + margin + font -> stringHeight(title));
    
    for(int i = 0; i < buttons.size(); i++){
        buttons[i].draw();
    }
    
    
}

bool SubPanel::isInside(int x, int y){
    
    if(x > pos.x && x < pos.x + width && y > pos.y && y < pos.y + height){
        return true;
    } else {
        return false;
    }
    
}

void SubPanel::setButtonsInactive(){
    
    
    for(int i = 0; i < buttons.size(); i++){
        
        buttons[i].isActive = false;
        
    }
    
}







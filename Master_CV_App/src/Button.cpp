//
//  Button.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/25/16.
//
//

#include "Button.hpp"


Button::Button(){
    
    
    
    
    
}

void Button::setup(string _name, int _dest, ofVec2f _pos){
    
    name = _name;
    clickDest = _dest;
    pos = _pos;
    

    

    //width and height set by NavPanel
//    width = 90;
//    height = 30;
    
    
    inactiveCol.set(255);
    activeCol.set(0, 120, 255);
    
    currentRectCol.set(inactiveCol);
    
    textCol.set(0);
    
    isActive = false;
    
}

void Button::setFont(ofTrueTypeFont *_font){
    font = _font;
    
    //now that we have a font, calculate the string
    //width and height for later
    strW = font -> stringWidth(name);
    strH = font -> stringHeight(name);
    
}

void Button::update(){
    
    if(isActive){
        currentRectCol = activeCol;
    } else {
        currentRectCol = inactiveCol;
    }
    
    
    
    
}

void Button::draw(){
    
    ofSetColor(currentRectCol);
    ofDrawRectangle(pos, width, height);
    
    ofSetColor(textCol);
    
    //Because the g's make the text look off center
    float fontDescenderHack;
    if(name == "Aggregate"){
        fontDescenderHack = 3;
    } else {
        fontDescenderHack = 0;
    }
    font -> drawString(name, pos.x + (width - strW)/2, pos.y + height/2 + strH/2 - fontDescenderHack);
    
}

bool Button::isInside(int x, int y){
    
    if(x > pos.x && x < pos.x + width && y > pos.y && y < pos.y + height){
        return true;
    } else {
        return false;
    }

}








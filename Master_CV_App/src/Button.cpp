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

void Button::setup(string _name, int _dest, ofVec2f _pos, ofTrueTypeFont *_font){
    
    name = _name;
    clickDest = _dest;
    pos = _pos;
    font = _font;
    
    //width and height set by NavPanel
//    width = 90;
//    height = 30;
    
    
    inactiveCol.set(255);
    activeCol.set(0, 120, 255);
    
    currentRectCol.set(inactiveCol);
    
    textCol.set(0);
    
    isActive = false;
    
    
    //calculate the string width and height for later
    int strW = font -> stringWidth(name);
    int strH = font -> stringHeight(name);
    
    textPos.set(pos.x + (width - strW)/2, pos.y + height/2 + strH/2);
    
    //tweak to raise the text position if the string contains
    //any letters with descenders
    std::size_t found = name.find_first_of("gjpqy");
    
    
    //if we find any descenders, add a little to the textPos
    if(found != std::string::npos){
        
        textPos.y -= 3;
        
    }
    
    
    
    
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
    
    font -> drawString(name, textPos.x, textPos.y);
    
}

bool Button::isInside(int x, int y){
    
    if(x > pos.x && x < pos.x + width && y > pos.y && y < pos.y + height){
        return true;
    } else {
        return false;
    }

}








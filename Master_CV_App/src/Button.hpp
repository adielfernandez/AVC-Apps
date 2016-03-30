//
//  Button.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/25/16.
//
//

#ifndef Button_hpp
#define Button_hpp

#include <stdio.h>

#endif /* Button_hpp */


#include "ofMain.h"
#pragma once


class Button{
    
public:
    

    
    //=========================
    
    Button();
    
    void setup(string _name, int _dest, ofVec2f _pos);
    void setFont(ofTrueTypeFont *_font);
    void update();
    void draw();
    
    bool isInside(int x, int y);
    
    int clickDest;
    int viewMode;
    int width, height;
    int strW, strH;
    
    ofVec2f pos;
    
    ofTrueTypeFont *font;
    
    string name;
    
    bool isActive;
    
    ofColor activeCol, inactiveCol, currentRectCol;
    ofColor textCol;
    
    
    
    
    
    
};
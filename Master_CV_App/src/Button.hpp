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
    
    void setup(string _name, int _dest, ofVec2f _pos, ofTrueTypeFont *_font);
    void update();
    void draw();
    
    bool isInside(int x, int y);
    
    int clickDest;
    int width, height;
    
    ofVec2f pos;
    ofVec2f textPos;
    
    ofTrueTypeFont *font;
    
    string name;
    
    bool isActive;
    
    ofColor activeCol, inactiveCol, currentRectCol;
    ofColor textCol;
    ofColor outlineCol;
    float outlineThickness;
    
    
    
    
    
    
};
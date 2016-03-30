//
//  SubPanel.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/25/16.
//
//

#ifndef SubPanel_hpp
#define SubPanel_hpp

#include <stdio.h>

#endif /* SubPanel_hpp */

#include "Button.hpp"
#include "ofMain.h"


class SubPanel{
    
public:
    
    SubPanel();
    
    void setup(ofVec2f _pos);
    void setFont(ofTrueTypeFont *_font);
    void update();
    void draw();
    bool isInside(int x, int y);
    
    //pointer to the viewMode variable
    //in ofApp that cycles the views
    int *viewChanger;
    
    ofTrueTypeFont *font;
    
    string title;
    
    ofVec2f pos;
    
    int margin;
    int width, height;
    int buttonWidth, buttonHeight;
    
    bool isActive;
    
    vector<Button> buttons;
    
};
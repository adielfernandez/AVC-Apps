//
//  NavPanel.hpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/25/16.
//
//

#ifndef NavPanel_hpp
#define NavPanel_hpp

#include <stdio.h>

#endif /* NavPanel_hpp */

#include "Button.hpp"
#include "SubPanel.hpp"
#include "ofMain.h"
#include "Camera.h"

#pragma once


class NavPanel{
    
public:
    
    NavPanel();
    
    void setup(int *_viewMode, vector<shared_ptr<Camera>> _cams);
    void update();
    void draw();
    
    void checkForClicks(int x, int y);
    void setAllInactive();
    
    //pointer to the viewMode variable
    //in ofApp that cycles the views
    int *viewMode;
    vector<shared_ptr<Camera>> cams;
    
    
    ofTrueTypeFont font;
    
    
    ofVec2f pos;
    
    float margin;
    float buttonWidth, buttonHeight;
    float width, height;
    
    int numSubPanels;
    vector<SubPanel> subPanels;
    SubPanel manipulationPanel;    
    
};
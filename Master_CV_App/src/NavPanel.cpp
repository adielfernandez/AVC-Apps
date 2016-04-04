//
//  NavPanel.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 3/25/16.
//
//

#include "NavPanel.hpp"


NavPanel::NavPanel(){
    
}



void NavPanel::setup(int *_viewMode, vector<shared_ptr<Camera>> _cams){

    viewMode = _viewMode;
    cams = _cams;
    
    
    font.load("fonts/Aller_Rg.ttf", 12);
    
    
    //corridor 1 and 6 panels are 3 buttons wide (Width = 3 buttons + 4 margins)
    //corridors 2-5 and OSC panel are 1 button wide (Width = 1 button + 2 margins)

    // B = buttonWidth;
    // M = margin;
    // windowWidth = 2 * ( B*3 + M*4 ) + 5 * ( B + M*2 )
    //             = 6*B + M*8 + 5*B + 10*M
    //             = 11 * B + 18 * M
    //
    // ==> B = (ofGetWidth() - 18 * M)/11

    margin = 10;
    buttonWidth = ((float)ofGetWidth() - 18 * margin)/11;
    buttonHeight = font.stringHeight("A") + 10;
    
    //spacing between the top of the
    //subpanel and the first button's y pos
    float buttonStartY = 35;
    
    //position of entire NavPanel
    pos = ofVec2f(0, ofGetHeight() - (buttonHeight + margin) * 3 - buttonStartY);
    
    int leftSide = 0;
    
    numSubPanels = 7;
    for(int i = 0; i < numSubPanels; i++){
        
        SubPanel s;
        s.setup(ofVec2f(0));
        
        s.margin = margin;
        s.buttonWidth = buttonWidth;
        s.buttonHeight = buttonHeight;
        

        
        if(i == 0 || i == 5){
            s.width = buttonWidth * 3 + margin * 4;
        } else {
            s.width = buttonWidth + margin * 2;
        }
        
        if(i < 6){
            s.title = "Corridor " + ofToString(i + 1);
        } else {
            s.title = "Other";
        }
        
        
        s.height = ofGetHeight() - pos.y;
        
        s.pos.set(leftSide, pos.y);
        leftSide += s.width;
        
        s.setFont(&font);
        
        subPanels.push_back(s);
        
    }
    
    
    //now go through the subpanels and add buttons
    
    //7 buttons in corridor 1
    for(int i = 0; i < 7; i++){
        
        Button b;
        
        string _name;
        int _destination;
        ofVec2f p;
        p = subPanels[0].pos;
        
        //last button is aggregate button
        //but is placed at top
        if(i == 6){
            _name = "Aggregate";
            _destination = 14;
            
            b.width = buttonWidth * 3 + margin * 2;
            b.height = buttonHeight;
            b.setup(_name, _destination, ofVec2f(p.x + margin, p.y + buttonStartY), &font);
            //first aggregate button is extra wide
            
        } else {
            
            //next 6 buttons are the individual cameras
            
            _name = "Cam-" + ofToString(i + 1);
            _destination = i;
            
            float x, y;
            x = p.x + margin + (i % 3) * (buttonWidth + margin);
            y = p.y + buttonStartY + ( i < 3 ? 1 : 2 ) * (buttonHeight + margin);
            
            
            b.width = buttonWidth;
            b.height = buttonHeight;
            b.setup(_name, _destination, ofVec2f(x, y), &font);
            
            
        }
        
        subPanels[0].buttons.push_back(b);
        
        
    }
    
    //just manually turn on the first subpanel and button to active
    subPanels[0].buttons[0].isActive = true;
    subPanels[0].isActive = true;
    
    //one button in corridors 2, 3, 4, and 5
    //(i.e. subPanels 1, 2, 3, 4)
    //(and cameras 7, 8, 9, 10
    for(int i = 1; i <= 4; i++){
        
        Button b;
        
        string _name;
        int _destination;
        ofVec2f p;
        p = subPanels[i].pos;

        
        _name = "Cam-" + ofToString(i + 6);
        _destination = i + 5; //viewMode for camera is zero indexed, name is not
        
        float x, y;
        x = p.x + margin;
        y = p.y + buttonStartY;
        
        b.width = buttonWidth;
        b.height = buttonHeight;
        b.setup(_name, _destination, ofVec2f(x, y), &font);
        
        
        
        subPanels[i].buttons.push_back(b);
    
    }
    
    //5 buttons in corridor 6
    //cameras 11, 12, 13, 14
    for(int i = 0; i < 5; i++){
        
        Button b;
        
        string _name;
        int _destination;
        ofVec2f p;
        p = subPanels[5].pos;
        
        //last button is aggregate button
        //but is placed at top
        if(i == 4){
            _name = "Aggregate";
            _destination = 15;
            
            b.width = buttonWidth * 3 + margin * 2;
            b.height = buttonHeight;
            b.setup(_name, _destination, ofVec2f(p.x + margin, p.y + buttonStartY), &font);
            //first aggregate button is extra wide
            
        } else {
            
            //next 6 buttons are the individual cameras
            
            _name = "Cam-" + ofToString(i + 11);
            _destination = i + 10;
            
            float x, y;
            x = p.x + margin + (i % 3) * (buttonWidth + margin);
            y = p.y + buttonStartY + ( i < 3 ? 1 : 2 ) * (buttonHeight + margin);
            
            
            b.width = buttonWidth;
            b.height = buttonHeight;
            b.setup(_name, _destination, ofVec2f(x, y), &font);
            
            
        }
        
        subPanels[5].buttons.push_back(b);
        
        
    }
    

    //one button for the last panel to the OSC page
    Button osc;
    

    ofVec2f p;
    p = subPanels[6].pos;
    
    float x, y;
    x = p.x + margin;
    y = p.y + buttonStartY;
    
    osc.width = buttonWidth;
    osc.height = buttonHeight;
    osc.setup("OSC Info", 16, ofVec2f(x, y), &font);
    
    
    Button allCams;
    
    y += buttonHeight + margin;
    
    allCams.width = buttonWidth;
    allCams.height = buttonHeight;
    allCams.setup("All Cams", 17, ofVec2f(x, y), &font);
    
    
    subPanels[6].buttons.push_back(allCams);
    subPanels[6].buttons.push_back(osc);
    
    
    
    //subpanel for each camera's manipulationMode
    manipulationPanel.setup(ofVec2f(0));
    
    manipulationPanel.margin = margin;
    manipulationPanel.buttonWidth = buttonWidth;
    manipulationPanel.buttonHeight = buttonHeight;
    manipulationPanel.width = buttonWidth * 2 + margin * 3;
    manipulationPanel.title = "Image Manipulation";
    manipulationPanel.height = buttonStartY + buttonHeight + margin;
    manipulationPanel.pos.set(ofGetWidth() - manipulationPanel.width - margin, pos.y - manipulationPanel.height - margin );
    manipulationPanel.setFont(&font);
    
    //add two buttons
    Button quadMap;
    Button cropping;
    
    //Quad Mapping: Camera manipulationMode = 0
    quadMap.width = buttonWidth;
    quadMap.height = buttonHeight;
    quadMap.setup("Quad Map", 0, manipulationPanel.pos + ofVec2f(margin, buttonStartY), &font);
    
    //Cropping: Camera manipulationMode = 1
    cropping.width = buttonWidth;
    cropping.height = buttonHeight;
    cropping.setup("Cropping", 1, manipulationPanel.pos + ofVec2f(margin * 2 + buttonWidth, buttonStartY), &font);
    
    manipulationPanel.buttons.push_back(quadMap);
    manipulationPanel.buttons.push_back(cropping);
    manipulationPanel.isActive = true;

}


void NavPanel::update(){
    
    for(int i = 0; i < subPanels.size(); i++){
        subPanels[i].update();
    }
    
    
    //check the manipulationMode variable in the current camera
    //and modify the button visuals accordingly
    if((*viewMode) >= 0 && (*viewMode) <= 13){
        
        if(cams[(*viewMode)] -> manipulationMode == 0){
            manipulationPanel.buttons[0].isActive = true;
            manipulationPanel.buttons[1].isActive = false;
        } else {
            manipulationPanel.buttons[0].isActive = false;
            manipulationPanel.buttons[1].isActive = true;
        }
        
        if(cams[*viewMode] -> soloCam == false){
            manipulationPanel.update();
        }
    }
    
    
    
    
    
    
    
}

void NavPanel::draw(){
    
    int lineGap = 15;
    ofSetLineWidth(2);
    ofFill();
    
    for(int i = 0; i < subPanels.size(); i++){
        subPanels[i].draw();

        //draw dividing lines between them
        ofSetColor(35);
        
        ofDrawLine(subPanels[i].pos.x, subPanels[i].pos.y + lineGap, subPanels[i].pos.x, ofGetHeight() - lineGap);
        
    }
    
    
    if((*viewMode) >= 0 && (*viewMode) <= 13){
        
        if(cams[*viewMode] -> soloCam == false){
            manipulationPanel.draw();
        }
    }
}


void NavPanel::checkForClicks(int x, int y){
    
    
    //go through all the buttons in the navpanel > subpanel > buttons vector
    //and see if we're inside them
    //first the subpanels
    for(int i = 0; i < subPanels.size(); i++){
        
        //if we're in the subPanel, check the buttons
        if(subPanels[i].isInside(x, y)){
            
            //then the buttons
            for(int j = 0; j < subPanels[i].buttons.size(); j++){
                
                //check if we're inside
                if(subPanels[i].buttons[j].isInside(x, y)){
                    
                    
                    //and change the current view
                    *viewMode = subPanels[i].buttons[j].clickDest;
                    
                    
                    //set all panels/buttons to inactive
                    setAllInactive();
                    
                    //then turn the correct ones to active
                    subPanels[i].isActive = true;
                    subPanels[i].buttons[j].isActive = true;
                    
                    //no need to check other buttons
                    break;

                }
            }
            
            //no need to check other subpanels
            break;
        }
    }
    
    //only check the manipulationMode buttons if we're in a camera view
    if((*viewMode) >= 0 && (*viewMode) <= 13){
    
        if(cams[*viewMode] -> soloCam == false){
            //check for clicks in the manipulationMode panel too
            if(manipulationPanel.buttons[0].isInside(x, y)){
                cams[*viewMode] -> manipulationMode = 0;
            }

            if(manipulationPanel.buttons[1].isInside(x, y)){
                cams[*viewMode] -> manipulationMode = 1;
            }
        }
    }
    
    
    
    
}


void NavPanel::setAllInactive(){
    
    //go through all the subPanels and buttons and make them inactive
    for(int i = 0; i < subPanels.size(); i++){
        
        subPanels[i].isActive = false;

        for(int j = 0; j < subPanels[i].buttons.size(); j++){
            subPanels[i].buttons[j].isActive = false;
        }
    }
    
}












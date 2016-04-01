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



void NavPanel::setup(int *_viewModeVar){

    viewChanger = _viewModeVar;
        
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
            
            b.setup(_name, _destination, ofVec2f(p.x + margin, p.y + buttonStartY));
            //first aggregate button is extra wide
            b.width = buttonWidth * 3 + margin * 2;
            b.height = buttonHeight;
            
        } else {
            
            //next 6 buttons are the individual cameras
            
            _name = "Cam-" + ofToString(i + 1);
            _destination = i;
            
            float x, y;
            x = p.x + margin + (i % 3) * (buttonWidth + margin);
            y = p.y + buttonStartY + ( i < 3 ? 1 : 2 ) * (buttonHeight + margin);
            
            
            b.setup(_name, _destination, ofVec2f(x, y));
            
            b.width = buttonWidth;
            b.height = buttonHeight;
            
        }
        b.setFont(&font);
        
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
        
        b.setup(_name, _destination, ofVec2f(x, y));
        
        b.width = buttonWidth;
        b.height = buttonHeight;
        
    
        b.setFont(&font);
        
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
            
            b.setup(_name, _destination, ofVec2f(p.x + margin, p.y + buttonStartY));
            //first aggregate button is extra wide
            b.width = buttonWidth * 3 + margin * 2;
            b.height = buttonHeight;
            
        } else {
            
            //next 6 buttons are the individual cameras
            
            _name = "Cam-" + ofToString(i + 11);
            _destination = i + 10;
            
            float x, y;
            x = p.x + margin + (i % 3) * (buttonWidth + margin);
            y = p.y + buttonStartY + ( i < 3 ? 1 : 2 ) * (buttonHeight + margin);
            
            
            b.setup(_name, _destination, ofVec2f(x, y));
            
            b.width = buttonWidth;
            b.height = buttonHeight;
            
        }
        b.setFont(&font);
        
        subPanels[5].buttons.push_back(b);
        
        
    }
    
    //one button for the last panel to the OSC page
    Button osc;
    

    ofVec2f p;
    p = subPanels[6].pos;
    
    float x, y;
    x = p.x + margin;
    y = p.y + buttonStartY;
    
    osc.setup("OSC Info", 16, ofVec2f(x, y));
    
    osc.width = buttonWidth;
    osc.height = buttonHeight;
    
    
    osc.setFont(&font);
    
    Button allCams;
    
    y += buttonHeight + margin;
    
    allCams.setup("All Cams", 17, ofVec2f(x, y));
    
    allCams.width = buttonWidth;
    allCams.height = buttonHeight;
    
    
    allCams.setFont(&font);
    
    subPanels[6].buttons.push_back(allCams);
    subPanels[6].buttons.push_back(osc);
    

}


void NavPanel::update(){
    
    for(int i = 0; i < subPanels.size(); i++){
        subPanels[i].update();
    }
    
    
}

void NavPanel::draw(){
    
    int lineGap = 15;
    ofSetLineWidth(2);
    
    for(int i = 0; i < subPanels.size(); i++){
        subPanels[i].draw();

        //draw dividing lines between them
        ofSetColor(35);
        
        ofDrawLine(subPanels[i].pos.x, subPanels[i].pos.y + lineGap, subPanels[i].pos.x, ofGetHeight() - lineGap);
        
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
                    *viewChanger = subPanels[i].buttons[j].clickDest;
                    
                    
                    //set all panels/buttons to inactive
                    setAllInactive();
                    
                    //then turn the correct ones to active
                    subPanels[i].isActive = true;
                    subPanels[i].buttons[j].isActive = true;
                    
                    
                    
                } else {
                    
                }
                
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












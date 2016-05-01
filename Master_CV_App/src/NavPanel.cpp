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



void NavPanel::setup(int *_viewMode, vector<shared_ptr<Camera>> _cams, Aggregator *_Lobby1, Aggregator *_Lobby2){

    viewMode = _viewMode;
    cams = _cams;
    Lobby1 = _Lobby1;
    Lobby2 = _Lobby2;
    
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
    height = (buttonHeight + margin) * 3 - buttonStartY;
    pos = ofVec2f(0, ofGetHeight() - (buttonHeight + margin) * 3 - buttonStartY);
    
    int leftSide = 0;
    
    int numSubPanels = 7;
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
    
    ofVec2f p;
    p = subPanels[6].pos;
    
    float x, y;
    x = p.x + margin;
    y = p.y + buttonStartY;

    //blobs button
    Button allBlobs;
    
    allBlobs.width = buttonWidth;
    allBlobs.height = buttonHeight;
    allBlobs.setup("People", 16, ofVec2f(x, y), &font);
    

    //all cams button
    Button allCams;
    
    y += buttonHeight + margin;
    
    allCams.width = buttonWidth;
    allCams.height = buttonHeight;
    allCams.setup("All Cams", 17, ofVec2f(x, y), &font);
    
    
    
    //osc data
    Button osc;
    
    y += buttonHeight + margin;
    
    osc.width = buttonWidth;
    osc.height = buttonHeight;
    osc.setup("OSC Info", 18, ofVec2f(x, y), &font);
    
    
    subPanels[6].buttons.push_back(allBlobs);
    subPanels[6].buttons.push_back(allCams);
    subPanels[6].buttons.push_back(osc);
    
    
    
    //subpanel for each camera's manipulationMode
    //this chooses between mapping the quad and cropping
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
    quadMap.setup("Mapping", 0, manipulationPanel.pos + ofVec2f(margin, buttonStartY), &font);
//    quadMap.outlineCol.set(255, 0, 255);
    quadMap.isActive = true;

    
    //Cropping: Camera manipulationMode = 1
    cropping.width = buttonWidth;
    cropping.height = buttonHeight;
    cropping.setup("Cropping", 1, manipulationPanel.pos + ofVec2f(margin * 2 + buttonWidth, buttonStartY), &font);
//    cropping.outlineCol.set(0, 255, 255);
    
    manipulationPanel.buttons.push_back(quadMap);
    manipulationPanel.buttons.push_back(cropping);
    manipulationPanel.isActive = true;
    
    
    //Subpanel for switching between mapping and masking modes
    //for aggregates and soloCams only
//    maskingPanel.setup(ofVec2f(0));
//    
//    maskingPanel.margin = margin;
//    maskingPanel.buttonWidth = buttonWidth;
//    maskingPanel.buttonHeight = buttonHeight;
//    maskingPanel.width = buttonWidth * 2 + margin * 3;
//    maskingPanel.title = "Map or Mask";
//    maskingPanel.height = buttonStartY + buttonHeight + margin;
//    maskingPanel.pos.set(ofGetWidth() - manipulationPanel.width - margin, pos.y - manipulationPanel.height - margin );
//    maskingPanel.setFont(&font);
//    
//    //add two buttons
//    Button map;
//    Button mask;
//    
//    //Mapping: Camera manipulationMode = 0
//    map.width = buttonWidth;
//    map.height = buttonHeight;
//    map.setup("Mapping", 0, manipulationPanel.pos + ofVec2f(margin, buttonStartY), &font);
//    map.isActive = true;
//    
//    //Masking: Camera manipulationMode = 1
//    mask.width = buttonWidth;
//    mask.height = buttonHeight;
//    mask.setup("Masking", 1, manipulationPanel.pos + ofVec2f(margin * 2 + buttonWidth, buttonStartY), &font);
//    
//    maskingPanel.buttons.push_back(map);
//    maskingPanel.buttons.push_back(mask);
//    maskingPanel.isActive = true;
    
}

void NavPanel::update(){
    
    for(int i = 0; i < subPanels.size(); i++){
    
        subPanels[i].update();
        
    }
    
    
//    maskingPanel.update();
    
    //check the manipulationMode variable in the current camera and Aggregates
    //and modify the button visuals accordingly
    if((*viewMode) >= 0 && (*viewMode) <= 15){
        
        //cameras
        if((*viewMode) <= 13){
            
            if(cams[(*viewMode)] -> manipulationMode == 0){
                
                manipulationPanel.buttons[0].isActive = true;
                manipulationPanel.buttons[1].isActive = false;
                
            } else {
                
                manipulationPanel.buttons[0].isActive = false;
                manipulationPanel.buttons[1].isActive = true;
                
            }
        
            //if we're in a soloCam or Aggregate, button 2 is
            //Masking, else it's cropping
            if(cams[(*viewMode)] -> soloCam){
                manipulationPanel.buttons[1].name = "Masking";
            } else {
                manipulationPanel.buttons[1].name = "Cropping";
            }
            
            //Lobby 1
        } else if((*viewMode) == 14){
            
            if(Lobby1 -> manipulationMode == 0){
                manipulationPanel.buttons[0].isActive = true;
                manipulationPanel.buttons[1].isActive = false;
            } else {
                manipulationPanel.buttons[0].isActive = false;
                manipulationPanel.buttons[1].isActive = true;
            }
            
            manipulationPanel.buttons[1].name = "Masking";
            
            //Lobby 2
        } else {

            if(Lobby2 -> manipulationMode == 0){
                manipulationPanel.buttons[0].isActive = true;
                manipulationPanel.buttons[1].isActive = false;
            } else {
                manipulationPanel.buttons[0].isActive = false;
                manipulationPanel.buttons[1].isActive = true;
            }
            
            manipulationPanel.buttons[1].name = "Masking";
            
        }
        
        
        manipulationPanel.update();

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
    
    //Draw the other subpanels
    if((*viewMode) >= 0 && (*viewMode) <= 15){
        
            manipulationPanel.draw();

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
    
    //only check the manipulationMode buttons if we're in a camera/aggregate view
    if((*viewMode) >= 0 && (*viewMode) <= 15){
        
        for(int i = 0; i < manipulationPanel.buttons.size(); i++){
            
            if(manipulationPanel.buttons[i].isInside(x, y)){
                
                //set all other buttons in panel to inactive
                manipulationPanel.setButtonsInactive();
                
                //then set THIS button to active
                manipulationPanel.buttons[i].isActive = true;
                
                //then set the right mode based on the button
                if((*viewMode) <= 13){
                    cams[*viewMode] -> manipulationMode = manipulationPanel.buttons[i].clickDest;
//                     cout << "Camera Button " + ofToString(i + 1) + " Pressed" << endl;
                
                } else if((*viewMode) == 14){
                    Lobby1 -> manipulationMode = manipulationPanel.buttons[i].clickDest;
//                    cout << "LOBBY 1 Button " + ofToString(i + 1) + " Pressed" << endl;
                } else {
                    Lobby2 -> manipulationMode = manipulationPanel.buttons[i].clickDest;
//                    cout << "LOBBY 2 Button " + ofToString(i + 1) + " Pressed" << endl;
                }
                
                
                //no need to check other buttons
                break;
                
            }
            
        }
            
        
    }
    
    
    
    
}


void NavPanel::setAllInactive(){
    
    //go through all the subPanels and buttons and make them inactive
    for(int i = 0; i < subPanels.size(); i++){
        
        subPanels[i].isActive = false;

        subPanels[i].setButtonsInactive();

    }
    
}












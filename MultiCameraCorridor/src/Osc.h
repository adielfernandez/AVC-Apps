
#ifndef osc_h
#define osc_h

#include <stdio.h>

#include "ofMain.h"
#include "ofxOsc.h"

#pragma once

class Osc {
public:

    Osc();
    
    // Methods
    void setup(string host);
    void sendBlob(int corridor, int uniqueId, ofVec2f pos, ofVec2f vel);
    
    int outPort;
    
    ofxOscSender sender;

    
};
#endif /* osc_h */

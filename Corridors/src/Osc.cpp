
#include "Osc.h"

Osc::Osc(){
    
    
}

void Osc::setup(string host){

    outPort = 54321;

    sender.setup(host, outPort);
    
}

void Osc::send(int corridor, int uniqueId, ofVec2f pos, ofVec2f vel){

    ofxOscMessage m;
    m.setAddress("/blob");
    m.addIntArg(corridor);
    m.addIntArg(uniqueId);
    m.addFloatArg(pos.x);
    m.addFloatArg(pos.y);
    m.addFloatArg(vel.x);
    m.addFloatArg(vel.y);
    sender.sendMessage(m);
    
}
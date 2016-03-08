//
//  Corridor.cpp
//  Corridors
//
//  Created by Adiel Fernandez on 2/26/16.
//
//

#include "Corridor.h"

using namespace ofxCv;


Corridor::Corridor(){
        
}


void Corridor::setupFeed(){
    
    gst.setPipeline("rtspsrc location=rtsp://admin:admin@" + IP + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! queue2 max-size-buffers=2 ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);
    
    gst.startPipeline();
    gst.play();
    
}


void Corridor::closeFeed(){
    gst.close();
}

void Corridor::setup(string _IP, string _name, bool _scaleDown, bool _useLiveFeed){
    
    //-----Camera Stream-----
    feedWidth = 640;
    feedHeight = 512;
    
    bScaleDown = _scaleDown;
    useLiveFeed = _useLiveFeed;
    
    IP = _IP;
    
    if(useLiveFeed){
    
        setupFeed();

        
    } else {
        movie.load("movies/flir_gray.mov");
        movie.setLoopState(OF_LOOP_NORMAL);
        movie.setPosition(ofRandom(movie.getTotalNumFrames())/movie.getTotalNumFrames());
        movie.play();
        
    }
    
    
    
    
    
    
    
    
    //-----Corridor UI-----
    name = _name;
    
    circleCol.set(255, 0, 255);
    circleGrab.set(0, 255, 0);
    
    
    //-----CV Objects && Image Processing-----

    
    bEnableQuadMapping = false;
    
    //initialize imageQuad vector with 4 points
    //this will be overwritten by xml settings later
    imageQuad.resize(4);
    imageQuad[0].set(0, 0);
    imageQuad[1].set(feedWidth, 0);
    imageQuad[2].set(feedWidth, feedHeight);
    imageQuad[3].set(0, feedHeight);
    
    //size of quad handles
    mapPtRad = 10;
    
    //bools we need to check if we're grabbing the
    //imageQuad points for texture mapping
    quadPtMouseLock.resize(4);
    for(int i = 0; i < quadPtMouseLock.size(); i++){
        quadPtMouseLock[i] = false;
    }
    
    //Make the mesh the same size as the normal feed frame
    //Re-use the imageQuad points since they are still rectangular at this point
    quadMappedMesh.addVertex(imageQuad[0]);
    quadMappedMesh.addVertex(imageQuad[1]);
    quadMappedMesh.addVertex(imageQuad[2]);
    quadMappedMesh.addVertex(imageQuad[3]);
    quadMappedMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    rawTex.allocate(feedWidth, feedHeight, GL_LUMINANCE);
    
    if(bScaleDown){
        
        fbo.allocate(feedWidth/2, feedHeight/2);
        
        subTex.allocate(feedWidth/2, feedHeight/2, GL_LUMINANCE);
        
        grayPix.allocate(feedWidth/2, feedHeight/2, 1);
        blurredPix.allocate(feedWidth/2, feedHeight/2, 1);
        threshPix.allocate(feedWidth/2, feedHeight/2, 1);
        thresholdImg.allocate(feedWidth/2, feedHeight/2, OF_IMAGE_GRAYSCALE);
        
        
    } else {
        fbo.allocate(feedWidth, feedHeight);
        
        subTex.allocate(feedWidth, feedHeight, GL_LUMINANCE);
        
        grayPix.allocate(feedWidth, feedHeight, 1);
        blurredPix.allocate(feedWidth, feedHeight, 1);
        threshPix.allocate(feedWidth, feedHeight, 1);
        thresholdImg.allocate(feedWidth, feedHeight, OF_IMAGE_GRAYSCALE);
    }

    
    //allocate ofPixel objects, all have 1 channel because grayscale
    
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
    
    //-----Gui-----
    corridorGui.setup(name);
    corridorGui.load(name + ".xml");
    
    
}



void Corridor::update(){
    
    
    imageQuad[0].set(corridorGui.quadPt0 -> x, corridorGui.quadPt0 -> y);
    imageQuad[1].set(corridorGui.quadPt1 -> x, corridorGui.quadPt1 -> y);
    imageQuad[2].set(corridorGui.quadPt2 -> x, corridorGui.quadPt2 -> y);
    imageQuad[3].set(corridorGui.quadPt3 -> x, corridorGui.quadPt3 -> y);
    

    
    //handle mouse interaction with map pts
    adjustedMouse.set(ofGetMouseX() - adjustedQuadOrigin.x, ofGetMouseY() - adjustedQuadOrigin.y);
    
    for(int i = 0; i < quadPtMouseLock.size(); i++){
        
        if(quadPtMouseLock[i]){
            imageQuad[i].x = ofClamp(adjustedMouse.x, 0, feedWidth);
            imageQuad[i].y = ofClamp(adjustedMouse.y, 0, feedHeight);
            
            //Update the gui so the values are stored when we save
            if(i == 0){
                
                corridorGui.quadPt0 = imageQuad[i];
                
            } else if(i == 1){
                
                corridorGui.quadPt1 = imageQuad[i];
                
            } else if(i == 2){
                
                corridorGui.quadPt2 = imageQuad[i];
                
            } else {
                
                corridorGui.quadPt3 = imageQuad[i];

            }
            
            
        }
        
    }
    
    
    
    //update the feed
    
    if(useLiveFeed){
        gst.update();
    } else {
        movie.update();
    }
    
    
    //get image from gst pipeline
    if(movie.isFrameNew() || gst.isFrameNew()){
        
        cameraFPS = (int)(1/(ofGetElapsedTimef() - lastFrameTime));
        lastFrameTime = ofGetElapsedTimef();
        
        //get texture from feed
        if(useLiveFeed){
            rawTex.loadData(gst.getPixels());
        } else {
            rawTex.loadData(movie.getPixels());  
        }
        
        
        //clear out the old mesh and remap the texture
        //to the control points we've set
        quadMappedMesh.clearTexCoords();
        quadMappedMesh.addTexCoord(imageQuad[0]);
        quadMappedMesh.addTexCoord(imageQuad[1]);
        quadMappedMesh.addTexCoord(imageQuad[2]);
        quadMappedMesh.addTexCoord(imageQuad[3]);
    
        //draw the mesh with the re-mapped texture to the FBO
        fbo.begin();
        rawTex.bind();
        ofSetColor(255);
        
        if(bScaleDown) ofScale(0.5, 0.5);
        
        quadMappedMesh.draw();
        rawTex.unbind();
        fbo.end();
        
        //steal the new texture from the FBO
        subTex = fbo.getTexture();
        
        
        //get the pixels from the fbo
        fbo.readToPixels(grayPix);
        
        //blur it
        GaussianBlur(grayPix, blurredPix, corridorGui.blurAmountSlider);
        
        //threshold it
        threshold(blurredPix, threshPix, corridorGui.thresholdSlider);
        
        //ERODE
        for(int e = 0; e < corridorGui.numErosionsSlider; e++){
            erode(threshPix);
        }
        
        //DILATE
        for(int d = 0; d < corridorGui.numDilationsSlider; d++){
            dilate(threshPix);
        }
        
        contours.setMinArea(corridorGui.minBlobAreaSlider);
        contours.setMaxArea(corridorGui.maxBlobAreaSlider);
        contours.setThreshold(254);  //only detect white
        
        // wait for half a frame before forgetting something
        contours.getTracker().setPersistence(corridorGui.persistenceSlider);
        
        // an object can move up to 32 pixels per frame
        contours.getTracker().setMaximumDistance(corridorGui.maxDistanceSlider);
        
        contours.findContours(threshPix);
        
        
    }
    
    
    //for mouse interaction
    if(bEnableQuadMapping){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }
    
}



void Corridor::drawGui(int x, int y){
    corridorGui.draw(x, y);
    


    
}


void Corridor::drawRaw(ofVec2f pos){
    
    //WARNING: certain things can be drawn with matrix translations
    // but others need to be drawn raw since they depend on mouse interaction
    
    
    ofPushMatrix();
    ofTranslate(pos);
    
    
    ofSetColor(255);
    rawTex.draw(0, 0);
    
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, 0, feedWidth, feedHeight);
    
    //draw lines between points
    ofPolyline quadMap(imageQuad);
    quadMap.close();
    
    ofSetLineWidth(1);
    
    ofSetColor(circleCol);
    quadMap.draw();
    
    //draw mapping points
    ofPushStyle();
    for(int i = 0; i < imageQuad.size(); i++){
        
        ofColor c;
        
        if(quadPtMouseLock[i]){
            c = circleGrab;
        } else {
            c = circleCol;
        }
        
        ofNoFill();
        ofSetColor(c);
        ofDrawCircle(imageQuad[i], mapPtRad);
        ofSetColor(255);
        ofDrawBitmapString(ofToString(i), imageQuad[i].x + mapPtRad, imageQuad[i].y - mapPtRad);
        
    }
    
    ofPopStyle();

    
    ofPopMatrix();
    
    
}

void Corridor::drawCV(ofVec2f pos, float scale){
    
    ofPushMatrix();
    
    ofTranslate(pos);
    ofScale(scale, scale);
    
    ofSetColor(255);
    ofNoFill();
    ofSetLineWidth(1);
    
    ofDrawRectangle(0, 0, subTex.getWidth(), subTex.getHeight());

    
    ofSetColor(255);
    subTex.draw(0, 0);
    
    if(corridorGui.drawThresholdToggle){
        
        thresholdImg.setFromPixels(threshPix);
        
        thresholdImg.draw(0, 0);
        
    }
    
    if(corridorGui.drawContoursToggle) {
        RectTracker& tracker = contours.getTracker();
        
        ofSetColor(255, 0, 0);
        
        contours.draw();
        
        for(int i = 0; i < contours.size(); i++) {
            ofPoint center = toOf(contours.getCenter(i));
            ofPushMatrix();
            ofTranslate(center.x, center.y);
            int label = contours.getLabel(i);
            ofVec2f velocity = toOf(contours.getVelocity(i));
            
            string msg = ofToString(label);
            
            if(corridorGui.showInfoToggle){
                msg = ofToString(label) + " : " + ofToString(center.x) + "," + ofToString(center.y) + " : " + ofToString(velocity.x) + "," + ofToString(velocity.y);
            }
            
            ofSetColor(0, 100, 255);
            ofDrawBitmapString(msg, 0, 0);
        }
    }

    ofPopMatrix();
    
    
    
}


void Corridor::mousePressed(ofMouseEventArgs &args){

    cout << "Mouse Pressed" << endl;
    
    //handle mouse interaction with quadMap points
    for(int i = 0; i < imageQuad.size(); i++){
        
        //need to adjust mouse position since we're
        //translating the raw camera view from the origin with pushmatrix
        float dist = ofDist(adjustedMouse.x, adjustedMouse.y, imageQuad[i].x, imageQuad[i].y);
        
        if(dist < mapPtRad){
            quadPtMouseLock[i] = true;
        } else {
            quadPtMouseLock[i] = false;
        }
        
        
    }
    
}

void Corridor::mouseReleased(ofMouseEventArgs &args){
    
    for(int i = 0; i < quadPtMouseLock.size(); i++){
        quadPtMouseLock[i] = false;
    }
    
}

void Corridor::mouseMoved(ofMouseEventArgs &args){}
void Corridor::mouseDragged(ofMouseEventArgs & args){}
void Corridor::mouseScrolled(ofMouseEventArgs & args){}
void Corridor::mouseEntered(ofMouseEventArgs & args){}
void Corridor::mouseExited(ofMouseEventArgs & args){}

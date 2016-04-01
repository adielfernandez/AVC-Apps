//
//  Camera.cpp
//  Corridors
//
//  Created by Adiel Fernandez on 2/26/16.
//
//

#include "Camera.h"

using namespace ofxCv;


Camera::Camera(){
        
}


void Camera::setupFeed(){

    gst.setPipeline("rtspsrc location=rtsp://admin:admin@" + IP + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! rtpjpegdepay ! jpegdec !  queue ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);
    
    gst.startPipeline();
    gst.play();
    
}

void Camera::setMovieFile(string file){
    
    fileName = file;
    
}


void Camera::closeFeed(){

    gst.close();
}

void Camera::setup(string _IP, string _name, bool _scaleDown, bool _useLiveFeed){
    
    //-----Camera Stream-----
    feedWidth = 640;
    feedHeight = 512;
    
    bScaleDown = _scaleDown;
    useLiveFeed = _useLiveFeed;
    
    
    if(bScaleDown){
        scaledWidth = feedWidth/2;
        scaledHeight = feedHeight/2;
    } else {
        scaledWidth = feedWidth;
        scaledHeight = feedHeight;
    }
    
    
    
    
    //video or feed?
    if(useLiveFeed){
    
        //Dont start feed yet, we'll start them later
        //but staggered
        
//        setupFeed();

        
    } else {
        
        movie.load(fileName);
        movie.setLoopState(OF_LOOP_NORMAL);
        movie.play();
        movie.setPosition(ofRandom(movie.getTotalNumFrames())/movie.getTotalNumFrames());
        
    }
    
    
    
    //-----Corridor UI-----
    name = _name;
    IP = _IP;
    
    circleCol.set(255, 0, 255);
    circleGrab.set(0, 255, 0);
    
    //size of quad handles
    mapPtRad = 10;
    
    
    
    //-----CV Objects && Image Processing-----

    //enable/disable so we don't read mouse when we're on another screen
    bEnableQuadMapping = false;
    
    //initialize imageQuad vector with 4 points
    //this will be overwritten by xml settings later
    imageQuad.resize(4);
    imageQuad[0].set(0, 0);
    imageQuad[1].set(feedWidth, 0);
    imageQuad[2].set(feedWidth, feedHeight);
    imageQuad[3].set(0, feedHeight);
    
    
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

    
    //allocate all the stuff we'll be using
    //Textures
    rawTex.allocate(feedWidth, feedHeight, GL_LUMINANCE);
    quadTex.allocate(scaledWidth, scaledHeight, GL_LUMINANCE);
    threshTex.allocate(scaledWidth, scaledHeight, GL_LUMINANCE);
    
    blurredPix.allocate(scaledWidth, scaledHeight, 1);
    grayPix.allocate(scaledWidth, scaledHeight, 1);
    threshPix.allocate(scaledWidth, scaledHeight, 1);
    thresholdImg.allocate(scaledWidth, scaledHeight, OF_IMAGE_GRAYSCALE);
    
    //FBO
    fbo.allocate(scaledWidth, scaledHeight);
    //(also clear it out)
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
    
    fboPix.allocate(scaledWidth, scaledHeight, 1);
    
    //-----Gui-----
    cameraGui.setup(name);
    cameraGui.load();
    
    
    started = false;
    numFramesRec = 0;
    
    
    
    
    //start the image processing thread
    imageProcessor.setup(&threshPix, &contours);
    

    
}



void Camera::update(){
    
    //update the quad points with changes from the GUI
    imageQuad[0].set(cameraGui.quadPt0 -> x, cameraGui.quadPt0 -> y);
    imageQuad[1].set(cameraGui.quadPt1 -> x, cameraGui.quadPt1 -> y);
    imageQuad[2].set(cameraGui.quadPt2 -> x, cameraGui.quadPt2 -> y);
    imageQuad[3].set(cameraGui.quadPt3 -> x, cameraGui.quadPt3 -> y);
    
    
    //handle mouse interaction with map pts
    adjustedMouse.set(ofGetMouseX() - adjustedQuadOrigin.x, ofGetMouseY() - adjustedQuadOrigin.y);
    
    for(int i = 0; i < quadPtMouseLock.size(); i++){
        
        if(quadPtMouseLock[i]){
            imageQuad[i].x = ofClamp(adjustedMouse.x, 0, feedWidth);
            imageQuad[i].y = ofClamp(adjustedMouse.y, 0, feedHeight);
            
            //Update the gui so the values are stored when we save
            if(i == 0){
                cameraGui.quadPt0 = imageQuad[i];
            } else if(i == 1){
                cameraGui.quadPt1 = imageQuad[i];
            } else if(i == 2){
                cameraGui.quadPt2 = imageQuad[i];
            } else {
                cameraGui.quadPt3 = imageQuad[i];
            }
            
        }
        
    }
    
    
    
    //update the feed/movie
    if(useLiveFeed){
        
        if(started){
            gst.update();
        }
        
        if(!started && ofGetElapsedTimeMillis() > staggerTime){
            started = true;
            setupFeed();
        }
        
    } else {
        movie.update();
    }
    
    
    
    //update the image processing thread
    
    
    
    
    
    //get image from gst pipeline
    if(movie.isFrameNew() || gst.isFrameNew()){
        
        numFramesRec++;
        
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
        
        
        //steal the new texture from the FBO for displaying later
        quadTex = fbo.getTexture();

        //get the pixels from the fbo
        fbo.readToPixels(fboPix);
        
        
        //construct a vector of ints with all the settings
        vector<int> settings;
        settings.resize(8);
        
        settings[0] = cameraGui.thresholdSlider;
        settings[1] = cameraGui.blurAmountSlider;
        settings[2] = cameraGui.numErosionsSlider;
        settings[3] = cameraGui.numDilationsSlider;
        settings[4] = cameraGui.minBlobAreaSlider;
        settings[5] = cameraGui.maxBlobAreaSlider;
        settings[6] = cameraGui.persistenceSlider;
        settings[7] = cameraGui.maxDistanceSlider;

        
        //pass the fbo pixels to the processing thread
        imageProcessor.analyze(fboPix, settings);
                
    }
    
    
    //Update the image processing thread
    //this will automatically fill threshTex and contours
    //with new data as it works
    imageProcessor.update();
    
    
    //for mouse interaction
    if(bEnableQuadMapping){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }
    
    

    
    
}



void Camera::drawGui(int x, int y){

    cameraGui.draw(x, y);
    
}


void Camera::drawRaw(ofVec2f pos){
    
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

void Camera::drawCV(ofVec2f pos, float scale){
    
    ofPushMatrix();
    
    ofTranslate(pos);
    ofScale(scale, scale);

    
    ofSetColor(255);
    quadTex.draw(0, 0);
    
    if(cameraGui.drawThresholdToggle){
        
        thresholdImg.setFromPixels(threshPix);
        thresholdImg.draw(0, 0);

        
    }
    
    if(cameraGui.drawContoursToggle) {
//        RectTracker& tracker = contours.getTracker();
        
        ofSetColor(255, 0, 0);
        
        contours.draw();
        
        for(int i = 0; i < contours.size(); i++) {
            ofPoint center = toOf(contours.getCenter(i));
            ofPushMatrix();
            ofTranslate(center.x, center.y);
            int label = contours.getLabel(i);
            ofVec2f velocity = toOf(contours.getVelocity(i));
            
            string msg = ofToString(label);
            
            if(cameraGui.showInfoToggle){
                msg = ofToString(label) + " : " + ofToString(center.x) + "," + ofToString(center.y) + " : " + ofToString(velocity.x) + "," + ofToString(velocity.y);
            }
            
            ofSetColor(0, 100, 255);
            ofDrawBitmapString(msg, 0, 0);
        }
    }
    
    //draw border
    ofSetColor(255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, quadTex.getWidth(), quadTex.getHeight());

    ofFill();
    
    ofPopMatrix();
    
    
    
}


void Camera::mousePressed(ofMouseEventArgs &args){
    
    
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

void Camera::mouseReleased(ofMouseEventArgs &args){
    
    for(int i = 0; i < quadPtMouseLock.size(); i++){
        quadPtMouseLock[i] = false;
    }
    
}

void Camera::mouseMoved(ofMouseEventArgs &args){}
void Camera::mouseDragged(ofMouseEventArgs & args){}
void Camera::mouseScrolled(ofMouseEventArgs & args){}
void Camera::mouseEntered(ofMouseEventArgs & args){}
void Camera::mouseExited(ofMouseEventArgs & args){}

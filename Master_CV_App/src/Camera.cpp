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

void Camera::setFont(ofTrueTypeFont *_font){
    
    font = _font;
    
}


void Camera::setupViewControl(int _thisView, int *_viewMode, ofVec2f _adjOrigin){
    
    thisView = _thisView;
    viewMode = _viewMode;
    adjustedOrigin = _adjOrigin;
    
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
    
    
    
    //-----Corridor UI-----
    name = _name;
    IP = _IP;
    
    
    //which camera setting we're tweaking
    manipulationMode = 0;
    
    quadMapCol.set(255, 0, 255);
    croppingCol.set(0, 255, 255);
    circleCol.set(quadMapCol);
    circleGrab.set(255, 0, 0);
    
    
    //size of quad handles
    mapPtRad = 10;
    titleSpace = 20;
    
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
    
    
    
    
    //load video data if we're not using the live feed
    //the feed will be started later
    if(!useLiveFeed){
        
        movie.load(fileName);
        movie.setLoopState(OF_LOOP_NORMAL);
        movie.play();
        movie.setPosition(ofRandom(movie.getTotalNumFrames())/movie.getTotalNumFrames());
        
    }
    
    

    
    
    
    //-----CV Objects && Image Processing-----
    
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

    
    
    //Cropping setup
    cropStart.set(0, 0);
    cropEnd.set(scaledWidth, scaledHeight);
    
    //bools that control if the points are attached to the mouse
    cropStartLock = false;
    cropEndLock = false;
    
    
    //allocate all the stuff we'll be using
    //Textures
    rawTex.allocate(feedWidth, feedHeight, GL_LUMINANCE);
    quadTex.allocate(scaledWidth, scaledHeight, GL_LUMINANCE);
    
    
    threadOutput.allocate(scaledWidth, scaledHeight, 1);
    threadOutputImg.allocate(scaledWidth, scaledHeight, OF_IMAGE_GRAYSCALE);
    
    //FBO
    fbo.allocate(scaledWidth, scaledHeight);
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
    
    fboPix.allocate(scaledWidth, scaledHeight, 1);
    
    
    //-----Gui-----
    cameraGui.setup(name, soloCam);
    cameraGui.loadSettings();
    
    
    started = false;
    numFramesRec = 0;
    
    
    
    
    //start the image processing thread
    imageProcessor.setup(&threadOutput, &contours, soloCam);
    
}


//===============================================================//
//---------------------------------------------------------------//
//                _  _ ___  ___  ____ ___ ____                   //
//                |  | |__] |  \ |__|  |  |___                   //
//                |__| |    |__/ |  |  |  |___                   //
//                                                               //
//---------------------------------------------------------------//
//===============================================================//



void Camera::update(){
    
    //update the quad points with changes from the GUI
    imageQuad[0].set(cameraGui.quadPt0 -> x, cameraGui.quadPt0 -> y);
    imageQuad[1].set(cameraGui.quadPt1 -> x, cameraGui.quadPt1 -> y);
    imageQuad[2].set(cameraGui.quadPt2 -> x, cameraGui.quadPt2 -> y);
    imageQuad[3].set(cameraGui.quadPt3 -> x, cameraGui.quadPt3 -> y);
    

    //update the crop points with changes from the GUI
    //but gui vals are normalized so scale them back up
    cropStart.set(scaledWidth * cameraGui.cropStart -> x, scaledHeight * cameraGui.cropStart -> y);
    cropEnd.set(scaledWidth * cameraGui.cropEnd -> x, scaledHeight * cameraGui.cropEnd -> y);

    
    //handle mouse interaction with map pts
    adjustedMouse.set(ofGetMouseX() - adjustedOrigin.x, ofGetMouseY() - adjustedOrigin.y - titleSpace);
    
    //Image manipulation:
    //0 = quad mapping
    //1 = cropping
    if(manipulationMode == 0){
    
        circleCol = quadMapCol;
        
        
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

    } else {
    
        circleCol = quadMapCol;
        
        //go through the two cropping points
        //upper left and lower right
        if(cropStartLock){
            cropStart.x = ofClamp(adjustedMouse.x, 0, scaledWidth);
            cropStart.y = ofClamp(adjustedMouse.y, 0, scaledHeight);
            
            //save a normalized version to the gui
            cameraGui.cropStart = cropStart/ofVec2f(scaledWidth, scaledHeight);
            
        }
        
        if(cropEndLock){
            cropEnd.x = ofClamp(adjustedMouse.x, 0, scaledWidth);
            cropEnd.y = ofClamp(adjustedMouse.y, 0, scaledHeight);
            
            //save a normalized version to the gui
            cameraGui.cropEnd = cropEnd/ofVec2f(scaledWidth, scaledHeight);
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
        settings.resize(12);
        
        settings[0] = cameraGui.thresholdSlider;
        settings[1] = cameraGui.blurAmountSlider;
        settings[2] = cameraGui.numErosionsSlider;
        settings[3] = cameraGui.numDilationsSlider;
        settings[4] = cameraGui.minBlobAreaSlider;
        settings[5] = cameraGui.maxBlobAreaSlider;
        settings[6] = cameraGui.persistenceSlider;
        settings[7] = cameraGui.maxDistanceSlider;
        settings[8] = (int)(cameraGui.cropStart -> x * scaledWidth);
        settings[9] = (int)(cameraGui.cropStart -> y * scaledHeight);
        settings[10] = (int)(cameraGui.cropEnd -> x * scaledWidth);
        settings[11] = (int)(cameraGui.cropEnd -> y * scaledHeight);

        
        //pass the fbo pixels to the processing thread
        imageProcessor.analyze(fboPix, settings);
                
    }
    
    
    //Update the image processing thread
    //this will automatically fill threadOutput (and contours if soloCam = true)
    //with new data as it works
    imageProcessor.update();
    
    
    if((*viewMode) == thisView){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }
    

    
    
    //gather data to track blob stats
    //only if we're soloCams
    if(soloCam){
        
        //get together all the average blob data for this corridor
        avgDir.set(0);
        avgVel.set(0);
        avgPos.set(0);
        avgSpeed = 0;
        
        corridorBundle.clear();
        corridorStats.clear();
        blobsBundle.clear();
        
        
        //corridors 2, 3, 4, 5 are:
        //cameras 6, 7, 8, 9 (zero-indexed)
        int thisCorridor = thisView - 4;  //i.e. cam 6 = corr 2
        
        
        //go through the blobs and start collecting data
        for(int i = 0; i < contours.size(); i++){
            
            //get data from contour
            int label = contours.getLabel(i);
            ofPoint center = toOf(contours.getCenter(i));
            ofPoint centerNormalized = center/ofVec2f(scaledWidth, scaledHeight);
            ofVec2f velocity = toOf(contours.getVelocity(i));
            
            //add it to our averages
            avgVel += velocity;
            avgSpeed += velocity.length();
            avgPos += centerNormalized;
            
            //prepare a message for this blob then
            //push it into the blobMessages vector
            ofxOscMessage m;
            
            
            
            
            m.setAddress("/corridor_" + ofToString(thisCorridor) + "/blob_data");
            m.addIntArg(label);
            m.addFloatArg(centerNormalized.x);
            m.addFloatArg(centerNormalized.y);
            m.addFloatArg(velocity.x);
            m.addFloatArg(velocity.x);
            
            blobsBundle.addMessage(m);
            
        }
        
        //average out all the data for this corridor
        avgSpeed = avgSpeed/float(contours.size());
        avgVel = avgVel/float(contours.size());
        avgDir = avgVel.getNormalized();
        avgPos = avgPos/float(contours.size());
        
        
        
        //prepare the corridor stats message
        corridorStats.setAddress("/corridor_" + ofToString(thisCorridor) + "_stats");
        corridorStats.addIntArg(contours.size());
        corridorStats.addFloatArg(avgPos.x);
        corridorStats.addFloatArg(avgPos.y);
        corridorStats.addFloatArg(avgDir.x);
        corridorStats.addFloatArg(avgDir.y);
        corridorStats.addFloatArg(avgSpeed);
        
        //now assemble the corridor bundle from the stats message and all the blobs
        corridorBundle.addMessage(corridorStats);
        corridorBundle.addBundle(blobsBundle);
    }
    

    
    
}



//===============================================================//
//---------------------------------------------------------------//
//                    ___  ____ ____ _ _ _                       //
//                    |  \ |__/ |__| | | |                       //
//                    |__/ |  \ |  | |_|_|                       //
//                                                               //
//---------------------------------------------------------------//
//===============================================================//


void Camera::drawGui(int x, int y){

    cameraGui.draw(x, y);
    
}


void Camera::drawMain(){
    
    
    ofSetColor(255);
    ofDrawBitmapString( ofToString(name) + " Feed FPS: " + ofToString(cameraFPS), 15, 60);
    

    
    ofVec2f secondSpot;
    
    if(manipulationMode == 0){
        
        string title = "Raw Feed (640x512) -> Quad Mapping";
        ofDrawBitmapString(title, adjustedOrigin.x, adjustedOrigin.y + 10);
        
        drawRaw(adjustedOrigin.x, adjustedOrigin.y + titleSpace);
        
        
        secondSpot.set(adjustedOrigin.x + feedWidth + 20, adjustedOrigin.y);
        

        
    } else {
        
        //this part wont happen even if manipulationMode is 1, unless soloCam is false
        //since there's no way to set the mode with the buttons gone
        
        string title = "Mapped & scaled to: " + ofToString(scaledWidth) + "x" + ofToString(scaledHeight) + ")";
        ofSetColor(255);
        ofDrawBitmapString(title, adjustedOrigin.x, adjustedOrigin.y + 10);
        drawQuad(adjustedOrigin.x, adjustedOrigin.y + titleSpace, 1.0f);
        
        secondSpot.set(adjustedOrigin.x + scaledWidth + 20, adjustedOrigin.y);
        
   
    }
    
    int w = cropEnd.x - cropStart.x;
    int h = cropEnd.y - cropStart.y;
    
    string title;
    if(soloCam){
        title = "Threshold + Contours (Scaled to: " + ofToString(w) + "x" + ofToString(h) + ")";
    } else {
        title = "Threshold + Contours (Cropped to: " + ofToString(w) + "x" + ofToString(h) + ")";
    }
    ofSetColor(255);
    ofDrawBitmapString(title, secondSpot.x, secondSpot.y + 10);
    
    
    //shrink down images just for display purposes
    float scale;
    if(bScaleDown){
        scale = 1.0;
    } else {
        scale = 0.5;
    }
    
    drawCV(secondSpot.x, secondSpot.y + titleSpace, scale);
    
    if(soloCam){
        //draw num blobs under CV image
        ofSetColor(255);
        ofDrawBitmapString("Num Blobs: " + ofToString(contours.size()), secondSpot.x, secondSpot.y + scaledHeight + titleSpace + 20);
    }
    
    drawGui(15, adjustedOrigin.y);
    
    
    

    
    
    
    
}




void Camera::drawRaw(int x, int y){
    
    //WARNING: certain things can be drawn with matrix translations
    // but others need to be drawn raw since they depend on mouse interaction
    
    
    ofPushMatrix();
    
    
    ofSetColor(255);
    ofTranslate(x, y);

    
    //raw camera feed
    ofSetColor(255);
    rawTex.draw(0, 0);
    
    //border
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, 0, feedWidth, feedHeight);
    
    //draw lines between points
    ofPolyline quadMap(imageQuad);
    quadMap.close();
    
    ofSetLineWidth(1);
    
    ofSetColor(quadMapCol);
    quadMap.draw();
    
    //draw mapping points
    ofPushStyle();
    for(int i = 0; i < imageQuad.size(); i++){
        
        ofColor c;
        
        if(quadPtMouseLock[i]){
            c = circleGrab;
        } else {
            c = quadMapCol;
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

void Camera::drawQuad(int x, int y, float scale){
    
    //WARNING: certain things can be drawn with matrix translations
    // but others need to be drawn raw since they depend on mouse interaction
    
    
    ofPushMatrix();
    
    ofTranslate(x, y);

    
    //raw camera feed
    ofSetColor(255);
    quadTex.draw(0, 0);
    
    //border
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(quadMapCol);
    ofDrawRectangle(0, 0, scaledWidth, scaledHeight);
    ofFill();
    
    //draw lines between points
    ofPolyline cropSquare;
    cropSquare.addVertex(cropStart);
    cropSquare.addVertex(cropEnd.x, cropStart.y);
    cropSquare.addVertex(cropEnd);
    cropSquare.addVertex(cropStart.x, cropEnd.y);
    cropSquare.close();
    
    ofSetLineWidth(1);
    
    ofSetColor(croppingCol);
    cropSquare.draw();
    
    //draw mapping points
    ofPushStyle();
    ofNoFill();
    
    ofColor handleCol;
    
    if(cropStartLock){
        handleCol = circleGrab;
    } else {
        handleCol = croppingCol;
    }
    
    ofSetColor(handleCol);
    ofDrawCircle(cropStart, mapPtRad);
    
    if(cropEndLock){
        handleCol = circleGrab;
    } else {
        handleCol = croppingCol;
    }
    
    ofSetColor(handleCol);
    ofDrawCircle(cropEnd, mapPtRad);
    
    ofSetColor(255);
    ofDrawBitmapString("Crop Start", cropStart.x + mapPtRad, cropStart.y + mapPtRad);
    ofDrawBitmapString("Crop End", cropEnd.x - mapPtRad - 70, cropEnd.y - mapPtRad);

    
    
    
    ofPopStyle();
    
    
    ofPopMatrix();
    
}


void Camera::drawCV(int x, int y, float scale){
    
    ofPushMatrix();
    
    ofTranslate(x, y);
    
    ofScale(scale, scale);

    
    //draw texture translucent and reddish
    ofSetColor(255, 100);
    quadTex.draw(0, 0);
    

    
    //draw light red tint over the quadTex then the crop on top
    
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(quadMapCol);
    ofDrawRectangle(0, 0, quadTex.getWidth(), quadTex.getHeight());
    ofFill();
    
    ofSetColor(255);
    drawCroppedTex(cropStart);


    
        
    threadOutputImg.setFromPixels(threadOutput);

    if(cameraGui.drawThresholdToggle){
        threadOutputImg.draw(cropStart);
    }
    
    //draw border
    ofNoFill();
    ofSetLineWidth(0.5);
    ofSetColor(croppingCol);
    ofDrawRectangle(cropStart, threadOutputImg.getWidth(), threadOutputImg.getHeight());

    
    if(cameraGui.drawContoursToggle){
        
        ofSetColor(255, 0, 0);
        ofPushMatrix();
        ofTranslate(cropStart.x, cropStart.y);
        
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
        ofPopMatrix();
        
    }
    
    

    
    ofPopMatrix();
    
    
    
}



void Camera::drawCroppedTex(ofVec2f pos){
    
    //then draw the texture
    int w = cropEnd.x - cropStart.x;
    int h = cropEnd.y - cropStart.y;
    
    ofSetColor(255);
    quadTex.drawSubsection(pos.x, pos.y, w, h, cropStart.x, cropStart.y, w, h);
    
}





void Camera::mousePressed(ofMouseEventArgs &args){
    
    if(manipulationMode == 0){
        
        //handle mouse interaction with quadMap points
        for(int i = 0; i < imageQuad.size(); i++){
            
            //need to adjust mouse position since we're
            //translating the raw camera view from the origin with pushmatrix
            float dist = ofDist(adjustedMouse.x, adjustedMouse.y, imageQuad[i].x, imageQuad[i].y);
            
            if(dist < mapPtRad){
                quadPtMouseLock[i] = true;
                
                //exit the for loop, prevents us from
                //grabbing multiple handles at once
                break;
            } else {
                quadPtMouseLock[i] = false;
            }
            
            
        }
        
    } else {
        
        //check for start point first then end point later
        //so we don't grab two points at once
        float dist = ofDist(adjustedMouse.x, adjustedMouse.y, cropStart.x, cropStart.y);
        
        if(dist < mapPtRad){
            cropStartLock = true;
            
        } else {
            cropStartLock = false;
            
            //if we're not in cropStart, check for cropEnd
            
            dist = ofDist(adjustedMouse.x, adjustedMouse.y, cropEnd.x, cropEnd.y);
            
            if(dist < mapPtRad){
                cropEndLock = true;
            } else {
                cropEndLock = false;
            }
            
        }
        
    }
    
}

void Camera::mouseReleased(ofMouseEventArgs &args){
    
    for(int i = 0; i < quadPtMouseLock.size(); i++){
        quadPtMouseLock[i] = false;
    }
    
    
    cropStartLock = false;
    cropEndLock = false;
    
}

void Camera::mouseMoved(ofMouseEventArgs &args){}
void Camera::mouseDragged(ofMouseEventArgs & args){}
void Camera::mouseScrolled(ofMouseEventArgs & args){}
void Camera::mouseEntered(ofMouseEventArgs & args){}
void Camera::mouseExited(ofMouseEventArgs & args){}

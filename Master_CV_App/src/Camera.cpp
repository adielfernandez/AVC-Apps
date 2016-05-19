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
    
    mappingCol.set(255, 0, 255);
    maskingCol.set(255, 200, 0);
    croppingCol.set(0, 255, 255);
    circleCol.set(mappingCol);
    circleGrab.set(255, 0, 0);
    
    
    //size of map handles
    mapPtRad = 10;
    maskToolSize = 10;
    titleSpace = 20;
    
    //-----Camera Stream-----
//    feedWidth = 640;
//    feedHeight = 512;
    
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
//        movie.setPosition(ofRandom(movie.getTotalNumFrames())/movie.getTotalNumFrames());
        
    }
    
    

    
    
    
    //-----CV Objects && Image Processing-----

    //map quad to 9 points in a rectangular 3x3 grid
    //these will be overwritten by settings loaded later
    numMapPts = 9;
    imageMapPts.resize(numMapPts);
    for(int i = 0; i < imageMapPts.size(); i++){
        
        int x = (i % 3) * feedWidth/2;
        int y = (i - i % 3)/3 * feedHeight/2;
        
        imageMapPts[i].set( x, y );
        
    }
    
    
    //bools we need to check if we're grabbing the
    //imageMapPts points for texture mapping
    mapPtMouseLock.resize(numMapPts);
    for(int i = 0; i < mapPtMouseLock.size(); i++){
        mapPtMouseLock[i] = false;
    }
    
    //Make the mesh the same size as the normal feed frame
    //Re-use the imageMapPts points since they are still rectangular at this point
    
    //BUT... add them using triangle fan with fan center at central point == imageMapPts[4]
    //then go clockwise around center from top left
    /*
     *     0 ___1___ 2
     *      |\  |  /|
     *      | \ | / |
     *     3|__\|/__|5  (4 at center)
     *      |  /|\  |
     *      | / | \ |
     *      |/__|__\|
     *     6    7    8
     *
     */
    mappedMesh.addVertex(imageMapPts[4]);
    mappedMesh.addVertex(imageMapPts[0]);
    mappedMesh.addVertex(imageMapPts[1]);
    mappedMesh.addVertex(imageMapPts[2]);
    mappedMesh.addVertex(imageMapPts[5]);
    mappedMesh.addVertex(imageMapPts[8]);
    mappedMesh.addVertex(imageMapPts[7]);
    mappedMesh.addVertex(imageMapPts[6]);
    mappedMesh.addVertex(imageMapPts[3]);
    mappedMesh.addVertex(imageMapPts[0]);  //and wrap back around so the fan closes
    
    mappedMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    
    
    //Cropping setup
    cropStart.set(0, 0);
    cropEnd.set(scaledWidth, scaledHeight);
    
    //bools that control if the points are attached to the mouse
    cropStartLock = false;
    cropEndLock = false;
    
    
    //allocate all the stuff we'll be using
    //Textures
    rawTex.allocate(feedWidth, feedHeight, GL_LUMINANCE);
    mappedTex.allocate(scaledWidth, scaledHeight, GL_LUMINANCE);
    
    blurredPix.allocate(scaledWidth, scaledHeight, 1);
    threshPix.allocate(scaledWidth, scaledHeight, 1);
    
    threadOutput.allocate(scaledWidth, scaledHeight, 1);
    threadOutputImg.allocate(scaledWidth, scaledHeight, OF_IMAGE_GRAYSCALE);
    
    //FBO
    fbo.allocate(scaledWidth, scaledHeight);
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
    
    fboPix.allocate(scaledWidth, scaledHeight, 1);
    
    
    //Mask
    maskChanged = true;
    maskBoundStart.set(0,0);
    maskBoundEnd.set(fboPix.getWidth(), fboPix.getHeight());
    
    if(soloCam){
        
        maskFileName = "masks/" + name + ".png";
        
        if(maskImg.load(maskFileName)){
            
            cout << name << " mask loaded" << endl;
            maskPix = maskImg.getPixels();
            
        } else {
            
            cout << maskFileName << " not found, creating..." << endl;
            
            maskPix.allocate(scaledWidth, scaledHeight, 1);
            maskPix.setColor(ofColor(0));
            
            maskImg.setFromPixels(maskPix);
            maskImg.save(maskFileName);
            
        }
        
        
    }
    

    
    
    //-----Gui-----
    cameraGui.setup(name, soloCam);
    cameraGui.loadSettings();
    
    
    //camera starting/restarting
    pipelineStarted = false;
    pipelineSetup = false;
    firstUpdate = false;
    numFramesRec = 0;
    lastFrameTime = 0;
    cameraFPS = 0;
    lastCamFPS = 0;
    avgCamFPS = 0;
    
    
    //Pipeline management
    
    //time we've actually sent the command to SET the pipeline
    //happens later, after the staggering time has passed
    pipelineSetTime = 0;
    
    //how long to wait before we try to actually START the pipeline
    gstWaitToStart = 30;
    
    
    //start the image processing thread
    if(soloCam) imageProcessor.setup(&threadOutput, &contours);
    
    
    //background colors (red means thread has crashed)
    backgroundInCol.set(80);
    backgroundOutCol.set(0);
    
    
    //Setup the blob processor
    filteredContours.setup(&contours);
    
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
    
    
    //update the feed/movie
    if(useLiveFeed){

        
        if(!pipelineSetup && ofGetElapsedTimeMillis() > staggerTime){
            
            cout << "\n\n" << name << ": SET pipeline at " << ofGetElapsedTimef() << endl;
            
            gst.setPipeline("rtspsrc location=rtsp://admin:admin@" + IP + ":554/cam/realmonitor?channel=1&subtype=1 latency=0 ! rtpjpegdepay ! jpegparse ! jpegdec ! queue2 ! decodebin ! videoconvert", OF_PIXELS_MONO, true, feedWidth, feedHeight);
            
            
            pipelineSetup = true;
            pipelineSetTime = ofGetElapsedTimeMillis();
            
            cout << "\n\n" << endl;
            
        }
        
        
        if(pipelineSetup && !pipelineStarted && ofGetElapsedTimeMillis() - pipelineSetTime > gstWaitToStart){
            
            //----------GSTREAMER ISSUE STARTS HERE----------
            //App crashes when opening the 13th camera, but only when running compiled app
            //(crash does not occur in Xcode).
            //Error happens after startPipeline() call from 13th camera.

            cout << "\n\n" << name << ": START pipeline at " << ofGetElapsedTimef() << endl;
            gst.startPipeline();
            cout << "\n\n" << endl;
            
            
            pipelineStarted = true;
            
        }
        
        
        if(pipelineStarted){
            
            if(!firstUpdate){
                cout << "\n\n" << name << ": First update at " << ofGetElapsedTimef() << endl;
                firstUpdate = true;
            }
            
            gst.update();
            
        }
        
        
    } else {
        
        movie.update();
    }
    
    
    
    
    
    
    
    //resets the mapping points to the default
    if(cameraGui.resetMap){
        
        //change the local map point vector
        for(int i = 0; i < imageMapPts.size(); i++){
            
            int x = (i % 3) * feedWidth/2;
            int y = (i - i % 3)/3 * feedHeight/2;
            
            imageMapPts[i].set( x, y );
            
        }
        
        //and reset the gui values as well
        cameraGui.mapPt0 = imageMapPts[0];
        cameraGui.mapPt1 = imageMapPts[1];
        cameraGui.mapPt2 = imageMapPts[2];
        cameraGui.mapPt3 = imageMapPts[3];
        cameraGui.mapPt4 = imageMapPts[4];
        cameraGui.mapPt5 = imageMapPts[5];
        cameraGui.mapPt6 = imageMapPts[6];
        cameraGui.mapPt7 = imageMapPts[7];
        cameraGui.mapPt8 = imageMapPts[8];
    
    }
    
    
    //Sets the map points to a 180 degree rotation
    if(cameraGui.rotate180){
        
        //change the local map point vector
        for(int i = 0; i < imageMapPts.size(); i++){
            
            int x = feedWidth - (i % 3) * feedWidth/2;
            int y = feedHeight - (i - i % 3)/3 * feedHeight/2;
            
            imageMapPts[i].set( x, y );
            
        }
        
        //and reset the gui values as well
        cameraGui.mapPt0 = imageMapPts[0];
        cameraGui.mapPt1 = imageMapPts[1];
        cameraGui.mapPt2 = imageMapPts[2];
        cameraGui.mapPt3 = imageMapPts[3];
        cameraGui.mapPt4 = imageMapPts[4];
        cameraGui.mapPt5 = imageMapPts[5];
        cameraGui.mapPt6 = imageMapPts[6];
        cameraGui.mapPt7 = imageMapPts[7];
        cameraGui.mapPt8 = imageMapPts[8];
        
    }
    
    
    if(!soloCam && cameraGui.resetCrop){
        
        //reset the crop points
        cropStart.set(0, 0);
        cropEnd.set(scaledWidth, scaledHeight);
        
        //set the gui to the new (normalized) crop points
        cameraGui.cropStart = cropStart/ofVec2f(scaledWidth, scaledHeight);
        cameraGui.cropEnd = cropEnd/ofVec2f(scaledWidth, scaledHeight);
        
    }
    
    
    
    
    //update the mapping points with changes from the GUI
    imageMapPts[0].set(cameraGui.mapPt0 -> x, cameraGui.mapPt0 -> y);
    imageMapPts[1].set(cameraGui.mapPt1 -> x, cameraGui.mapPt1 -> y);
    imageMapPts[2].set(cameraGui.mapPt2 -> x, cameraGui.mapPt2 -> y);
    imageMapPts[3].set(cameraGui.mapPt3 -> x, cameraGui.mapPt3 -> y);
    imageMapPts[4].set(cameraGui.mapPt4 -> x, cameraGui.mapPt4 -> y);
    imageMapPts[5].set(cameraGui.mapPt5 -> x, cameraGui.mapPt5 -> y);
    imageMapPts[6].set(cameraGui.mapPt6 -> x, cameraGui.mapPt6 -> y);
    imageMapPts[7].set(cameraGui.mapPt7 -> x, cameraGui.mapPt7 -> y);
    imageMapPts[8].set(cameraGui.mapPt8 -> x, cameraGui.mapPt8 -> y);

    //update the crop points with changes from the GUI
    //but gui vals are normalized so scale them back up
    cropStart.set(scaledWidth * cameraGui.cropStart -> x, scaledHeight * cameraGui.cropStart -> y);
    cropEnd.set(scaledWidth * cameraGui.cropEnd -> x, scaledHeight * cameraGui.cropEnd -> y);

    
    //handle mouse interaction with map pts
    adjustedMouse.set(ofGetMouseX() - adjustedOrigin.x, ofGetMouseY() - adjustedOrigin.y - titleSpace);
    
    //Image manipulation:
    //0 = mapping
    //1 = cropping
    if(manipulationMode == 0){
    
        circleCol = mappingCol;
        
        
        for(int i = 0; i < mapPtMouseLock.size(); i++){
            
            if(mapPtMouseLock[i]){
                imageMapPts[i].x = ofClamp(adjustedMouse.x, 0, feedWidth);
                imageMapPts[i].y = ofClamp(adjustedMouse.y, 0, feedHeight);
                
                //Update the gui so the values are stored when we save
                if(i == 0){
                    cameraGui.mapPt0 = imageMapPts[i];
                } else if(i == 1){
                    cameraGui.mapPt1 = imageMapPts[i];
                } else if(i == 2){
                    cameraGui.mapPt2 = imageMapPts[i];
                } else if(i == 3){
                    cameraGui.mapPt3 = imageMapPts[i];
                } else if(i == 4){
                    cameraGui.mapPt4 = imageMapPts[i];
                } else if(i == 5){
                    cameraGui.mapPt5 = imageMapPts[i];
                } else if(i == 6){
                    cameraGui.mapPt6 = imageMapPts[i];
                } else if(i == 7){
                    cameraGui.mapPt7 = imageMapPts[i];
                } else {
                    cameraGui.mapPt8 = imageMapPts[i];
                }
                
            }
            
        }

    } else {
    
        //masking or cropping
        if(soloCam){
            
            //if we're inside the mask and clicking
            //set the pixels of the mask
            if(maskPressed && cameraGui.useMask){
                
                //go through all the pixels inside the cursor
                //and set the pixel value to white.
                //Going through the cursor pixels should be faster
                //than going through all the pixels of the pixel object
                for(int i = 0; i < maskToolSize * maskToolSize; i++){
                    
                    //XY value from the for loop iterator
                    int x = i % maskToolSize;
                    int y = (i - x)/maskToolSize;
                    
                    //then adjust to the mouse position within the pixel object
                    //(also minus half because the cursor is centered at mouse)
                    x += adjustedMouse.x - maskToolSize/2;
                    y += adjustedMouse.y - maskToolSize/2;
                    
                    //only set pixels of the mask for cursors pixels that
                    //are actually inside (near-border cases)
                    if(x >= 0 && y >= 0 && x <= maskPix.getWidth() && y <= maskPix.getHeight()){
                        
                        int pixel = y * maskPix.getWidth() + x;
                        
                        int value = (cameraGui.drawOrErase ? 255 : 0);
                        
                        maskPix.setColor(pixel, ofColor(value));
                    }
                    
                }
             
                maskChanged = true;
                
            }
            
            
        } else {
        
            circleCol = mappingCol;
            
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
        
    }
    
    //convenience variables to hold the cropped area dimensions
    croppedWidth = cropEnd.x - cropStart.x;
    croppedHeight = cropEnd.y - cropStart.y;
    
    
    
    //clearing, saving and loading the mask
    if(soloCam){
        if(cameraGui.clearMask){
            maskPix.setColor(0);
            maskBoundStart.set(0, 0);
            maskBoundEnd.set(0, 0);
        }
        
        if(cameraGui.saveMask){
            maskImg.setFromPixels(maskImg);
            maskImg.save(maskFileName);
        }
        
        if(cameraGui.loadMask){
            maskImg.load(maskFileName);
            maskPix = maskImg.getPixels();
            maskChanged = true;
        }
    }
    
    
    
    

    
    
    
    
    
    //get image from gst pipeline
    if(movie.isFrameNew() || gst.isFrameNew()){
        
//        cout << name << ": new frame" << endl;
        
        numFramesRec++;
        
        lastCamFPS = cameraFPS;
        cameraFPS = (int)(1/(ofGetElapsedTimef() - (float)lastFrameTime/1000.0f));
        lastFrameTime = ofGetElapsedTimeMillis();
        avgCamFPS = (lastCamFPS + cameraFPS)/2;
        
        //get texture from feed
        if(useLiveFeed){
            rawTex.loadData(gst.getPixels());
        } else {
            rawTex.loadData(movie.getPixels());  
        }
        
        //clear out the old mesh and remap the texture
        //to the control points we've set
        //(see setup() for order explanation)
        mappedMesh.clearTexCoords();
        mappedMesh.addTexCoord(imageMapPts[4]);
        mappedMesh.addTexCoord(imageMapPts[0]);
        mappedMesh.addTexCoord(imageMapPts[1]);
        mappedMesh.addTexCoord(imageMapPts[2]);
        mappedMesh.addTexCoord(imageMapPts[5]);
        mappedMesh.addTexCoord(imageMapPts[8]);
        mappedMesh.addTexCoord(imageMapPts[7]);
        mappedMesh.addTexCoord(imageMapPts[6]);
        mappedMesh.addTexCoord(imageMapPts[3]);
        mappedMesh.addTexCoord(imageMapPts[0]);  //and wrap back around so the fan closes
        
//        mappedMesh.addTexCoord(imageMapPts[0]);
//        mappedMesh.addTexCoord(imageMapPts[1]);
//        mappedMesh.addTexCoord(imageMapPts[2]);
//        mappedMesh.addTexCoord(imageMapPts[3]);
        
        //draw the mesh with the re-mapped texture to the FBO
        fbo.begin();
        rawTex.bind();
        ofSetColor(255);
        
        if(bScaleDown) ofScale(0.5, 0.5);
        
        mappedMesh.draw();
        rawTex.unbind();
        fbo.end();
        
        
        //steal the new texture from the FBO for displaying later
        mappedTex = fbo.getTexture();

        //get the pixels from the fbo
        fbo.readToPixels(fboPix);
        
        
        fboPix.setImageType(OF_IMAGE_GRAYSCALE);

        
        
        //if we're a solo camera use the thread to do CV stuff
        //other wise, just crop the pixels to be sent to the aggregate
        if(soloCam){

            //subtract the mask before sending to the CV thread
            //go through all the pixels and set them according to the mask
            //subtract 4 RGB & A pixels for every mask pixel
            if(cameraGui.useMask){
                
                //if the mask has changed we need to go through and find the X and Y bounds
                if(maskChanged){
                    
                    //set the bounds to be way off so we can correct them
                    maskBoundEnd.set(0, 0);
                    maskBoundStart.set(1000000, 1000000);
                    
                    int numWhite = 0;
                    
                    for(int i = 0; i < fboPix.getWidth() * fboPix.getHeight(); i++){
                    
                        
                        if(maskPix[i] == 255){

                            //get the X & Y from the index
                            int x = i % fboPix.getWidth();
                            int y = (i - x)/fboPix.getWidth();
                        
                            //if the mask is new, go through it and find the
                            //left, right, top and bottom bounds
                            
                            if(x < maskBoundStart.x) maskBoundStart.x = x;
                            if(y < maskBoundStart.y) maskBoundStart.y = y;
                            
                            if(x > maskBoundEnd.x) maskBoundEnd.x = x;
                            if(y > maskBoundEnd.y) maskBoundEnd.y = y;
                            
                            //also mask the image while we're here
                            fboPix[i] = 0;

                            numWhite++;
                        }
                    
                    }
                    
                    //if there are no lit pixels in the mask then make the start and end 0,0
                    if(numWhite == 0){
                        maskBoundEnd.set(0, 0);
                        maskBoundStart.set(0, 0);
                    }

                    maskChanged = false;
                    
                } else {
                    
                    
                    //go through the image and mask out but
                    //only go through the optimized region
                    for(int y = maskBoundStart.y; y < maskBoundEnd.y; y++){
                        for(int x = maskBoundStart.x; x < maskBoundEnd.x; x++){
                        
                            int index = y * fboPix.getWidth() + x;
                            
                            if(maskPix[index] == 255) fboPix[index] = 0;
                            
                        }
                    }
                    
                    
                    
                }
            }
            
            
            //construct a vector of ints with all the settings
            vector<int> settings;
            settings.resize(13);
            
            settings[0] = cameraGui.blurAmountSlider;
            settings[1] = cameraGui.numErosionsSlider;
            settings[2] = cameraGui.numDilationsSlider;
            
            settings[3] = cameraGui.learningTime;
            settings[4] = cameraGui.useBgDiff;      //bool casted as int into vector
            settings[5] = cameraGui.resetBG;        //bool casted as int into vector
            settings[6] = cameraGui.thresholdSlider;
            settings[7] = cameraGui.minBlobAreaSlider;
            settings[8] = cameraGui.maxBlobAreaSlider;
            settings[9] = cameraGui.persistenceSlider;
            settings[10] = cameraGui.maxDistanceSlider;
            settings[11] = (int)(cameraGui.contrastSlider * 1000);        //vector holds ints so mult by 1000
            settings[12] = (int)(cameraGui.contrastPhaseSlider * 1000);   //then divide by 1000 in the thread
            

            //pass the fbo pixels to the processing thread
            imageProcessor.analyze(fboPix, settings);
            
            //update the blob filter
            if(cameraGui.useBlobFilter)
                filteredContours.update(cameraGui.filterRadiusSlider, cameraGui.stillTimeSlider, cameraGui.speedThresholdSlider);
            
            
        } else {
            
            //just manually fill the threadOutput pixels
            //without the thread
            
            
            //Do blurring and thresholding for single cameras
            //BEFORE being sent to the aggregate (and aggregate CV thread)
            if(cameraGui.blurAmountSlider > 0){
                ofxCv::GaussianBlur(fboPix, blurredPix, cameraGui.blurAmountSlider);
            } else {
                
                blurredPix = fboPix;
                
            }
            
            if(cameraGui.useThreshold){
                ofxCv::threshold(blurredPix, threshPix, cameraGui.thresholdSlider);
                threshPix.cropTo(threadOutput, cropStart.x, cropStart.y, croppedWidth, croppedHeight);
            } else {
                blurredPix.cropTo(threadOutput, cropStart.x, cropStart.y, croppedWidth, croppedHeight);
            }
            
            
            
        }
    }
    
    
    //Update the image processing thread
    //this will automatically fill threadOutput (and contours if soloCam = true)
    //with new data as it works
    if(soloCam) imageProcessor.update();
    

    
    //add the threadOutput to an image for drawing later
    threadOutputImg.setFromPixels(threadOutput);
    
    
    
    
    
    if((*viewMode) == thisView){
        ofRegisterMouseEvents(this);
    } else {
        ofUnregisterMouseEvents(this);
    }
    

    
    
    //gather data to track blob stats
    //only if we're soloCams
    if(soloCam)  gatherOscStats();
    
    
    if(soloCam && ofGetElapsedTimeMillis() - imageProcessor.lastDataSendTime > 1000){
        backgroundInCol.lerp(ofColor(100, 0, 0), 0.08);
        backgroundOutCol.lerp(ofColor(10, 0, 0), 0.08);
    } else {
        backgroundInCol.lerp(ofColor(100), 0.2);
        backgroundOutCol.lerp(ofColor(0), 0.2);
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
    ofDrawBitmapString( ofToString(name) + " Feed FPS: " + ofToString(avgCamFPS), 15, 60);
    

    
    ofVec2f secondSpot;
    
    if(manipulationMode == 0){
        
        string title = "Map/Warp the image (Raw: 640x512)";
        ofDrawBitmapString(title, adjustedOrigin.x, adjustedOrigin.y + 10);
        
        drawRawWindow(adjustedOrigin.x, adjustedOrigin.y + titleSpace);
        
        
        secondSpot.set(adjustedOrigin.x + feedWidth + 20, adjustedOrigin.y);
        

        
    } else {
        
        if(soloCam){
            
            
            //draws the already mapped image with cropping data over it
            string title = "Select Black-out Mask (Scaled: " + ofToString(scaledWidth) + "x" + ofToString(scaledHeight) + ")";
            ofSetColor(255);
            ofDrawBitmapString(title, adjustedOrigin.x, adjustedOrigin.y + 10);
            drawMaskingWindow(adjustedOrigin.x, adjustedOrigin.y + titleSpace, 1.0f);
            
            
        } else {

            
            //draws the already mapped image with cropping data over it
            string title = "Select Cropping region (Scaled: " + ofToString(scaledWidth) + "x" + ofToString(scaledHeight) + ")";
            ofSetColor(255);
            ofDrawBitmapString(title, adjustedOrigin.x, adjustedOrigin.y + 10);
            drawCroppingWindow(adjustedOrigin.x, adjustedOrigin.y + titleSpace, 1.0f);
            
            
        }
        
        secondSpot.set(adjustedOrigin.x + scaledWidth + 20, adjustedOrigin.y);
        
   
    }
    
    

    
    //shrink down images just for display purposes
    float scale;
    if(bScaleDown){
        scale = 1.0;
    } else {
        scale = 0.5;
    }
    
    drawPostCvWindow(secondSpot.x, secondSpot.y + titleSpace, scale);
    
    
    drawGui(15, adjustedOrigin.y);
    
    
    
    
    //debug info:
    
    string title;
    if(soloCam){
        title = "Thresholded + Contoured (Scaled: " + ofToString(scaledWidth) + "x" + ofToString(scaledHeight) + ")";
        
        
        
        //Blob data
        string blobData = "Num Blobs: " + ofToString(contours.size()) + "\n";
        int blobDataHeight = secondSpot.y + scaledHeight + titleSpace + 20;
        
        for(int i = 0; i < contours.size(); i++){
            blobData += ofToString(contours.getLabel(i)) + " - X: " + ofToString(contours.getCenter(i).x) + ", Y: " + ofToString(contours.getCenter(i).y) + "\n";
        }
        
        ofSetColor(255);
        ofDrawBitmapString(blobData, secondSpot.x, blobDataHeight);
        
        //PROCESSED Blob data
        string processedBlobData = "PROCESSED Blobs: " + ofToString(filteredContours.size()) + "\n";
        
        for(int i = 0; i < filteredContours.size(); i++){
            processedBlobData += ofToString(filteredContours.getLabel(i)) + " - X: " + ofToString(filteredContours.getCenter(i).x) + ", Y: " + ofToString(filteredContours.getCenter(i).y) + "\n";
        }
        
        ofSetColor(255);
        ofDrawBitmapString(processedBlobData, secondSpot.x, blobDataHeight + 180);
        
        
        
        
    } else {
        
        
        title = "Threshold + Contours (Cropped to: " + ofToString(croppedWidth) + "x" + ofToString(croppedHeight) + ")";
    }
    
    
    ofSetColor(255);
    ofDrawBitmapString(title, secondSpot.x, secondSpot.y + 10);
    
    ofDrawBitmapString("Num Frames Received: " + ofToString(numFramesRec), secondSpot.x, 15);
    
    if(soloCam){
        ofDrawBitmapString("CV Thread ID: " + ofToString(imageProcessor.getThreadId()), secondSpot.x, 30);
        ofDrawBitmapString("Last Data from Thread: " + ofToString(ofGetElapsedTimeMillis() - imageProcessor.lastDataSendTime) + " ms ago", secondSpot.x, 45);
    }

    


    //warning for camera cropping
    if(!soloCam && manipulationMode == 1){
        
        ofSetColor(255, 0, 0);
        font -> drawString("Make sure Aggregate BG Diff is OFF before cropping!", adjustedOrigin.x, adjustedOrigin.y + scaledHeight + 70 + font -> stringHeight("A"));
        
    }
    

    
    
    
    
}




void Camera::drawRawWindow(int x, int y){
    
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
    ofSetColor(mappingCol);
    ofSetLineWidth(0.5);
    
    ofMesh mapLines;
    
    //we're actually manipulating texcoords so those
    //are the ones we need to draw
    for(int i = 0; i < mappedMesh.getNumTexCoords(); i++){
        mapLines.addVertex(mappedMesh.getTexCoord(i));
    }
    mapLines.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    
    mapLines.drawWireframe();
    
    //draw mapping points
    ofPushStyle();
    for(int i = 0; i < imageMapPts.size(); i++){
        
        ofColor c;
        
        if(mapPtMouseLock[i]){
            c = circleGrab;
        } else {
            c = mappingCol;
        }
        
        ofNoFill();
        ofSetColor(c);
        ofDrawCircle(imageMapPts[i], mapPtRad);
        ofSetColor(255);
        ofDrawBitmapString(ofToString(i), imageMapPts[i].x + mapPtRad, imageMapPts[i].y - mapPtRad);
        
    }
    
    
    
    ofPopStyle();

    
    ofPopMatrix();
    
    
}

void Camera::drawCroppingWindow(int x, int y, float scale){
    
    //WARNING: certain things can be drawn with matrix translations
    // but others need to be drawn raw since they depend on mouse interaction
    
    
    ofPushMatrix();
    
    ofTranslate(x, y);

    
    //raw camera feed
    ofSetColor(255);
    mappedTex.draw(0, 0);
    
    //border
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(mappingCol);
    ofDrawRectangle(0, 0, scaledWidth, scaledHeight);
    ofFill();
    
    //draw border around crop with tick marks
    drawCropSquare();
    
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

    
    ofSetColor(200);
    ofDrawBitmapString("W: " + ofToString(croppedWidth) + "\nH: " + ofToString(croppedHeight) , 0, scaledHeight + 10);
    
    ofPopStyle();
    
    
    ofPopMatrix();
    
}


void Camera::drawMaskingWindow(int x, int y, float scale){
    
    //WARNING: certain things can be drawn with matrix translations
    // but others need to be drawn raw since they depend on mouse interaction
    
    
    ofPushMatrix();
    
    ofTranslate(x, y);
    
    
    //raw camera feed
    ofSetColor(255);
    mappedTex.draw(0, 0);
    
    
    //draw mask
    if(cameraGui.useMask){
        maskImg.setFromPixels(maskPix);
        
        ofSetColor(maskingCol, 100);
        maskImg.draw(0, 0);
        
        ofNoFill();
        ofSetLineWidth(1);
        ofSetColor(maskingCol);
        ofDrawRectangle(maskBoundStart.x, maskBoundStart.y, maskBoundEnd.x - maskBoundStart.x + 1, maskBoundEnd.y - maskBoundStart.y + 1);
        ofFill();
        
        
    }
    
    //border
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(maskingCol);
    ofDrawRectangle(0, 0, scaledWidth, scaledHeight);
    ofFill();
    
    //draw cursor if we're using the mask feature and we're inside the mask
    if(cameraGui.useMask && adjustedMouse.x > 0 && adjustedMouse.x < scaledWidth && adjustedMouse.y > 0 && adjustedMouse.y < scaledHeight){
        
        ofHideCursor();
        
        //draw a filled rect if we're drawing a blank one with an ex if we're erasing
        if(cameraGui.drawOrErase){
            
            ofFill();
            ofSetColor(maskingCol);
            ofDrawRectangle(adjustedMouse.x - maskToolSize/2, adjustedMouse.y - maskToolSize/2, maskToolSize, maskToolSize);
            
        } else {
            
            //Draw x from corner to corner
            ofSetLineWidth(1);
            ofSetColor(255, 0, 0);
            ofDrawLine(adjustedMouse.x - maskToolSize/2, adjustedMouse.y - maskToolSize/2, adjustedMouse.x + maskToolSize/2, adjustedMouse.y + maskToolSize/2);
            ofDrawLine(adjustedMouse.x - maskToolSize/2, adjustedMouse.y + maskToolSize/2, adjustedMouse.x + maskToolSize/2, adjustedMouse.y - maskToolSize/2);
            
            ofNoFill();
            ofSetColor(255);
            ofDrawRectangle(adjustedMouse.x - maskToolSize/2, adjustedMouse.y - maskToolSize/2, maskToolSize, maskToolSize);
            
        }
        
    } else {
        ofShowCursor();
    }
    

    
    
    ofPopMatrix();

    
    
    
    
}



void Camera::drawPostCvWindow(int x, int y, float scale){
    
    ofPushMatrix();
    
    ofTranslate(x, y);
    
    ofScale(scale, scale);

    
    //draw texture translucent and reddish
    ofSetColor(255);
    mappedTex.draw(0, 0);

    
    if(soloCam){
        //draw the cropped raw texture
        ofSetColor(255);
        drawCroppedTex(cropStart);


    } else {
    
        ofSetColor(255);
        threadOutputImg.draw(cropStart);
        
    }
    

    if(cameraGui.drawThresholdToggle){
        threadOutputImg.draw(cropStart);
    }
    
    
    //draw border around larger box
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(mappingCol);
    ofDrawRectangle(0, 0, mappedTex.getWidth(), mappedTex.getHeight());
    ofFill();
    
    
    //draw an X if we haven't received a frame in a while
    if(ofGetElapsedTimeMillis() - lastFrameTime > 100){
        
        ofSetColor(255, 0, 0);
        ofSetLineWidth(2);
        ofDrawLine(0, 0, mappedTex.getWidth(), mappedTex.getHeight());
        ofDrawLine(mappedTex.getWidth(), 0, 0, mappedTex.getHeight());
        
    }
    
    
    //draw border around crop with tick marks
    if(!soloCam) drawCropSquare();
    
    
    
    if(cameraGui.drawContoursToggle){
        
        ofSetColor(255, 0, 0);
        ofPushMatrix();
        ofTranslate(cropStart.x, cropStart.y);
        ofSetLineWidth(1.0);
        contours.draw();
        
        for(int i = 0; i < contours.size(); i++) {
            ofPoint center = toOf(contours.getCenter(i));
            ofPushMatrix();
            ofTranslate(center.x, center.y);
            int label = contours.getLabel(i);
            ofVec2f velocity = toOf(contours.getVelocity(i));
            
            ofSetColor(0, 255, 0);
            ofDrawRectangle(-3, -3, 6, 6);
            
            string msg = ofToString(label);
            
            if(cameraGui.showInfoToggle){
                msg = ofToString(label) + " : " + ofToString(center.x) + "," + ofToString(center.y) + " : " + ofToString(velocity.x) + "," + ofToString(velocity.y);
            }
            
            ofSetColor(0, 100, 255);
            ofDrawBitmapString(msg, 0, 0);
            
            ofPopMatrix();
            
        }
        
        ofPopMatrix();
        
    }
    
    //draw processed blobs
    if(cameraGui.drawFilteredBlobs && cameraGui.useBlobFilter){
        filteredContours.draw();
    }
    
    

    
    ofPopMatrix();
    
    
    
}



void Camera::drawCroppedTex(ofVec2f pos){
    
    //then draw the texture
    int w = croppedWidth;
    int h = croppedHeight;
    
    mappedTex.drawSubsection(pos.x, pos.y, w, h, cropStart.x, cropStart.y, w, h);
    
}

void Camera::drawCropSquare(){
    
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(1.0);
    ofSetColor(croppingCol);
    ofDrawRectangle(cropStart, croppedWidth, croppedHeight);
    
    int tickLength = 10;
    ofSetLineWidth(2.0);
    //top middle
    ofDrawLine(cropStart.x + croppedWidth/2, cropStart.y, cropStart.x + croppedWidth/2, cropStart.y + tickLength);
    //right middle
    ofDrawLine(cropStart.x + croppedWidth, cropStart.y + croppedHeight/2, cropStart.x + croppedWidth - tickLength, cropStart.y + croppedHeight/2);
    //bottom middle
    ofDrawLine(cropStart.x + croppedWidth/2, cropStart.y + croppedHeight, cropStart.x + croppedWidth/2, cropStart.y + croppedHeight - tickLength);
    //left middle
    ofDrawLine(cropStart.x, cropStart.y + croppedHeight/2, cropStart.x + tickLength, cropStart.y + croppedHeight/2);
    //center cross hair
    ofDrawLine(cropStart.x + croppedWidth/2 - tickLength/2, cropStart.y + croppedHeight/2, cropStart.x + croppedWidth/2 + tickLength/2, cropStart.y + croppedHeight/2);
    ofDrawLine(cropStart.x + croppedWidth/2, cropStart.y + croppedHeight/2 - tickLength/2, cropStart.x + croppedWidth/2, cropStart.y + croppedHeight/2 + tickLength/2);

    ofPopStyle();
    
    
}



void Camera::gatherOscStats(){
    
    //get together all the average blob data for this corridor
    avgDir.set(0);
    avgVel.set(0);
    avgPos.set(0);
    avgSpeed = 0;
    density = 0;
    
    corridorBundle.clear();
    corridorStartFlag.clear();
    corridorStats.clear();
    blobsBundle.clear();
    

    
    //corridors 2, 3, 4, 5 are:
    //cameras 6, 7, 8, 9 (zero-indexed)
    int thisCorridor = thisView - 4;  //i.e. cam 6 = corr 2
    
    
    
    //set the corridor start flag message
    corridorStartFlag.setAddress("/corridor_" + ofToString(thisCorridor) + "/start_blobs");
    blobsBundle.addMessage(corridorStartFlag);
    
    //go through the blobs and start collecting data
    
    int numberOfContours;
    
    if(cameraGui.useBlobFilter){
        numberOfContours = filteredContours.size();
    } else {
        numberOfContours = contours.size();
    }

    
    //go through the blobs and start collecting data
    for(int i = 0; i < numberOfContours; i++){
        
        //prepare data containers
        int label;
        ofPoint center;
        ofPoint centerNormalized;
        ofVec2f velocity;
        bool still;
        
        //get the right data
        if(cameraGui.useBlobFilter){
            
            //get data from the BlobFilter
            label = filteredContours.getLabel(i);
            center = filteredContours.getCenter(i);
            velocity = filteredContours.getVelocity(i);
            still = filteredContours.getStill(i);
            
        } else {
            
            //get data from ContourFinder
            label = contours.getLabel(i);
            center = toOf(contours.getCenter(i));
            velocity = toOf(contours.getVelocity(i));
            still = false;
            
        }
        
        centerNormalized = center/ofVec2f(scaledWidth, scaledHeight);
        
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
        m.addFloatArg(still ? 1.0f : 0.0f);
        
        blobsBundle.addMessage(m);
        
    }
    
    //average out all the data for this corridor
    if(numberOfContours > 0){
        avgSpeed = avgSpeed/float(numberOfContours);
        avgVel = avgVel/float(numberOfContours);
        avgPos = avgPos/float(numberOfContours);
    } else {
        avgSpeed = 0;
        avgVel.set(0);
        avgPos.set(0);
    }
    density = numberOfContours;
    avgDir = avgVel.getNormalized();
    

    
    //prepare the corridor stats message
    corridorStats.setAddress("/corridor_" + ofToString(thisCorridor) + "/stats");
    corridorStats.addIntArg(numberOfContours);
    corridorStats.addFloatArg(avgPos.x/(float)scaledWidth);
    corridorStats.addFloatArg(avgPos.y/(float)scaledHeight);
    corridorStats.addFloatArg(avgDir.x);
    corridorStats.addFloatArg(avgDir.y);
    corridorStats.addFloatArg(avgSpeed);
    
    //now assemble the corridor bundle from the stats message and all the blobs
//    corridorBundle.addMessage(corridorStartFlag);
    corridorBundle.addBundle(blobsBundle);
    corridorBundle.addMessage(corridorStats);
    
    
}


void Camera::saveAllSettings(){
    
    
    cameraGui.saveSettings();
    
    if(soloCam){

        maskImg.setFromPixels(maskImg);
        maskImg.save(maskFileName);

    }
    
    
}




void Camera::mousePressed(ofMouseEventArgs &args){
    
    //mapping the quad
    if(manipulationMode == 0){
        
        //handle mouse interaction with Map points
        for(int i = 0; i < imageMapPts.size(); i++){
            
            //need to adjust mouse position since we're
            //translating the raw camera view from the origin with pushmatrix
            float dist = ofDist(adjustedMouse.x, adjustedMouse.y, imageMapPts[i].x, imageMapPts[i].y);
            
            if(dist < mapPtRad){
                mapPtMouseLock[i] = true;
                
                //exit the for loop, prevents us from
                //grabbing multiple handles at once
                break;
                
            } else {
                mapPtMouseLock[i] = false;
            }
            
            
        }
        
    } else {
        
        //cropping
        if(!soloCam){
        
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
            
            
        //Masking
        } else {
            
            if(adjustedMouse.x > 0 && adjustedMouse.x < scaledWidth && adjustedMouse.y > 0 && adjustedMouse.y < scaledHeight){
                
                maskPressed = true;
                
                
            }
            
        }
        
        
    }
    
}

void Camera::mouseReleased(ofMouseEventArgs &args){
    
    for(int i = 0; i < mapPtMouseLock.size(); i++){
        mapPtMouseLock[i] = false;
    }
    
    
    cropStartLock = false;
    cropEndLock = false;
    maskPressed = false;
    
}

void Camera::mouseMoved(ofMouseEventArgs &args){}
void Camera::mouseDragged(ofMouseEventArgs & args){}
void Camera::mouseScrolled(ofMouseEventArgs & args){}
void Camera::mouseEntered(ofMouseEventArgs & args){}
void Camera::mouseExited(ofMouseEventArgs & args){}

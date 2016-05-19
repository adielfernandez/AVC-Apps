//
//  BlobFilter.cpp
//  Master_CV_App
//
//  Created by TERRELBY on 5/10/16.
//
//

#include "BlobFilter.hpp"


using namespace ofxCv;


BlobFilter::BlobFilter(){
    
    
}


void BlobFilter::setup(ofxCv::ContourFinder *_rawContours, string _corridorName){
    
    rawContours = _rawContours;
    corridorName = _corridorName;

    
    personRadius = 20;
    stillTimeout = 2000;
    
    numControlPts = 11;
    
    //Input holds the pixel value of the blob center
    controlPtsInput.resize(numControlPts);
    
    //Output holds the real point in space normalized from 0-1
    controlPtsOutput.resize(numControlPts);
    
    if(corridorName == "Lobby1Aggregate"){
        
        controlPtsInput[0] = 21;
        controlPtsOutput[0] = 0.0f;

        controlPtsInput[1] = 92;
        controlPtsOutput[1] = 0.1f;
        
        controlPtsInput[2] = 216;
        controlPtsOutput[2] = 0.2f;
        
        controlPtsInput[3] = 305;
        controlPtsOutput[3] = 0.3f;
        
        controlPtsInput[4] = 347;
        controlPtsOutput[4] = 0.4f;
        
        controlPtsInput[5] = 454;
        controlPtsOutput[5] = 0.5f;
        
        controlPtsInput[6] = 579;
        controlPtsOutput[6] = 0.6f;
        
        controlPtsInput[7] = 628;  //was 618
        controlPtsOutput[7] = 0.7f;
        
        controlPtsInput[8] = 694;   //was 684
        controlPtsOutput[8] = 0.8f;
        
        controlPtsInput[9] = 836;   //was 819
        controlPtsOutput[9] = 0.9f;

        controlPtsInput[10] = 902;
        controlPtsOutput[10] = 1.0f;
        
    } else if(corridorName == "Lobby2Aggregate"){
        
        controlPtsInput[0] = 0.0f;
        controlPtsOutput[0] = 0.0f;
        
        controlPtsInput[1] = 0.0f;
        controlPtsOutput[1] = 0.1f;
        
        controlPtsInput[2] = 0.0f;
        controlPtsOutput[2] = 0.2f;
        
        controlPtsInput[3] = 0.0f;
        controlPtsOutput[3] = 0.3f;
        
        controlPtsInput[4] = 0.0f;
        controlPtsOutput[4] = 0.4f;
        
        controlPtsInput[5] = 0.0f;
        controlPtsOutput[5] = 0.5f;
        
        controlPtsInput[6] = 0.0f;
        controlPtsOutput[6] = 0.6f;
        
        controlPtsInput[7] = 0.0f;
        controlPtsOutput[7] = 0.7f;
        
        controlPtsInput[8] = 0.0f;
        controlPtsOutput[8] = 0.8f;
        
        controlPtsInput[9] = 0.0f;
        controlPtsOutput[9] = 0.9f;
        
        controlPtsInput[10] = 0.0f;
        controlPtsOutput[10] = 1.0f;
        
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
}

void BlobFilter::update(int _personRadius, int _stillTimeout, float _speedThresh, int _width, int _height){
    
    personRadius = _personRadius;
    stillTimeout = _stillTimeout;
    speedThresh = _speedThresh;
    
    scaledWidth = _width;
    scaledHeight = _height;
    
    /*
     * Filtering workflow:
     * -------------------
     *
     * 1) Go through the contours, starting with number 0,
     * 2) Look for other blobs that are "personRadius" away.
            2a) If none found, add it to the vectors
     * 3) If one is found, store the index in foundBlobs vector and
     * 4) Look for more that are close to any of the previously found "foundBlobs".
     * 5) After finding all blobs that are close to each other, add them to the vectors
     * 6) Repeat and look for other blobs that HAVENT been found yet (i.e. arent in foundBlobs)
     *
     */
    
    
    //prepare to be filled again
//    processedBlobs.clear();
    
    //temporary blob vector to hold new blobs before merging with the processedBlobs
    tempBlobs.clear();
    
    
    //this vector will store the indices of blobs inside the contour finder that
    //have been processed.
    vector<int> foundBlobs;
    
    
    
    //"newBlobStart" will hold the next slot to be used by the processed blobs so
    //we know where the next blob will begin:
    //
    //Example: 3 blobs found to be close and will represent one "processed blob"
    //Their indices are stored:
    //foundBlobs[0] = 3;
    //foundBlobs[1] = 7;
    //foundBlobs[2] = 8;
    //
    //So... newBlobStart = 3
    
    int newBlobStart = 0;
    
    
    //go through the contours and start looking for blobs
    
    for(int i = 0; i < rawContours -> size(); i++){
        
        //holds the number of subBlobs inside one processed blob
        int numSubBlobs = 0;
        
        //go through the found blobs to see if we've found this one
        //already in the nested loop below
        if(std::find(foundBlobs.begin(), foundBlobs.end(), i) == foundBlobs.end()){
            
            //if we're here we haven't found it so it's new!
            
            //Since it's new, store it
            foundBlobs.push_back(i);
            numSubBlobs++;
            
//            cout << "Found a main blob - Pos: " << rawContours -> getCenter(i) <<  endl;
            
            
            //get ready to find subBlobs...
            //this will be set to true if a new sub blob is found,
            //keeping it in the do/while loop
            bool bNewSubBlob;
            
            do{

                bNewSubBlob = false;
                
                //go through all blobs...
                for(int j = 0; j < rawContours -> size(); j++){
                    
                    //... that haven't been found yet
                    if(std::find(foundBlobs.begin(), foundBlobs.end(), j) == foundBlobs.end()){
                        
                        //if we're inside here, j is a new index
                        //that hasn't been dealt with before
                        
                        //check it against the last one we found
                        int lastFound = foundBlobs[foundBlobs.size() - 1];
                        
                        float distSq = ofDistSquared(rawContours -> getCenter(lastFound).x, rawContours -> getCenter(lastFound).y, rawContours -> getCenter(j).x, rawContours -> getCenter(j).y);
                        
                        //check if these two blobs are close
                        if(distSq < personRadius * personRadius){
                            
                            //if so, add it to foundBlobs
                            foundBlobs.push_back(j);
                            numSubBlobs++;
//                            cout << "Found a sub blob - Pos: " << rawContours -> getCenter(j) << endl;
                            
                            //indicate that we've found a new subBlob
                            bNewSubBlob = true;
                            
                        }
                        
                    }
                    
                }
                
                //If we've found any new subBlobs, we need to go through
                //again and check for any new blobs against this new one.
            }while(bNewSubBlob);
            
            //at this point we've gathered all the blobs that are close to i
            //(and the subBlobs that are close to those) so group them together
            int lowestID = 10000000;
            int lowestIDIndex = 0;
            ofVec2f avgVel(0);
            ofVec2f avgPos(0);
            ofVec2f oldestPos(0);
            
            vector<ofVec2f> thisSubBlob;
            
            //Go through all the indices in the foundBlobs, starting at newBlobStart
            for(int b = newBlobStart; b < newBlobStart + numSubBlobs; b++){
            
                //-----label-----
                //Grab the lowest one (oldest)
                if(lowestID > rawContours -> getLabel(foundBlobs[b])){
                    lowestID = rawContours -> getLabel(foundBlobs[b]);
                    lowestIDIndex = b;
                }
                
                //-----Position-----
                ofVec2f thisPos = toOf(rawContours -> getCenter(foundBlobs[b]));
                avgPos += thisPos;
                
                //-----velocity-----
                avgVel += toOf(rawContours -> getVelocity(foundBlobs[b]));
             
                //-----sub blob positions-----
                //(for drawing later)
                thisSubBlob.push_back(thisPos);
            }
            
            //Finish averaging
            avgPos /= (float)numSubBlobs;
            avgVel /= (float)numSubBlobs;
            
            oldestPos = toOf(rawContours -> getCenter(lowestIDIndex));
            
            //create a ProcessedBlob and push it into the vector
            ProcessedBlob p;
            
            p.ID = lowestID;
            
//            p.center = avgPos;
            p.center = oldestPos;
            p.centerNorm = oldestPos/ofVec2f(scaledWidth, scaledHeight);
            p.vel = avgVel;
            p.subBlobs = thisSubBlob;
            tempBlobs.push_back(p);
            
            
            //Keep track of where the next processed blob will start
            newBlobStart += numSubBlobs;
            
            
            
            
        } // if( i is unique ) statement

        
    } //main rawContours for loop


    
    //now go through the new blobs and compare them with the master blobs
    //If:
    //temp has ID that master doesnt, add it
    //master has ID that temp doesnt, delete it
    //if they both share a ID, update the position, velocity
    
    //check if processedBlobs has an ID that temp doesnt
    //do deletions first to avoid bad access errors
    for(vector<ProcessedBlob>::iterator it = processedBlobs.begin(); it != processedBlobs.end();){
        
        bool found = false;
        
        for(int i = 0; i < tempBlobs.size(); i++){
         
            if(tempBlobs[i].ID == (*it).ID){
                //we've found a match, erase it and break out of the tempBlobs loop
                found  = true;
            }
            
        }

        //if we've checked ALL the tempBlobs and the processedBlob isn't
        //found anywhere in the new tempBlobs vector delete it
        if(!found){
            
            it = processedBlobs.erase(it);
        
        } else {
            
            //if we did find a match we'll update/add it later just move on for now
            ++it;
            
        }
        
    }
    
    
    
    //now do the adding and updating
    for(int i = 0; i < tempBlobs.size(); i++){
        
        //go through all the processedBlobs and see if this ID is in there
        bool newBlob = true;
        
        //this will hold the index of the processedBlob that shares this ID so we can update it
        int whichOne = -1;
        
        for(int j = 0; j < processedBlobs.size(); j++){
            
            if(tempBlobs[i].ID == processedBlobs[j].ID){
                //this is a new blob
                newBlob = false;
                whichOne = j;

            }
            
        }
        
        //if it's a new blob we'll update it, if it's not new we'll add it whole
        if(newBlob){
            ProcessedBlob pB;
            
            pB.ID = tempBlobs[i].ID;
            pB.center = tempBlobs[i].center;
            pB.centerNorm = tempBlobs[i].centerNorm;
            pB.vel = tempBlobs[i].vel;
            pB.subBlobs = tempBlobs[i].subBlobs;
            pB.lastMoveTime = ofGetElapsedTimeMillis();
            
            processedBlobs.push_back(pB);

        } else {
            
            //update center, velocity, subBlobs and check if the still bool needs to flip
            processedBlobs[whichOne].center = tempBlobs[i].center;
            processedBlobs[whichOne].centerNorm = tempBlobs[i].centerNorm;
            
            //straight replacement of old velocity
//            processedBlobs[whichOne].vel = tempBlobs[i].vel;
            
            //averaging of old with new velocity
            processedBlobs[whichOne].vel = (tempBlobs[i].vel + processedBlobs[whichOne].vel)/2.0f;
            
            //get new subBlobs vector
            processedBlobs[whichOne].subBlobs = tempBlobs[i].subBlobs;
            
            //if we're not moving
            if(processedBlobs[whichOne].vel.lengthSquared() < speedThresh * speedThresh){

                //check the time and see if we need to time out
                if(ofGetElapsedTimeMillis() - processedBlobs[whichOne].lastMoveTime > stillTimeout){
                    processedBlobs[whichOne].still = true;
                }

            } else {

                //if we're not still
                processedBlobs[whichOne].still = false;

                //update the lastMoveTime so it stops once we've stopped moving
                processedBlobs[whichOne].lastMoveTime = ofGetElapsedTimeMillis();
                
            }

        }
        
    }
    
    
    //Blobs are updated and new, now we need to go through them and re-map
    //their positions according to the control points we set along the way.
    
    if(corridorName == "Lobby1Aggregate"){
        for(int i = 0; i < processedBlobs.size(); i++){
            
            //find the X value of the blob and find which zone it's in
            //then remap according to points it falls between
            int leftIndex = 0;
            int rightIndex = 1;
            
            for(int j = 0; j < numControlPts; j++){
                
                //find the control point to the left
                if(processedBlobs[i].center.x > controlPtsInput[j]){
                    leftIndex = j;
                    rightIndex = j + 1;
                }
            }
            
            //now we have the bounds so remap (and clamp)
            float newX = ofMap(processedBlobs[i].center.x, controlPtsInput[leftIndex], controlPtsInput[rightIndex], controlPtsOutput[leftIndex], controlPtsOutput[rightIndex], true);
            
            //Put the mapped value back in the blob
            processedBlobs[i].centerNorm.x = newX;
            
            //Since we're here, scale it up for the non normalized value
            processedBlobs[i].center.x = newX * scaledWidth;
            
            
        }
    }
    
}

void BlobFilter::setDrawInfo(bool draw){
    
    drawInfo = draw;
    
}

void BlobFilter::draw(){

    for(int i = 0; i < processedBlobs.size(); i++){
        
        //draw sub blobs 
        auto subBlobs = processedBlobs[i].subBlobs;
        
        for(int j = 0; j < subBlobs.size(); j++){
            
            //radius
            ofSetColor(0, 200, 255, 180);
            ofNoFill();
            ofSetLineWidth(1.0);
            ofDrawCircle(subBlobs[j].x, subBlobs[j].y, personRadius);
            
            //center
            ofFill();
            ofDrawCircle(subBlobs[j].x, subBlobs[j].y, 3);
            
            //draw lines between them
            if(j > 0){
                ofSetColor(0, 200 * 0.8, 255 * 0.8);
                ofSetLineWidth(2.0);
                ofDrawLine(subBlobs[j - 1].x, subBlobs[j - 1].y, subBlobs[j].x, subBlobs[j].y);
            }
            
            
        }
        
        //draw centroid in yellow if moving, red if still
        if(processedBlobs[i].still){
            ofSetColor(255, 0, 0, 255);
        } else {
            ofSetColor(255, 200, 0, 255);
        }
        
        
        ofDrawCircle(processedBlobs[i].center.x, processedBlobs[i].center.y, 6);
        
        if(drawInfo){
            //draw their info
            string info;
            info += "ID: " + ofToString(processedBlobs[i].ID) + "\n";
            
            
            //Pixel position
            info += "Pos: " + ofToString(processedBlobs[i].center) + "\n";
            
            //Normalized position (with fixed precision)
            stringstream posX;
            posX << fixed << setprecision(2) << processedBlobs[i].centerNorm.x;
            
            stringstream posY;
            posY << fixed << setprecision(2) << processedBlobs[i].centerNorm.y;
            
            string posString = posX.str() + ", " + posY.str();
            
            info += "Pos: " + posString + "\n";
            
            
            //velocity with fixed precision (for easier reading)
            
            stringstream velX;
            velX << fixed << setprecision(2) << processedBlobs[i].vel.x;
            
            stringstream velY;
            velY << fixed << setprecision(2) << processedBlobs[i].vel.y;
            
            string velString = velX.str() + ", " + velY.str();
            
            
            info += "Vel: " + velString + "\n";
            
            ofSetColor(255);
            ofDrawBitmapString(info, processedBlobs[i].center.x + 5, processedBlobs[i].center.y - 5);
        }
    }

}


int BlobFilter::size(){
    
    return processedBlobs.size();
    
}

int BlobFilter::getLabel(int i){
    
    return processedBlobs[i].ID;
    
}

ofPoint BlobFilter::getCenter(int i){
    
    return processedBlobs[i].center;
    
}

ofPoint BlobFilter::getCenterNormalized(int i){
    
    return processedBlobs[i].centerNorm;
    
}

ofVec2f BlobFilter::getVelocity(int i){
    
    return processedBlobs[i].vel;
    
}

bool BlobFilter::getStill(int i){
    
    return processedBlobs[i].still;
    
}










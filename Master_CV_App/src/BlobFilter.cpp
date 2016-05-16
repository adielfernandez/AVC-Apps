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


void BlobFilter::setup(ofxCv::ContourFinder *_rawContours){
    
    rawContours = _rawContours;
    
    personRadius = 20;
    
}

void BlobFilter::update(int rad){
    
    personRadius = rad;
    
    
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
    processedBlobs.clear();
    
    
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
    
    
    //go through the contours and start looking for blobgs
    
    for(int i = 0; i < rawContours -> size(); i++){
        
        //holds the number of subBlobs inside one processed blob
        int numSubBlobs = 0;
        
        
        //Determines if we'll do anything with this i value
        bool bNewBaseBlob = true;
        
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
            ofVec2f avgVel(0);
            ofVec2f avgPos(0);
            
            vector<ofVec2f> thisSubBlob;
            
            //Go through all the indices in the foundBlobs, starting at newBlobStart
            for(int b = newBlobStart; b < newBlobStart + numSubBlobs; b++){
            
                //-----label-----
                //Grab the lowest one (oldest)
                if(lowestID > rawContours -> getLabel(foundBlobs[b]))
                    lowestID = rawContours -> getLabel(foundBlobs[b]);
                
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
            
            //create a ProcessedBlob and push it into the vector
            ProcessedBlob p;
            
            p.ID = lowestID;
            
            p.center = avgPos;
//            cout << "newBlobStart = " << newBlobStart << ", numSubBlobs = " << numSubBlobs << endl;
//            cout << "Adding " << p.center << " to processedBlobs\n" << endl;
            
            
            p.vel = avgVel;
            p.subBlobs = thisSubBlob;
            
            processedBlobs.push_back(p);
            
            
            //Keep track of where the next processed blob will start
            newBlobStart += numSubBlobs;
            
            
            
            
        } // if( i is unique ) statement

        
    } //main rawContours for loop


    
    
    
    
    
    
    
    
    
    
    
    
    
    
}


void BlobFilter::draw(){

    for(int i = 0; i < processedBlobs.size(); i++){

        //draw centroid in yellow
        ofSetColor(255, 200, 0, 255);
        ofDrawCircle(processedBlobs[i].center.x, processedBlobs[i].center.y, 6);
        
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

ofVec2f BlobFilter::getVelocity(int i){
    
    return processedBlobs[i].vel;
    
}












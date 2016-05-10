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

void BlobFilter::process(){
    
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
    
    
    
    //clear out the local data and prepare to fill it
    //with new processed blob data
    labels.clear();
    centers.clear();
    velocities.clear();
    
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
            
            
            //get ready to find subBlobs, this will be set to true
            //if a new sub blob is found, keeping it in the do/while loop
            bool bNewSubBlob = false;
            
            do{
                
                //go through all blobs...
                for(int j = 0; j < rawContours -> size(); j++){
                    
                    //... that haven't been found yet
                    if(std::find(foundBlobs.begin(), foundBlobs.end(), j) == foundBlobs.end()){
                        
                        //if we're inside here, j is a new index
                        //that hasn't been dealt with before
                        
                        //check it agains the last one we found
                        int lastFound = foundBlobs[foundBlobs.size() - 1];
                        
                        float distSq = ofDistSquared(rawContours -> getCenter(lastFound).x, rawContours -> getCenter(lastFound).y, rawContours -> getCenter(j).x, rawContours -> getCenter(j).y);
                        
                        //check if these two blobs are close
                        if(distSq < personRadius * personRadius){
                            
                            //if so, add it to foundBlobs
                            foundBlobs.push_back(j);
                            numSubBlobs++;
                            
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
            
            //Go through all the indices in the foundBlobs, starting at newBlobStart
            for(int b = newBlobStart; b < newBlobStart + numSubBlobs; b++){
            
                //-----label-----
                //Grab the lowest one (oldest)
                
                
            }
            
            
            //Keep track of where the next processed blob will start
            newBlobStart = numSubBlobs;
            
            
            
            
        } // if( i is unique ) statement

        
    } //main rawContours for loop



    
    
    
    
    
    
    
    
}

int BlobFilter::size(){
    
    int num = 0;
    
    //they should be the same, but just in case only send the smallest of them
    //so there are no bad access errors when we loop through them later
    num = MIN(labels.size(), centers.size());
    num = MIN(num, velocities.size());
    
    return num;
    
}

ofPoint BlobFilter::getCenter(int i){
    
    if(i > 0 && i < centers.size()){
        return centers[i];
    } else {
        return ofPoint(0, 0);
    }
    
}

ofVec2f BlobFilter::getVelocity(int i){
    
    
    if(i > 0 && i < velocities.size()){
        return velocities[i];
    } else {
        return ofVec2f(0, 0);
    }
    
    
    
}












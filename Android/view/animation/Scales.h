//
//  Scales.h
//  cocos2dx
//
//  Created by Saul Howard on 12/12/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Scales_h
#define cocos2dx_Scales_h

#include "AndroidMacros.h"

#include "Android/animation/TimeInterpolator.h"

#include <vector>

using namespace std;

ANDROID_BEGIN

class LinearScale {
    
private:
    
    TimeInterpolator *mInterpolator;
    vector<vector<float>> mRanges;
    vector<vector<float>> mDomains;
    
public:
    
    LinearScale() {}
    
    ~LinearScale() {
        delete mInterpolator;
    }
    
    LinearScale(vector<float> range, vector<float> domain, TimeInterpolator *interpolator) :
        LinearScale(vector<vector<float>>({range}), vector<vector<float>>({domain}), interpolator) {
    }
    
    LinearScale(vector<vector<float>> ranges, vector<vector<float>> domains, TimeInterpolator *interpolator) {
        mRanges = ranges;
        mDomains = domains;
        mInterpolator = interpolator;
    }
    
    float scale(float value) {
        
        for (int i = 0; i < mRanges.size(); i++) {
            vector<float> range = mRanges[i];
            
            if (range.size() == 2 && value >= range[0] && value < range[1]) {
                float percent = (value - range[0]) / (range[1] - range[0]);
                float domain = mDomains[i][1] - mDomains[i][0];
                return mInterpolator->getInterpolation(percent) * domain + mDomains[i][0];
            }
        }
        
        return 0;
    }
};

ANDROID_END

#endif

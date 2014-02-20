//
//  Directions.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Directions_h
#define Androidpp_Directions_h

#include "AndroidMacros.h"

#include <vector>

using namespace std;

ANDROID_BEGIN

/**
 * Stores information about bidirectional (left-to-right or right-to-left)
 * text within the layout of a line.
 */
class Directions {
    
public:
    
    Directions() {
        mDirections = vector<int>();
    }
    
    // Directions represents directional runs within a line of text.
    // Runs are pairs of ints listed in visual order, starting from the
    // leading margin.  The first int of each pair is the offset from
    // the first character of the line to the start of the run.  The
    // second int represents both the length and level of the run.
    // The length is in the lower bits, accessed by masking with
    // DIR_LENGTH_MASK.  The level is in the higher bits, accessed
    // by shifting by DIR_LEVEL_SHIFT and masking by DIR_LEVEL_MASK.
    // To simply test for an RTL direction, test the bit using
    // DIR_RTL_FLAG, if set then the direction is rtl.
    
    vector<int> mDirections;
    
    Directions(vector<int> dirs) {
        mDirections = dirs;
    }
    
    inline bool operator== (Directions &rhs) {
        return mDirections == rhs.mDirections;
    }
    
    inline bool operator!= (Directions &rhs) {
        return !(*this == rhs);
    }
    
    static const int DIR_LEFT_TO_RIGHT = 1;
    static const int DIR_RIGHT_TO_LEFT = -1;
    
    static Directions DIRS_ALL_LEFT_TO_RIGHT;
    static Directions DIRS_ALL_RIGHT_TO_LEFT;
    
    static const int DIR_REQUEST_LTR = 1;
    static const int DIR_REQUEST_RTL = -1;
    static const int DIR_REQUEST_DEFAULT_LTR = 2;
    static const int DIR_REQUEST_DEFAULT_RTL = -2;
    
    static const int RUN_LENGTH_MASK = 0x03ffffff;
    static const int RUN_LEVEL_SHIFT = 26;
    static const int RUN_LEVEL_MASK = 0x3f;
    static const int RUN_RTL_FLAG = 1 << RUN_LEVEL_SHIFT;
};

ANDROID_END

#endif

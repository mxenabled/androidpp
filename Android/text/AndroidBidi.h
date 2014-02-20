//
//  AndroidBidi.h
//  Androidpp
//
//  Created by Saul Howard on 1/23/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_AndroidBidi_h
#define Androidpp_AndroidBidi_h

#include "AndroidMacros.h"

#include "Android/text/Directions.h"
#include "Android/utils/Exceptions.h"

#include <unicode/ubidi.h>
#include <unicode/unistr.h>

#include <string>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class AndroidBidi {
    
public:
    
    static int runBidi(int dir, UnicodeString str, vector<int8_t> &info, int n, bool haveInfo) {
        
        int result = 0;
        
        UErrorCode status = U_ZERO_ERROR;
        UBiDi* bidi = ubidi_openSized(n, 0, &status);
        ubidi_setPara(bidi, str.getBuffer(), n, dir, NULL, &status);
        if (U_SUCCESS(status)) {
            for (int i = 0; i < n; ++i) {
                info[i] = ubidi_getLevelAt(bidi, i);
            }
            result = ubidi_getParaLevel(bidi);
        } else {
            throw Exception();
        }
        ubidi_close(bidi);
        
        return result;
    }
    
    static int bidi(int dir, UnicodeString chs, vector<int8_t> &chInfo, int n, bool haveInfo) {
        
        if (n < 0 || chs.length() < n || chInfo.size() < n) {
            throw IndexOutOfBoundsException();
        }
        
        switch(dir) {
            case Directions::DIR_REQUEST_LTR: dir = 0; break;
            case Directions::DIR_REQUEST_RTL: dir = 1; break;
            case Directions::DIR_REQUEST_DEFAULT_LTR: dir = -2; break;
            case Directions::DIR_REQUEST_DEFAULT_RTL: dir = -1; break;
            default: dir = 0; break;
        }
        
        int result = runBidi(dir, chs, chInfo, n, haveInfo);
        result = (result & 0x1) == 0 ? Directions::DIR_LEFT_TO_RIGHT : Directions::DIR_RIGHT_TO_LEFT;
        return result;
    }
    
    /**
     * Returns run direction information for a line within a paragraph.
     *
     * @param dir base line direction, either Layout.DIR_LEFT_TO_RIGHT or
     *     Layout.DIR_RIGHT_TO_LEFT
     * @param levels levels as returned from {@link #bidi}
     * @param lstart start of the line in the levels array
     * @param chars the character array (used to determine whitespace)
     * @param cstart the start of the line in the chars array
     * @param len the length of the line
     * @return the directions
     */
    static Directions directions(int dir, vector<int8_t> levels, int lstart,
                                        UnicodeString chars, int cstart, int len) {
        
        int baseLevel = dir == Directions::DIR_LEFT_TO_RIGHT ? 0 : 1;
        int curLevel = levels[lstart];
        int minLevel = curLevel;
        int runCount = 1;
        for (int i = lstart + 1, e = lstart + len; i < e; ++i) {
            int level = levels[i];
            if (level != curLevel) {
                curLevel = level;
                ++runCount;
            }
        }
        
        // add final run for trailing counter-directional whitespace
        int visLen = len;
        if ((curLevel & 1) != (baseLevel & 1)) {
            // look for visible end
            while (--visLen >= 0) {
                UChar32 ch = chars.char32At(cstart + visLen);
                
                if (ch == '\n') {
                    --visLen;
                    break;
                }
                
                if (ch != ' ' && ch != '\t') {
                    break;
                }
            }
            ++visLen;
            if (visLen != len) {
                ++runCount;
            }
        }
        
        if (runCount == 1 && minLevel == baseLevel) {
            // we're done, only one run on this line
            if ((minLevel & 1) != 0) {
                return Directions::DIRS_ALL_RIGHT_TO_LEFT;
            }
            return Directions::DIRS_ALL_LEFT_TO_RIGHT;
        }
        
        vector<int> ld = vector<int>(runCount * 2);
        int maxLevel = minLevel;
        int levelBits = minLevel << Directions::RUN_LEVEL_SHIFT;
        {
            // Start of first pair is always 0, we write
            // length then start at each new run, and the
            // last run length after we're done.
            int n = 1;
            int prev = lstart;
            curLevel = minLevel;
            for (int i = lstart, e = lstart + visLen; i < e; ++i) {
                int level = levels[i];
                if (level != curLevel) {
                    curLevel = level;
                    if (level > maxLevel) {
                        maxLevel = level;
                    } else if (level < minLevel) {
                        minLevel = level;
                    }
                    // XXX ignore run length limit of 2^RUN_LEVEL_SHIFT
                    ld[n++] = (i - prev) | levelBits;
                    ld[n++] = i - lstart;
                    levelBits = curLevel << Directions::RUN_LEVEL_SHIFT;
                    prev = i;
                }
            }
            ld[n] = (lstart + visLen - prev) | levelBits;
            if (visLen < len) {
                ld[++n] = visLen;
                ld[++n] = (len - visLen) | (baseLevel << Directions::RUN_LEVEL_SHIFT);
            }
        }
        
        // See if we need to swap any runs.
        // If the min level run direction doesn't match the base
        // direction, we always need to swap (at this point
        // we have more than one run).
        // Otherwise, we don't need to swap the lowest level.
        // Since there are no logically adjacent runs at the same
        // level, if the max level is the same as the (new) min
        // level, we have a series of alternating levels that
        // is already in order, so there's no more to do.
        //
        bool swap = false;
        if ((minLevel & 1) == baseLevel) {
            minLevel += 1;
            swap = maxLevel > minLevel;
        } else {
            swap = runCount > 1;
        }
        if (swap) {
            for (int level = maxLevel - 1; level >= minLevel; --level) {
                for (int i = 0; i < ld.size(); i += 2) {
                    if (levels[ld[i]] >= level) {
                        int e = i + 2;
                        while (e < ld.size() && levels[ld[e]] >= level) {
                            e += 2;
                        }
                        for (int low = i, hi = e - 2; low < hi; low += 2, hi -= 2) {
                            int x = ld[low]; ld[low] = ld[hi]; ld[hi] = x;
                            x = ld[low+1]; ld[low+1] = ld[hi+1]; ld[hi+1] = x;
                        }
                        i = e + 2;
                    }
                }
            }
        }
        return Directions(ld);
    }

};

ANDROID_END

#endif

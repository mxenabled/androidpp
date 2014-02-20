//
//  GraphicsOperations.h
//  Androidpp
//
//  Created by Saul Howard on 1/16/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_GraphicsOperations_h
#define Androidpp_GraphicsOperations_h

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"

#include <vector>

using namespace std;

ANDROID_BEGIN

class Canvas;
class Paint;

/**
 * Please implement this interface if your CharSequence can do quick
 * draw/measure/widths calculations from an internal array.
 * {@hide}
 */
class GraphicsOperations : public virtual CharSequence {
    
public:
    
    /**
     * Just like {@link Canvas#drawText}.
     */
    virtual void drawText(shared_ptr<Canvas> c, int start, int end, float x, float y, Paint *p) = 0;
    
    /**
     * Just like {@link Canvas#drawTextRun}.
     * {@hide}
     */
    virtual void drawTextRun(shared_ptr<Canvas> c, int start, int end, int contextStart, int contextEnd,
                     float x, float y, int flags, Paint *p) = 0;
    
    /**
     * Just like {@link Paint#measureText}.
     */
    virtual float measureText(int start, int end, Paint *p) = 0;
    
    /**
     * Just like {@link Paint#getTextWidths}.
     */
    virtual int getTextWidths(int start, int end, vector<float> widths, Paint *p) = 0;
    
    /**
     * Just like {@link Paint#getTextRunAdvances}.
     * @hide
     */
    virtual float getTextRunAdvances(int start, int end, int contextStart, int contextEnd,
                             int flags, vector<float> advances, int advancesIndex, Paint *paint) = 0;
    
    /**
     * Just like {@link Paint#getTextRunCursor}.
     * @hide
     */
    virtual int getTextRunCursor(int contextStart, int contextEnd, int flags, int offset, int cursorOpt, Paint *p) = 0;
};

ANDROID_END

#endif

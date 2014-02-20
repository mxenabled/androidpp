/*
 * File:   Paint.h
 * Author: saulhoward
 *
 * Created on October 21, 2013, 3:56 PM
 */

#ifndef PAINT_H
#define	PAINT_H

#include "AndroidMacros.h"

#include "Android/graphics/ColorFilter.h"
#include "Android/graphics/Shader.h"
#include "Android/graphics/TextLayout.h"

#include <SkPaint.h>

#include <unicode/unistr.h>

#include <sstream>
#include <string>
#include <vector>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class Typeface;

class Paint : public SkPaint {

public:
    
    enum MoveOpt {
        AFTER, AT_OR_AFTER, BEFORE, AT_OR_BEFORE, AT
    };
    
    /**
     * Convenience method for callers that want to have FontMetrics values as
     * integers.
     */
    class FontMetricsInt {
        
    public:
        
        int   top;
        int   ascent;
        int   descent;
        int   bottom;
        int   leading;
        
        virtual string toString() {
            stringstream ss;
            ss << "FontMetricsInt: top=" << top << " ascent=" << ascent <<
            " descent=" << descent << " bottom=" << bottom <<
            " leading=" << leading;

            return ss.str();
        }
    };
    
    /**
     * Flag for getTextRunAdvances indicating left-to-right run direction.
     * @hide
     */
    static const int DIRECTION_LTR = 0;
    
    /**
     * Flag for getTextRunAdvances indicating right-to-left run direction.
     * @hide
     */
    static const int DIRECTION_RTL = 1;
    
    /**
     * Option for getTextRunCursor to compute the valid cursor after
     * offset or the limit of the context, whichever is less.
     * @hide
     */
    static const int CURSOR_AFTER = 0;
    
    /**
     * Option for getTextRunCursor to compute the valid cursor at or after
     * the offset or the limit of the context, whichever is less.
     * @hide
     */
    static const int CURSOR_AT_OR_AFTER = 1;
    
    /**
     * Option for getTextRunCursor to compute the valid cursor before
     * offset or the start of the context, whichever is greater.
     * @hide
     */
    static const int CURSOR_BEFORE = 2;
    
    /**
     * Option for getTextRunCursor to compute the valid cursor at or before
     * offset or the start of the context, whichever is greater.
     * @hide
     */
    static const int CURSOR_AT_OR_BEFORE = 3;
    
    /**
     * Option for getTextRunCursor to return offset if the cursor at offset
     * is valid, or -1 if it isn't.
     * @hide
     */
    static const int CURSOR_AT = 4;
    
    /**
     * Bidi flag to detect paragraph direction via heuristics, defaulting to
     * LTR.
     *
     * @hide
     */
    static const int BIDI_DEFAULT_LTR = 0x2;
    
    int mBidiFlags = BIDI_DEFAULT_LTR;
    
    bool hasShadow = false;
    float shadowDx = 0;
    float shadowDy = 0;
    float shadowRadius = 0;
    int shadowColor = 0;
    
    Paint() :SkPaint() {}
    
    Paint(Paint::Flags flags) : Paint() {
        setFlags(flags);
        mCompatScaling = mInvCompatScaling = 1;
    }
    
    float ascent() {
        FontMetrics metrics;
        getFontMetrics(&metrics);
        return SkScalarToFixed(metrics.fAscent);
    }
    
    ColorFilter *getColorFilter() { return mColorFilter; }
    int getFontMetricsInt(shared_ptr<FontMetricsInt> fmi);
    Shader *getShader() { return mShader; }
    int getTextRunCursor(UnicodeString text, int contextStart, int contextLength,
                         int flags, int offset, int cursorOpt);
    float getTextRunAdvances(UnicodeString chars, int start, int end,
                             int contextIndex, int contextCount, int flags, vector<float> *advances,
                             int advancesIndex);
    int getTextWidths(UnicodeString text, int index, int count,
                      vector<float> widths);
    Typeface *getTypeface();
    
    float measureText(string text);
    float measureText(string text, int index, int count);
    float measureText(UnicodeString text);
    float measureText(UnicodeString text, int index, int count);
    
    void setColorFilter(ColorFilter *colorFilter);
    void setCompatibilityScaling(float factor);
    void setShader(Shader *shader);
    
    /**
     * This draws a shadow layer below the main layer, with the specified
     * offset and color, and blur radius. If radius is 0, then the shadow
     * layer is removed.
     */
    void setShadowLayer(float radius, float dx, float dy, int color);
    
private:
    
    /**
     * Maximum cursor option value.
     */
    static const int CURSOR_OPT_MAX_VALUE = CURSOR_AT;
    
    bool mHasCompatScaling = false;
    float mCompatScaling;
    float mInvCompatScaling;
    
    Shader *mShader = NULL;
    ColorFilter *mColorFilter = NULL;
    
    float getTextRunAdvancesInternal(UnicodeString chars, int index, int count,
                                     int contextIndex, int contextCount, int flags, vector<float> *advances,
                                     int advancesIndex);
    
    const UChar* getChars(UnicodeString &chars);
};

ANDROID_END

#endif	/* PAINT_H */


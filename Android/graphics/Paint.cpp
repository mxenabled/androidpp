/*
 * File:   Paint.cpp
 * Author: saulhoward
 *
 * Created on October 21, 2013, 3:56 PM
 */

#include "Paint.h"

#include "Android/utils/Exceptions.h"
#include "Android/graphics/Typeface.h"
#include "Android/utils/System.h"

#include <SkBlurDrawLooper.h>

#include <math.h>

ANDROID_BEGIN

void Paint::setColorFilter(ColorFilter *colorFilter) {
    mColorFilter = colorFilter;
}

void Paint::setShader(Shader *shader) {
    mShader = shader;
}

/**
 * Return the width of the text.
 *
 * @param text  The text to measure. Cannot be null.
 * @return      The width of the text
 */
float Paint::measureText(string text) {
    return measureText(text, 0, text.length());
}

/**
 * Return the width of the text.
 *
 * @param text  The text to measure. Cannot be null.
 * @param index The index of the first character to start measuring
 * @param count THe number of characters to measure, beginning with start
 * @return      The width of the text
 */
float Paint::measureText(string text, int index, int count) {
    return measureText(System::convert(text), index, count);
}

/**
 * Return the width of the text.
 *
 * @param text  The text to measure. Cannot be null.
 * @return      The width of the text
 */
float Paint::measureText(UnicodeString text) {
    return measureText(text, 0, text.length());
}

/**
 * Return the width of the text.
 *
 * @param text  The text to measure. Cannot be null.
 * @param index The index of the first character to start measuring
 * @param count THe number of characters to measure, beginning with start
 * @return      The width of the text
 */
float Paint::measureText(UnicodeString text, int index, int count) {
    
    if ((index | count) < 0 || index + count > text.length()) {
        throw ArrayIndexOutOfBoundsException();
    }
    
    if (text.length() == 0 || count == 0) {
        return 0.0f;
    }
    if (!mHasCompatScaling) {
        float result = 0;
        TextLayout::getTextRunAdvances(this, text.getBuffer(), index, count, text.length(), mBidiFlags, NULL, &result);
        return ceil(result);
    }
    
    const float oldSize = getTextSize();
    setTextSize(oldSize * mCompatScaling);
    float w = 0;
    TextLayout::getTextRunAdvances(this, text.getBuffer(), index, count, text.length(), mBidiFlags, NULL, &w);
    setTextSize(oldSize);
    return ceil(w * mInvCompatScaling);
}

int Paint::getFontMetricsInt(shared_ptr<FontMetricsInt> fmi) {
    FontMetrics metrics;
    
    getFontMetrics(&metrics);
    int ascent = SkScalarRound(metrics.fAscent);
    int descent = SkScalarRound(metrics.fDescent);
    int leading = SkScalarRound(metrics.fLeading);
    
    if (fmi != NULL) {
        fmi->top = SkScalarFloor(metrics.fTop);
        fmi->ascent = ascent;
        fmi->descent = descent;
        fmi->bottom = SkScalarCeil(metrics.fBottom);
        fmi->leading = leading;
    }
    
    return descent - ascent + leading;
}

/**
 * Return the advance widths for the characters in the string.
 *
 * @param text     The text to measure. Cannot be null.
 * @param index    The index of the first char to to measure
 * @param count    The number of chars starting with index to measure
 * @param widths   array to receive the advance widths of the characters.
 *                 Must be at least a large as count.
 * @return         the actual number of widths returned.
 */
int Paint::getTextWidths(UnicodeString text, int index, int count,
                         vector<float> widths) {

    if ((index | count) < 0 || index + count > text.length()
        || count > widths.size()) {
        throw ArrayIndexOutOfBoundsException();
    }
    
    if (text.length() == 0 || count == 0) {
        return 0;
    }
    if (!mHasCompatScaling) {
        TextLayout::getTextRunAdvances(this, text.getBuffer(), 0, count, count,
                                       mBidiFlags, &widths[0], NULL /* dont need totalAdvance */);
        return count;
    }
    
    const float oldSize = getTextSize();
    setTextSize(oldSize * mCompatScaling);
    int res = count;
    TextLayout::getTextRunAdvances(this, text.getBuffer(), 0, count, count,
                                   mBidiFlags, &widths[0], NULL /* dont need totalAdvance */);
    setTextSize(oldSize);
    for (int i=0; i<res; i++) {
        widths[i] *= mInvCompatScaling;
    }
    return res;
}

/**
 * Convenience overload that takes a char array instead of a
 * String.
 *
 * @see #getTextRunAdvances(String, int, int, int, int, int, float[], int)
 * @hide
 */
float Paint::getTextRunAdvances(UnicodeString chars, int start, int end,
                                int contextIndex, int contextCount, int flags, vector<float> *advances,
                                int advancesIndex) {
    
    if (flags != DIRECTION_LTR && flags != DIRECTION_RTL) {
        stringstream str;
        str << "unknown flags value: " << flags;
        throw IllegalArgumentException(str.str());
    }
    
    int count = end - start;
    
    if ((start | count | contextIndex | contextCount | advancesIndex
         | (start - contextIndex) | (contextCount - count)
         | ((contextIndex + contextCount) - (start + count))
         | (chars.length() - (contextIndex + contextCount))
         | (advances == NULL ? 0 : (advances->size() - (advancesIndex + count)))) < 0) {
             throw IndexOutOfBoundsException();
         }
    
    if (chars.length() == 0 || count == 0){
        return 0.0f;
    }
    if (!mHasCompatScaling) {
        return getTextRunAdvancesInternal(chars, start, count, contextIndex, contextCount, flags, advances, advancesIndex);
    }
    
    const float oldSize = getTextSize();
    setTextSize(oldSize * mCompatScaling);
    float res = getTextRunAdvancesInternal(chars, start, count, contextIndex, contextCount, flags, advances, advancesIndex);
    setTextSize(oldSize);
    
    if (advances != NULL) {
        for (int i = advancesIndex, e = i + count; i < e; i++) {
            (*advances)[i] *= mInvCompatScaling;
        }
    }
    
    return res * mInvCompatScaling; // assume errors are not significant
}

float Paint::getTextRunAdvancesInternal(UnicodeString chars, int index, int count,
                                int contextIndex, int contextCount, int flags, vector<float> *advances,
                                int advancesIndex) {
    float result = 0;
    if (advancesIndex == 0) {
        TextLayout::getTextRunAdvances(this, chars.getBuffer(), index, count, contextCount, flags, (advances == NULL ? NULL : &advances->at(0)), &result);
    } else {
        const UChar *uchars = getChars(chars);
        TextLayout::getTextRunAdvancesICU(this, uchars, index, count, contextCount, flags, (advances == NULL ? NULL : &advances->at(0)), result);
        delete uchars;
    }
    
    return result;
}

const UChar *Paint::getChars(UnicodeString &chars) {
    
    UChar *uchars = new UChar[chars.length()];
    
    for (int i = 0; i < chars.length(); i++) {
        uchars[i] = (UChar) chars.charAt(i);
    }
    
    return uchars;
}

/**
 * Returns the next cursor position in the run.  This avoids placing the
 * cursor between surrogates, between characters that form conjuncts,
 * between base characters and combining marks, or within a reordering
 * cluster.
 *
 * <p>ContextStart and offset are relative to the start of text.
 * The context is the shaping context for cursor movement, generally
 * the bounds of the metric span enclosing the cursor in the direction of
 * movement.
 *
 * <p>If cursorOpt is {@link #CURSOR_AT} and the offset is not a valid
 * cursor position, this returns -1.  Otherwise this will never return a
 * value before contextStart or after contextStart + contextLength.
 *
 * @param text the text
 * @param contextStart the start of the context
 * @param contextLength the length of the context
 * @param flags either {@link #DIRECTION_RTL} or {@link #DIRECTION_LTR}
 * @param offset the cursor position to move from
 * @param cursorOpt how to move the cursor, one of {@link #CURSOR_AFTER},
 * {@link #CURSOR_AT_OR_AFTER}, {@link #CURSOR_BEFORE},
 * {@link #CURSOR_AT_OR_BEFORE}, or {@link #CURSOR_AT}
 * @return the offset of the next position, or -1
 * @hide
 */
int Paint::getTextRunCursor(UnicodeString text, int contextStart, int contextLength,
                            int flags, int offset, int cursorOpt) {
    int contextEnd = contextStart + contextLength;
    if (((contextStart | contextEnd | offset | (contextEnd - contextStart)
          | (offset - contextStart) | (contextEnd - offset)
          | (text.length() - contextEnd) | cursorOpt) < 0)
        || cursorOpt > CURSOR_OPT_MAX_VALUE) {
        throw IndexOutOfBoundsException();
    }
    
    float scalarArray[contextEnd];
    TextLayout::getTextRunAdvances(this, text.getBuffer(), contextStart, contextEnd, contextEnd, flags, &scalarArray[0], NULL);
    
    int pos = offset - contextStart;
    switch (cursorOpt) {
        case AFTER:
            if (pos < contextEnd) {
                pos += 1;
            }
            // fall through
        case AT_OR_AFTER:
            while (pos < contextEnd && scalarArray[pos] == 0) {
                ++pos;
            }
            break;
        case BEFORE:
            if (pos > 0) {
                --pos;
            }
            // fall through
        case AT_OR_BEFORE:
            while (pos > 0 && scalarArray[pos] == 0) {
                --pos;
            }
            break;
        case AT:
        default:
            if (scalarArray[pos] == 0) {
                pos = -1;
            }
            break;
    }
    
    if (pos != -1) {
        pos += contextStart;
    }
    
    return pos;
}

Typeface *Paint::getTypeface() {
    return (Typeface*) SkPaint::getTypeface();
}

void Paint::setCompatibilityScaling(float factor) {
    if (factor == 1.0) {
        mHasCompatScaling = false;
        mCompatScaling = mInvCompatScaling = 1.0f;
    } else {
        mHasCompatScaling = true;
        mCompatScaling = factor;
        mInvCompatScaling = 1.0f/factor;
    }
}

/**
 * This draws a shadow layer below the main layer, with the specified
 * offset and color, and blur radius. If radius is 0, then the shadow
 * layer is removed.
 */
void Paint::setShadowLayer(float radius, float dx, float dy, int color) {
    hasShadow = radius > 0.0f;
    shadowRadius = radius;
    shadowDx = dx;
    shadowDy = dy;
    shadowColor = color;
    
    if (radius <= 0) {
        setLooper(NULL);
    }
    else {
        setLooper(new SkBlurDrawLooper(SkFloatToScalar(radius),
                                              SkFloatToScalar(dx),
                                              SkFloatToScalar(dy),
                                              (SkColor)color))->unref();
    }
}

ANDROID_END
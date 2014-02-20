//
//  Character.h
//  Androidpp
//
//  Created by Saul Howard on 1/21/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Character_h
#define Androidpp_Character_h

#include "AndroidMacros.h"

#include <unicode/unistr.h>
#include <string>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class Character {
    
public:
    
    /**
     * General category "Nd" in the Unicode specification.
     * @since   1.1
     */
    static const int8_t DECIMAL_DIGIT_NUMBER = 9;
    
    /**
     * The minimum radix available for conversion to and from strings.
     * The constant value of this field is the smallest value permitted
     * for the radix argument in radix-conversion methods such as the
     * <code>digit</code> method, the <code>forDigit</code>
     * method, and the <code>toString</code> method of class
     * <code>Integer</code>.
     *
     * @see     java.lang.Character#digit(char, int)
     * @see     java.lang.Character#forDigit(int, int)
     * @see     java.lang.Integer#toString(int, int)
     * @see     java.lang.Integer#valueOf(java.lang.String)
     */
    static const int MIN_RADIX = 2;
    
    /**
     * The maximum radix available for conversion to and from strings.
     * The constant value of this field is the largest value permitted
     * for the radix argument in radix-conversion methods such as the
     * <code>digit</code> method, the <code>forDigit</code>
     * method, and the <code>toString</code> method of class
     * <code>Integer</code>.
     *
     * @see     java.lang.Character#digit(char, int)
     * @see     java.lang.Character#forDigit(int, int)
     * @see     java.lang.Integer#toString(int, int)
     * @see     java.lang.Integer#valueOf(java.lang.String)
     */
    static const int MAX_RADIX = 36;
    
    static const int DIRECTIONALITY_UNDEFINED = -1;
    static const int DIRECTIONALITY_LEFT_TO_RIGHT = 0;
    static const int DIRECTIONALITY_RIGHT_TO_LEFT = 1;
    static const int DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC = 2;
    static const int DIRECTIONALITY_EUROPEAN_NUMBER = 3;
    static const int DIRECTIONALITY_EUROPEAN_NUMBER_SEPARATOR = 4;
    static const int DIRECTIONALITY_EUROPEAN_NUMBER_TERMINATOR = 5;
    static const int DIRECTIONALITY_ARABIC_NUMBER = 6;
    static const int DIRECTIONALITY_COMMON_NUMBER_SEPARATOR = 7;
    static const int DIRECTIONALITY_NONSPACING_MARK = 8;
    static const int DIRECTIONALITY_BOUNDARY_NEUTRAL = 9;
    static const int DIRECTIONALITY_PARAGRAPH_SEPARATOR = 10;
    static const int DIRECTIONALITY_SEGMENT_SEPARATOR = 11;
    static const int DIRECTIONALITY_WHITESPACE = 12;
    static const int DIRECTIONALITY_OTHER_NEUTRALS = 13;
    static const int DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING = 14;
    static const int DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE = 15;
    static const int DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING = 16;
    static const int DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE = 17;
    static const int DIRECTIONALITY_POP_DIRECTIONAL_FORMAT = 18;
    
    /**
     * The minimum value of a Unicode high-surrogate code unit in the
     * UTF-16 encoding. A high-surrogate is also known as a
     * <i>leading-surrogate</i>.
     *
     * @since 1.5
     */
    static const int32_t MIN_HIGH_SURROGATE = 0xD800;
    
    /**
     * The maximum value of a Unicode high-surrogate code unit in the
     * UTF-16 encoding. A high-surrogate is also known as a
     * <i>leading-surrogate</i>.
     *
     * @since 1.5
     */
    static const int32_t MAX_HIGH_SURROGATE = 0xDBFF;
    
    /**
     * The minimum value of a Unicode low-surrogate code unit in the
     * UTF-16 encoding. A low-surrogate is also known as a
     * <i>trailing-surrogate</i>.
     *
     * @since 1.5
     */
    static const int32_t MIN_LOW_SURROGATE  = 0xDC00;
    
    /**
     * The maximum value of a Unicode low-surrogate code unit in the
     * UTF-16 encoding. A low-surrogate is also known as a
     * <i>trailing-surrogate</i>.
     *
     * @since 1.5
     */
    static const int32_t MAX_LOW_SURROGATE  = 0xDFFF;
    
    /**
     * The minimum value of a supplementary code point.
     *
     * @since 1.5
     */
    static const int32_t MIN_SUPPLEMENTARY_CODE_POINT = 0x010000;
    
    /**
     * Returns the code point at the given index of the
     * <code>char</code> array. If the <code>char</code> value at
     * the given index in the <code>char</code> array is in the
     * high-surrogate range, the following index is less than the
     * length of the <code>char</code> array, and the
     * <code>char</code> value at the following index is in the
     * low-surrogate range, then the supplementary code point
     * corresponding to this surrogate pair is returned. Otherwise,
     * the <code>char</code> value at the given index is returned.
     *
     * @param a the <code>char</code> array
     * @param index the index to the <code>char</code> values (Unicode
     * code units) in the <code>char</code> array to be converted
     * @return the Unicode code point at the given index
     * @exception NullPointerException if <code>a</code> is null.
     * @exception IndexOutOfBoundsException if the value
     * <code>index</code> is negative or not less than
     * the length of the <code>char</code> array.
     * @since  1.5
     */
    static int32_t codePointAt(UnicodeString a, int index) {
        return codePointAtImpl(a, index, a.length());
    }
    
    /**
     * Converts the specified surrogate pair to its supplementary code
     * point value. This method does not validate the specified
     * surrogate pair. The caller must validate it using {@link
     * #isSurrogatePair(char, char) isSurrogatePair} if necessary.
     *
     * @param  high the high-surrogate code unit
     * @param  low the low-surrogate code unit
     * @return the supplementary code point composed from the
     *         specified surrogate pair.
     * @since  1.5
     */
    static int32_t toCodePoint(int32_t high, int32_t low) {
        return ((high - MIN_HIGH_SURROGATE) << 10)
        + (low - MIN_LOW_SURROGATE) + MIN_SUPPLEMENTARY_CODE_POINT;
    }
    
private:
    
    static int32_t codePointAtImpl(UnicodeString a, int index, int limit) {
        UChar c1 = a[index++];
        if (isHighSurrogate(c1)) {
            if (index < limit) {
                UChar c2 = a[index];
                if (isLowSurrogate(c2)) {
                    return toCodePoint(c1, c2);
                }
            }
        }
        return c1;
    }
    
public:
    
    /**
     * Determines if the given <code>char</code> value is a
     * high-surrogate code unit (also known as <i>leading-surrogate
     * code unit</i>). Such values do not represent characters by
     * themselves, but are used in the representation of <a
     * href="#supplementary">supplementary characters</a> in the
     * UTF-16 encoding.
     *
     * <p>This method returns <code>true</code> if and only if
     * <blockquote><pre>ch >= '&#92;uD800' && ch <= '&#92;uDBFF'
     * </pre></blockquote>
     * is <code>true</code>.
     *
     * @param   ch   the <code>char</code> value to be tested.
     * @return  <code>true</code> if the <code>char</code> value
     *          is between '&#92;uD800' and '&#92;uDBFF' inclusive;
     *          <code>false</code> otherwise.
     * @see     java.lang.Character#isLowSurrogate(char)
     * @see     Character.UnicodeBlock#of(int)
     * @since   1.5
     */
    static bool isHighSurrogate(int32_t ch) {
        return ch >= MIN_HIGH_SURROGATE && ch <= MAX_HIGH_SURROGATE;
    }
    
    /**
     * Determines if the given <code>char</code> value is a
     * low-surrogate code unit (also known as <i>trailing-surrogate code
     * unit</i>). Such values do not represent characters by themselves,
     * but are used in the representation of <a
     * href="#supplementary">supplementary characters</a> in the UTF-16 encoding.
     *
     * <p> This method returns <code>true</code> if and only if
     * <blockquote><pre>ch >= '&#92;uDC00' && ch <= '&#92;uDFFF'
     * </pre></blockquote> is <code>true</code>.
     *
     * @param   ch   the <code>char</code> value to be tested.
     * @return  <code>true</code> if the <code>char</code> value
     *          is between '&#92;uDC00' and '&#92;uDFFF' inclusive;
     *          <code>false</code> otherwise.
     * @see java.lang.Character#isHighSurrogate(char)
     * @since   1.5
     */
    static bool isLowSurrogate(int32_t ch) {
        return ch >= MIN_LOW_SURROGATE && ch <= MAX_LOW_SURROGATE;
    }
    
};

ANDROID_END

#endif

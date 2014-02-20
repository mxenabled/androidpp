/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#include "jni.h"


#ifndef TEXTLAYOUT_H
#define	TEXTLAYOUT_H

#include "SkCanvas.h"
#include "SkPaint.h"

#include <unicode/utypes.h>

#include "TextLayoutCache.h"

#include <memory>

using namespace std;
using namespace icu;

namespace android {

#define UNICODE_NOT_A_CHAR              0xffff
#define UNICODE_ZWSP                    0x200b
#define UNICODE_FIRST_LOW_SURROGATE     0xdc00
#define UNICODE_FIRST_HIGH_SURROGATE    0xd800
#define UNICODE_FIRST_PRIVATE_USE       0xe000
#define UNICODE_FIRST_RTL_CHAR          0x0590

/*
 * Temporary buffer size
 */
#define CHAR_BUFFER_SIZE 80

/**
 * Turn on for using the Cache
 */
#define USE_TEXT_LAYOUT_CACHE 1

enum {
    kBidi_LTR = 0,
    kBidi_RTL = 1,
    kBidi_Default_LTR = 2,
    kBidi_Default_RTL = 3,
    kBidi_Force_LTR = 4,
    kBidi_Force_RTL = 5,

    kBidi_Mask = 0x7
};

enum {
    kDirection_LTR = 0,
    kDirection_RTL = 1,

    kDirection_Mask = 0x1
};

class TextLayout {
public:

    static void getTextRunAdvances(SkPaint* paint, const UChar* chars, int start,
                                   int count, int contextCount, int dirFlags,
                                   float* resultAdvances, float* resultTotalAdvance);

    static void getTextRunAdvancesICU(SkPaint* paint, const UChar* chars, int start,
                                   int count, int contextCount, int dirFlags,
                                   float* resultAdvances, float& resultTotalAdvance);

    static void getTextPath(SkPaint* paint, const UChar* text, size_t len,
                            int bidiFlags, float x, float y, SkPath* path);

    static void drawTextOnPath(SkPaint* paint, const UChar* text, size_t len,
                               int bidiFlags, float hOffset, float vOffset,
                               SkPath* path, SkCanvas* canvas);

private:
    static bool needsLayout(const UChar* text, int len, int bidiFlags);

    static void handleText(SkPaint* paint, const UChar* text, size_t len,
                           int bidiFlags, float x, float y, SkPath* path);

    static void computeAdvancesWithICU(SkPaint* paint, const UChar* chars,
            size_t start, size_t count, size_t contextCount, int dirFlags,
            float* outAdvances, float* outTotalAdvance);
};
} // namespace android

#endif // TEXTLAYOUT_H
//
//  TextPaint.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "TextPaint.h"

ANDROID_BEGIN

TextPaint::TextPaint() : Paint() {
}

TextPaint::TextPaint(Paint::Flags flags) : Paint(flags) {
}

//TextPaint::TextPaint(Paint *p) : Paint(p) {
//}

/**
 * Defines a custom underline for this Paint.
 * @param color underline solid color
 * @param thickness underline thickness
 * @hide
 */
void TextPaint::setUnderlineText(int color, float thickness) {
    underlineColor = color;
    underlineThickness = thickness;
}

ANDROID_END
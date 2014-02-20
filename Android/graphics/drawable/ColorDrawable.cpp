//
//  ColorDrawable.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/13/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "ColorDrawable.h"

#include "Android/internal/R.h"

#include <ui/PixelFormat.h>

ANDROID_BEGIN

ColorDrawable::ColorDrawable(int color) : ColorDrawable(ColorState()) {
    setColor(color, false);
}

ColorDrawable::ColorDrawable(ColorState state) {
    m_state = state;
}

void ColorDrawable::draw(shared_ptr<Canvas> canvas) {
    if ((m_state.m_useColor >> 24) != 0) {
        m_paint.setColor(m_state.m_useColor);
        canvas->drawRect(getBounds(), &m_paint);
    }
}

Drawable::ConstantState *ColorDrawable::getConstantState() {
    m_state.m_changingConfigurations = getChangingConfigurations();
    return &m_state;
}

shared_ptr<Drawable> ColorDrawable::mutate() {
    if (!m_mutated && Drawable::mutate() == shared_from_this()) {
        m_state = ColorState(m_state);
        m_mutated = true;
    }
    return shared_from_this();
}

void ColorDrawable::setAlpha(int alpha) {
    alpha += alpha >> 7;   // make it 0..256
    int baseAlpha = m_state.m_baseColor >> 24;
    int useAlpha = baseAlpha * alpha >> 8;
    int oldUseColor = m_state.m_useColor;
    m_state.m_useColor = (m_state.m_baseColor << 8 >> 8) | (useAlpha << 24);
    if (oldUseColor != m_state.m_useColor) {
        invalidateSelf();
    }
}

void ColorDrawable::setColor(int color, bool invalidate) {
    if (m_state.m_baseColor != color || m_state.m_useColor != color) {
        m_state.m_baseColor = m_state.m_useColor = color;
        if (invalidate) {
            invalidateSelf();
        }
    }
}

void ColorDrawable::setColor(int color) {
    setColor(color, true);
}

int ColorDrawable::getOpacity() {
    switch (m_state.m_useColor >> 24) {
        case 255:
            return PIXEL_FORMAT_OPAQUE;
        case 0:
            return PIXEL_FORMAT_TRANSPARENT;
    }
    return PIXEL_FORMAT_TRANSLUCENT;
}

void ColorDrawable::inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    Drawable::inflate(r, parser, attrs);
    
    attrs->loadAttributes();
    
    int color = m_state.m_baseColor;
    color = attrs->getAttributeColorValue(r, R::styleable::color, color);
    m_state.m_baseColor = m_state.m_useColor = color;
}

ANDROID_END
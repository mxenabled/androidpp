//
//  ColorDrawable.h
//  cocos2dx
//
//  Created by Saul Howard on 11/13/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__ColorDrawable__
#define __cocos2dx__ColorDrawable__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/Paint.h"

#include "cocos2d.h"

#include <vector>
#include <memory>

using namespace std;

ANDROID_BEGIN

class ColorDrawable : public Drawable {
    
private:
    
    class ColorState : public ConstantState {
    public:
        int m_baseColor = 0; // base color, independent of setAlpha()
        int m_useColor = 0;  // basecolor modulated by setAlpha()
        int m_changingConfigurations = 0;
        
//        ColorState() {}
//        
//        ColorState(const ColorState &state) {
//            m_baseColor = state.m_baseColor;
//            m_useColor = state.m_useColor;
//            m_changingConfigurations = state.m_changingConfigurations;
//        }
        
        virtual shared_ptr<Drawable> newDrawable() {
            return make_shared<ColorDrawable>(*this);
        }
        
        virtual shared_ptr<Drawable> newDrawable(Resources *res) {
            return make_shared<ColorDrawable>(*this);
        }
        
        virtual int getChangingConfigurations() {
            return m_changingConfigurations;
        }
    };
    
    void setColor(int color, bool invalidate);
    
    ColorState m_state;
    Paint m_paint;
    bool m_mutated = false;
    
public:
    
    ColorDrawable() : ColorDrawable(ColorState()) {}
    ColorDrawable(int color);
    ColorDrawable(ColorState state);
    
    virtual int getChangingConfigurations() {
        return Drawable::getChangingConfigurations() | m_state.m_changingConfigurations;
    }
    
    virtual void draw(androidcpp::shared_ptr<Canvas> canvas);
    int getAlpha() { return m_state.m_useColor >> 24; }
    int getColor() { return m_state.m_useColor; }
    virtual Drawable::ConstantState *getConstantState();
    int getOpacity();
    virtual void inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs);
    virtual shared_ptr<Drawable> mutate();
    void setAlpha(int alpha);
    void setColor(int color);
    virtual void setColorFilter(ColorFilter *colorFilter) {}
    
};

ANDROID_END

#endif /* defined(__cocos2dx__ColorDrawable__) */

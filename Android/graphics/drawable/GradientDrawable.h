//
//  GradientDrawable.h
//  cocos2dx
//
//  Created by Saul Howard on 11/19/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__GradientDrawable__
#define __cocos2dx__GradientDrawable__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/Gradient.h"
#include "Android/graphics/Paint.h"
#include "Android/graphics/Path.h"

#include <string>
#include <memory>

using namespace std;

ANDROID_BEGIN

class GradientDrawable : public Drawable {
    
public:
    /**
     * Shape is a rectangle, possibly with rounded corners
     */
    static const int RECTANGLE = 0;
    
    /**
     * Shape is an ellipse
     */
    static const int OVAL = 1;
    
    /**
     * Shape is a line
     */
    static const int LINE = 2;
    
    /**
     * Shape is a ring.
     */
    static const int RING = 3;
    
    /**
     * Gradient is linear (default.)
     */
    static const int LINEAR_GRADIENT = 0;
    
    /**
     * Gradient is circular.
     */
    static const int RADIAL_GRADIENT = 1;
    
    /**
     * Gradient is a sweep.
     */
    static const int SWEEP_GRADIENT  = 2;
    
    /**
     * Controls how the gradient is oriented relative to the drawable's bounds
     */
    enum Orientation {
        /** draw the gradient from the top to the bottom */
        TOP_BOTTOM,
        /** draw the gradient from the top-right to the bottom-left */
        TR_BL,
        /** draw the gradient from the right to the left */
        RIGHT_LEFT,
        /** draw the gradient from the bottom-right to the top-left */
        BR_TL,
        /** draw the gradient from the bottom to the top */
        BOTTOM_TOP,
        /** draw the gradient from the bottom-left to the top-right */
        BL_TR,
        /** draw the gradient from the left to the right */
        LEFT_RIGHT,
        /** draw the gradient from the top-left to the bottom-right */
        TL_BR,
    };
    
    GradientDrawable() : GradientDrawable(GradientState(Orientation::TOP_BOTTOM, vector<int>())) {
    }
    
    /**
     * Create a new gradient drawable given an orientation and an array
     * of colors for the gradient.
     */
    GradientDrawable(Orientation orientation, vector<int> colors) : GradientDrawable(GradientState(orientation, colors)) {
    }
    
    virtual void draw(shared_ptr<Canvas> canvas);
    
    virtual int getChangingConfigurations();
    virtual ConstantState *getConstantState();
    virtual int getIntrinsicHeight();
    virtual int getIntrinsicWidth();
    virtual int getOpacity();
    Orientation getOrientation();
    virtual bool getPadding(Rect &padding);
    
    virtual void inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs);
    virtual shared_ptr<Drawable> mutate();
    
    virtual void setAlpha(int alpha);
    void setColor(int argb);
    virtual void setColorFilter(ColorFilter *cf);
    void setColors(vector<int> colors);
    void setCornerRadii(vector<float> radii);
    void setCornerRadius(float radius);
    virtual void setDither(bool dither);
    void setGradientCenter(float x, float y);
    void setGradientType(int gradient);
    void setGradientRadius(float gradientRadius);
    void setOrientation(Orientation orientation);
    void setShape(int shape);
    void setSize(int width, int height);
    void setStroke(int width, int color);
    void setStroke(int width, int color, float dashWidth, float dashGap);
    void setUseLevel(bool useLevel);
    
protected:
    
    virtual void onBoundsChange(Rect r);
    virtual bool onLevelChange(int level);
    
private:
    
    class GradientState : public ConstantState {
        
    public:
        int mChangingConfigurations = 0;
        int mShape = RECTANGLE;
        int mGradient = LINEAR_GRADIENT;
        Orientation mOrientation;
        vector<int> mColors;
        vector<int> mTempColors; // no need to copy
        vector<float> mTempPositions; // no need to copy
        vector<float> mPositions;
        bool mHasSolidColor = false;
        int mSolidColor = 0;
        int mStrokeWidth = -1;   // if >= 0 use stroking.
        int mStrokeColor = 0;
        float mStrokeDashWidth = 0.0f;
        float mStrokeDashGap = 0.0f;
        float mRadius = 0.0f;    // use this if mRadiusArray is null
        vector<float> mRadiusArray;
        Rect mPadding;
        int mWidth = -1;
        int mHeight = -1;
        float mInnerRadiusRatio = 0.0f;
        float mThicknessRatio = 0.0f;
        int mInnerRadius = 0;
        int mThickness = 0;
        
        GradientState() {}
        
        GradientState(const GradientState &state) {
            mChangingConfigurations = state.mChangingConfigurations;
            mShape = state.mShape;
            mGradient = state.mGradient;
            mOrientation = state.mOrientation;
            mColors = state.mColors;
            mPositions = state.mPositions;
            mHasSolidColor = state.mHasSolidColor;
            mSolidColor = state.mSolidColor;
            mStrokeWidth = state.mStrokeWidth;
            mStrokeColor = state.mStrokeColor;
            mStrokeDashWidth = state.mStrokeDashWidth;
            mStrokeDashGap = state.mStrokeDashGap;
            mRadius = state.mRadius;
            mRadiusArray = state.mRadiusArray;
            mPadding.set(state.mPadding);
            mWidth = state.mWidth;
            mHeight = state.mHeight;
            mInnerRadiusRatio = state.mInnerRadiusRatio;
            mThicknessRatio = state.mThicknessRatio;
            mInnerRadius = state.mInnerRadius;
            mThickness = state.mThickness;
            mCenterX = state.mCenterX;
            mCenterY = state.mCenterY;
            mGradientRadius = state.mGradientRadius;
            mUseLevel = state.mUseLevel;
            mUseLevelForShape = state.mUseLevelForShape;
            mOpaque = state.mOpaque;
        }
        
        GradientState(Orientation orientation, vector<int> colors) {
            mOrientation = orientation;
            setColors(colors);
        }
        
        virtual shared_ptr<Drawable> newDrawable() {
            return make_shared<GradientDrawable>(*this);
        }
        
        virtual shared_ptr<Drawable> newDrawable(Resources *res) {
            return make_shared<GradientDrawable>(*this);
        }
        
        virtual int getChangingConfigurations() {
            return mChangingConfigurations;
        }
        
        void setShape(int shape) {
            mShape = shape;
            computeOpacity();
        }
        
        void setGradientType(int gradient) {
            mGradient = gradient;
        }
        
        void setGradientCenter(float x, float y) {
            mCenterX = x;
            mCenterY = y;
        }
        
        void setColors(vector<int> colors) {
            mHasSolidColor = false;
            mColors = colors;
            computeOpacity();
        }
        
        void setSolidColor(int argb) {
            mHasSolidColor = true;
            mSolidColor = argb;
            mColors.clear();
            computeOpacity();
        }
        
        void setStroke(int width, int color) {
            mStrokeWidth = width;
            mStrokeColor = color;
            computeOpacity();
        }
        
        void setStroke(int width, int color, float dashWidth, float dashGap) {
            mStrokeWidth = width;
            mStrokeColor = color;
            mStrokeDashWidth = dashWidth;
            mStrokeDashGap = dashGap;
            computeOpacity();
        }
        
        void setCornerRadius(float radius) {
            if (radius < 0) {
                radius = 0;
            }
            mRadius = radius;
            mRadiusArray.clear();
        }
        
        void setCornerRadii(vector<float> radii) {
            mRadiusArray = radii;
            if (radii.empty()) {
                mRadius = 0;
            }
        }
        
        void setSize(int width, int height) {
            mWidth = width;
            mHeight = height;
        }
        
        void setGradientRadius(float gradientRadius) {
            mGradientRadius = gradientRadius;
        }
        
        float mCenterX = 0.5f;
        float mCenterY = 0.5f;
        float mGradientRadius = 0.5f;
        bool mUseLevel;
        bool mUseLevelForShape;
        bool mOpaque;
        
        void computeOpacity() {
            if (mShape != RECTANGLE) {
                mOpaque = false;
                return;
            }
            
            if (mRadius > 0 || !mRadiusArray.empty()) {
                mOpaque = false;
                return;
            }
            
            if (mStrokeWidth > 0 && !isOpaque(mStrokeColor)) {
                mOpaque = false;
                return;
            }
            
            if (mHasSolidColor) {
                mOpaque = isOpaque(mSolidColor);
                return;
            }
            
            if (!mColors.empty()) {
                for (int i = 0; i < mColors.size(); i++) {
                    if (!isOpaque(mColors[i])) {
                        mOpaque = false;
                        return;
                    }
                }
            }
            
            mOpaque = true;
        }
        
        static bool isOpaque(int color) {
            return ((color >> 24) & 0xff) == 0xff;
        }
    };
    
    GradientState m_gradientState;
    
    Paint m_fillPaint;
    Rect mPadding;
    Paint m_strokePaint;   // optional, set by the caller
    ColorFilter *m_colorFilter = NULL;   // optional, set by the caller
    int m_alpha = 0xFF;  // m_odified by the caller
    bool m_dither = false;
    
    Path m_path;
    RectF m_rect;
    
    Paint m_layerPaint;    // internal, used if we use saveLayer()
    bool m_rectIsDirty = false;   // internal state
    bool m_mutated = false;
    Path m_ringPath;
    bool m_pathIsDirty = true;
    
    void buildRing(GradientState st);
    static float getFloatOrFraction(AttributeSet *attrs, const char* index, float defaultValue);
    int getGradientType(string gradient);
    int getShapeType(string shape);
    void initializeWithState(GradientState state);
    int modulateAlpha(int alpha);
    bool ensureValidRect();
    
public:
    
    GradientDrawable(GradientState state) :
    m_fillPaint(Paint::Flags::kAntiAlias_Flag),
    m_strokePaint(Paint::Flags::kAntiAlias_Flag)
    {
        m_rect = RectF();
        m_gradientState = state;
        initializeWithState(state);
        m_rectIsDirty = true;
        m_mutated = false;
        m_strokePaint.setStyle(Paint::Style::kStroke_Style);
    }
};

ANDROID_END

#endif /* defined(__cocos2dx__GradientDrawable__) */

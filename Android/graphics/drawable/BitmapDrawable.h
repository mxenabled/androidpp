//
//  BitmapDrawable.h
//  cocos2dx
//
//  Created by Saul Howard on 11/14/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__BitmapDrawable__
#define __cocos2dx__BitmapDrawable__

#include "AndroidMacros.h"

#include "Android/view/Gravity.h"
#include "Android/utils/DisplayMetrics.h"
#include "Android/graphics/Bitmap.h"
#include "Android/graphics/BitmapFactory.h"
#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/Paint.h"

#include <SkShader.h>
#include <memory>

using namespace std;

ANDROID_BEGIN

class BitmapDrawable : public Drawable {
    
public:
    
    class BitmapState : public ConstantState {
        
    public:
        
        Bitmap *m_bitmap = NULL;
        int m_changingConfigurations = 0;
        int m_gravity = Gravity::FILL;
        Paint m_paint;
        SkShader::TileMode m_tileModeX;
        SkShader::TileMode m_tileModeY;
        bool m_useShader = false;
        int m_targetDensity = DisplayMetrics::DENSITY_DEFAULT;
        bool m_rebuildShader = false;
        
        BitmapState() {
        }
        
        BitmapState(Bitmap *bitmap) : m_bitmap(bitmap) {
            m_paint.setFlags(DEFAULT_PAINT_FLAGS);
        }
        
        BitmapState(const char* filepath) {
            m_paint.setFlags(DEFAULT_PAINT_FLAGS);
            m_bitmap = new Bitmap();
            BitmapFactory::decodeFile(m_bitmap, filepath);
        }
        
        BitmapState(BitmapState *bitmapState) : BitmapState(bitmapState->m_bitmap) {
            m_changingConfigurations = bitmapState->m_changingConfigurations;
            m_gravity = bitmapState->m_gravity;
            m_tileModeX = bitmapState->m_tileModeX;
            m_tileModeY = bitmapState->m_tileModeY;
            m_useShader = bitmapState->m_useShader;
            m_targetDensity = bitmapState->m_targetDensity;
            m_paint = Paint(bitmapState->m_paint);
            m_rebuildShader = bitmapState->m_rebuildShader;
        }

        BitmapState(const BitmapState &bitmapState) : BitmapState(bitmapState.m_bitmap) {
            m_changingConfigurations = bitmapState.m_changingConfigurations;
            m_gravity = bitmapState.m_gravity;
            m_tileModeX = bitmapState.m_tileModeX;
            m_tileModeY = bitmapState.m_tileModeY;
            m_useShader = bitmapState.m_useShader;
            m_targetDensity = bitmapState.m_targetDensity;
            m_paint = Paint(bitmapState.m_paint);
            m_rebuildShader = bitmapState.m_rebuildShader;
        }
    
        virtual shared_ptr<Drawable> newDrawable() {
            return make_shared<BitmapDrawable>(*this, (Resources*) NULL);
        }
        
        virtual shared_ptr<Drawable> newDrawable(Resources *res) {
            return make_shared<BitmapDrawable>(*this, res);
        }
        
        virtual int getChangingConfigurations() {
            return m_changingConfigurations;
        }
    };
    
private:
    
    static const uint32_t DEFAULT_PAINT_FLAGS;
    
    BitmapState m_bitmapState;
    Bitmap *m_bitmap;
    int m_targetDensity = 0;
    
    Rect m_dstRect;   // Gravity.apply() sets this
    
    bool m_applyGravity = false;
    bool m_mutated = false;
    
    // These are scaled to match the target density.
    int m_bitmapWidth = 0;
    int m_bitmapHeight = 0;
    
    void computeBitmapSize();
    Bitmap *initBitmap(const char* filepath);
    void setBitmap(Bitmap *bitmap);
    
public:
    
    BitmapDrawable();
    BitmapDrawable(Resources *res);
    BitmapDrawable(Bitmap *bitmap);
    BitmapDrawable(Resources *res, Bitmap *bitmap);
    BitmapDrawable(const char* filepath);
    BitmapDrawable(Resources *res, const char* filepath);
    BitmapDrawable(BitmapState state, Resources *res);
    
    virtual ~BitmapDrawable() {
        if (m_bitmap) delete m_bitmap;
    }
    
    virtual void draw(shared_ptr<Canvas> canvas);
    
    Bitmap *getBitmap();
    virtual int getChangingConfigurations();
    virtual ConstantState *getConstantState();
    int getGravity();
    virtual int getIntrinsicHeight();
    virtual int getIntrinsicWidth();
    virtual int getOpacity();
    Paint getPaint();
    SkShader::TileMode getTileModeX();
    SkShader::TileMode getTileModeY();
    
    bool hasAntiAlias();
    virtual void inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs);
    virtual shared_ptr<Drawable> mutate();
    
    virtual void setAlpha(int alpha);
    void setAntiAlias(bool aa);
    virtual void setColorFilter(ColorFilter *cf);
    virtual void setDither(bool dither);
    virtual void setFilterBitmap(bool filter);
    void setGravity(int gravity);
    void setTargetDensity(DisplayMetrics *metrics);
    void setTargetDensity(int density);
    void setTileModeX(SkShader::TileMode mode);
    void setTileModeY(SkShader::TileMode mode);
    void setTileModeXY(SkShader::TileMode xmode, SkShader::TileMode ymode);
    
protected:
    
    virtual void onBoundsChange(Rect bounds);
};

ANDROID_END

#endif /* defined(__cocos2dx__BitmapDrawable__) */

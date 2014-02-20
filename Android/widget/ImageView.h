//
//  ImageView.h
//  cocos2dx
//
//  Created by BJ Neilsen on 12/2/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "AndroidMacros.h"

#include "Android/content/Context.h"
#include "Android/content/res/Resources.h"
#include "Android/graphics/Bitmap.h"
#include "Android/graphics/Canvas.h"
#include "Android/graphics/Matrix.h"
#include "Android/graphics/drawable/BitmapDrawable.h"
#include "Android/graphics/drawable/Drawable.h"
#include "Android/utils/AttributeSet.h"
#include "Android/view/View.h"

#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <climits>

ANDROID_BEGIN

class ImageView : public View {
    
public:
    enum ScaleType {
        MATRIX        = 0,
        FIT_XY        = 1,
        FIT_START     = 2,
        FIT_CENTER    = 3,
        FIT_END       = 4,
        CENTER        = 5,
        CENTER_CROP   = 6,
        CENTER_INSIDE = 7
    };
    
private:
    //  Uri *m_uri;
    int m_resource = 0;
    Matrix *m_matrix = NULL;
    ScaleType m_scaleType = MATRIX;
    bool m_haveFrame = false;
    bool m_adjustViewBounds = false;
    int m_maxWidth = INT_MAX;
    int m_maxHeight = INT_MAX;
    
    ColorFilter *m_colorFilter = NULL;
    int m_alpha = 255;
    int m_viewAlphaScale = 256;
    bool m_colorMod = false;
    
    shared_ptr<Drawable> m_drawable = NULL;
    vector<int> m_state;
    bool m_mergeState = false;
    int m_level = 0;
    int m_drawableWidth = 0;
    int m_drawableHeight = 0;
    Matrix *m_drawMatrix = NULL;
    
    // FIXME Avoid allocations... ??
    RectF *m_tempSrc = new RectF();
    RectF *m_tempDst = new RectF();
    
    bool m_cropToPadding;
    
    int m_baseline = -1;
    bool m_baselineAlignBottom = false;
    
    // AdjustViewBounds behavior will be in compatibility mode for older apps.
    bool m_adjustViewBoundsCompat = false;
    void initImageView();
    void resolveUri();
    void updateDrawable(shared_ptr<Drawable> d);
    void resizeFromDrawable();
    int resolveAdjustedSize(int desiredSize, int maxSize, int measureSpec);
    void configureBounds();
    void applyColorMod();
    
    static Matrix::ScaleToFit scaleTypeToScaleToFit(ScaleType st)  {
        // ScaleToFit enum to their corresponding Matrix.ScaleToFit
        return static_cast<Matrix::ScaleToFit>(st - 1);
    }
    
    map<string, int> scaleTypeMap = {
        { "matrix", 0 },
        { "fitXY", 1 },
        { "fitStart", 2 },
        { "fitCenter", 3 },
        { "fitEnd", 4 },
        { "center", 5 },
        { "centerCrop", 6 },
        { "centerInside", 7 }
    };
    
protected:
    bool verifyDrawable(shared_ptr<Drawable> dr);
    void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    bool setFrame(int l, int t, int r, int b);
    void drawableStateChanged();
    void onDraw(shared_ptr<Canvas> canvas);
    void onAttachedToWindow();
    void onDetachedFromWindow();
    
public:
    ImageView(Context *context);
    ImageView(Context *context, AttributeSet *attrs);
    
    void jumpDrawablesToCurrentState();
    void invalidateDrawable(shared_ptr<Drawable> dr);
    bool hasOverlappingRendering();
    //  void onPopulateAccessibilityEvent(AccessibilityEvent *event);
    bool getAdjustViewBounds();
    void setAdjustViewBounds(bool adjustViewBounds);
    int getMaxWidth();
    void setMaxWidth(int maxWidth);
    int getMaxHeight();
    void setMaxHeight(int maxHeight);
    shared_ptr<Drawable> getDrawable();
    void setImageResource(int resId);
    //  void setImageURI(Uri *uri);
    void setImageDrawable(shared_ptr<Drawable> drawable);
    void setImageBitmap(Bitmap *bm);
    void setImageState(vector<int> state, bool merge);
    void setSelected(bool selected);
    void setImageLevel(int level);
    void setScaleType(ScaleType scaleType);
    ScaleType getScaleType();
    Matrix *getImageMatrix();
    void setImageMatrix(Matrix *matrix);
    bool getCropToPadding();
    void setCropToPadding(bool cropToPadding);
    vector<int> onCreateDrawableState(int extraSpace);
    int getBaseline();
    void setBaseline(int baseline);
    void setBaselineAlignBottom(bool aligned);
    bool getBaselineAlignBottom();
    //  void setColorFilter(int color, PorterDuff.Mode mode);
    void setColorFilter(int color);
    void setColorFilter(ColorFilter *cf);
    void clearColorFilter();
    ColorFilter *getColorFilter();
    int getImageAlpha();
    void setImageAlpha(int alpha);
    void setAlpha(int alpha);
    void setVisibility(int visibility);
    static int getScaleType(const char* scaleType);
    //  void onInitializeAccessibilityEvent(AccessibilityEvent *event);
    //  void onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo *info);
};

ANDROID_END

#endif /* defined(IMAGEVIEW_H) */

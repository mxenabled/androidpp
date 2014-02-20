//
//  ImageView.cpp
//  cocos2dx
//
//  Created by BJ Neilsen on 12/2/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "ImageView.h"

ANDROID_BEGIN

//
// Public Functions
//

ImageView::ImageView(Context *context) : View(context) {
    initImageView();
}

ImageView::ImageView(Context *context, AttributeSet *attrs) : View(context, attrs) {
    initImageView();
    attrs->loadAttributes();
    
    for (int i = 0; i < attrs->getAttributeCount(); i++) {
        const char *attrName = attrs->getAttributeName(i);
        
        if (strcmp(R::styleable::src, attrName) == 0) {
            shared_ptr<Drawable> d = attrs->getAttributeDrawableValue(context->getResources(), attrName);
            if (d != NULL) {
                setImageDrawable(d);
            }
        } else if (strcmp(R::styleable::baselineAlignBottom, attrName) == 0) {
            m_baselineAlignBottom = attrs->getAttributeBooleanValue(R::styleable::baselineAlignBottom, false);
        } else if (strcmp(R::styleable::baseline, attrName) == 0) {
            m_baseline = attrs->getAttributeIntValue(R::styleable::baseline, -1);
        } else if (strcmp(R::styleable::adjustViewBounds, attrName) == 0) {
            setAdjustViewBounds(attrs->getAttributeBooleanValue(R::styleable::adjustViewBounds, false));
        } else if (strcmp(R::styleable::maxWidth, attrName) == 0) {
            setMaxWidth(attrs->getDimensionValue(context->getResources(), R::styleable::maxWidth, INT_MAX));
        } else if (strcmp(R::styleable::maxHeight, attrName) == 0) {
            setMaxHeight(attrs->getDimensionValue(context->getResources(), R::styleable::maxHeight, INT_MAX));
        } else if (strcmp(R::styleable::scaleType, attrName) == 0) {
            int index = getScaleType(attrs->getAttributeValue(R::styleable::scaleType));
            if (index >= 0) {
                setScaleType(static_cast<ScaleType>(index));
            }
        } else if (strcmp(R::styleable::tint, attrName) == 0) {
            int tint = attrs->getAttributeIntValue(R::styleable::tint, 0);
            if (tint != 0) {
                // TODO: implement tint
               // setColorFilter(tint);
            }
        } else if (strcmp(R::styleable::drawableAlpha, attrName) == 0) {
            int alpha = attrs->getAttributeIntValue(R::styleable::drawableAlpha, 255);
            if (alpha != 255) {
                setAlpha(alpha);
            }
        } else if (strcmp(R::styleable::cropToPadding, attrName) == 0) {
            m_cropToPadding = attrs->getAttributeBooleanValue(R::styleable::cropToPadding, false);
        }
    }
}

int ImageView::getScaleType(const char* scaleType) {
    if (strcmp(scaleType, "matrix") == 0) {
        return MATRIX;
    } else if (strcmp(scaleType, "fitXY") == 0) {
        return FIT_XY;
    } else if (strcmp(scaleType, "fitStart") == 0) {
        return FIT_START;
    } else if (strcmp(scaleType, "fitCenter") == 0) {
        return FIT_CENTER;
    } else if (strcmp(scaleType, "fitEnd") == 0) {
        return FIT_END;
    } else if (strcmp(scaleType, "center") == 0) {
        return CENTER;
    } else if (strcmp(scaleType, "centerCrop") == 0) {
        return CENTER_CROP;
    } else if (strcmp(scaleType, "centerInside") == 0) {
        return CENTER_INSIDE;
    } else {
        return -1;
    }
}

void ImageView::jumpDrawablesToCurrentState() {
    View::jumpDrawablesToCurrentState();
    if (m_drawable != NULL) m_drawable->jumpToCurrentState();
}

void ImageView::invalidateDrawable(shared_ptr<Drawable> dr) {
    if (dr == m_drawable) {
        /* we invalidate the whole view in this case because it's very
         * hard to know where the drawable actually is. This is made
         * complicated because of the offsets and transformations that
         * can be applied. In theory we could get the drawable's bounds
         * and run them through the transformation and offsets, but this
         * is probably not worth the effort.
         */
        invalidate();
    } else {
        invalidateDrawable(dr);
    }
}

bool ImageView::hasOverlappingRendering() {
    return (View::getBackground() != NULL && View::getBackground()->getCurrent() != NULL);
}

// TODO needs AccessibilityEvent, CharSequence (or equiv) types declared
//
// void ImageView::onPopulateAccessibilityEvent(AccessibilityEvent event) {
//   onPopulateAccessibilityEvent(event);
//   CharSequence contentDescription = getContentDescription();
//   if (!TextUtils.isEmpty(contentDescription)) {
//     event.getText().add(contentDescription);
//   }
// }

/**
 * True when ImageView is adjusting its bounds
 * to preserve the aspect ratio of its drawable
 *
 * @return whether to adjust the bounds of this view
 * to presrve the original aspect ratio of the drawable
 *
 * @see #setAdjustViewBounds(boolean)
 *
 * @attr ref android.R.styleable#ImageView_adjustViewBounds
 */
bool ImageView::getAdjustViewBounds() {
    return m_adjustViewBounds;
}

/**
 * Set this to true if you want the ImageView to adjust its bounds
 * to preserve the aspect ratio of its drawable.
 *
 * <p><strong>Note:</strong> If the application targets API level 17 or lower,
 * adjustViewBounds will allow the drawable to shrink the view bounds, but not grow
 * to fill available measured space in all cases. This is for compatibility with
 * legacy {@link android.view.View.MeasureSpec MeasureSpec} and
 * {@link android.widget.RelativeLayout RelativeLayout} behavior.</p>
 *
 * @param adjustViewBounds Whether to adjust the bounds of this view
 * to preserve the original aspect ratio of the drawable.
 *
 * @see #getAdjustViewBounds()
 *
 * @attr ref android.R.styleable#ImageView_adjustViewBounds
 */
void ImageView::setAdjustViewBounds(bool adjustViewBounds) {
    m_adjustViewBounds = adjustViewBounds;
    if (adjustViewBounds) {
        setScaleType(FIT_CENTER);
    }
}

/**
 * The maximum width of this view.
 *
 * @return The maximum width of this view
 *
 * @see #setMaxWidth(int)
 *
 * @attr ref android.R.styleable#ImageView_maxWidth
 */
int ImageView::getMaxWidth() {
    return m_maxWidth;
}

/**
 * An optional argument to supply a maximum width for this view. Only valid if
 * {@link #setAdjustViewBounds(boolean)} has been set to true. To set an image to be a maximum
 * of 100 x 100 while preserving the original aspect ratio, do the following: 1) set
 * adjustViewBounds to true 2) set maxWidth and maxHeight to 100 3) set the height and width
 * layout params to WRAP_CONTENT.
 *
 * <p>
 * Note that this view could be still smaller than 100 x 100 using this approach if the original
 * image is small. To set an image to a fixed size, specify that size in the layout params and
 * then use {@link #setScaleType(android.widget.ImageView.ScaleType)} to determine how to fit
 * the image within the bounds.
 * </p>
 *
 * @param maxWidth maximum width for this view
 *
 * @see #getMaxWidth()
 *
 * @attr ref android.R.styleable#ImageView_maxWidth
 */
void ImageView::setMaxWidth(int maxWidth) {
    m_maxWidth = maxWidth;
}

/**
 * The maximum height of this view.
 *
 * @return The maximum height of this view
 *
 * @see #setMaxHeight(int)
 *
 * @attr ref android.R.styleable#ImageView_maxHeight
 */
int ImageView::getMaxHeight() {
    return m_maxHeight;
}

/**
 * An optional argument to supply a maximum height for this view. Only valid if
 * {@link #setAdjustViewBounds(boolean)} has been set to true. To set an image to be a
 * maximum of 100 x 100 while preserving the original aspect ratio, do the following: 1) set
 * adjustViewBounds to true 2) set maxWidth and maxHeight to 100 3) set the height and width
 * layout params to WRAP_CONTENT.
 *
 * <p>
 * Note that this view could be still smaller than 100 x 100 using this approach if the original
 * image is small. To set an image to a fixed size, specify that size in the layout params and
 * then use {@link #setScaleType(android.widget.ImageView.ScaleType)} to determine how to fit
 * the image within the bounds.
 * </p>
 *
 * @param maxHeight maximum height for this view
 *
 * @see #getMaxHeight()
 *
 * @attr ref android.R.styleable#ImageView_maxHeight
 */
void ImageView::setMaxHeight(int maxHeight) {
    m_maxHeight = maxHeight;
}

/** Return the view's drawable, or null if no drawable has been
  assigned.
  */
shared_ptr<Drawable> ImageView::getDrawable() {
    return m_drawable;
}

/**
 * Sets a drawable as the content of this ImageView.
 *
 * <p class="note">This does Bitmap reading and decoding on the UI
 * thread, which can cause a latency hiccup.  If that's a concern,
 * consider using {@link #setImageDrawable(android.graphics.drawable.Drawable)} or
 * {@link #setImageBitmap(android.graphics.Bitmap)} and
 * {@link android.graphics.BitmapFactory} instead.</p>
 *
 * @param resId the resource identifier of the drawable
 *
 * @attr ref android.R.styleable#ImageView_src
 */
// TODO needs Uri type (for m_uri attr)
//
void ImageView::setImageResource(int resId) {
    if (m_resource != resId) {
        updateDrawable(NULL);
        m_resource = resId;
        // FIXME clear out resource pointer
        //    m_uri = NULL;
        
        const int oldWidth = m_drawableWidth;
        const int oldHeight = m_drawableHeight;
        
        // FIXME Need to fetch the resource and set it
        //    resolveUri();
        
        if (oldWidth != m_drawableWidth || oldHeight != m_drawableHeight) {
            requestLayout();
        }
        invalidate();
    }
}

/**
 * Sets the content of this ImageView to the specified Uri.
 *
 * <p class="note">This does Bitmap reading and decoding on the UI
 * thread, which can cause a latency hiccup.  If that's a concern,
 * consider using {@link #setImageDrawable(android.graphics.drawable.Drawable)} or
 * {@link #setImageBitmap(android.graphics.Bitmap)} and
 * {@link android.graphics.BitmapFactory} instead.</p>
 *
 * @param uri The Uri of an image
 */
// TODO needs Uri type defined
//
//void ImageView::setImageURI(Uri uri) {
//  if (m_resource != 0 ||
//    (m_uri != uri &&
//     (uri == NULL || m_uri == NULL || !uri.equals(m_uri)))) {
//       updateDrawable(NULL);
//       m_resource = 0;
//       m_uri = uri;
//
//       final int oldWidth = m_drawableWidth;
//       final int oldHeight = m_drawableHeight;
//
//       resolveUri();
//
//       if (oldWidth != m_drawableWidth || oldHeight != m_drawableHeight) {
//         requestLayout();
//       }
//       invalidate();
//     }
//}

/**
 * Sets a drawable as the content of this ImageView.
 *
 * @param drawable The drawable to set
 */
void ImageView::setImageDrawable(shared_ptr<Drawable> drawable) {
    if (m_drawable != drawable) {
        m_resource = 0;
        // TODO uncomment m_uri set to NULL
        // m_uri = NULL;
        
        const int oldWidth = m_drawableWidth;
        const int oldHeight = m_drawableHeight;
        
        updateDrawable(drawable);
        
        if (oldWidth != m_drawableWidth || oldHeight != m_drawableHeight) {
            requestLayout();
        }
        invalidate();
    }
}

/**
 * Sets a Bitmap as the content of this ImageView.
 *
 * @param bm The bitmap to set
 */
void ImageView::setImageBitmap(Bitmap *bm) {
    // if this is used frequently, may handle bitmaps explicitly
    // to reduce the intermediate drawable object
    setImageDrawable(make_shared<BitmapDrawable>(getContext()->getResources(), bm));
}

void ImageView::setImageState(vector<int> state, bool merge) {
    m_state = state;
    m_mergeState = merge;
    if (m_drawable != NULL) {
        View::refreshDrawableState();
        resizeFromDrawable();
    }
}

void ImageView::setSelected(bool selected) {
    View::setSelected(selected);
    resizeFromDrawable();
}

/**
 * Sets the image level, when it is constructed from a
 * {@link android.graphics.drawable.LevelListDrawable}.
 *
 * @param level The new level for the image.
 */
void ImageView::setImageLevel(int level) {
    m_level = level;
    if (m_drawable != NULL) {
        m_drawable->setLevel(level);
        resizeFromDrawable();
    }
}

/**
 * Controls how the image should be resized or moved to match the size
 * of this ImageView.
 *
 * @param scaleType The desired scaling mode.
 *
 * @attr ref android.R.styleable#ImageView_scaleType
 */
void ImageView::setScaleType(ScaleType scaleType) {
    if (m_scaleType != scaleType) {
        m_scaleType = scaleType;
        
        View::setWillNotCacheDrawing(m_scaleType == CENTER);
        
        requestLayout();
        invalidate();
    }
}

/**
 * Return the current scale type in use by this ImageView.
 *
 * @see ImageView.ScaleType
 *
 * @attr ref android.R.styleable#ImageView_scaleType
 */
ImageView::ScaleType ImageView::getScaleType() {
    return m_scaleType;
}

/** Return the view's optional matrix. This is applied to the
  view's drawable when it is drawn. If there is not matrix,
  this method will return an identity matrix.
  Do not change this matrix in place but make a copy.
  If you want a different matrix applied to the drawable,
  be sure to call setImageMatrix().
  */
Matrix *ImageView::getImageMatrix() {
    if (m_drawMatrix == NULL) {
        return new Matrix();
    }
    return m_drawMatrix;
}

void ImageView::setImageMatrix(Matrix *matrix) {
    // collaps null and identity to just null
    if (matrix != NULL && matrix->isIdentity()) {
        matrix = NULL;
    }
    
    // don't invalidate unless we're actually changing our matrix
    if ((matrix == NULL && !m_matrix->isIdentity()) || (matrix != NULL && m_matrix != matrix)) {
        m_matrix = matrix;
        configureBounds();
        invalidate();
    }
}

/**
 * Return whether this ImageView crops to padding.
 *
 * @return whether this ImageView crops to padding
 *
 * @see #setCropToPadding(boolean)
 *
 * @attr ref android.R.styleable#ImageView_cropToPadding
 */
bool ImageView::getCropToPadding() {
    return m_cropToPadding;
}

/**
 * Sets whether this ImageView will crop to padding.
 *
 * @param cropToPadding whether this ImageView will crop to padding
 *
 * @see #getCropToPadding()
 *
 * @attr ref android.R.styleable#ImageView_cropToPadding
 */
void ImageView::setCropToPadding(bool cropToPadding) {
    if (m_cropToPadding != cropToPadding) {
        m_cropToPadding = cropToPadding;
        requestLayout();
        invalidate();
    }
}

vector<int> ImageView::onCreateDrawableState(int extraSpace) {
    if (m_state.empty()) {
        return View::onCreateDrawableState(extraSpace);
    } else if (!m_mergeState) {
        return m_state;
    } else {
        vector<int> currentState = View::onCreateDrawableState(extraSpace + m_state.size());
        return View::mergeDrawableStates(currentState, m_state);
    }
}

/**
 * <p>Return the offset of the widget's text baseline from the widget's top
 * boundary. </p>
 *
 * @return the offset of the baseline within the widget's bounds or -1
 *         if baseline alignment is not supported.
 */
int ImageView::getBaseline() {
    if (m_baselineAlignBottom) {
        return getMeasuredHeight();
    } else {
        return m_baseline;
    }
}

/**
 * <p>Set the offset of the widget's text baseline from the widget's top
 * boundary.  This value is overridden by the {@link #setBaselineAlignBottom(boolean)}
 * property.</p>
 *
 * @param baseline The baseline to use, or -1 if none is to be provided.
 *
 * @see #setBaseline(int)
 * @attr ref android.R.styleable#ImageView_baseline
 */
void ImageView::setBaseline(int baseline) {
    if (m_baseline != baseline) {
        m_baseline = baseline;
        requestLayout();
    }
}

/**
 * Set whether to set the baseline of this view to the bottom of the view.
 * Setting this value overrides any calls to setBaseline.
 *
 * @param aligned If true, the image view will be baseline aligned with
 *      based on its bottom edge.
 *
 * @attr ref android.R.styleable#ImageView_baselineAlignBottom
 */
void ImageView::setBaselineAlignBottom(bool aligned) {
    if (m_baselineAlignBottom != aligned) {
        m_baselineAlignBottom = aligned;
        requestLayout();
    }
}

/**
 * Return whether this view's baseline will be considered the bottom of the view.
 *
 * @see #setBaselineAlignBottom(boolean)
 */
bool ImageView::getBaselineAlignBottom() {
    return m_baselineAlignBottom;
}

/**
 * Set a tinting option for the image.
 *
 * @param color Color tint to apply.
 * @param mode How to apply the color.  The standard mode is
 * {@link PorterDuff.Mode#SRC_ATOP}
 *
 * @attr ref android.R.styleable#ImageView_tint
 */
// TODO Need PorterDuff type
//
//void ImageView::setColorFilter(int color, PorterDuff.Mode mode) {
//  setColorFilter(new PorterDuffColorFilter(color, mode));
//}

/**
 * Set a tinting option for the image. Assumes
 * {@link PorterDuff.Mode#SRC_ATOP} blending mode.
 *
 * @param color Color tint to apply.
 * @attr ref android.R.styleable#ImageView_tint
 */
// TODO Need PorterDuff type
//
//void ImageView::setColorFilter(int color) {
//  setColorFilter(color, PorterDuff.Mode.SRC_ATOP);
//}

void ImageView::clearColorFilter() {
    setColorFilter((ColorFilter*)NULL);
}

/**
 * Returns the active color filter for this ImageView.
 *
 * @return the active color filter for this ImageView
 *
 * @see #setColorFilter(android.graphics.ColorFilter)
 */
ColorFilter *ImageView::getColorFilter() {
    return m_colorFilter;
}

/**
 * Apply an arbitrary colorfilter to the image.
 *
 * @param cf the colorfilter to apply (may be null)
 *
 * @see #getColorFilter()
 */
void ImageView::setColorFilter(ColorFilter *cf) {
    if (m_colorFilter != cf) {
        m_colorFilter = cf;
        m_colorMod = true;
        applyColorMod();
        invalidate();
    }
}

/**
 * Returns the alpha that will be applied to the drawable of this ImageView.
 *
 * @return the alpha that will be applied to the drawable of this ImageView
 *
 * @see #setImageAlpha(int)
 */
int ImageView::getImageAlpha() {
    return m_alpha;
}

/**
 * Sets the alpha value that should be applied to the image.
 *
 * @param alpha the alpha value that should be applied to the image
 *
 * @see #getImageAlpha()
 */
void ImageView::setImageAlpha(int alpha) {
    setAlpha(alpha);
}

/**
 * Sets the alpha value that should be applied to the image.
 *
 * @param alpha the alpha value that should be applied to the image
 *
 * @deprecated use #setImageAlpha(int) instead
 */
void ImageView::setAlpha(int alpha) {
    alpha &= 0xFF;          // keep it legal
    if (m_alpha != alpha) {
        m_alpha = alpha;
        m_colorMod = true;
        applyColorMod();
        invalidate();
    }
}

void ImageView::setVisibility(int visibility) {
    setVisibility(visibility);
    if (m_drawable != NULL) {
        m_drawable->setVisible(visibility == VISIBLE, false);
    }
}

// TODO Need AccessibilityEvent type defined
//
//void ImageView::onInitializeAccessibilityEvent(AccessibilityEvent event) {
//  View::onInitializeAccessibilityEvent(event);
//  event->setClassName(ImageView.class.getName());
//}

// TODO Need AccessibilityNodeInfo type defined
//
//void ImageView::onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo info) {
//  onInitializeAccessibilityNodeInfo(info);
//  info->setClassName(ImageView.class.getName());
//}

//
// Protected Functions
//

bool ImageView::verifyDrawable(shared_ptr<Drawable> dr) {
    return m_drawable == dr || View::verifyDrawable(dr);
}

void ImageView::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    // resolveUri(); TODO: need to implement this.
    int w;
    int h;
    
    // Desired aspect ratio of the view's contents (not including padding)
    float desiredAspect = 0.0f;
    
    // We are allowed to change the view's width
    bool resizeWidth = false;
    
    // We are allowed to change the view's height
    bool resizeHeight = false;
    
    const int widthSpecMode = MeasureSpec::getMode(widthMeasureSpec);
    const int heightSpecMode = MeasureSpec::getMode(heightMeasureSpec);
    
    if (m_drawable == NULL) {
        // If no drawable, its intrinsic size is 0.
        m_drawableWidth = -1;
        m_drawableHeight = -1;
        w = h = 0;
    } else {
        w = m_drawableWidth;
        h = m_drawableHeight;
        if (w <= 0) w = 1;
        if (h <= 0) h = 1;
        
        // We are supposed to adjust view bounds to match the aspect
        // ratio of our drawable. See if that is possible.
        if (m_adjustViewBounds) {
            resizeWidth = widthSpecMode != MeasureSpec::EXACTLY;
            resizeHeight = heightSpecMode != MeasureSpec::EXACTLY;
            
            desiredAspect = (float) w / (float) h;
        }
    }
    
    int pleft = mPaddingLeft;
    int pright = mPaddingRight;
    int ptop = mPaddingTop;
    int pbottom = mPaddingBottom;
    
    int widthSize;
    int heightSize;
    
    if (resizeWidth || resizeHeight) {
        /* If we get here, it means we want to resize to match the
         drawables aspect ratio, and we have the freedom to change at
         least one dimension.
         */
        
        // Get the max possible width given our constraints
        widthSize = resolveAdjustedSize(w + pleft + pright, m_maxWidth, widthMeasureSpec);
        
        // Get the max possible height given our constraints
        heightSize = resolveAdjustedSize(h + ptop + pbottom, m_maxHeight, heightMeasureSpec);
        
        if (desiredAspect != 0.0f) {
            // See what our actual aspect ratio is
            float actualAspect = (float)(widthSize - pleft - pright) /
            (heightSize - ptop - pbottom);
            
            if (abs(actualAspect - desiredAspect) > 0.0000001) {
                
                bool done = false;
                
                // Try adjusting width to be proportional to height
                if (resizeWidth) {
                    int newWidth = (int)(desiredAspect * (heightSize - ptop - pbottom)) +
                    pleft + pright;
                    
                    // Allow the width to outgrow its original estimate if height is fixed.
                    if (!resizeHeight && !m_adjustViewBoundsCompat) {
                        widthSize = resolveAdjustedSize(newWidth, m_maxWidth, widthMeasureSpec);
                    }
                    
                    if (newWidth <= widthSize) {
                        widthSize = newWidth;
                        done = true;
                    }
                }
                
                // Try adjusting height to be proportional to width
                if (!done && resizeHeight) {
                    int newHeight = (int)((widthSize - pleft - pright) / desiredAspect) +
                    ptop + pbottom;
                    
                    // Allow the height to outgrow its original estimate if width is fixed.
                    if (!resizeWidth && !m_adjustViewBoundsCompat) {
                        heightSize = resolveAdjustedSize(newHeight, m_maxHeight,
                                                         heightMeasureSpec);
                    }
                    
                    if (newHeight <= heightSize) {
                        heightSize = newHeight;
                    }
                }
            }
        }
    } else {
        /* We either don't want to preserve the drawables aspect ratio,
         or we are not allowed to change view dimensions. Just measure in
         the normal way.
         */
        w += pleft + pright;
        h += ptop + pbottom;
        
        w = max(w, getSuggestedMinimumWidth());
        h = max(h, getSuggestedMinimumHeight());
        
        widthSize = resolveSizeAndState(w, widthMeasureSpec, 0);
        heightSize = resolveSizeAndState(h, heightMeasureSpec, 0);
    }
    
    setMeasuredDimension(widthSize, heightSize);
}

bool ImageView::setFrame(int l, int t, int r, int b) {
    bool changed = View::setFrame(l, t, r, b);
    m_haveFrame = true;
    configureBounds();
    return changed;
}

void ImageView::drawableStateChanged() {
    View::drawableStateChanged();
    shared_ptr<Drawable> d = m_drawable;
    if (d != NULL && d->isStateful()) {
        d->setState(getDrawableState());
    }
}

void ImageView::onDraw(shared_ptr<Canvas> canvas) {
    View::onDraw(canvas);
    
    if (m_drawable == NULL) {
        return; // couldn't resolve the URI
    }
    
    if (m_drawableWidth == 0 || m_drawableHeight == 0) {
        return;     // nothing to draw (empty bounds)
    }
    
    if (m_drawMatrix == NULL && mPaddingTop == 0 && mPaddingLeft == 0) {
        m_drawable->draw(canvas);
    } else {
        int saveCount = canvas->getSaveCount();
        canvas->save();
        
        if (m_cropToPadding) {
            const int scrollX = mScrollX;
            const int scrollY = mScrollY;
            canvas->clipRect(scrollX + mPaddingLeft, scrollY + mPaddingTop,
                             scrollX + mRight - mLeft - mPaddingRight,
                             scrollY + mBottom - mTop - mPaddingBottom, SkRegion::kIntersect_Op);
        }
        
        canvas->translate(mPaddingLeft, mPaddingTop);
        
        if (m_drawMatrix != NULL) {
            canvas->concatMatrix(m_drawMatrix);
        }
        m_drawable->draw(canvas);
        canvas->restoreToCount(saveCount);
    }
}

void ImageView::onAttachedToWindow() {
    View::onAttachedToWindow();
    if (m_drawable != NULL) {
        m_drawable->setVisible(getVisibility() == VISIBLE, false);
    }
}

void ImageView::onDetachedFromWindow() {
    View::onDetachedFromWindow();
    if (m_drawable != NULL) {
        m_drawable->setVisible(false, false);
    }
}

//
// Private Functions
//

void ImageView::initImageView() {
  m_matrix     = new Matrix();
  m_scaleType  = FIT_CENTER;
  // FIXME getApplicationInfo() not a function of Context type
    m_adjustViewBoundsCompat = false;
//  m_adjustViewBoundsCompat = m_context->getApplicationInfo()->targetSdkVersion <=
//  Build::VERSION_CODES::JELLY_BEAN_MR1;
}

// FIXME need to have a Uri type before we can implement this probably.
//
//void ImageView::resolveUri() {
//  if (m_drawable != NULL) {
//    return;
//  }
//
//  Resources *rsrc = m_context->getResources();
//  if (rsrc == NULL) {
//    return;
//  }
//
//  shared_ptr<Drawable> d = NULL;
//
//  if (m_resource != 0) {
//    try {
//      d = rsrc->getDrawable(m_resource);
//    } catch (exception *e) {
//
//      ostringstream ss;
//      ss << m_resource;
//      Log::w("ImageView", "Unable to find resource: " + ss.str(), e);
//      // Don't try again.
//      m_uri = NULL;
//    }
//  } else if (m_uri != NULL) {
//    String scheme = m_uri->getScheme();
//    if (ContentResolver::SCHEME_ANDROID_RESOURCE.equals(scheme)) {
//      try {
//        // Load drawable through Resources, to get the source density information
//        ContentResolver.OpenResourceIdResult r =
//          m_context.getContentResolver().getResourceId(m_uri);
//        d = r.r.getDrawable(r.id);
//      } catch (Exception e) {
//        Log.w("ImageView", "Unable to open content: " + m_uri, e);
//      }
//    } else if (ContentResolver.SCHEME_CONTENT.equals(scheme)
//      || ContentResolver.SCHEME_FILE.equals(scheme)) {
//        InputStream stream = NULL;
//        try {
//          stream = m_context.getContentResolver().openInputStream(m_uri);
//          d = Drawable.createFromStream(stream, NULL);
//        } catch (Exception e) {
//          Log.w("ImageView", "Unable to open content: " + m_uri, e);
//        } finally {
//          if (stream != NULL) {
//            try {
//              stream.close();
//            } catch (IOException e) {
//              Log.w("ImageView", "Unable to close content: " + m_uri, e);
//            }
//          }
//        }
//      } else {
//        d = Drawable.createFromPath(m_uri.toString());
//      }
//
//    if (d == NULL) {
//      System.out.println("resolveUri failed on bad bitmap uri: " + m_uri);
//      // Don't try again.
//      m_uri = NULL;
//    }
//  } else {
//    return;
//  }
//
//  updateDrawable(d);
//}

void ImageView::updateDrawable(shared_ptr<Drawable> d) {
    if (m_drawable != NULL) {
        m_drawable->setCallback(NULL);
        unscheduleDrawable(m_drawable.get());
    }
    m_drawable = d;
    if (d != NULL) {
        d->setCallback(this);
        if (d->isStateful()) {
            d->setState(getDrawableState());
        }
        d->setLevel(m_level);
        d->setLayoutDirection(getLayoutDirection());
        m_drawableWidth = d->getIntrinsicWidth();
        m_drawableHeight = d->getIntrinsicHeight();
        applyColorMod();
        configureBounds();
    } else {
        m_drawableWidth = m_drawableHeight = -1;
    }
}

void ImageView::resizeFromDrawable() {
    shared_ptr<Drawable> d = m_drawable;
    if (d != NULL) {
        int w = d->getIntrinsicWidth();
        if (w < 0) w = m_drawableWidth;
        int h = d->getIntrinsicHeight();
        if (h < 0) h = m_drawableHeight;
        if (w != m_drawableWidth || h != m_drawableHeight) {
            m_drawableWidth = w;
            m_drawableHeight = h;
            requestLayout();
        }
    }
}

int ImageView::resolveAdjustedSize(int desiredSize, int maxSize, int measureSpec) {
    int result = desiredSize;
    int specMode = MeasureSpec::getMode(measureSpec);
    int specSize =  MeasureSpec::getSize(measureSpec);
    switch (specMode) {
        case MeasureSpec::UNSPECIFIED:
            /* Parent says we can be as big as we want. Just don't be larger
             than max size imposed on ourselves.
             */
            result = min(desiredSize, maxSize);
            break;
        case MeasureSpec::AT_MOST:
            // Parent says we can be as big as we want, up to specSize.
            // Don't be larger than specSize, and don't be larger than
            // the max size imposed on ourselves.
            result = min(min(desiredSize, specSize), maxSize);
            break;
        case MeasureSpec::EXACTLY:
            // No choice. Do what we are told.
            result = specSize;
            break;
    }
    return result;
}

void ImageView::configureBounds() {
    if (m_drawable == NULL || !m_haveFrame) {
        return;
    }
    
    int dwidth = m_drawableWidth;
    int dheight = m_drawableHeight;
    
    int vwidth = getWidth() - mPaddingLeft - mPaddingRight;
    int vheight = getHeight() - mPaddingTop - mPaddingBottom;
    
    bool fits = (dwidth < 0 || vwidth == dwidth) &&
                (dheight < 0 || vheight == dheight);
    
    if (dwidth <= 0 || dheight <= 0 || FIT_XY == m_scaleType) {
        /* If the drawable has no intrinsic size, or we're told to
         scaletofit, then we just fill our entire view.
         */
        m_drawable->setBounds(0, 0, vwidth, vheight);
        m_drawMatrix = NULL;
    }
    else {
        // We need to do the scaling ourself, so have the drawable
        // use its native size.
        m_drawable->setBounds(0, 0, dwidth, dheight);
        
        if (MATRIX == m_scaleType) {
            // Use the specified matrix as-is.
            if (m_matrix->isIdentity()) {
                m_drawMatrix = NULL;
            }
            else {
                m_drawMatrix = m_matrix;
            }
        }
        else if (fits) {
            // The bitmap fits exactly, no transform needed.
            m_drawMatrix = NULL;
        }
        else if (CENTER == m_scaleType) {
            // Center bitmap in view, no scaling.
            m_drawMatrix = m_matrix;
            m_drawMatrix->setTranslate((int) ((vwidth - dwidth) * 0.5f + 0.5f),
                                       (int) ((vheight - dheight) * 0.5f + 0.5f));
        }
        else if (CENTER_CROP == m_scaleType) {
            m_drawMatrix = m_matrix;
            
            float scale;
            float dx = 0, dy = 0;
            
            if (dwidth * vheight > vwidth * dheight) {
                scale = (float) vheight / (float) dheight;
                dx = (vwidth - dwidth * scale) * 0.5f;
            }
            else {
                scale = (float) vwidth / (float) dwidth;
                dy = (vheight - dheight * scale) * 0.5f;
            }
            
            m_drawMatrix->setScale(scale, scale);
            m_drawMatrix->postTranslate((int) (dx + 0.5f), (int) (dy + 0.5f));
        }
        else if (CENTER_INSIDE == m_scaleType) {
            m_drawMatrix = m_matrix;
            float scale;
            float dx;
            float dy;
            
            if (dwidth <= vwidth && dheight <= vheight) {
                scale = 1.0f;
            } else {
                scale = min((float) vwidth / (float) dwidth,
                            (float) vheight / (float) dheight);
            }
            
            dx = (int) ((vwidth - dwidth * scale) * 0.5f + 0.5f);
            dy = (int) ((vheight - dheight * scale) * 0.5f + 0.5f);
            
            m_drawMatrix->setScale(scale, scale);
            m_drawMatrix->postTranslate(dx, dy);
        }
        else {
            // Generate the required transform.
            m_tempSrc->set(0, 0, dwidth, dheight);
            m_tempDst->set(0, 0, vwidth, vheight);
            
            m_drawMatrix = m_matrix;
            m_drawMatrix->setRectToRect(*m_tempSrc, *m_tempDst, scaleTypeToScaleToFit(m_scaleType));
        }
    }
}

void ImageView::applyColorMod() {
    // Only mutate and apply when modifications have occurred. This should
    // not reset the m_colorMod flag, since these filters need to be
    // re-applied if the Drawable is changed.
    if (m_drawable != NULL && m_colorMod) {
        m_drawable = m_drawable->mutate();
        m_drawable->setColorFilter(m_colorFilter);
        m_drawable->setAlpha(m_alpha * m_viewAlphaScale >> 8);
    }
}

ANDROID_END

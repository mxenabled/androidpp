//
//  GradientDrawable.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/19/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "GradientDrawable.h"

#include "Android/graphics/DashPathEffect.h"
#include "Android/graphics/Gradient.h"
#include "Android/internal/R.h"

#include <algorithm>

ANDROID_BEGIN

bool GradientDrawable::getPadding(Rect &padding) {
    if (!mPadding.isEmpty()) {
        padding.set(mPadding);
        return true;
    } else {
        return Drawable::getPadding(padding);
    }
}

/**
 * <p>Specify radii for each of the 4 corners. For each corner, the array
 * contains 2 values, <code>[X_radius, Y_radius]</code>. The corners are ordered
 * top-left, top-right, bottom-right, bottom-left. This property
 * is honored only when the shape is of type {@link #RECTANGLE}.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param radii 4 pairs of X and Y radius for each corner, specified in pixels.
 *              The length of this array must be >= 8
 *
 * @see #mutate()
 * @see #setCornerRadii(vector<float>)
 * @see #setShape(int)
 */
void GradientDrawable::setCornerRadii(vector<float> radii) {
    m_gradientState.setCornerRadii(radii);
    m_pathIsDirty = true;
    invalidateSelf();
}

/**
 * <p>Specify radius for the corners of the gradient. If this is > 0, then the
 * drawable is drawn in a round-rectangle, rather than a rectangle. This property
 * is honored only when the shape is of type {@link #RECTANGLE}.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param radius The radius in pixels of the corners of the rectangle shape
 *
 * @see #mutate()
 * @see #setCornerRadii(vector<float>)
 * @see #setShape(int)
 */
void GradientDrawable::setCornerRadius(float radius) {
    m_gradientState.setCornerRadius(radius);
    m_pathIsDirty = true;
    invalidateSelf();
}

/**
 * <p>Set the stroke width and color for the drawable. If width is zero,
 * then no stroke is drawn.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param width The width in pixels of the stroke
 * @param color The color of the stroke
 *
 * @see #mutate()
 * @see #setStroke(int, int, float, float)
 */
void GradientDrawable::setStroke(int width, int color) {
    setStroke(width, color, 0, 0);
}

/**
 * <p>Set the stroke width and color for the drawable. If width is zero,
 * then no stroke is drawn. This method can also be used to dash the stroke.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param width The width in pixels of the stroke
 * @param color The color of the stroke
 * @param dashWidth The length in pixels of the dashes, set to 0 to disable dashes
 * @param dashGap The gap in pixels between dashes
 *
 * @see #mutate()
 * @see #setStroke(int, int)
 */
void GradientDrawable::setStroke(int width, int color, float dashWidth, float dashGap) {
    
    m_gradientState.setStroke(width, color, dashWidth, dashGap);
    
    m_strokePaint.setStrokeWidth(width);
    m_strokePaint.setColor(color);
    
    DashPathEffect *e = NULL;
    if (dashWidth > 0) {
        float interval[2] = {dashWidth, dashGap};
        e = new DashPathEffect(interval, 2, 0.0f);
    }
    m_strokePaint.setPathEffect(e);
    invalidateSelf();
}


/**
 * <p>Sets the size of the shape drawn by this drawable.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param width The width of the shape used by this drawable
 * @param height The height of the shape used by this drawable
 *
 * @see #mutate()
 * @see #setGradientType(int)
 */
void GradientDrawable::setSize(int width, int height) {
    m_gradientState.setSize(width, height);
    m_pathIsDirty = true;
    invalidateSelf();
}

/**
 * <p>Sets the type of shape used to draw the gradient.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param shape The desired shape for this drawable: {@link #LINE},
 *              {@link #OVAL}, {@link #RECTANGLE} or {@link #RING}
 *
 * @see #mutate()
 */
void GradientDrawable::setShape(int shape) {
    m_ringPath.reset();
    m_pathIsDirty = true;
    m_gradientState.setShape(shape);
    invalidateSelf();
}

/**
 * <p>Sets the type of gradient used by this drawable..</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param gradient The type of the gradient: {@link #LINEAR_GRADIENT},
 *                 {@link #RADIAL_GRADIENT} or {@link #SWEEP_GRADIENT}
 *
 * @see #mutate()
 */
void GradientDrawable::setGradientType(int gradient) {
    m_gradientState.setGradientType(gradient);
    m_rectIsDirty = true;
    invalidateSelf();
}

/**
 * <p>Sets the center location of the gradient. The radius is honored only when
 * the gradient type is set to {@link #RADIAL_GRADIENT} or {@link #SWEEP_GRADIENT}.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param x The x coordinate of the gradient's center
 * @param y The y coordinate of the gradient's center
 *
 * @see #mutate()
 * @see #setGradientType(int)
 */
void GradientDrawable::setGradientCenter(float x, float y) {
    m_gradientState.setGradientCenter(x, y);
    m_rectIsDirty = true;
    invalidateSelf();
}

/**
 * <p>Sets the radius of the gradient. The radius is honored only when the
 * gradient type is set to {@link #RADIAL_GRADIENT}.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param gradientRadius The radius of the gradient in pixels
 *
 * @see #mutate()
 * @see #setGradientType(int)
 */
void GradientDrawable::setGradientRadius(float gradientRadius) {
    m_gradientState.setGradientRadius(gradientRadius);
    m_rectIsDirty = true;
    invalidateSelf();
}

/**
 * <p>Sets whether or not this drawable will honor its <code>level</code>
 * property.</p>
 * <p><strong>Note</strong>: changing this property will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing this property.</p>
 *
 * @param useLevel True if this drawable should honor its level, false otherwise
 *
 * @see #mutate()
 * @see #setLevel(int)
 * @see #getLevel()
 */
void GradientDrawable::setUseLevel(bool useLevel) {
    m_gradientState.mUseLevel = useLevel;
    m_rectIsDirty = true;
    invalidateSelf();
}

int GradientDrawable::modulateAlpha(int alpha) {
    int scale = m_alpha + (m_alpha >> 7);
    return alpha * scale >> 8;
}

/**
 * Returns the orientation of the gradient defined in this drawable.
 */
GradientDrawable::Orientation GradientDrawable::getOrientation() {
    return m_gradientState.mOrientation;
}

/**
 * <p>Changes the orientation of the gradient defined in this drawable.</p>
 * <p><strong>Note</strong>: changing orientation will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing the Orientation::</p>
 *
 * @param orientation The desired orientation (angle) of the gradient
 *
 * @see #mutate()
 */
void GradientDrawable::setOrientation(Orientation orientation) {
    m_gradientState.mOrientation = orientation;
    m_rectIsDirty = true;
    invalidateSelf();
}

/**
 * <p>Sets the colors used to draw the gradient. Each color is specified as an
 * ARGB integer and the array must contain at least 2 colors.</p>
 * <p><strong>Note</strong>: changing orientation will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing the Orientation::</p>
 *
 * @param colors 2 or more ARGB colors
 *
 * @see #mutate()
 * @see #setColor(int)
 */
void GradientDrawable::setColors(vector<int> colors) {
    m_gradientState.setColors(colors);
    m_rectIsDirty = true;
    invalidateSelf();
}

void GradientDrawable::draw(shared_ptr<Canvas> canvas) {
    if (!ensureValidRect()) {
        // nothing to draw
        return;
    }
    
    // remember the alpha values, in case we temporarily overwrite them
    // when we modulate them with m_alpha
    const int prevFillAlpha = m_fillPaint.getAlpha();
    const int prevStrokeAlpha = m_strokePaint.getAlpha();
    // compute the modulate alpha values
    const int currFillAlpha = modulateAlpha(prevFillAlpha);
    const int currStrokeAlpha = modulateAlpha(prevStrokeAlpha);
    
    const bool haveStroke = currStrokeAlpha > 0 && m_strokePaint.getStrokeWidth() > 0;
    const bool haveFill = currFillAlpha > 0;
    const GradientState st = m_gradientState;
    /*  we need a layer iff we're drawing both a fill and stroke, and the
     stroke is non-opaque, and our shapetype actually supports
     fill+stroke. Otherwise we can just draw the stroke (if any) on top
     of the fill (if any) without worrying about blending artifacts.
     */
    const bool useLayer = haveStroke && haveFill && st.mShape != LINE &&
    currStrokeAlpha < 255 && (m_alpha < 255 || m_colorFilter != NULL);
    
    /*  Drawing with a layer is slower than diRect drawing, but it
     allows us to apply paint effects like alpha and colorfilter to
     the result of multiple separate draws. In our case, if the user
     asks for a non-opaque alpha value (via setAlpha), and we're
     stroking, then we need to apply the alpha AFTER we've drawn
     both the fill and the stroke.
     */
    if (useLayer) {
        m_layerPaint.setDither(m_dither);
        m_layerPaint.setAlpha(m_alpha);
        m_layerPaint.setColorFilter(m_colorFilter);
        
        float rad = m_strokePaint.getStrokeWidth();
        canvas->saveLayer(m_rect.left() - rad, m_rect.top() - rad,
                          m_rect.right() + rad, m_rect.bottom() + rad,
                          &m_layerPaint, canvas->HAS_ALPHA_LAYER_SAVE_FLAG);
        
        // don't perform the filter in our individual paints
        // since the layer will do it for us
        m_fillPaint.setColorFilter(NULL);
        m_strokePaint.setColorFilter(NULL);
    } else {
        /*  if we're not using a layer, apply the dither/filter to our
         individual paints
         */
        m_fillPaint.setAlpha(currFillAlpha);
        m_fillPaint.setDither(m_dither);
        m_fillPaint.setColorFilter(m_colorFilter);
        if (m_colorFilter != NULL && !m_gradientState.mHasSolidColor) {
            m_fillPaint.setColor(m_alpha << 24);
        }
        if (haveStroke) {
            m_strokePaint.setAlpha(currStrokeAlpha);
            m_strokePaint.setDither(m_dither);
            m_strokePaint.setColorFilter(m_colorFilter);
        }
    }
    
    switch (st.mShape) {
        case RECTANGLE:
            if (!st.mRadiusArray.empty()) {
                if (m_pathIsDirty || m_rectIsDirty) {
                    m_path.reset();
                    m_path.addRoundRect(m_rect, st.mRadiusArray, Path::Direction::kCW_Direction);
                    m_pathIsDirty = m_rectIsDirty = false;
                }
                canvas->drawPath(&m_path, &m_fillPaint);
                if (haveStroke) {
                    canvas->drawPath(&m_path, &m_strokePaint);
                }
            } else if (st.mRadius > 0.0f) {
                // since the caller is only giving us 1 value, we will force
                // it to be square if the Rect is too small in one dimension
                // to show it. If we did nothing, Skia would clamp the rad
                // independently along each axis, giving us a thin ellipse
                // if the Rect were very wide but not very tall
                float rad = st.mRadius;
                float r = min(m_rect.width(), m_rect.height()) * 0.5f;
                if (rad > r) {
                    rad = r;
                }
                canvas->drawRoundRect(m_rect, rad, rad, &m_fillPaint);
                if (haveStroke) {
                    canvas->drawRoundRect(m_rect, rad, rad, &m_strokePaint);
                }
            } else {
                if (m_fillPaint.getColor() != 0 || m_colorFilter != NULL ||
                    m_fillPaint.getShader() != NULL) {
                    canvas->drawRect(m_rect, &m_fillPaint);
                }
                if (haveStroke) {
                    canvas->drawRect(m_rect, &m_strokePaint);
                }
            }
            break;
        case OVAL:
            canvas->drawOval(m_rect, &m_fillPaint);
            if (haveStroke) {
                canvas->drawOval(m_rect, &m_strokePaint);
            }
            break;
        case LINE: {
            RectF &r = m_rect;
            float y = r.centerY();
            canvas->drawLine(r.left(), y, r.right(), y, &m_strokePaint);
            break;
        }
        case RING:
            buildRing(st);
            canvas->drawPath(&m_ringPath, &m_fillPaint);
            if (haveStroke) {
                canvas->drawPath(&m_ringPath, &m_strokePaint);
            }
            break;
    }
    
    if (useLayer) {
        canvas->restore();
    } else {
        m_fillPaint.setAlpha(prevFillAlpha);
        if (haveStroke) {
            m_strokePaint.setAlpha(prevStrokeAlpha);
        }
    }
}

void GradientDrawable::buildRing(GradientState st) {
    
    if (!m_ringPath.isEmpty() && (!st.mUseLevelForShape || !m_pathIsDirty)) return;
    m_pathIsDirty = false;
    
    float sweep = st.mUseLevelForShape ? (360.0f * getLevel() / 10000.0f) : 360.0f;
    
    RectF bounds = RectF(m_rect);
    
    float x = bounds.width() / 2.0f;
    float y = bounds.height() / 2.0f;
    
    float thickness = st.mThickness != -1 ?
    st.mThickness : bounds.width() / st.mThicknessRatio;
    // inner radius
    float radius = st.mInnerRadius != -1 ?
    st.mInnerRadius : bounds.width() / st.mInnerRadiusRatio;
    
    RectF innerBounds = RectF(bounds);
    innerBounds.inset(x - radius, y - radius);
    
    bounds = RectF(innerBounds);
    bounds.inset(-thickness, -thickness);
    
    if (m_ringPath.isEmpty()) {
        m_ringPath = Path();
    } else {
        m_ringPath.reset();
    }
    
    // arcTo treats the sweep angle mod 360, so check for that, since we
    // think 360 means draw the entire oval
    if (sweep < 360 && sweep > -360) {
        m_ringPath.setFillType(Path::FillType::kEvenOdd_FillType);
        // inner top
        m_ringPath.moveTo(x + radius, y);
        // outer top
        m_ringPath.lineTo(x + radius + thickness, y);
        // outer arc
        m_ringPath.arcTo(bounds, 0.0f, sweep, false);
        // inner arc
        m_ringPath.arcTo(innerBounds, sweep, -sweep, false);
        m_ringPath.close();
    } else {
        // add the entire ovals
        m_ringPath.addOval(bounds, Path::Direction::kCW_Direction);
        m_ringPath.addOval(innerBounds, Path::Direction::kCCW_Direction);
    }
}

/**
 * <p>Changes this drawbale to use a single color instead of a gradient.</p>
 * <p><strong>Note</strong>: changing color will affect all instances
 * of a drawable loaded from a resource. It is recommended to invoke
 * {@link #mutate()} before changing the color.</p>
 *
 * @param argb The color used to fill the shape
 *
 * @see #mutate()
 * @see #setColors(vector<int>)
 */
void GradientDrawable::setColor(int argb) {
    m_gradientState.setSolidColor(argb);
    m_fillPaint.setColor(argb);
    this->invalidateSelf();
}

int GradientDrawable::getChangingConfigurations() {
    return Drawable::getChangingConfigurations() | m_gradientState.mChangingConfigurations;
}

void GradientDrawable::setAlpha(int alpha) {
    if (alpha != m_alpha) {
        m_alpha = alpha;
        this->invalidateSelf();
    }
}

void GradientDrawable::setDither(bool dither) {
    if (dither != m_dither) {
        m_dither = dither;
        invalidateSelf();
    }
}

void GradientDrawable::setColorFilter(ColorFilter *cf) {
    if (cf != m_colorFilter) {
        m_colorFilter = cf;
        invalidateSelf();
    }
}

int GradientDrawable::getOpacity() {
    return m_gradientState.mOpaque ? PixelFormats::PIXEL_FORMAT_OPAQUE : PixelFormats::PIXEL_FORMAT_TRANSLUCENT;
}

void GradientDrawable::onBoundsChange(Rect r) {
    Drawable::onBoundsChange(r);
    m_ringPath.reset();
    m_pathIsDirty = true;
    m_rectIsDirty = true;
}

bool GradientDrawable::onLevelChange(int level) {
    Drawable::onLevelChange(level);
    m_rectIsDirty = true;
    m_pathIsDirty = true;
    invalidateSelf();
    return true;
}

/**
 * This checks m_rectIsDirty, and if it is true, recomputes both our drawing
 * rectangle (m_rect) and the gradient itself, since it depends on our
 * rectangle too.
 * @return true if the resulting rectangle is not empty, false otherwise
 */
bool GradientDrawable::ensureValidRect() {
    if (m_rectIsDirty) {
        m_rectIsDirty = false;
        
        Rect bounds = getBounds();
        float inset = 0;
        
        inset = m_strokePaint.getStrokeWidth() * 0.5f;
        
        m_rect.set(bounds.left() + inset, bounds.top() + inset,
                   bounds.right() - inset, bounds.bottom() - inset);
        
        vector<int> *colors = &m_gradientState.mColors;
        if (colors != NULL && colors->size() > 0) {
            float x0, x1, y0, y1;
            
            if (m_gradientState.mGradient == LINEAR_GRADIENT) {
                const float level = m_gradientState.mUseLevel ? (float) getLevel() / 10000.0f : 1.0f;
                switch (m_gradientState.mOrientation) {
                    case TOP_BOTTOM:
                        x0 = m_rect.left();            y0 = m_rect.top();
                        x1 = x0;                     y1 = level * m_rect.bottom();
                        break;
                    case TR_BL:
                        x0 = m_rect.right();           y0 = m_rect.top();
                        x1 = level * m_rect.left();    y1 = level * m_rect.bottom();
                        break;
                    case RIGHT_LEFT:
                        x0 = m_rect.right();           y0 = m_rect.top();
                        x1 = level * m_rect.left();    y1 = y0;
                        break;
                    case BR_TL:
                        x0 = m_rect.right();           y0 = m_rect.bottom();
                        x1 = level * m_rect.left();    y1 = level * m_rect.top();
                        break;
                    case BOTTOM_TOP:
                        x0 = m_rect.left();            y0 = m_rect.bottom();
                        x1 = x0;                     y1 = level * m_rect.top();
                        break;
                    case BL_TR:
                        x0 = m_rect.left();            y0 = m_rect.bottom();
                        x1 = level * m_rect.right();   y1 = level * m_rect.top();
                        break;
                    case LEFT_RIGHT:
                        x0 = m_rect.left();            y0 = m_rect.top();
                        x1 = level * m_rect.right();   y1 = y0;
                        break;
                    default:/* TL_BR */
                        x0 = m_rect.left();            y0 = m_rect.top();
                        x1 = level * m_rect.right();   y1 = level * m_rect.bottom();
                        break;
                }
                
                m_fillPaint.setShader(Gradient::createLinear(x0, y0, x1, y1, *colors, m_gradientState.mPositions, Shader::TileMode::kClamp_TileMode));
                if (!m_gradientState.mHasSolidColor) {
                    m_fillPaint.setColor(m_alpha << 24);
                }
            } else if (m_gradientState.mGradient == RADIAL_GRADIENT) {
                x0 = m_rect.left() + (m_rect.right() - m_rect.left()) * m_gradientState.mCenterX;
                y0 = m_rect.top() + (m_rect.bottom() - m_rect.top()) * m_gradientState.mCenterY;
                
                const float level = m_gradientState.mUseLevel ? (float) getLevel() / 10000.0f : 1.0f;
                
                m_fillPaint.setShader(Gradient::createRadial(x0, y0,
                                                             level * m_gradientState.mGradientRadius, *colors, vector<float>(),
                                                             Shader::TileMode::kClamp_TileMode));
                if (!m_gradientState.mHasSolidColor) {
                    m_fillPaint.setColor(m_alpha << 24);
                }
            } else if (m_gradientState.mGradient == SWEEP_GRADIENT) {
                x0 = m_rect.left() + (m_rect.right() - m_rect.left()) * m_gradientState.mCenterX;
                y0 = m_rect.top() + (m_rect.bottom() - m_rect.top()) * m_gradientState.mCenterY;
                
                vector<int> *tempColors = colors;
                vector<float> *tempPositions = NULL;
                
                if (m_gradientState.mUseLevel) {
                    tempColors = &m_gradientState.mTempColors;
                    const int length = colors->size();
                    if (tempColors == NULL || tempColors->size() != length + 1) {
                        m_gradientState.mTempColors = vector<int>(length + 1);
                        tempColors = &m_gradientState.mTempColors;
                    }
                    colors = tempColors;
                    tempColors[length] = colors[length - 1];
                    
                    tempPositions = &m_gradientState.mTempPositions;
                    const float fraction = 1.0f / (float) (length - 1);
                    if (tempPositions == NULL || tempPositions->size() != length + 1) {
                        m_gradientState.mTempPositions = vector<float>(length + 1);
                        tempPositions = &m_gradientState.mTempPositions;
                    }
                    
                    const float level = (float) getLevel() / 10000.0f;
                    for (int i = 0; i < length; i++) {
                        tempPositions->at(i) = i * fraction * level;
                    }
                    tempPositions->at(length) = 1.0f;
                    
                }
                m_fillPaint.setShader(Gradient::createSweep(x0, y0, *tempColors, *tempPositions));
                if (!m_gradientState.mHasSolidColor) {
                    m_fillPaint.setColor(m_alpha << 24);
                }
            }
        }
    }
    return !m_rect.isEmpty();
}

int GradientDrawable::getShapeType(string shape) {
    
    if (shape.compare("oval") == 0) {
        return OVAL;
    } else if (shape.compare("line") == 0) {
        return LINE;
    } else if (shape.compare("ring") == 0) {
        return RING;
    }
    
    return RECTANGLE;
}

int GradientDrawable::getGradientType(string gradient) {
    
    if (gradient.compare("linear_gradient") == 0) {
        return LINEAR_GRADIENT;
    } else if (gradient.compare("radial_gradient") == 0) {
        return RADIAL_GRADIENT;
    } else if (gradient.compare("sweep_gradient") == 0) {
        return SWEEP_GRADIENT;
    }
    
    return LINEAR_GRADIENT;
}

void GradientDrawable::inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    
    attrs->loadAttributes();
    
    GradientState &st = m_gradientState;
    
    Drawable::inflateWithAttributes(r, parser, attrs, R::styleable::visible);
    
    string shape(attrs->getAttributeValue(R::styleable::shape));
    int shapeType = getShapeType(shape);
    bool dither = attrs->getAttributeBooleanValue(R::styleable::dither, false);
    
    if (shapeType == RING) {
        st.mInnerRadius = attrs->getDimensionValue(r, R::styleable::innerRadius, -1);
        if (st.mInnerRadius == -1) {
            st.mInnerRadiusRatio = attrs->getAttributeFloatValue(R::styleable::innerRadiusRatio, 3.0f);
        }
        st.mThickness = attrs->getDimensionValue(r, R::styleable::thickness, -1);
        if (st.mThickness == -1) {
            st.mThicknessRatio = attrs->getAttributeFloatValue(R::styleable::thicknessRatio, 9.0f);
        }
        st.mUseLevelForShape = attrs->getAttributeBooleanValue(R::styleable::useLevel, true);
    }
    
    setShape(shapeType);
    setDither(dither);
    
    if (parser.hasChildren()) {
        
        parser.firstChild();
        
        do {
            
            attrs->loadAttributes();
            const string name(parser.getName());
            
            if (name.compare("size") == 0) {
                
                int width = attrs->getDimensionValue(r, R::styleable::width, -1);
                int height = attrs->getDimensionValue(r, R::styleable::height, -1);
                
                setSize(width, height);
                
            } else if (name.compare("gradient") == 0) {
                int startColor = attrs->getAttributeColorValue(r, R::styleable::startColor, 0);
                int centerColor = attrs->getAttributeColorValue(r, R::styleable::centerColor, -1);
                bool hasCenterColor = (centerColor != -1);
                if (!hasCenterColor) {
                    centerColor = 0;
                }
                int endColor = attrs->getAttributeColorValue(r, R::styleable::endColor, 0);
                int gradientType = attrs->getAttributeIntValue(R::styleable::type,
                                                               LINEAR_GRADIENT);
                
                st.mCenterX = getFloatOrFraction(
                                                 attrs,
                                                 R::styleable::centerX,
                                                 0.5f);
                
                st.mCenterY = getFloatOrFraction(
                                                 attrs,
                                                 R::styleable::centerY,
                                                 0.5f);
                
                st.mUseLevel = attrs->getAttributeBooleanValue(
                                                               R::styleable::useLevel, false);
                st.mGradient = gradientType;
                
                if (gradientType == LINEAR_GRADIENT) {
                    int angle = (int) attrs->getAttributeFloatValue(R::styleable::angle, 0.0f);
                    angle %= 360;
                    if (angle % 45 != 0) {
                        //                       throw new XmlPullParserException(a.getPositionDescription()
                        //                                                        + "<gradient> tag requires 'angle' attribute to "
                        //                                                        + "be a multiple of 45");
                    }
                    
                    switch (angle) {
                        case 0:
                            st.mOrientation = Orientation::LEFT_RIGHT;
                            break;
                        case 45:
                            st.mOrientation = Orientation::BL_TR;
                            break;
                        case 90:
                            st.mOrientation = Orientation::BOTTOM_TOP;
                            break;
                        case 135:
                            st.mOrientation = Orientation::BR_TL;
                            break;
                        case 180:
                            st.mOrientation = Orientation::RIGHT_LEFT;
                            break;
                        case 225:
                            st.mOrientation = Orientation::TR_BL;
                            break;
                        case 270:
                            st.mOrientation = Orientation::TOP_BOTTOM;
                            break;
                        case 315:
                            st.mOrientation = Orientation::TL_BR;
                            break;
                    }
                } else {
                    st.mGradientRadius = attrs->getAttributeFloatValue(R::styleable::gradientRadius, 1.0f);
                }
                
                if (hasCenterColor) {
                    st.mColors = vector<int>(3);
                    st.mColors[0] = startColor;
                    st.mColors[1] = centerColor;
                    st.mColors[2] = endColor;
                    
                    st.mPositions = vector<float>(3);
                    st.mPositions[0] = 0.0f;
                    // Since 0.5f is default value, try to take the one that isn't 0.5f
                    st.mPositions[1] = st.mCenterX != 0.5f ? st.mCenterX : st.mCenterY;
                    st.mPositions[2] = 1.0f;
                } else {
                    st.mColors = vector<int>(2);
                    st.mColors[0] = startColor;
                    st.mColors[1] = endColor;
                }
                
            } else if (name.compare("solid") == 0) {
                
                int argb = attrs->getAttributeColorValue(r, R::styleable::color, 0);
                setColor(argb);
                
            } else if (name.compare("stroke") == 0) {
                
                int width = attrs->getDimensionValue(r, R::styleable::width, 0);
                int color = attrs->getAttributeColorValue(r, R::styleable::color, 0);
                float dashWidth = attrs->getDimensionValue(r, R::styleable::dashWidth, 0);
                if (dashWidth != 0.0f) {
                    float dashGap = attrs->getDimensionValue(r, R::styleable::dashGap, 0);
                    setStroke(width, color, dashWidth, dashGap);
                } else {
                    setStroke(width, color);
                }
                
            } else if (name.compare("corners") == 0) {
                int radius = attrs->getDimensionValue(r, R::styleable::radius, 0);
                setCornerRadius(radius);
                int topLeftRadius = attrs->getDimensionValue(r, R::styleable::topLeftRadius, radius);
                int topRightRadius = attrs->getDimensionValue(r, R::styleable::topRightRadius, radius);
                int bottomLeftRadius = attrs->getDimensionValue(r, R::styleable::bottomLeftRadius, radius);
                int bottomRightRadius = attrs->getDimensionValue(r, R::styleable::bottomRightRadius, radius);
                if (topLeftRadius != radius || topRightRadius != radius ||
                    bottomLeftRadius != radius || bottomRightRadius != radius) {
                    // The corner radii are specified in clockwise order (see Path.addRoundRect())
                    vector<float> radii = {
                        (float) topLeftRadius, (float) topLeftRadius,
                        (float) topRightRadius, (float) topRightRadius,
                        (float) bottomRightRadius, (float) bottomRightRadius,
                        (float) bottomLeftRadius, (float) bottomLeftRadius
                    };
                    setCornerRadii(radii);
                }
            } else if (name.compare("padding") == 0) {
                
                mPadding = Rect(
                                     attrs->getDimensionValue(r, R::styleable::left, 0),
                                     attrs->getDimensionValue(r, R::styleable::top, 0),
                                     attrs->getDimensionValue(r, R::styleable::right, 0),
                                     attrs->getDimensionValue(r, R::styleable::bottom, 0));
                m_gradientState.mPadding = mPadding;
                
            } else {
                
                CCLOG("Bad element under <shape>: %s", name.c_str());
            }
            
        } while (parser.next());
        
        parser.toParent();
    }
    
    m_gradientState.computeOpacity();
}

float GradientDrawable::getFloatOrFraction(AttributeSet *attrs, const char* index, float defaultValue) {
    return attrs->getAttributeFloatValue(index, defaultValue);
}

int GradientDrawable::getIntrinsicWidth() {
    return m_gradientState.mWidth;
}

int GradientDrawable::getIntrinsicHeight() {
    return m_gradientState.mHeight;
}

Drawable::ConstantState *GradientDrawable::getConstantState() {
    m_gradientState.mChangingConfigurations = getChangingConfigurations();
    return &m_gradientState;
}

shared_ptr<Drawable> GradientDrawable::mutate() {
    if (!m_mutated && Drawable::mutate() == shared_from_this()) {
        m_gradientState = GradientState(m_gradientState);
        initializeWithState(m_gradientState);
        m_mutated = true;
    }
    return shared_from_this();
}

void GradientDrawable::initializeWithState(GradientState state) {
    if (state.mHasSolidColor) {
        m_fillPaint.setColor(state.mSolidColor);
    } else if (state.mColors.empty()) {
        // If we don't have a solid color and we don't have a gradient,
        // the app is stroking the shape, set the color to the default
        // value of state.mSolidColor
        m_fillPaint.setColor(0);
    }
    mPadding = state.mPadding;
    if (state.mStrokeWidth >= 0) {
        m_strokePaint = Paint(Paint::Flags::kAntiAlias_Flag);
        m_strokePaint.setStyle(Paint::Style::kStroke_Style);
        m_strokePaint.setStrokeWidth(state.mStrokeWidth);
        m_strokePaint.setColor(state.mStrokeColor);
        
        if (state.mStrokeDashWidth != 0.0f) {
            float interval[2] = {state.mStrokeDashWidth, state.mStrokeDashGap};
            DashPathEffect *e = new DashPathEffect(interval, 2, 0);
            m_strokePaint.setPathEffect(e);
        }
    }
}

ANDROID_END
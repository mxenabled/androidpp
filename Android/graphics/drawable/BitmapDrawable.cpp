//
//  BitmapDrawable.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/14/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "BitmapDrawable.h"


#include "Android/graphics/Shader.h"

ANDROID_BEGIN

const uint32_t BitmapDrawable::DEFAULT_PAINT_FLAGS = Paint::Flags::kFilterBitmap_Flag | Paint::Flags::kDither_Flag;

/**
 * Create an empty drawable, not dealing with density.
 * @deprecated Use {@link #BitmapDrawable(android.content.res.Resources, android.graphics.Bitmap)}
 * instead to specify a bitmap to draw with and ensure the correct density is set.
 */
BitmapDrawable::BitmapDrawable() {
    m_bitmapState = BitmapState((Bitmap*)NULL);
}

/**
 * Create an empty drawable, setting initial target density based on
 * the display metrics of the resources.
 * @deprecated Use {@link #BitmapDrawable(android.content.res.Resources, android.graphics.Bitmap)}
 * instead to specify a bitmap to draw with.
 */
BitmapDrawable::BitmapDrawable(Resources *res) {
    m_bitmapState = BitmapState((Bitmap*)NULL);
    m_bitmapState.m_targetDensity = m_targetDensity;
}

/**
 * Create drawable from a bitmap, not dealing with density.
 * @deprecated Use {@link #BitmapDrawable(Resources, Bitmap)} to ensure
 * that the drawable has correctly set its target density.
 */
BitmapDrawable::BitmapDrawable(Bitmap *bitmap) : BitmapDrawable(BitmapState(bitmap), (Resources*) NULL) {
}

/**
 * Create drawable from a bitmap, setting initial target density based on
 * the display metrics of the resources.
 */
BitmapDrawable::BitmapDrawable(Resources *res, Bitmap *bitmap) : BitmapDrawable(BitmapState(bitmap), res) {
    m_bitmapState.m_targetDensity = m_targetDensity;
}

/**
 * Create a drawable by opening a given file path and decoding the bitmap.
 * @deprecated Use {@link #BitmapDrawable(Resources, String)} to ensure
 * that the drawable has correctly set its target density.
 */
BitmapDrawable::BitmapDrawable(const char* filepath) :
BitmapDrawable(BitmapState(initBitmap(filepath)), (Resources*) NULL) {

}

Bitmap *BitmapDrawable::initBitmap(const char* filepath) {
    Bitmap *bitmap = new Bitmap();
    BitmapFactory::decodeFile(bitmap, filepath);
    return bitmap;
}

/**
 * Create a drawable by opening a given file path and decoding the bitmap.
 */
BitmapDrawable::BitmapDrawable(Resources *res, const char* filepath) : BitmapDrawable(BitmapState(initBitmap(filepath)), (Resources*) NULL) {
    m_bitmapState.m_targetDensity = m_targetDensity;
}

/**
 * Returns the paint used to render this drawable.
 */
Paint BitmapDrawable::getPaint() {
    return m_bitmapState.m_paint;
}

/**
 * Returns the bitmap used by this drawable to render. May be NULL.
 */
Bitmap *BitmapDrawable::getBitmap() {
    return m_bitmap;
}

void BitmapDrawable::computeBitmapSize() {
    m_bitmapWidth = m_bitmap->getScaledWidth(m_targetDensity);
    m_bitmapHeight = m_bitmap->getScaledHeight(m_targetDensity);
}

void BitmapDrawable::setBitmap(Bitmap *bitmap) {
    if (bitmap != m_bitmap) {
        m_bitmap = bitmap;
        if (bitmap) {
            computeBitmapSize();
        } else {
            m_bitmapWidth = m_bitmapHeight = -1;
        }
        invalidateSelf();
    }
}

void BitmapDrawable::setColorFilter(ColorFilter *cf) {
    m_bitmapState.m_paint.setColorFilter(cf);
    invalidateSelf();
}

/**
 * Set the density scale at which this drawable will be rendered.
 *
 * @param metrics The DisplayMetrics indicating the density scale for this drawable.
 *
 * @see android.graphics.Bitmap#setDensity(int)
 * @see android.graphics.Bitmap#getDensity()
 */
void BitmapDrawable::setTargetDensity(DisplayMetrics *metrics) {
    setTargetDensity(metrics->densityDpi);
}

/**
 * Set the density at which this drawable will be rendered.
 *
 * @param density The density scale for this drawable.
 *
 * @see android.graphics.Bitmap#setDensity(int)
 * @see android.graphics.Bitmap#getDensity()
 */
void BitmapDrawable::setTargetDensity(int density) {
    if (m_targetDensity != density) {
        m_targetDensity = density == 0 ? DisplayMetrics::DENSITY_DEFAULT : density;
        if (m_bitmap != NULL) {
            computeBitmapSize();
        }
        invalidateSelf();
    }
}

/** Get the gravity used to position/stretch the bitmap within its bounds.
 * See android.view.Gravity
 * @return the gravity applied to the bitmap
 */
int BitmapDrawable::getGravity() {
    return m_bitmapState.m_gravity;
}

/** Set the gravity used to position/stretch the bitmap within its bounds.
 See android.view.Gravity
 * @param gravity the gravity
 */
void BitmapDrawable::setGravity(int gravity) {
    if (m_bitmapState.m_gravity != gravity) {
        m_bitmapState.m_gravity = gravity;
        m_applyGravity = true;
        invalidateSelf();
    }
}

/**
 * Enables or disables anti-aliasing for this drawable. Anti-aliasing affects
 * the edges of the bitmap only so it applies only when the drawable is rotated.
 *
 * @param aa True if the bitmap should be anti-aliased, false otherwise.
 *
 * @see #hasAntiAlias()
 */
void BitmapDrawable::setAntiAlias(bool aa) {
    m_bitmapState.m_paint.setAntiAlias(aa);
    invalidateSelf();
}

/**
 * Indicates whether anti-aliasing is enabled for this drawable.
 *
 * @return True if anti-aliasing is enabled, false otherwise.
 *
 * @see #setAntiAlias(bool)
 */
bool BitmapDrawable::hasAntiAlias() {
    return m_bitmapState.m_paint.isAntiAlias();
}

void BitmapDrawable::setFilterBitmap(bool filter) {
    m_bitmapState.m_paint.setFilterBitmap(filter);
    invalidateSelf();
}

void BitmapDrawable::setDither(bool dither) {
    m_bitmapState.m_paint.setDither(dither);
    invalidateSelf();
}

/**
 * Indicates the repeat behavior of this drawable on the X axis.
 *
 * @return {@link SkShader::TileMode#CLAMP} if the bitmap does not repeat,
 *         {@link SkShader::TileMode#REPEAT} or {@link SkShader::TileMode#MIRROR} otherwise.
 */
SkShader::TileMode BitmapDrawable::getTileModeX() {
    return m_bitmapState.m_tileModeX;
}

/**
 * Indicates the repeat behavior of this drawable on the Y axis.
 *
 * @return {@link SkShader::TileMode#CLAMP} if the bitmap does not repeat,
 *         {@link SkShader::TileMode#REPEAT} or {@link SkShader::TileMode#MIRROR} otherwise.
 */
SkShader::TileMode BitmapDrawable::getTileModeY() {
    return m_bitmapState.m_tileModeY;
}

/**
 * Sets the repeat behavior of this drawable on the X axis. By default, the drawable
 * does not repeat its bitmap. Using {@link SkShader::TileMode#REPEAT} or
 * {@link SkShader::TileMode#MIRROR} the bitmap can be repeated (or tiled) if the bitmap
 * is smaller than this drawable.
 *
 * @param mode The repeat mode for this drawable.
 *
 * @see #setTileModeY(android.graphics.SkShader::TileMode)
 * @see #setTileModeXY(android.graphics.SkShader::TileMode, android.graphics.SkShader::TileMode)
 */
void BitmapDrawable::setTileModeX(SkShader::TileMode mode) {
    setTileModeXY(mode, m_bitmapState.m_tileModeY);
}

/**
 * Sets the repeat behavior of this drawable on the Y axis. By default, the drawable
 * does not repeat its bitmap. Using {@link SkShader::TileMode#REPEAT} or
 * {@link SkShader::TileMode#MIRROR} the bitmap can be repeated (or tiled) if the bitmap
 * is smaller than this drawable.
 *
 * @param mode The repeat mode for this drawable.
 *
 * @see #setTileModeX(android.graphics.SkShader::TileMode)
 * @see #setTileModeXY(android.graphics.SkShader::TileMode, android.graphics.SkShader::TileMode)
 */
void BitmapDrawable::setTileModeY(SkShader::TileMode mode) {
    setTileModeXY(m_bitmapState.m_tileModeX, mode);
}

/**
 * Sets the repeat behavior of this drawable on both axis. By default, the drawable
 * does not repeat its bitmap. Using {@link SkShader::TileMode#REPEAT} or
 * {@link SkShader::TileMode#MIRROR} the bitmap can be repeated (or tiled) if the bitmap
 * is smaller than this drawable.
 *
 * @param xmode The X repeat mode for this drawable.
 * @param ymode The Y repeat mode for this drawable.
 *
 * @see #setTileModeX(android.graphics.SkShader::TileMode)
 * @see #setTileModeY(android.graphics.SkShader::TileMode)
 */
void BitmapDrawable::setTileModeXY(SkShader::TileMode xmode, SkShader::TileMode ymode) {
    if (m_bitmapState.m_tileModeX != xmode || m_bitmapState.m_tileModeY != ymode) {
        m_bitmapState.m_tileModeX = xmode;
        m_bitmapState.m_tileModeY = ymode;
        m_bitmapState.m_useShader = true;
        m_bitmapState.m_rebuildShader = true;
        invalidateSelf();
    }
}

int BitmapDrawable::getChangingConfigurations() {
    return Drawable::getChangingConfigurations() | m_bitmapState.m_changingConfigurations;
}

void BitmapDrawable::onBoundsChange(Rect bounds) {
    Drawable::onBoundsChange(bounds);
    m_applyGravity = true;
}

void BitmapDrawable::draw(androidcpp::shared_ptr<Canvas> canvas) {
    if (m_bitmap) {
        if (m_bitmapState.m_rebuildShader) {
            SkShader::TileMode tmx = m_bitmapState.m_tileModeX;
            SkShader::TileMode tmy = m_bitmapState.m_tileModeY;
            
            if (!m_bitmapState.m_useShader) {
                m_bitmapState.m_paint.setShader(NULL);
            } else {
                m_bitmapState.m_paint.setShader(Shader::createBitmapShader(m_bitmap, tmx, tmy));
            }
            m_bitmapState.m_rebuildShader = false;
            copyBounds(m_dstRect);
        }
        
        SkShader *shader = m_bitmapState.m_paint.getShader();
        if (!shader) {
            if (m_applyGravity) {
                const int layoutDirection = getLayoutDirection();
                Gravity::apply(m_bitmapState.m_gravity, m_bitmapWidth, m_bitmapHeight,
                              getBounds(), m_dstRect, layoutDirection);
                m_applyGravity = false;
            }
            int left = 0;
            int top = 0;
            int right = m_bitmap->width();
            int bottom = m_bitmap->height();
            canvas->drawBitmapRect(m_bitmap, left, top, right, bottom, m_dstRect.left(), m_dstRect.top(), m_dstRect.right(), m_dstRect.bottom(), &m_bitmapState.m_paint);
        } else {
            if (m_applyGravity) {
                copyBounds(m_dstRect);
                m_applyGravity = false;
            }
            canvas->drawRect(m_dstRect, &m_bitmapState.m_paint);
        }
    }
}

void BitmapDrawable::setAlpha(int alpha) {
    int oldAlpha = m_bitmapState.m_paint.getAlpha();
    if (alpha != oldAlpha) {
        m_bitmapState.m_paint.setAlpha(alpha);
        invalidateSelf();
    }
}

/**
 * A mutable BitmapDrawable still shares its Bitmap with any other Drawable
 * that comes from the same resource.
 *
 * @return This drawable.
 */
shared_ptr<Drawable> BitmapDrawable::mutate() {
    if (!m_mutated && Drawable::mutate() == shared_from_this()) {
        m_bitmapState = BitmapState(m_bitmapState);
        m_mutated = true;
    }
    return shared_from_this();
}

int BitmapDrawable::getIntrinsicWidth() {
    return m_bitmapWidth;
}

int BitmapDrawable::getIntrinsicHeight() {
    return m_bitmapHeight;
}

int BitmapDrawable::getOpacity() {
    
    if (m_bitmapState.m_gravity != Gravity::FILL) {
        return PIXEL_FORMAT_TRANSLUCENT;
    }
    
    return (!m_bitmap || m_bitmap->hasAlpha() || m_bitmapState.m_paint.getAlpha() < 255) ? PIXEL_FORMAT_TRANSLUCENT : PIXEL_FORMAT_OPAQUE;
}

Drawable::ConstantState *BitmapDrawable::getConstantState() {
    m_bitmapState.m_changingConfigurations = getChangingConfigurations();
    return &m_bitmapState;
}

void BitmapDrawable::inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    Drawable::inflate(r, parser, attrs);
    
    attrs->loadAttributes();
    
    string id(attrs->getAttributeValue(R::styleable::src));
    if (id.empty()) {
//        throw new XmlPullParserException(parser.getPositionDescription() +
//                                         ": <bitmap> requires a valid src attribute");
    }
    
    if (id.find("@drawable/") != string::npos) {
        id = id.substr(10, id.size());
    }
    
    const string path = r->getBitmapPath(id);
    
    if (path.empty()) return;
    
    Bitmap *bitmap = new Bitmap();
    BitmapFactory::decodeFile(bitmap, path.c_str());

    m_bitmapState.m_bitmap = bitmap;
    setBitmap(bitmap);
    
    DisplayMetrics metrics = r->getDisplayMetrics();
    setTargetDensity(&metrics);
    
    Paint &paint = m_bitmapState.m_paint;
    paint.setAntiAlias(attrs->getAttributeBooleanValue(R::styleable::antialias, paint.isAntiAlias()));
    paint.setFilterBitmap(attrs->getAttributeBooleanValue(R::styleable::filter, paint.isFilterBitmap()));
    paint.setDither(attrs->getAttributeBooleanValue(R::styleable::dither, paint.isDither()));
    
    const char* gravityAttr = attrs->getAttributeValue(R::styleable::gravity);
    int gravity = Gravity::FILL;
    
    if (gravityAttr) {
        string gravityStr(gravityAttr);
        gravity = Gravity::getGravity(gravityStr, Gravity::FILL);
    }
    
    setGravity(gravity);
    
    const char* tileModeAttr = attrs->getAttributeValue(R::styleable::tileMode);
    
    if (tileModeAttr) {
        string tileModeStr(tileModeAttr);
        Shader::TileMode mode = Shader::getTileMode(tileModeStr, Shader::TileMode::kTileModeCount);
        setTileModeXY(mode, mode);
    }
}

BitmapDrawable::BitmapDrawable(BitmapState state, Resources *res) {
    m_bitmapState = state;
    if (res) {
        m_targetDensity = res->getDisplayMetrics().densityDpi;
    } else {
        m_targetDensity = state.m_targetDensity;
    }
    setBitmap(m_bitmapState.m_bitmap);
}

ANDROID_END
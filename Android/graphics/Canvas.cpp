/*
 * File:   Canvas.cpp
 * Author: saulhoward
 *
 * Created on October 21, 2013, 3:39 PM
 */

#include "Canvas.h"

#include "cocos2d.h"

#include "Android/graphics/Bitmap.h"
#include "Android/graphics/paint.h"
#include "Android/graphics/Path.h"
#include "Android/graphics/Shader.h"
#include "Android/graphics/TextLayoutCache.h"
#include "Android/graphics/TextLayout.h"
#include "Android/text/GraphicsOperations.h"
#include "Android/text/String.h"
#include "Android/text/SpannedString.h"
#include "Android/text/SpannableString.h"
#include "Android/text/TextUtils.h"
#include "Android/utils/Exceptions.h"
#include "Android/utils/System.h"
#include "Android/view/GLES20DisplayList.h"

#include <SkColorFilter.h>

ANDROID_BEGIN

#define MODIFIER_SHADOW 1
#define MODIFIER_SHADER 2
#define MODIFIER_COLOR_FILTER 4

Canvas::Canvas(bool translucent) : Canvas(false, translucent) {
}

Canvas::Canvas(bool recording, bool translucent) {
    m_opaque = !translucent;
    if (recording) {
        createDisplayListRenderer();
    } else {
        createRenderer();
    }
}

Canvas::Canvas(GLES20DisplayList *displayList) : Canvas(true, true) {
    mDisplayList = displayList;
}

Canvas::~Canvas() {
    destroyRenderer();
}

// ----------------------------------------------------------------------------
// DisplayList
// ----------------------------------------------------------------------------

void Canvas::start() {
    mDisplayList->mChildrenDisplayLists.clear();
}

DisplayList *Canvas::end(DisplayList *displayList) {
    return getDisplayList(displayList);
}

void Canvas::recycle() {
    resetDisplayListRenderer();
}

// ----------------------------------------------------------------------------
// Caching
// ----------------------------------------------------------------------------

void Canvas::flushCaches(Caches::FlushMode mode) {
    if (Caches::hasInstance()) {
        Caches::getInstance().flush(mode);
    }
}

void Canvas::initCaches() {
    if (Caches::hasInstance()) {
        Caches::getInstance().init();
    }
}

void Canvas::terminateCaches() {
    if (Caches::hasInstance()) {
        Caches::getInstance().terminate();
    }
}

// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------

void Canvas::destroyRenderer() {
    delete m_renderer;
}

void Canvas::createRenderer() {
    m_renderer = new OpenGLRenderer;
}

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------

void Canvas::setViewport(int width, int height) {

    m_width = width;
    m_height = height;

    m_renderer->setViewport(width, height);
}

int Canvas::prepare(bool opaque) {
    return m_renderer->prepare(opaque);
}

int Canvas::prepareDirty(int left, int top, int right, int bottom, bool opaque) {
    return m_renderer->prepareDirty(left, top, right, bottom, opaque);
}

void Canvas::finish() {
    m_renderer->finish();
}

int Canvas::getStencilSize() {
    return OpenGLRenderer::getStencilSize();
}

// ----------------------------------------------------------------------------
// Functor
// ----------------------------------------------------------------------------

int Canvas::callDrawGLFunction(Functor* functor) {
    android::uirenderer::Rect dirty;
    return m_renderer->callDrawGLFunction(functor, dirty);
}

void Canvas::detachFunctor(Functor* functor) {
    m_renderer->detachFunctor(functor);
}

void Canvas::attachFunctor(Functor* functor) {
    m_renderer->attachFunctor(functor);
}

int Canvas::invokeFunctors(Rect *dirty) {
    android::uirenderer::Rect bounds;
    status_t status = m_renderer->invokeFunctors(bounds);
    if (status != DrawGlInfo::kStatusDone && dirty != NULL) {
        dirty->set(bounds.left, bounds.top, bounds.right, bounds.bottom);
    }
    return status;
}

// ----------------------------------------------------------------------------
// Misc
// ----------------------------------------------------------------------------

int Canvas::getMaxTextureWidth() {
    return Caches::getInstance().maxTextureSize;
}

int Canvas::getMaxTextureHeight() {
    return Caches::getInstance().maxTextureSize;
}

int Canvas::getWidth() {
    return m_width;
}

int Canvas::getHeight() {
    return m_height;
}

bool Canvas::isOpaque() {
    return m_opaque;
}

// ----------------------------------------------------------------------------
// State
// ----------------------------------------------------------------------------

int Canvas::save() {
    return save(Canvas::CLIP_SAVE_FLAG | Canvas::MATRIX_SAVE_FLAG);
}

int Canvas::save(int flags) {
    return m_renderer->save(flags);
}

int Canvas::getSaveCount() {
    return m_renderer->getSaveCount();
}

void Canvas::restore() {
    m_renderer->restore();
}

void Canvas::restoreToCount(int saveCount) {
    m_renderer->restoreToCount(saveCount);
}

// ----------------------------------------------------------------------------
// Display lists
// ----------------------------------------------------------------------------

android::uirenderer::DisplayList* Canvas::getDisplayList(android::uirenderer::DisplayList* displayList) {
    return m_displayRenderer->getDisplayList(displayList);
}

int Canvas::getDisplayListSize(android::uirenderer::DisplayList* displayList) {
    return displayList->getSize();
}

void Canvas::setDisplayListName(android::uirenderer::DisplayList* displayList, const char* textArray) {
    if (textArray) {
        displayList->setName(textArray);
    }
}

void Canvas::createDisplayListRenderer() {
    m_renderer = m_displayRenderer = new DisplayListRenderer;
}

void Canvas::resetDisplayListRenderer() {
    m_displayRenderer->reset();
}

void Canvas::destroyDisplayList(android::uirenderer::DisplayList* displayList) {
    android::uirenderer::DisplayList::destroyDisplayListDeferred(displayList);
}

int Canvas::drawDisplayList(android::uirenderer::DisplayList* displayList, Rect *dirty, int flags) {
    android::uirenderer::Rect bounds;
    status_t status = m_renderer->drawDisplayList(displayList, bounds, flags);
    if (status != DrawGlInfo::kStatusDone && dirty) {
        dirty->set(bounds.left, bounds.top, bounds.right, bounds.bottom);
    }
    return status;
}

void Canvas::outputDisplayList(android::uirenderer::DisplayList* displayList) {
    m_renderer->outputDisplayList(displayList);
}

// ----------------------------------------------------------------------------
// Layers
// ----------------------------------------------------------------------------

void Canvas::interrupt() {
    m_renderer->interrupt();
}

void Canvas::resume() {
    m_renderer->resume();
}

void Canvas::drawLayer(Layer* layer, float x, float y, Paint* paint) {
    m_renderer->drawLayer(layer, x, y, paint);
}

int Canvas::saveLayer(float left, float top, float right, float bottom, Paint* paint, int saveFlags) {
    return m_renderer->saveLayer(left, top, right, bottom, paint, saveFlags);
}

int Canvas::saveLayerClip(Paint* paint, int saveFlags) {
    const android::uirenderer::Rect& bounds(m_renderer->getClipBounds());
    return m_renderer->saveLayer(bounds.left, bounds.top, bounds.right, bounds.bottom,
            paint, saveFlags);
}

int Canvas::saveLayerAlpha(float left, float top, float right, float bottom,
        int alpha, int saveFlags) {
    return m_renderer->saveLayerAlpha(left, top, right, bottom, alpha, saveFlags);
}

int Canvas::saveLayerAlphaClip(int alpha, int saveFlags) {
    const android::uirenderer::Rect& bounds(m_renderer->getClipBounds());
    return m_renderer->saveLayerAlpha(bounds.left, bounds.top, bounds.right, bounds.bottom,
            alpha, saveFlags);
}

void Canvas::createLayerRenderer(Layer* layer) {
    if (layer) {
        m_renderer = new LayerRenderer(layer);
    }
}

Layer* Canvas::createTextureLayer(bool isOpaque, int* storage) {
    Layer* layer = LayerRenderer::createTextureLayer(isOpaque);

    if (layer) {
        storage[0] = layer->getTexture();
    }

    return layer;
}

Layer* Canvas::createLayer(int width, int height, bool isOpaque, int* storage) {
    Layer* layer = LayerRenderer::createLayer(width, height, isOpaque);

    if (layer) {
        storage[0] = layer->getWidth();
        storage[1] = layer->getHeight();
    }

    return layer;
}

void Canvas::resizeLayer(Layer* layer, int width, int height, int* storage) {
    LayerRenderer::resizeLayer(layer, width, height);

    storage[0] = layer->getWidth();
    storage[1] = layer->getHeight();
}

//void updateTextureLayer(Layer* layer, int width, int height, bool isOpaque, jobject surface) {
//    float transform[16];
//    shared_ptr<SurfaceTexture> surfaceTexture(SurfaceTexture_getSurfaceTexture(env, surface));
//
//    if (surfaceTexture->updateTexImage() == NO_ERROR) {
//        surfaceTexture->getTransformMatrix(transform);
//        GLenum renderTarget = surfaceTexture->getCurrentTextureTarget();
//
//        LayerRenderer::updateTextureLayer(layer, width, height, isOpaque, renderTarget, transform);
//    }
//}

void Canvas::updateRenderLayer(Layer* layer, android::uirenderer::DisplayList* displayList,
        int left, int top, int right, int bottom) {
    layer->updateDeferred(m_renderer, displayList, left, top, right, bottom);
}

void Canvas::setTextureLayerTransform(Layer* layer, SkMatrix* matrix) {
    layer->getTransform().load(*matrix);
}

void Canvas::destroyLayer(Layer* layer) {
    LayerRenderer::destroyLayer(layer);
}

void Canvas::destroyLayerDeferred(Layer* layer) {
    LayerRenderer::destroyLayerDeferred(layer);
}

void Canvas::flushLayer(Layer* layer) {
    LayerRenderer::flushLayer(layer);
}

bool Canvas::copyLayer(Layer* layer, Bitmap *bitmap) {
    return LayerRenderer::copyLayer(layer, bitmap);
}

// ----------------------------------------------------------------------------
// Clipping
// ----------------------------------------------------------------------------

bool Canvas::quickReject(float left, float top, float right, float bottom) {
    return m_renderer->quickReject(left, top, right, bottom);
}

bool Canvas::clipRectF(float left, float top, float right, float bottom, SkRegion::Op op) {
    return m_renderer->clipRect(left, top, right, bottom, op);
}

bool Canvas::clipRect(int left, int top, int right, int bottom, SkRegion::Op op) {
    return m_renderer->clipRect(float(left), float(top), float(right), float(bottom), op);
}

bool Canvas::getClipBounds(Rect *rect) {
    const android::uirenderer::Rect& bounds(m_renderer->getClipBounds());

    rect->set(int(bounds.left), int(bounds.top), int(bounds.right), int(bounds.bottom));

    return !bounds.isEmpty();
}

// ----------------------------------------------------------------------------
// Transforms
// ----------------------------------------------------------------------------

void Canvas::translate(float dx, float dy) {
    if (dx != 0.0f || dy != 0.0f) m_renderer->translate(dx, dy);
}

void Canvas::rotate(float degrees) {
    m_renderer->rotate(degrees);
}

void Canvas::rotate(float degrees, float px, float py) {
    translate(px, py);
    rotate(degrees);
    translate(-px, -py);
}

void Canvas::scale(float sx, float sy) {
    m_renderer->scale(sx, sy);
}

void Canvas::scale(float sx, float sy, float px, float py) {
    translate(px, py);
    scale(sx, sy);
    translate(-px, -py);
}

void Canvas::skew(float sx, float sy) {
    m_renderer->skew(sx, sy);
}

void Canvas::setMatrix(SkMatrix* matrix) {
    m_renderer->setMatrix(matrix);
}

void Canvas::getMatrix(SkMatrix* matrix) {
    m_renderer->getMatrix(matrix);
}

void Canvas::concatMatrix(SkMatrix* matrix) {
    m_renderer->concatMatrix(matrix);
}

// ----------------------------------------------------------------------------
// Drawing
// ----------------------------------------------------------------------------

void Canvas::drawBitmap(Bitmap *bitmap, float left, float top, Paint* paint) {
    int modifiers = paint ? setupModifiers(bitmap, paint) : MODIFIER_NONE;
    m_renderer->drawBitmap(bitmap, left, top, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawBitmapRect(Bitmap *bitmap,
        float srcLeft, float srcTop, float srcRight, float srcBottom,
        float dstLeft, float dstTop, float dstRight, float dstBottom, Paint* paint) {
    int modifiers = paint ? setupModifiers(bitmap, paint) : MODIFIER_NONE;
    m_renderer->drawBitmap(bitmap, srcLeft, srcTop, srcRight, srcBottom,
            dstLeft, dstTop, dstRight, dstBottom, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawBitmapRect(Bitmap *bitmap,
                            Rect src,
                            Rect dst, Paint* paint) {
    int modifiers = paint ? setupModifiers(bitmap, paint) : MODIFIER_NONE;
    m_renderer->drawBitmap(bitmap, src.left(), src.top(), src.right(), src.bottom(),
                           dst.left(), dst.top(), dst.right(), dst.bottom(), paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawBitmapMatrix(Bitmap *bitmap, Matrix* matrix, Paint* paint) {
    int modifiers = paint ? setupModifiers(bitmap, paint) : MODIFIER_NONE;
    m_renderer->drawBitmap(bitmap, matrix, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawBitmapData(const SkColor* src, int stride,
        float left, float top, int width, int height, bool hasAlpha, Paint* paint) {
    Bitmap *bitmap = new Bitmap();
    bitmap->setConfig(hasAlpha ? Bitmap::kARGB_8888_Config : Bitmap::kRGB_565_Config,
            width, height);

    if (!bitmap->allocPixels()) {
        delete bitmap;
        return;
    }

    if (!Bitmap::setPixels(src, stride, 0, 0, width, height, *bitmap)) {
        delete bitmap;
        return;
    }
    
    int modifiers = paint ? setupModifiers(bitmap, paint) : MODIFIER_NONE;
    m_renderer->drawBitmapData(bitmap, left, top, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);

    // If the renderer is a deferred renderer it will own the bitmap
    if (!m_renderer->isDeferred()) {
        bitmap = NULL;
    }
}

void Canvas::drawBitmapMesh(Bitmap *bitmap, int meshWidth, int meshHeight, float* verticesArray, int offset,
                            int* colorsArray, int colorOffset, Paint* paint) {
    int modifiers = paint ? setupModifiers(bitmap, paint) : MODIFIER_NONE;
    m_renderer->drawBitmapMesh(bitmap, meshWidth, meshHeight, verticesArray + offset, colorsArray + colorOffset, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawPatch(Bitmap *bitmap, Res_png_9patch* patch,
        float left, float top, float right, float bottom, Paint* paint) {
    // This object allows the renderer to allocate a global JNI ref to the buffer object.
    Res_png_9patch::deserialize(patch);

    m_renderer->drawPatch(bitmap, &patch->xDivs[0], &patch->yDivs[0],
            &patch->colors[0], patch->numXDivs, patch->numYDivs, patch->numColors,
            left, top, right, bottom, paint);
}

void Canvas::onPreDraw(Rect *dirty) {

    if (dirty) {
        prepareDirty(dirty->left(), dirty->top(), dirty->right(), dirty->bottom(), m_opaque);
    } else {
        prepare(m_opaque);
    }
}

void Canvas::onPostDraw() {
    finish();
}

void Canvas::drawColor(int color) {
    m_renderer->drawColor(color, SkXfermode::Mode::kSrcOver_Mode);
}

void Canvas::drawColor(int color, SkXfermode::Mode mode) {
    m_renderer->drawColor(color, mode);
}

void Canvas::drawARGB(int a, int r, int g, int b) {
    drawColor((a & 0xFF) << 24 | (r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF));
}

void Canvas::drawRect(float left, float top, float right, float bottom, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawRect(left, top, right, bottom, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawRect(Rect rect, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawRect(rect.left(), rect.top(), rect.right(), rect.bottom(), paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawRect(RectF rect, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawRect(rect.left(), rect.top(), rect.right(), rect.bottom(), paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawRoundRect(float left, float top, float right, float bottom,
                           float rx, float ry, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawRoundRect(left, top, right, bottom, rx, ry, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawRoundRect(Rect rect, float rx, float ry, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawRoundRect(rect.left(), rect.top(), rect.right(), rect.bottom(), rx, ry, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawRoundRect(Rect rect, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawRoundRect(rect.left(), rect.top(), rect.right(), rect.bottom(), 0, 0, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawRoundRect(RectF rect, float rx, float ry, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawRoundRect(rect.left(), rect.top(), rect.right(), rect.bottom(), rx, ry, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawCircle(float x, float y, float radius, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawCircle(x, y, radius, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawOval(float left, float top, float right, float bottom,
        Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawOval(left, top, right, bottom, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawOval(RectF rect, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawOval(rect.left(), rect.top(), rect.right(), rect.bottom(), paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawArc(float left, float top, float right, float bottom,
        float startAngle, float sweepAngle, bool useCenter, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawArc(left, top, right, bottom, startAngle, sweepAngle, useCenter, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawRects(SkRegion* region, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    SkRegion::Iterator it(*region);
    while (!it.done()) {
        const SkIRect& r = it.rect();
        m_renderer->drawRect(r.fLeft, r.fTop, r.fRight, r.fBottom, paint);
        it.next();
    }
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawPoints(float* storage, int offset, int count, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawPoints(storage + offset, count, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawPath(Path* path, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawPath(path, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawLines(float* points, int offset, int count, Paint* paint) {
    int modifiers = setupModifiers(paint, MODIFIER_COLOR_FILTER | MODIFIER_SHADER);
    m_renderer->drawLines(points + offset, count, paint);
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawLine(float startX, float startY, float stopX, float stopY, Paint* paint) {
    float line[4] = {startX, startY, stopX, stopY};
    drawLines(line, 0, 4, paint);
}

// ----------------------------------------------------------------------------
// Shaders and color filters
// ----------------------------------------------------------------------------

int Canvas::setupModifiers(Paint *paint) {
    int modifiers = MODIFIER_NONE;
    
    if (paint->hasShadow) {
        setupShadow(paint->shadowRadius, paint->shadowDx, paint->shadowDy, paint->shadowColor);
        modifiers |= MODIFIER_SHADOW;
    }
    
    const Shader *shader = paint->getShader();
    if (shader) {
        setupShader(shader->shader);
        modifiers |= MODIFIER_SHADER;
    }
    
    ColorFilter *filter = paint->getColorFilter();
    if (filter) {
//        setupColorFilter(filter->colorFilter);
        modifiers |= MODIFIER_COLOR_FILTER;
    }
    
    return modifiers;
}

int Canvas::setupModifiers(Paint *paint, int flags) {
    int modifiers = MODIFIER_NONE;
    
    if (paint->hasShadow && (flags & MODIFIER_SHADOW) != 0) {
        setupShadow(paint->shadowRadius, paint->shadowDx, paint->shadowDy, paint->shadowColor);
        modifiers |= MODIFIER_SHADOW;
    }
    
    Shader *shader = paint->getShader();
    if (shader && (flags & MODIFIER_SHADER) != 0) {
        setupShader(shader->shader);
        modifiers |= MODIFIER_SHADER;
    }
    
        ColorFilter *filter = paint->getColorFilter();
        if (filter && (flags & MODIFIER_COLOR_FILTER) != 0) {
//            setupColorFilter(filter->colorFilter);
            modifiers |= MODIFIER_COLOR_FILTER;
        }
    
    return modifiers;
}

int Canvas::setupModifiers(Bitmap *b, Paint *paint) {
    if (b->getConfig() != Bitmap::Config::kA8_Config) {
        ColorFilter *filter = paint->getColorFilter();
        if (filter) {
//            setupColorFilter(filter->colorFilter);
            return MODIFIER_COLOR_FILTER;
        }
        
        return MODIFIER_NONE;
    } else {
        return setupModifiers(paint);
    }
}

void Canvas::resetModifiers(int modifiers) {
    if (modifiers & MODIFIER_SHADOW) m_renderer->resetShadow();
    if (modifiers & MODIFIER_SHADER) m_renderer->resetShader();
    if (modifiers & MODIFIER_COLOR_FILTER) m_renderer->resetColorFilter();
}

void Canvas::setupShader(SkiaShader* shader) {
    m_renderer->setupShader(shader);
}

void Canvas::setupColorFilter(SkiaColorFilter* filter) {
    m_renderer->setupColorFilter(filter);
}

void Canvas::setupShadow(float radius, float dx, float dy, int color) {
    m_renderer->setupShadow(radius, dx, dy, color);
}

// ----------------------------------------------------------------------------
// Draw filters
// ----------------------------------------------------------------------------

void Canvas::setupPaintFilter(int clearBits, int setBits) {
    m_renderer->setupPaintFilter(clearBits, setBits);
}

void Canvas::resetPaintFilter() {
    m_renderer->resetPaintFilter();
}

// ----------------------------------------------------------------------------
// Text
// ----------------------------------------------------------------------------

void Canvas::renderText(const UChar* text, int count, float x, float y, int flags, Paint* paint) {
    renderText(text, 0, count, x, y, flags, paint);
}

void Canvas::renderText(const UChar* text, int start, int count, float x, float y, int flags, Paint* paint) {
    
    int modifiers = setupModifiers(paint);
    
    shared_ptr<TextLayoutValue> value = TextLayoutEngine::getInstance().getValue(paint,
            text, start, count, count, flags);
    
    if (value == NULL) {
        return;
    }
    
    const UChar* glyphs = value->getGlyphs();
    size_t glyphsCount = value->getGlyphsCount();
    int bytesCount = glyphsCount * sizeof(UChar);
    
    m_renderer->drawText((const char*) glyphs, bytesCount, glyphsCount, x, y, paint);
    
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::renderTextOnPath(const UChar* text, int count,
                              SkPath* path, float hOffset, float vOffset, int flags, Paint* paint) {
    
    int modifiers = setupModifiers(paint);
    
    shared_ptr<TextLayoutValue> value = TextLayoutEngine::getInstance().getValue(paint,
            text, 0, count, count, flags);
    
    if (value == NULL) {
        return;
    }
    
    const UChar* glyphs = value->getGlyphs();
    size_t glyphsCount = value->getGlyphsCount();
    int bytesCount = glyphsCount * sizeof(UChar);
    
    m_renderer->drawTextOnPath((const char*) glyphs, bytesCount, glyphsCount, path,
                               hOffset, vOffset, paint);
    
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::renderTextRun(const UChar* text,
        int start, int count, int contextCount, float x, float y,
                           int flags, Paint* paint) {
    
    int modifiers = setupModifiers(paint);
    
    shared_ptr<TextLayoutValue> value = TextLayoutEngine::getInstance().getValue(paint,
            text, start, count, contextCount, flags);
    
    if (value == NULL) {
        return;
    }
    
    const UChar* glyphs = value->getGlyphs();
    size_t glyphsCount = value->getGlyphsCount();
    int bytesCount = glyphsCount * sizeof(UChar);
    
    m_renderer->drawText((const char*) glyphs, bytesCount, glyphsCount, x, y, paint);
    
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::renderPosText(const UChar* text, int count,
                           const float* positions, int dirFlags, Paint* paint) {
    
    int modifiers = setupModifiers(paint);
    
    shared_ptr<TextLayoutValue> value = TextLayoutEngine::getInstance().getValue(paint,
                                                                         text, 0, count, count, dirFlags);
    if (value == NULL) {
        return;
    }
    
    const UChar* glyphs = value->getGlyphs();
    size_t glyphsCount = value->getGlyphsCount();
    if (count < int(glyphsCount)) glyphsCount = count;
    int bytesCount = glyphsCount * sizeof(UChar);
    
    m_renderer->drawPosText((const char*) glyphs, bytesCount, glyphsCount, positions, paint);
    
    if (modifiers != MODIFIER_NONE) resetModifiers(modifiers);
}

void Canvas::drawTextArray(UChar* textArray, int index, int count,
        float x, float y, int flags, Paint* paint) {
    renderText(textArray + index, count, x, y, flags, paint);
}

void Canvas::drawText(shared_ptr<CharSequence> text, float x, float y, Paint *paint) {
    drawText(text, 0, text->length(), x, y, paint);
}

void Canvas::drawText(shared_ptr<CharSequence> text, int start, int end, float x, float y, Paint *paint) {
    
    shared_ptr<GraphicsOperations> graphics;
    
    if (dynamic_pointer_cast<String>(text) != NULL || dynamic_pointer_cast<SpannedString>(text) != NULL ||
        dynamic_pointer_cast<SpannableString>(text) != NULL) {
        drawText(text->toString()->toCharArray(), start, end, x, y, paint);
    } else if ((graphics = dynamic_pointer_cast<GraphicsOperations>(text)) != NULL) {
        graphics->drawText(shared_from_this(), start, end, x, y, paint);
    } else {
        UnicodeString *buf = TextUtils::obtain(end - start);
        TextUtils::getChars(text, start, end, *buf, 0);
        drawText(buf->getBuffer(), start, end, x, y, paint);
        TextUtils::recycle(buf);
    }
}

void Canvas::drawText(string text, int start, int end, float x, float y, Paint *paint) {
    drawText(System::convert(text), start, end, x, y, paint);
}

void Canvas::drawText(string text, float x, float y, Paint *paint) {
    drawText(text, 0, text.length(), x, y, paint);
}

void Canvas::drawText(UnicodeString text, float x, float y, Paint *paint) {
    drawText(text, 0, text.length(), x, y, paint);
}

void Canvas::drawText(UnicodeString text, int start, int end, float x, float y, Paint* paint) {
    if ((start | end | (end - start) | (text.length() - end)) < 0) {
        throw IndexOutOfBoundsException();
    }
    
    drawText(text.getBuffer(), start, end, x, y, paint->mBidiFlags, paint);
}

void Canvas::drawText(const UChar* textArray, int start, int end, float x, float y, int flags, Paint* paint) {
    int count = end - start;
    renderText(textArray, start, count, x, y, flags, paint);
}

void Canvas::drawTextArrayOnPath(UChar* textArray, int index, int count,
        SkPath* path, float hOffset, float vOffset, int flags, Paint* paint) {
    renderTextOnPath(textArray + index, count, path,
            hOffset, vOffset, flags, paint);
}

void Canvas::drawTextOnPath(const UChar* textArray, int start, int end,
        SkPath* path, float hOffset, float vOffset, int flags, Paint* paint) {
    renderTextOnPath(textArray + start, end - start, path,
            hOffset, vOffset, flags, paint);
}

void Canvas::drawTextRunArray(UChar* textArray, int index, int count,
        int contextIndex, int contextCount, float x, float y, int dirFlags,
        Paint* paint) {
    renderTextRun(textArray + contextIndex, index - contextIndex,
            count, contextCount, x, y, dirFlags, paint);
}

void Canvas::drawTextRun(shared_ptr<CharSequence> text, int start, int end,
                 int contextStart, int contextEnd, float x, float y, int dir,
                 Paint* paint) {
    
    if (text == NULL) {
        throw NullPointerException("text is null");
    }
    if (paint == NULL) {
        throw NullPointerException("paint is null");
    }
    if ((start | end | end - start | text->length() - end) < 0) {
        throw IndexOutOfBoundsException();
    }
    
    int flags = dir == 0 ? 0 : 1;
    
    shared_ptr<GraphicsOperations> go = NULL;
    
    if (dynamic_pointer_cast<String>(text) != NULL || dynamic_pointer_cast<SpannedString>(text) != NULL ||
        dynamic_pointer_cast<SpannableString>(text) != NULL) {
        drawTextRun(text->toString()->toCharArray(), start, end,
                           contextStart, contextEnd, x, y, flags, paint);
    } else if ((go = dynamic_pointer_cast<GraphicsOperations>(text)) != NULL) {
        go->drawTextRun(shared_from_this(), start, end, contextStart, contextEnd, x, y, flags, paint);
    } else {
        int contextLen = contextEnd - contextStart;
        int len = end - start;
        UnicodeString *buf = TextUtils::obtain(contextLen);
        TextUtils::getChars(text, contextStart, contextEnd, *buf, 0);
        drawTextRun(buf->getBuffer(), start - contextStart, len,
                           0, contextLen, x, y, flags, paint);
        TextUtils::recycle(buf);
    }
}

void Canvas::drawTextRun(const UChar* textArray, int start, int end,
        int contextStart, int contextEnd, float x, float y, int dirFlags,
        Paint* paint) {
    int count = end - start;
    int contextCount = contextEnd - contextStart;
    renderTextRun(textArray + contextStart, start - contextStart,
            count, contextCount, x, y, dirFlags, paint);
}

void Canvas::drawPosTextArray(UChar* text, int index, int count,
        float* positions, Paint* paint) {
    renderPosText(text + index, count, positions, kBidi_LTR, paint);
}

void Canvas::drawPosText(const UChar* text, int start, int end,
        float* pos, Paint* paint) {
    renderPosText(text + start, end - start, pos, kBidi_LTR, paint);
}

// ----------------------------------------------------------------------------
// Common
// ----------------------------------------------------------------------------

bool Canvas::isAvailable() {

    char prop[PROPERTY_VALUE_MAX];
    if (property_get("ro.kernel.qemu", prop, NULL) == 0) {
        // not in the emulator
        return true;
    }
    // In the emulator this property will be set to 1 when hardware GLES is
    // enabled, 0 otherwise. On old emulator versions it will be undefined.
    property_get("ro.kernel.qemu.gles", prop, "0");
    return atoi(prop) == 1;
}

ANDROID_END
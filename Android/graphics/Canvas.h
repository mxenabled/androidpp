/*
 * File:   Canvas.h
 * Author: saulhoward
 *
 * Created on October 21, 2013, 3:39 PM
 */

#ifndef CANVAS_H
#define	CANVAS_H

#include "AndroidMacros.h"

#include "Android/graphics/HardwareRenderer.h"
#include "Android/graphics/Rect.h"
#include "Android/graphics/Matrix.h"
#include "Android/graphics/NinePatch.h"
#include "Android/text/CharSequence.h"

#include <OpenGLRenderer.h>
#include <DisplayListRenderer.h>
#include <LayerRenderer.h>
#include <SkiaShader.h>
#include <SkiaColorFilter.h>

#include <private/hwui/DrawGlInfo.h>

#include <Caches.h>

#include <SkCanvas.h>
#include <SkRegion.h>
#include <SkScalerContext.h>
#include <SkTemplates.h>
#include <SkXfermode.h>

#include <Rect.h>

#include <unicode/unistr.h>

#include <string>
#include <memory>

using namespace icu;
using namespace std;
using namespace android;
using namespace android::uirenderer;

ANDROID_BEGIN

class Bitmap;
class Paint;
class GLES20DisplayList;
class Path;

class Canvas : public enable_shared_from_this<Canvas> {

    friend class HardwareRenderer;

public:
    
    /**
     * Flag for drawTextRun indicating left-to-right run direction.
     * @hide
     */
    static const int DIRECTION_LTR = 0;
    
    /**
     * Flag for drawTextRun indicating right-to-left run direction.
     * @hide
     */
    static const int DIRECTION_RTL = 1;

    /** restore the current matrix when restore() is called */
    static const int MATRIX_SAVE_FLAG = 0x01;
    /** restore the current clip when restore() is called */
    static const int CLIP_SAVE_FLAG = 0x02;
    /** the layer needs to per-pixel alpha */
    static const int HAS_ALPHA_LAYER_SAVE_FLAG = 0x04;
    /** the layer needs to 8-bits per color component */
    static const int FULL_COLOR_LAYER_SAVE_FLAG = 0x08;
    /** clip against the layer's bounds */
    static const int CLIP_TO_LAYER_SAVE_FLAG = 0x10;
    /** restore everything when restore() is called */
    static const int ALL_SAVE_FLAG = 0x1F;

    Canvas(bool translucent);
    Canvas(bool recording, bool translucent);
    Canvas(GLES20DisplayList *displayList);
    virtual ~Canvas();

    void start();
    DisplayList *end(DisplayList *displayList);
    void recycle();

    void flushCaches(Caches::FlushMode mode);
    void initCaches();
    void terminateCaches();

    void destroyRenderer();
    void createRenderer();

    void setViewport(int width, int height);
    int prepare(bool opaque);
    int prepareDirty(int left, int top, int right, int bottom, bool opaque);
    void finish();
    int getStencilSize();

    int callDrawGLFunction(Functor* functor);
    void detachFunctor(Functor* functor);
    void attachFunctor(Functor* functor);
    int invokeFunctors(Rect *dirty);

    int getMaxTextureWidth();
    int getMaxTextureHeight();
    int getWidth();
    int getHeight();
    bool isOpaque();

    int save();
    int save(int flags);
    int getSaveCount();
    void restore();
    void restoreToCount(int saveCount);

    android::uirenderer::DisplayList* getDisplayList(android::uirenderer::DisplayList* displayList);
    int getDisplayListSize(android::uirenderer::DisplayList* displayList);
    void setDisplayListName(android::uirenderer::DisplayList* displayList, const char* textArray);
    void createDisplayListRenderer();
    void resetDisplayListRenderer();
    void destroyDisplayList(android::uirenderer::DisplayList* displayList);
    int drawDisplayList(android::uirenderer::DisplayList* displayList, Rect *dirty, int flags);
    void outputDisplayList(android::uirenderer::DisplayList* displayList);

    void interrupt();
    void resume();
    void drawLayer(Layer* layer, float x, float y, Paint* paint);
    int saveLayer(float left, float top, float right, float bottom, Paint* paint, int saveFlags);
    int saveLayerClip(Paint* paint, int saveFlags);
    int saveLayerAlpha(float left, float top, float right, float bottom, int alpha, int saveFlags);
    int saveLayerAlphaClip(int alpha, int saveFlags);
    void createLayerRenderer(Layer* layer);
    Layer* createTextureLayer(bool isOpaque, int* storage);
    Layer* createLayer(int width, int height, bool isOpaque, int* storage);
    void resizeLayer(Layer* layer, int width, int height, int* storage);
    void updateRenderLayer(Layer* layer, android::uirenderer::DisplayList* displayList,
        int left, int top, int right, int bottom);
    void setTextureLayerTransform(Layer* layer, SkMatrix* matrix);
    void destroyLayer(Layer* layer);
    void destroyLayerDeferred(Layer* layer);
    void flushLayer(Layer* layer);
    bool copyLayer(Layer* layer, Bitmap *bitmap);

    bool quickReject(float left, float top, float right, float bottom);
    bool clipRectF(float left, float top, float right, float bottom, SkRegion::Op op);
    bool clipRect(int left, int top, int right, int bottom, SkRegion::Op op);
    bool clipRect(int left, int top, int right, int bottom) { return clipRect(left, top, right, bottom, SkRegion::Op::kIntersect_Op); }
    bool clipRect(Rect &rect) { return clipRect(rect.left(), rect.top(), rect.right(), rect.bottom()); }
    bool getClipBounds(Rect *rect);

    void translate(float dx, float dy);
    void rotate(float degrees);
    void rotate(float degrees, float px, float py);
    void scale(float sx, float sy);
    void scale(float sx, float sy, float px, float py);
    void skew(float sx, float sy);
    void setMatrix(SkMatrix* matrix);
    void getMatrix(SkMatrix* matrix);
    void concatMatrix(SkMatrix* matrix);

    void onPreDraw(Rect *dirty);
    void onPostDraw();
    void drawBitmap(Bitmap *bitmap, float left, float top, Paint* paint);
    void drawBitmapRect(Bitmap *bitmap,
                        float srcLeft, float srcTop, float srcRight, float srcBottom,
                        float dstLeft, float dstTop, float dstRight, float dstBottom, Paint* paint);
    void drawBitmapRect(Bitmap *bitmap,
                        Rect src,
                        Rect dst, Paint* paint);
    void drawBitmapMatrix(Bitmap *bitmap, Matrix* matrix, Paint* paint);
    void drawBitmapData(const SkColor* src, int stride, float left, float top, int width, int height, bool hasAlpha, Paint* paint);
    void drawBitmapMesh(Bitmap *bitmap, int meshWidth, int meshHeight, float* verticesArray, int offset,
                        int* colorsArray, int colorOffset, Paint* paint);
    void drawPatch(Bitmap *bitmap, Res_png_9patch* patch,
                   float left, float top, float right, float bottom, Paint* paint);
    void drawColor(int color);
    void drawColor(int color, SkXfermode::Mode mode);
    void drawARGB(int a, int r, int g, int b);
    void drawRect(float left, float top, float right, float bottom, Paint* paint);
    void drawRect(Rect rect, Paint* paint);
    void drawRect(RectF rect, Paint* paint);
    void drawRoundRect(float left, float top, float right, float bottom, float rx, float ry, Paint* paint);
    void drawRoundRect(Rect rect, float rx, float ry, Paint* paint);
    void drawRoundRect(Rect rect, Paint* paint);
    void drawRoundRect(RectF rect, float rx, float ry, Paint* paint);
    void drawCircle(float x, float y, float radius, Paint* paint);
    void drawOval(float left, float top, float right, float bottom, Paint* paint);
    void drawOval(RectF rect, Paint* paint);
    void drawArc(float left, float top, float right, float bottom,
        float startAngle, float sweepAngle, bool useCenter, Paint* paint);
    void drawRects(SkRegion* region, Paint* paint);
    void drawPoints(float* storage, int offset, int count, Paint* paint);
    void drawPath(Path* path, Paint* paint);
    void drawLines(float* points, int offset, int count, Paint* paint);
    void drawLine(float startX, float startY, float stopX, float stopY, Paint* paint);
    
    int setupModifiers(Paint *paint);
    int setupModifiers(Paint *paint, int flags);
    int setupModifiers(Bitmap *b, Paint *paint);
    void resetModifiers(int modifiers);
    
    void setupShader(SkiaShader* shader);
    void setupColorFilter(SkiaColorFilter* filter);
    void setupShadow(float radius, float dx, float dy, int color);

    void setupPaintFilter(int clearBits, int setBits);
    void resetPaintFilter();
    
    void drawTextArray(UChar* textArray, int index, int count, float x, float y, int flags, Paint* paint);
    
    /**
     * Draw the specified range of text, specified by start/end, with its
     * origin at (x,y), in the specified Paint. The origin is interpreted
     * based on the Align setting in the Paint.
     *
     * @param text     The text to be drawn
     * @param start    The index of the first character in text to draw
     * @param end      (end - 1) is the index of the last character in text
     *                 to draw
     * @param x        The x-coordinate of origin for where to draw the text
     * @param y        The y-coordinate of origin for where to draw the text
     * @param paint The paint used for the text (e.g. color, size, style)
     */
    void drawText(shared_ptr<CharSequence> text, int start, int end, float x, float y, Paint *paint);
    void drawText(shared_ptr<CharSequence> text, float x, float y, Paint *paint);
    void drawText(string text, int start, int end, float x, float y, Paint *paint);
    void drawText(string text, float x, float y, Paint *paint);
    void drawText(UnicodeString text, float x, float y, Paint *paint);
    void drawText(UnicodeString text, int start, int end, float x, float y, Paint* paint);
    void drawText(const UChar* textArray, int start, int end, float x, float y, int flags, Paint* paint);
    void drawTextArrayOnPath(UChar* textArray, int index, int count,
                             SkPath* path, float hOffset, float vOffset, int flags, Paint* paint);
    void drawTextOnPath(const UChar* textArray, int start, int end,
                        SkPath* path, float hOffset, float vOffset, int flags, Paint* paint);
    void drawTextRunArray(UChar* textArray, int index, int count,
                          int contextIndex, int contextCount, float x, float y, int dirFlags,
                          Paint* paint);
    void drawTextRun(shared_ptr<CharSequence> text, int start, int end,
                     int contextStart, int contextEnd, float x, float y, int dir,
                     Paint* paint);
    void drawTextRun(const UChar* textArray, int start, int end,
                     int contextStart, int contextEnd, float x, float y, int dirFlags,
                     Paint* paint);
    void drawPosTextArray(UChar* text, int index, int count, float* positions, Paint* paint);
    void drawPosText(const UChar* text, int start, int end, float* pos, Paint* paint);

    bool isAvailable();

private:
    
    static const int MODIFIER_NONE = 0;
    static const int MODIFIER_SHADOW = 1;
    static const int MODIFIER_SHADER = 2;
    static const int MODIFIER_COLOR_FILTER = 4;

    OpenGLRenderer *m_renderer = NULL;
    GLES20DisplayList *mDisplayList = NULL;
    DisplayListRenderer *m_displayRenderer = NULL;
    int m_width = 0;
    int m_height = 0;
    bool m_opaque = false;
    
    void renderText(const UChar* text, int count, float x, float y, int flags, Paint* paint);
    void renderText(const UChar* text, int start, int count, float x, float y, int flags, Paint* paint);
    void renderTextOnPath(const UChar* text, int count,
                          SkPath* path, float hOffset, float vOffset, int flags, Paint* paint);
    void renderTextRun(const UChar* text,
                       int start, int count, int contextCount, float x, float y,
                       int flags, Paint* paint);
    void renderPosText(const UChar* text, int count,
                       const float* positions, int dirFlags, Paint* paint);
};

ANDROID_END

#endif	/* CANVAS_H */


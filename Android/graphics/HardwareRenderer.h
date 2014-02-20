/*
 * File:   HardwareRenderer.h
 * Author: saulhoward
 *
 * Created on October 25, 2013, 3:41 PM
 */

#ifndef HARDWARERENDERER_H
#define	HARDWARERENDERER_H

#include "AndroidMacros.h"
#include "Android/graphics/Rect.h"
#include <memory>

using namespace std;

ANDROID_BEGIN

class View;
class AttachInfo;
class Canvas;
class GLES20DisplayList;

class HardwareRenderer {
public:
    HardwareRenderer(bool translucent);
    virtual ~HardwareRenderer();

    GLES20DisplayList *createDisplayList();
    void draw(shared_ptr<View> view, AttachInfo *attachInfo, Rect *dirty);
    bool initialize();
    int getWidth();
    int getHeight();
    void setup(int width, int height);

    static HardwareRenderer *create(bool translucent);
private:
    bool m_translucent = false;
    shared_ptr<Canvas> m_canvas;
    int m_width = 0;
    int m_height = 0;
    Rect *m_redrawClip = NULL;
    GLES20DisplayList *buildDisplayList(shared_ptr<View> view, shared_ptr<Canvas> canvas);
    int drawDisplayList(AttachInfo *attachInfo, shared_ptr<Canvas> canvas, GLES20DisplayList *displayList, int status);
    void handleFunctorStatus(AttachInfo *attachInfo, int status);
};

ANDROID_END

#endif	/* HARDWARERENDERER_H */


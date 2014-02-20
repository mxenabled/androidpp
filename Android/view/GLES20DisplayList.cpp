/*
 * File:   DisplayList.cpp
 * Author: saulhoward
 *
 * Created on October 28, 2013, 10:01 AM
 */

#include "GLES20DisplayList.h"

#include "Android/graphics/Canvas.h"
#include <memory.h>

ANDROID_BEGIN

GLES20DisplayList::GLES20DisplayList() : m_canvas(NULL) {
}

GLES20DisplayList::~GLES20DisplayList() {
}


shared_ptr<Canvas> GLES20DisplayList::start(int width, int height) {

    m_isValid = false;

    if (!m_canvas) {
        m_canvas = make_shared<Canvas>(this);
    }

    m_canvas->start();
    m_canvas->setViewport(width, height);
    m_canvas->onPreDraw(NULL);

    return m_canvas;
}

void GLES20DisplayList::end() {
    if (m_canvas) {
        m_canvas->onPostDraw();
        m_canvas->end(this);
        m_canvas->recycle();
        m_canvas = NULL;
        m_isValid = true;
    }
}

ANDROID_END
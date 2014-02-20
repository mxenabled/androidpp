/*
 * File:   DisplayList.h
 * Author: saulhoward
 *
 * Created on October 28, 2013, 10:01 AM
 */

#ifndef GLES20DISPLAYLIST_H
#define	GLES20DISPLAYLIST_H

#include "AndroidMacros.h"

#include "Android/utils/Exceptions.h"

#include <memory>
#include <vector>
#include <DisplayListRenderer.h>

ANDROID_BEGIN

using namespace android::uirenderer;

class Canvas;

class GLES20DisplayList : public DisplayList {

    friend class Canvas;

public:

    static const int FLAG_CLIP_CHILDREN = 0x1;
    static const int STATUS_INVOKE = 0x2;
    static const int STATUS_DONE = 0x0;
    static const int STATUS_DRAW = 0x1;

    GLES20DisplayList();
    virtual ~GLES20DisplayList();
    
    void clear() {
        throw Exception("clear not implemented in GLES20DisplayList");
    }

    shared_ptr<Canvas> start(int width, int height);
    void end();
    bool isValid() { return m_isValid; }
private:
    bool m_isValid = false;
    shared_ptr<Canvas> m_canvas;
    std::vector<GLES20DisplayList*> mChildrenDisplayLists;
};

ANDROID_END

#endif	/* GLES20DISPLAYLIST_H */


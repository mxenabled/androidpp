/*
 * File:   HardwareRenderer.cpp
 * Author: saulhoward
 *
 * Created on October 25, 2013, 3:41 PM
 */

#include "HardwareRenderer.h"

#include "Android/graphics/Canvas.h"

#include "Android/view/View.h"
#include "Android/view/AttachInfo.h"
#include "Android/view/GLES20DisplayList.h"

ANDROID_BEGIN

HardwareRenderer::HardwareRenderer(bool translucent) {
    m_translucent = translucent;
    m_redrawClip = new Rect();
}

HardwareRenderer::~HardwareRenderer() {
}

HardwareRenderer *HardwareRenderer::create(bool translucent) {
    return new HardwareRenderer(translucent);
}

GLES20DisplayList *HardwareRenderer::createDisplayList() {
    return new GLES20DisplayList();
}

void HardwareRenderer::draw(shared_ptr<View> view, AttachInfo *attachInfo, Rect *dirty) {

    if (m_canvas) {

        attachInfo->m_ignoreDirtyState = true;
        
        view->mPrivateFlags |= View::PFLAG_DRAWN;

        shared_ptr<Canvas> &canvas = m_canvas;
        attachInfo->m_hardwareCanvas = canvas;

        GLES20DisplayList *displayList = buildDisplayList(view, canvas);

        int saveCount = 0;
        int status = GLES20DisplayList::STATUS_DONE;

        canvas->onPreDraw(NULL);

        saveCount = canvas->save();
        if (displayList) {
            status |= drawDisplayList(attachInfo, canvas, displayList, status);
        } else {
            view->draw(canvas);
        }
        canvas->restoreToCount(saveCount);
        view->m_recreateDisplayList = false;

        canvas->onPostDraw();
        
        attachInfo->m_ignoreDirtyState = false;
    }
}

int HardwareRenderer::drawDisplayList(AttachInfo *attachInfo, shared_ptr<Canvas> canvas, GLES20DisplayList *displayList, int status) {

    status |= canvas->drawDisplayList(displayList, m_redrawClip, GLES20DisplayList::FLAG_CLIP_CHILDREN);

    handleFunctorStatus(attachInfo, status);

    return status;
}

void HardwareRenderer::handleFunctorStatus(AttachInfo *attachInfo, int status) {
    // If the draw flag is set, functors will be invoked while executing
    // the tree of display lists
    if ((status & GLES20DisplayList::STATUS_DRAW) != 0) {
        if (m_redrawClip->isEmpty()) {
            attachInfo->mViewRootImpl->invalidate();
        } else {
            std::vector<int> location = std::vector<int>(2);
            attachInfo->mViewRootImpl->invalidateChildInParent(location, *m_redrawClip);
            m_redrawClip->setEmpty();
        }
    }

//    if ((status & GLES20DisplayList::STATUS_INVOKE) != 0 ||
//            attachInfo.mHandler.hasCallbacks(mFunctorsRunnable)) {
//        attachInfo.mHandler.removeCallbacks(mFunctorsRunnable);
//        mFunctorsRunnable.attachInfo = attachInfo;
//        attachInfo.mHandler.postDelayed(mFunctorsRunnable, FUNCTOR_PROCESS_DELAY);
//    }
}

GLES20DisplayList *HardwareRenderer::buildDisplayList(shared_ptr<View> view, shared_ptr<Canvas> canvas) {
    view->m_recreateDisplayList = (view->mPrivateFlags & View::PFLAG_INVALIDATED)
            == View::PFLAG_INVALIDATED;
    view->mPrivateFlags &= ~View::PFLAG_INVALIDATED;

    GLES20DisplayList *displayList = view->getDisplayList();

    return displayList;
}

bool HardwareRenderer::initialize() {
    m_canvas = make_shared<Canvas>(m_translucent);
    return (m_canvas != NULL);
}

int HardwareRenderer::getWidth() {
    return m_width;
}

int HardwareRenderer::getHeight() {
    return m_height;
}

void HardwareRenderer::setup(int width, int height) {
    m_canvas->setViewport(width, height);
    m_width = width;
    m_height = height;
}

ANDROID_END
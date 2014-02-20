//
//  AttachInfo.c
//  Androidpp
//
//  Created by Saul Howard on 2/10/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "AttachInfo.h"

#include "Android/view/ViewTreeObserver.h"
#include "Android/graphics/Rect.h"

ANDROID_BEGIN

AttachInfo::AttachInfo(ViewRootImpl *viewRootImpl, sp<Handler> handler) : mRootView(NULL), mViewRequestingLayout(NULL), mViewRootImpl(NULL), mInvalidateChildLocation(2), mTmpTransformRect(RectF()), mTmpInvalRect(Rect()) {
    mHandler = handler;
    mViewRootImpl = viewRootImpl;
    mTreeObserver =  new ViewTreeObserver();
}

ANDROID_END
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include "JniHelper.h"
#include "cocoa/CCString.h"
#include "Java_org_cocos2dx_lib_Cocos2dxGLSurfaceView.h"

#define  CLASS_NAME "org/cocos2dx/lib/Cocos2dxGLSurfaceView"

using namespace cocos2d;

void requestRender() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "render", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}
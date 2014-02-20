#include "AndroidAppDelegate.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "Main"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

extern "C" {

	jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	    JniHelper::setJavaVM(vm);

	    return JNI_VERSION_1_4;
	}

	void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv* env, jobject thiz, jint w, jint h) {

		cocos2d::CCEGLView* view = cocos2d::CCDirector::sharedDirector()->getOpenGLView();

        if (!view) {

            CCEGLView *view = CCEGLView::sharedOpenGLView();
            view->setFrameSize(w, h);

            AndroidAppDelegate *pAppDelegate = new AndroidAppDelegate(w, h);
            CCApplication::sharedApplication()->run();

        } else {

            ccDrawInit();
            ccGLInvalidateStateCache();

            CCShaderCache::sharedShaderCache()->reloadDefaultShaders();
            CCTextureCache::reloadAllTextures();

			CCNotificationCenter::sharedNotificationCenter()->postNotification(EVNET_COME_TO_FOREGROUND, NULL);
            CCDirector::sharedDirector()->setGLDefaultValues();

			if (view->getFrameSize().width != w || view->getFrameSize().height != h) {
				view->setFrameSize(w, h);
				view->setDesignResolutionSize(w, h, kResolutionShowAll);
			}
        }
    }
}

//
//  xp-macros.h
//  cocos2dx
//
//  Created by Saul Howard on 10/31/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_xp_macros_h
#define cocos2dx_xp_macros_h

#ifdef BUILD_FOR_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#endif

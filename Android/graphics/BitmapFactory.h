//
//  BitmapFactory.h
//  cocos2dx
//
//  Created by Saul Howard on 11/13/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_BitmapFactory_h
#define cocos2dx_BitmapFactory_h

#include "AndroidMacros.h"

#include "cocos2d.h"

#include "Android/graphics/Bitmap.h"

#include <SkImageDecoder.h>
#include <memory>

using namespace std;

ANDROID_BEGIN

class BitmapFactory {
public:
    static bool decodeFile(Bitmap *bitmap, const char* filePath) {
        
        cocos2d::CCFileUtils *fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
        
        if (fileUtils->isFileExist(filePath)) {
            unsigned long size = 0;
            char* pBuffer = (char*) cocos2d::CCFileUtils::sharedFileUtils()->getFileData(filePath, "rt", &size);
            
            if (pBuffer && size > 0) {
                SkImageDecoder::DecodeMemory(pBuffer, size, bitmap);
                return !bitmap->pixelRef();
            }
        }
        
        return false;
    }
};

ANDROID_END

#endif

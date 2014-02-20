//
//  NinePatch.cpp
//  cocos2dx
//
//  Created by Saul Howard on 12/3/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "NinePatch.h"

#include <string.h>
#include <stdlib.h>

#if BUILD_FOR_ANDROID
#define htonl(x) ((uint32_t) (x));
#define ntohl(x) ((uint32_t) (x));
#endif

ANDROID_BEGIN

void Res_png_9patch::deviceToFile()
{
    for (int i = 0; i < numXDivs; i++) {
        xDivs[i] = htonl(xDivs[i]);
    }
    for (int i = 0; i < numYDivs; i++) {
        yDivs[i] = htonl(yDivs[i]);
    }
    paddingLeft = htonl(paddingLeft);
    paddingRight = htonl(paddingRight);
    paddingTop = htonl(paddingTop);
    paddingBottom = htonl(paddingBottom);
    for (int i=0; i<numColors; i++) {
        colors[i] = htonl(colors[i]);
    }
}

void Res_png_9patch::fileToDevice()
{
    for (int i = 0; i < numXDivs; i++) {
        xDivs[i] = ntohl(xDivs[i]);
    }
    for (int i = 0; i < numYDivs; i++) {
        yDivs[i] = ntohl(yDivs[i]);
    }
    paddingLeft = ntohl(paddingLeft);
    paddingRight = ntohl(paddingRight);
    paddingTop = ntohl(paddingTop);
    paddingBottom = ntohl(paddingBottom);
    for (int i=0; i<numColors; i++) {
        colors[i] = ntohl(colors[i]);
    }
}

size_t Res_png_9patch::serializedSize()
{
    // The size of this struct is 32 bytes on the 32-bit target system
    // 4 * int8_t
    // 4 * int32_t
    // 3 * pointer
    return 32
    + numXDivs * sizeof(int32_t)
    + numYDivs * sizeof(int32_t)
    + numColors * sizeof(uint32_t);
}

void* Res_png_9patch::serialize()
{
    // Use calloc since we're going to leave a few holes in the data
    // and want this to run cleanly under valgrind
    void* newData = calloc(1, serializedSize());
    serialize(newData);
    return newData;
}

void Res_png_9patch::serialize(void * outData)
{
    char* data = (char*) outData;
    memmove(data, &wasDeserialized, 4);     // copy  wasDeserialized, numXDivs, numYDivs, numColors
    memmove(data + 12, &paddingLeft, 16);   // copy paddingXXXX
    data += 32;
    
    memmove(data, this->xDivs, numXDivs * sizeof(int32_t));
    data +=  numXDivs * sizeof(int32_t);
    memmove(data, this->yDivs, numYDivs * sizeof(int32_t));
    data +=  numYDivs * sizeof(int32_t);
    memmove(data, this->colors, numColors * sizeof(uint32_t));
}

Res_png_9patch* Res_png_9patch::deserialize(const void* inData)
{
    if (sizeof(void*) != sizeof(int32_t)) {
        return NULL;
    }
    deserializeInternal(inData, (Res_png_9patch*) inData);
    return (Res_png_9patch*) inData;
}

void Res_png_9patch::deserializeInternal(const void* inData, Res_png_9patch* outData) {
    char* patch = (char*) inData;
    if (inData != outData) {
        memmove(&outData->wasDeserialized, patch, 4);     // copy  wasDeserialized, numXDivs, numYDivs, numColors
        memmove(&outData->paddingLeft, patch + 12, 4);     // copy  wasDeserialized, numXDivs, numYDivs, numColors
    }
    outData->wasDeserialized = true;
    char* data = (char*)outData;
    data +=  sizeof(Res_png_9patch);
    outData->xDivs = (int32_t*) data;
    data +=  outData->numXDivs * sizeof(int32_t);
    outData->yDivs = (int32_t*) data;
    data +=  outData->numYDivs * sizeof(int32_t);
    outData->colors = (uint32_t*) data;
}

ANDROID_END
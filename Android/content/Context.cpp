/*
 * File:   Context.cpp
 * Author: saulhoward
 *
 * Created on October 10, 2013, 9:32 AM
 */

#include "Context.h"

#include "cocos2d.h"

#include "Android/view/View.h"

ANDROID_BEGIN

Context::~Context() {
}

Resources *Context::getResources() {
    return &m_resources;
}

ANDROID_END
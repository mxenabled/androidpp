/*
 * File:   Context.h
 * Author: saulhoward
 *
 * Created on October 10, 2013, 9:32 AM
 */

#ifndef CONTEXT_H
#define	CONTEXT_H

#include "AndroidMacros.h"

#include "Android/content/res/Resources.h"

ANDROID_BEGIN

class View;

class Context {
    
    friend class Activity;
    
public:
    
    Context() : m_resources(Resources()) {}
    
    Context(Resources resources) : m_resources(resources) {
    }
    virtual ~Context();

    Resources *getResources();

private:
    
    Resources m_resources;
};

ANDROID_END

#endif	/* CONTEXT_H */


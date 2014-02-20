/*
 * File:   AttributeSet.h
 * Author: saulhoward
 *
 * Created on October 7, 2013, 10:34 AM
 */

#ifndef ATTRIBUTESET_H
#define	ATTRIBUTESET_H

#include "Android/content/Context.h"

#include "AttributeSet.h"

#include <memory>
#include <string>

#include <unicode/unistr.h>

using namespace std;
using namespace icu;

ANDROID_BEGIN

class ColorStateList;
class Drawable;
class Resources;

class AttributeSet {

public:
    
    virtual void loadAttributes() = 0;
    virtual int getAttributeCount() = 0;
    virtual const char *getAttributeName(int index) = 0;
//    virtual char *getAttributeValue(int index) = 0;
    virtual const char *getAttributeValue(const char *name) = 0;
    virtual UnicodeString getAttributeUnicodeString(const char *name) = 0;
    virtual string getAttributeId(const char *name) = 0;
//    virtual char *getPositionDescription() = 0;
//    virtual int getAttributeNameResource(int index) = 0;
//    virtual int getAttributeListValue(const char *nameSpace, const char *attribute, const char *options[], int defaultValue) = 0;
    virtual bool getAttributeBooleanValue(const char *attribute, bool defaultValue) = 0;
//    virtual int getAttributeResourceValue(const char *nameSpace, const char *attribute, int defaultValue) = 0;
    virtual int getAttributeIntValue(const char *attribute, int defaultValue) = 0;
    virtual unsigned int getAttributeUnsignedIntValue(const char *attribute, unsigned int defaultValue) = 0;
    virtual float getAttributeFloatValue(const char *attribute, float defaultValue) = 0;
    virtual int getAttributeColorValue(Resources *res, const char *attribute, int defaultValue) = 0;
    virtual shared_ptr<ColorStateList> getAttributeColorStateList(Resources *res, const char *attribute) = 0;
//    virtual int getAttributeListValue(int index, const char *options[], int defaultValue) = 0;
//    virtual bool getAttributeBooleanValue(int index, bool defaultValue) = 0;
//    virtual int getAttributeResourceValue(int index, int defaultValue) = 0;
//    virtual int getAttributeIntValue(int index, int defaultValue) = 0;
//    virtual unsigned int getAttributeUnsignedIntValue(int index, unsigned int defaultValue) = 0;
//    virtual float getAttributeFloatValue(int index, float defaultValue) = 0;
    virtual const char *getIdAttribute() = 0;
//    virtual char *getClassAttribute() = 0;
//    virtual int getIdAttributeResourceValue(int defaultValue) = 0;
//    virtual int getStyleAttribute() = 0;
    virtual int getDimensionValue(Resources *res, const char *attribute, int defaultValue) = 0;
    virtual shared_ptr<Drawable> getAttributeDrawableValue(Resources *res, const char *attribute) = 0;
};

ANDROID_END

#endif	/* ATTRIBUTESET_H */


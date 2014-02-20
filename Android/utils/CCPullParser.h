/*
 * File:   CCPullParser.h
 * Author: saulhoward
 *
 * Created on October 7, 2013, 2:00 PM
 */

#ifndef __CCPULLPARSER_H__
#define	__CCPULLPARSER_H__

#include "cocos2d.h"

#include "AndroidMacros.h"

#include "Android/utils/AttributeSet.h"

#include <unicode/unistr.h>

#include <map>
#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class ColorStateList;

class CCPullParser : public AttributeSet {
    
public:
    
    CCPullParser() {}
    
    CCPullParser(const CCPullParser &parser); // Not supported due to tinyxml
    
    bool init(const char *pszFile);
    tinyxml2::XMLNode *next();
    tinyxml2::XMLNode *firstChild();
    void toParent();
    bool hasChildren();
    const char *getName();
    const char *getValue();
    
    virtual void loadAttributes();
    
    virtual int getAttributeCount();
    virtual const char *getAttributeName(int index);
    virtual const char *getAttributeValue(const char *name);
    virtual UnicodeString getAttributeUnicodeString(const char *name);
    virtual string getAttributeId(const char *name);
    virtual bool getAttributeBooleanValue(const char *attribute, bool defaultValue);
    virtual int getAttributeIntValue(const char *attribute, int defaultValue);
    virtual unsigned int getAttributeUnsignedIntValue(const char *attribute, unsigned int defaultValue);
    virtual float getAttributeFloatValue(const char *attribute, float defaultValue);
    virtual const char *getIdAttribute();
    virtual int getDimensionValue(Resources *res, const char *attribute, int defaultValue);
    virtual int getAttributeColorValue(Resources *res, const char *attribute, int defaultValue);
    virtual shared_ptr<ColorStateList> getAttributeColorStateList(Resources *res, const char *attribute);
    virtual shared_ptr<Drawable> getAttributeDrawableValue(Resources *res, const char *attribute);
    
private:
    
    tinyxml2::XMLDocument m_doc;
    tinyxml2::XMLNode *m_root;
    map<string, const tinyxml2::XMLAttribute*> m_attrs;
    vector<string> m_attrsName;
    
    bool m_attrsLoaded;
    
    void setRoot(tinyxml2::XMLNode *root);
    bool parse(const char *pszFile);
    const tinyxml2::XMLAttribute *getAttribute(const char *name);
    
};

ANDROID_END

#endif	/* CCPULLPARSER_H */


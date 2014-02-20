/*
 * File:   CCPullParser.cpp
 * Author: saulhoward
 *
 * Created on October 7, 2013, 2:00 PM
 */

#include "CCPullParser.h"

#include "cocos2d.h"

#include "Android/content/res/ColorStateList.h"
#include "Android/internal/R.h"
#include "Android/view/View.h"
#include "Android/view/LayoutParams.h"
#include "Android/graphics/Color.h"

#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/drawable/ColorDrawable.h"
#include "Android/graphics/drawable/BitmapDrawable.h"
#include "Android/utils/System.h"

using namespace std;

ANDROID_BEGIN

bool CCPullParser::init(const char *pszFile) {
    return parse(pszFile);
}

bool CCPullParser::parse(const char *pszFile) {
    
    cocos2d::CCFileUtils *fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    bool bRet = false;
    
    if (!fileUtils->isFileExist(pszFile)) {
        CCLOG("Could not find %s", pszFile);
        return bRet;
    }
    
    unsigned long size = 0;

    char* pBuffer = (char*) fileUtils->getFileData(pszFile, "rt", &size);

    if (pBuffer != NULL && size > 0) {
        tinyxml2::XMLError result = m_doc.Parse(pBuffer, size);
        bRet = tinyxml2::XML_SUCCESS == result;
        setRoot(m_doc.FirstChild()->NextSibling());
    }
    CC_SAFE_DELETE_ARRAY(pBuffer);
    
    return bRet && m_root;
}

tinyxml2::XMLNode *CCPullParser::next() {

    if (m_root->NextSibling()) {

        setRoot(m_root->NextSibling());

        return m_root;
    }

    return NULL;
}

tinyxml2::XMLNode *CCPullParser::firstChild() {

    if (m_root->FirstChild()) {

        setRoot(m_root->FirstChild());

        return m_root;
    }

    return NULL;
}

void CCPullParser::toParent() {
    setRoot(m_root->Parent());
}

bool CCPullParser::hasChildren() {
    return !m_root->NoChildren();
}

const char *CCPullParser::getName() {

    if (m_root) {
        return m_root->Value();
    }

    return NULL;
}

const char *CCPullParser::getValue() {
    
    if (m_root && m_root->FirstChild()) {
        tinyxml2::XMLText *text = m_root->FirstChild()->ToText();
        return text->Value();
    }
    
    return "";
}

void CCPullParser::setRoot(tinyxml2::XMLNode *root) {
    m_root = root;
    m_attrsLoaded = false;
}

void CCPullParser::loadAttributes() {

    if (m_attrsLoaded) return;

    m_attrs.clear();
    m_attrsName.clear();

    tinyxml2::XMLElement *element = m_root->ToElement();
    const tinyxml2::XMLAttribute *attr = element->FirstAttribute();

    while (attr) {
        string name(attr->Name());
        
        // Remove android namespace if found
        size_t found = name.find("android:");
        if (found != string::npos) {
            name = name.substr(8, name.size());
        }
        
        m_attrsName.push_back(name);
        m_attrs.insert(make_pair(name, attr));
        attr = attr->Next();
    }

    m_attrsLoaded = true;
}

int CCPullParser::getAttributeCount() {
    return m_attrs.size();
}

const char *CCPullParser::getAttributeName(int index) {
    return m_attrsName.at(index).c_str();
}

const char *CCPullParser::getAttributeValue(const char *name) {
    return getAttribute(name) ? getAttribute(name)->Value() : NULL;
}

UnicodeString CCPullParser::getAttributeUnicodeString(const char *name) {
    string value(getAttributeValue(name));
    UnicodeString str = System::convert(value);
    
    str.findAndReplace(UNICODE_STRING_SIMPLE("\\n"), UNICODE_STRING_SIMPLE("\n"));
    str.findAndReplace(UNICODE_STRING_SIMPLE("\\r"), UNICODE_STRING_SIMPLE("\r"));
    str.findAndReplace(UNICODE_STRING_SIMPLE("\\t"), UNICODE_STRING_SIMPLE("\t"));
    
    return str;
}

string CCPullParser::getAttributeId(const char *name) {
    
    string id;
    const char* value = getAttributeValue(name);
    
    if (!value) return id;
    
    id = string(value);
    
    size_t found = id.find("@id/");
    if (found != string::npos) {
        id = id.substr(4, id.size());
    }
    
    return id;
}

bool CCPullParser::getAttributeBooleanValue(const char *attribute, bool defaultValue) {
    return getAttribute(attribute) ?  getAttribute(attribute)->BoolValue() : defaultValue;
}

int CCPullParser::getAttributeIntValue(const char *attribute, int defaultValue) {
    return getAttribute(attribute) ? getAttribute(attribute)->IntValue() : defaultValue;
}

unsigned int CCPullParser::getAttributeUnsignedIntValue(const char *attribute, unsigned int defaultValue) {
    return getAttribute(attribute) ? getAttribute(attribute)->UnsignedValue() : defaultValue;
}

float CCPullParser::getAttributeFloatValue(const char *attribute, float defaultValue) {
    return getAttribute(attribute) ? getAttribute(attribute)->FloatValue() : defaultValue;
}

const char *CCPullParser::getIdAttribute() {
    return getAttributeValue(R::styleable::id);
}

int CCPullParser::getDimensionValue(Resources *res, const char *attribute, int defaultValue) {

    const char* charValue = getAttributeValue(attribute);

    if (!charValue) return defaultValue;

    string value(charValue);

    if (value.compare(R::layout::match_parent) == 0) {
        return LayoutParams::MATCH_PARENT;
    }

    if (value.compare(R::layout::wrap_content) == 0) {
        return LayoutParams::WRAP_CONTENT;
    }

    string amount = value.substr(0, value.length() - 2);
    
    return atoi(amount.c_str()) * res->getDisplayMetrics().density;
}

int CCPullParser::getAttributeColorValue(Resources *res, const char *attribute, int defaultValue) {

    const char* charValue = getAttributeValue(attribute);

    if (!charValue) return defaultValue;
    
    string color = string(charValue);
    
    size_t found = color.find("@color/");
    if (found != string::npos) {
        color = color.substr(7, color.size());
        return res->getColor(color);
    }

    return Color::parseColor(color);
}

shared_ptr<ColorStateList> CCPullParser::getAttributeColorStateList(Resources *res, const char *attribute) {
    
    const char* charValue = getAttributeValue(attribute);
    
    if (!charValue) return NULL;
    
    return res->getColorStateList(string(charValue));
}

const tinyxml2::XMLAttribute *CCPullParser::getAttribute(const char* name) {

    map<string, const tinyxml2::XMLAttribute*>::iterator it = m_attrs.find(name);

    if (it != m_attrs.end()) {
        return it->second;
    }

    return NULL;
}

shared_ptr<Drawable> CCPullParser::getAttributeDrawableValue(Resources *res, const char *attribute) {
    
    string value(getAttributeValue(attribute));
    
    return res->getDrawable(value);
}

ANDROID_END
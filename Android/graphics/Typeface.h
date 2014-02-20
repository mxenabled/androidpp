//
//  Typeface.h
//  Androidpp
//
//  Created by Saul Howard on 1/28/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__Typeface__
#define __Androidpp__Typeface__

#include "AndroidMacros.h"

#include "Android/utils/Object.h"

#include <SkTypeface.h>

#include "cocos2d.h"

#include <vector>
#include <string>

using namespace std;

ANDROID_BEGIN

/**
 * The Typeface class specifies the typeface and intrinsic style of a font.
 * This is used in the paint, along with optionally Paint settings like
 * textSize, textSkewX, textScaleX to specify
 * how text appears when drawn (and measured).
 */
class Typeface : public SkTypeface {
    
public:
    
    static const int NORMAL = 0;
    static const int BOLD = 1;
    static const int ITALIC = 2;
    static const int BOLD_ITALIC = 3;
    
    /** The default NORMAL typeface object */
    static Typeface *DEFAULT;
    /**
     * The default BOLD typeface object. Note: this may be not actually be
     * bold, depending on what fonts are installed. Call getStyle() to know
     * for sure.
     */
    static Typeface *DEFAULT_BOLD;
    /** The NORMAL style of the default sans serif typeface. */
    static Typeface *SANS_SERIF;
    /** The NORMAL style of the default serif typeface. */
    static Typeface *SERIF;
    /** The NORMAL style of the default monospace typeface. */
    static Typeface *MONOSPACE;
    
    static void initialize();
    
private:
    
    static vector<Typeface*> sDefaults;
    static vector<vector<Typeface*>> sTypefaceCache;
    
public:
    
    /** Returns the typeface's intrinsic style attributes */
    int getStyle() { return style(); }
    
    /**
     * Create a typeface object given a family name, and option style information.
     * If null is passed for the name, then the "default" font will be chosen.
     * The resulting typeface object can be queried (getStyle()) to discover what
     * its "real" style characteristics are.
     *
     * @param familyName May be null. The name of the font family.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    static Typeface *create(const char* familyName, Style style) {
        return (Typeface*) CreateFromName(familyName, style);
    }
    
    /**
     * Create a typeface object that best matches the specified existing
     * typeface and the specified Style. Use this call if you want to pick a new
     * style from the same family of an existing typeface object. If family is
     * null, this selects from the default font's family.
     *
     * @param family May be null. The name of the existing type face.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    static Typeface *create(Typeface *family, int style);
    
    /**
     * Returns one of the default typeface objects, based on the specified style
     *
     * @return the default typeface that corresponds to the style
     */
    static Typeface *defaultFromStyle(int style) {
        return sDefaults[style];
    }
    
    /**
     * Create a new typeface from the specified font file.
     *
     * @param path The path to the font data.
     * @return The new typeface.
     */
    static Typeface *createFromFile(string path) {
        return (Typeface*) CreateFromFile(path.c_str());
    }
    
    static int getStyle(string name, int defaultStyle);
};

ANDROID_END

#endif /* defined(__Androidpp__Typeface__) */

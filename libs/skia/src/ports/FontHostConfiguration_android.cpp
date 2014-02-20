/* libs/graphics/ports/FontHostConfiguration_android.cpp
**
** Copyright 2011, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include "FontHostConfiguration_android.h"
#include "SkString.h"
#include "SkTDArray.h"
#include <expat.h>
#include <cutils/properties.h>

//#if BUILD_FOR_ANDROID
//#define SYSTEM_FONTS_FILE "/system/etc/system_fonts.xml"
//#define FALLBACK_FONTS_FILE "/system/etc/fallback_fonts.xml"
//#define VENDOR_FONTS_FILE "/vendor/etc/fallback_fonts.xml"
//#else
#include "cocos2d.h"
#define SYSTEM_FONTS_FILE "res/data/fonts/system_fonts.xml"
#define FALLBACK_FONTS_FILE "res/data/fonts/fallback_fonts.xml"
#define VENDOR_FONTS_FILE "res/data/fonts/fallback_fonts.xml"
//#endif

// These defines are used to determine the kind of tag that we're currently
// populating with data. We only care about the sibling tags nameset and fileset
// for now.
#define NO_TAG 0
#define NAMESET_TAG 1
#define FILESET_TAG 2

/**
 * The FamilyData structure is passed around by the parser so that each handler
 * can read these variables that are relevant to the current parsing.
 */
struct FamilyData {
    FamilyData(XML_Parser *parserRef, SkTDArray<FontFamily*> &familiesRef) :
            parser(parserRef), families(familiesRef), currentTag(NO_TAG) {};

    XML_Parser *parser;                // The expat parser doing the work
    SkTDArray<FontFamily*> &families;  // The array that each family is put into as it is parsed
    FontFamily *currentFamily;         // The current family being created
    int currentTag;                    // A flag to indicate whether we're in nameset/fileset tags
};

/**
 * Handler for arbitrary text. This is used to parse the text inside each name
 * or file tag. The resulting strings are put into the fNames or fFileNames arrays.
 */
void textHandler(void *data, const char *s, int len) {
    FamilyData *familyData = (FamilyData*) data;
    // Make sure we're in the right state to store this name information
    if (familyData->currentFamily &&
            (familyData->currentTag == NAMESET_TAG || familyData->currentTag == FILESET_TAG)) {
        // Malloc new buffer to store the string
        char *buff;
        buff = (char*) malloc((len + 1) * sizeof(char));
        strncpy(buff, s, len);
        buff[len] = '\0';
        switch (familyData->currentTag) {
        case NAMESET_TAG:
            *(familyData->currentFamily->fNames.append()) = buff;
            break;
        case FILESET_TAG:
            *(familyData->currentFamily->fFileNames.append()) = buff;
            break;
        default:
            // Noop - don't care about any text that's not in the Fonts or Names list
            break;
        }
    }
}

/**
 * Handler for the start of a tag. The only tags we expect are family, nameset,
 * fileset, name, and file.
 */
void startElementHandler(void *data, const char *tag, const char **atts) {
    FamilyData *familyData = (FamilyData*) data;
    int len = strlen(tag);
    if (strncmp(tag, "family", len)== 0) {
        familyData->currentFamily = new FontFamily();
        familyData->currentFamily->order = -1;
        // The Family tag has an optional "order" attribute with an integer value >= 0
        // If this attribute does not exist, the default value is -1
        for (int i = 0; atts[i] != NULL; i += 2) {
            const char* attribute = atts[i];
            const char* valueString = atts[i+1];
            int value;
            int len = sscanf(valueString, "%d", &value);
            if (len > 0) {
                familyData->currentFamily->order = value;
            }
        }
    } else if (len == 7 && strncmp(tag, "nameset", len)== 0) {
        familyData->currentTag = NAMESET_TAG;
    } else if (len == 7 && strncmp(tag, "fileset", len) == 0) {
        familyData->currentTag = FILESET_TAG;
    } else if ((strncmp(tag, "name", len) == 0 && familyData->currentTag == NAMESET_TAG) ||
            (strncmp(tag, "file", len) == 0 && familyData->currentTag == FILESET_TAG)) {
        // If it's a Name, parse the text inside
        XML_SetCharacterDataHandler(*familyData->parser, textHandler);
    }
}

/**
 * Handler for the end of tags. We only care about family, nameset, fileset,
 * name, and file.
 */
void endElementHandler(void *data, const char *tag) {
    FamilyData *familyData = (FamilyData*) data;
    int len = strlen(tag);
    if (strncmp(tag, "family", len)== 0) {
        // Done parsing a Family - store the created currentFamily in the families array
        *familyData->families.append() = familyData->currentFamily;
        familyData->currentFamily = NULL;
    } else if (len == 7 && strncmp(tag, "nameset", len)== 0) {
        familyData->currentTag = NO_TAG;
    } else if (len == 7 && strncmp(tag, "fileset", len)== 0) {
        familyData->currentTag = NO_TAG;
    } else if ((strncmp(tag, "name", len) == 0 && familyData->currentTag == NAMESET_TAG) ||
            (strncmp(tag, "file", len) == 0 && familyData->currentTag == FILESET_TAG)) {
        // Disable the arbitrary text handler installed to load Name data
        XML_SetCharacterDataHandler(*familyData->parser, NULL);
    }
}

#if !defined(SK_BUILD_FOR_ANDROID_NDK)
/**
 * Read the persistent locale.
 */
void getLocale(char* language, char* region)
{
    char propLang[PROPERTY_VALUE_MAX], propRegn[PROPERTY_VALUE_MAX];

    property_get("persist.sys.language", propLang, "");
    property_get("persist.sys.country", propRegn, "");
    if (*propLang == 0 && *propRegn == 0) {
        /* Set to ro properties, default is en_US */
        property_get("ro.product.locale.language", propLang, "en");
        property_get("ro.product.locale.region", propRegn, "US");
    }
    strncat(language, propLang, 2);
    strncat(region, propRegn, 2);
}
#endif

/**
 * This function parses the given filename and stores the results in the given
 * families array.
 */
void parseConfigFile(const char *filename, SkTDArray<FontFamily*> &families) {
    XML_Parser parser = XML_ParserCreate(NULL);
    FamilyData *familyData = new FamilyData(&parser, families);
    XML_SetUserData(parser, familyData);
    XML_SetElementHandler(parser, startElementHandler, endElementHandler);
    
    unsigned long size = 0;
    cocos2d::CCFileUtils *fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    char* buff = (char*) fileUtils->getFileData(filename, "r", &size);
    XML_Parse(parser, buff, size, true);
}

void getSystemFontFamilies(SkTDArray<FontFamily*> &fontFamilies) {
    parseConfigFile(SYSTEM_FONTS_FILE, fontFamilies);
}

void getFallbackFontFamilies(SkTDArray<FontFamily*> &fallbackFonts) {
    SkTDArray<FontFamily*> vendorFonts;
    parseConfigFile(FALLBACK_FONTS_FILE, fallbackFonts);
    parseConfigFile(VENDOR_FONTS_FILE, vendorFonts);

    // This loop inserts the vendor fallback fonts in the correct order in the
    // overall fallbacks list.
    int currentOrder = -1;
    for (int i = 0; i < vendorFonts.count(); ++i) {
        FontFamily* family = vendorFonts[i];
        int order = family->order;
        if (order < 0) {
            if (currentOrder < 0) {
                // Default case - just add it to the end of the fallback list
                *fallbackFonts.append() = family;
            } else {
                // no order specified on this font, but we're incrementing the order
                // based on an earlier order insertion request
                *fallbackFonts.insert(currentOrder++) = family;
            }
        } else {
            // Add the font into the fallback list in the specified order. Set
            // currentOrder for correct placement of other fonts in the vendor list.
            *fallbackFonts.insert(order) = family;
            currentOrder = order + 1;
        }
    }
}

/**
 * Loads data on font families from various expected configuration files. The
 * resulting data is returned in the given fontFamilies array.
 */
void getFontFamilies(SkTDArray<FontFamily*> &fontFamilies) {
    SkTDArray<FontFamily*> fallbackFonts;

    getSystemFontFamilies(fontFamilies);
    getFallbackFontFamilies(fallbackFonts);

    // Append all fallback fonts to system fonts
    for (int i = 0; i < fallbackFonts.count(); ++i) {
        *fontFamilies.append() = fallbackFonts[i];
    }
}

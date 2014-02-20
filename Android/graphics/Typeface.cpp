//
//  Typeface.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/28/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Typeface.h"

ANDROID_BEGIN

Typeface *Typeface::DEFAULT = NULL;
Typeface *Typeface::DEFAULT_BOLD = NULL;
Typeface *Typeface::SANS_SERIF = NULL;
Typeface *Typeface::SERIF = NULL;
Typeface *Typeface::MONOSPACE = NULL;

vector<Typeface*> Typeface::sDefaults = vector<Typeface*>();

vector<vector<Typeface*>> Typeface::sTypefaceCache = vector<vector<Typeface*>>(3);

Typeface *Typeface::create(Typeface *family, int style) {
    return (Typeface*) CreateFromTypeface(family, (Style) style);
}

void Typeface::initialize() {
    
    if (sDefaults.size() == 0) {
        DEFAULT = create(NULL, Style::kNormal);
        DEFAULT_BOLD = create(NULL, Style::kBold);
        SANS_SERIF = create("sans-serif", Style::kNormal);
        SERIF = create("serif", Style::kNormal);
        MONOSPACE = create("monospace", Style::kNormal);
        sDefaults = { DEFAULT, DEFAULT_BOLD, create(NULL, Style::kItalic), create(NULL, Style::kBoldItalic) };
    }
}

int Typeface::getStyle(string name, int defaultStyle) {
    
    if (name.empty()) return defaultStyle;
    
    if (name.compare("normal") == 0) {
        return NORMAL;
    }
    
    if (name.compare("bold") == 0) {
        return BOLD;
    }
    
    if (name.compare("italic") == 0) {
        return ITALIC;
    }
}

ANDROID_END
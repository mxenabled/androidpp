//
//  ClickableSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_ClickableSpan_h
#define Androidpp_ClickableSpan_h

#include "AndroidMacros.h"

#include "Android/text/style/CharacterStyle.h"
#include "Android/text/style/UpdateAppearance.h"
#include "Android/text/TextPaint.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class View;

/**
 * If an object of this type is attached to the text of a TextView
 * with a movement method of LinkMovementMethod, the affected spans of
 * text can be selected.  If clicked, the {@link #onClick} method will
 * be called.
 */
class ClickableSpan : public CharacterStyle, public UpdateAppearance {
    
public:
    
    /**
     * Performs the click action associated with this span.
     */
    virtual void onClick(View *widget) = 0;
    
    /**
     * Makes the text underlined and in the link color.
     */
    void updateDrawState(shared_ptr<TextPaint> ds) {
        ds->setColor(ds->linkColor);
        ds->setUnderlineText(true);
    }
    
    virtual string getType() {
        return "ClickableSpan";
    };
};

ANDROID_END

#endif

//
//  SpellCheckSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_SpellCheckSpan_h
#define Androidpp_SpellCheckSpan_h

#include "AndroidMacros.h"

ANDROID_BEGIN

/**
 * A SpellCheckSpan is an internal data structure created by the TextView's SpellChecker to
 * annotate portions of the text that are about to or currently being spell checked. They are
 * automatically removed once the spell check is completed.
 *
 * @hide
 */
class SpellCheckSpan {
    
private:
    
    bool mSpellCheckInProgress;
    
public:
    
    SpellCheckSpan() {
        mSpellCheckInProgress = false;
    }
    
//    SpellCheckSpan(Parcel src) {
//        mSpellCheckInProgress = (src.readInt() != 0);
//    }
    
    void setSpellCheckInProgress(bool inProgress) {
        mSpellCheckInProgress = inProgress;
    }
    
    bool isSpellCheckInProgress() {
        return mSpellCheckInProgress;
    }
    
//    @Override
//    public int describeContents() {
//        return 0;
//    }
//    
//    @Override
//    public void writeToParcel(Parcel dest, int flags) {
//        dest.writeInt(mSpellCheckInProgress ? 1 : 0);
//    }
//    
//    @Override
//    public int getSpanTypeId() {
//        return TextUtils.SPELL_CHECK_SPAN;
//    }
    
    virtual string getType() {
        return "SpellCheckSpan";
    };
};

ANDROID_END

#endif

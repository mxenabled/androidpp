//
//  AlignmentSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_AlignmentSpan_h
#define Androidpp_AlignmentSpan_h

#include "AndroidMacros.h"

#include "Android/text/Layout.h"
#include "Android/text/style/ParagraphStyle.h"

ANDROID_BEGIN

class AlignmentSpan : public ParagraphStyle {
    
public:
    
    virtual Layout::Alignment getAlignment() = 0;
    
    virtual string getType() {
        return "AlignmentSpan";
    };
    
    class Standard;
};

class AlignmentSpan::Standard : public AlignmentSpan {
    
public:
    
    Standard(Layout::Alignment align) {
        mAlignment = align;
    }
    
//    Standard(Parcel src) {
//        mAlignment = Layout.Alignment.valueOf(src.readString());
//    }
//    
//    int getSpanTypeId() {
//        return TextUtils::ALIGNMENT_SPAN;
//    }
//    
//    int describeContents() {
//        return 0;
//    }
//    
//    void writeToParcel(Parcel dest, int flags) {
//        dest.writeString(mAlignment.name());
//    }
    
    Layout::Alignment getAlignment() {
        return mAlignment;
    }
    
private:
    
    Layout::Alignment mAlignment;
};

ANDROID_END

#endif

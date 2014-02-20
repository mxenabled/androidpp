//
//  ReplacementTransformationMethod.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/30/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "ReplacementTransformationMethod.h"

#include "Android/text/TextUtils.h"
#include "Android/text/Editable.h"
#include "Android/text/SpannedString.h"

ANDROID_BEGIN

shared_ptr<CharSequence> ReplacementTransformationMethod::getTransformation(shared_ptr<CharSequence> source, View *v) {
    UnicodeString original = getOriginal();
    UnicodeString replacement = getReplacement();
    
    /*
     * Short circuit for faster display if the text will never change.
     */
    if (dynamic_pointer_cast<Editable>(source) == NULL) {
        /*
         * Check whether the text does not contain any of the
         * source characters so can be used unchanged.
         */
        bool doNothing = true;
        int n = original.length();
        for (int i = 0; i < n; i++) {
            if (TextUtils::indexOf(source, original[i]) >= 0) {
                doNothing = false;
                break;
            }
        }
        if (doNothing) {
            return source;
        }
        
        if (dynamic_pointer_cast<Spannable>(source) == NULL) {
            
            shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(source);
            
            /*
             * The text contains some of the source characters,
             * but they can be flattened out now instead of
             * at display time.
             */
            if (spanned != NULL) {
                return make_shared<SpannedString>(make_shared<SpannedReplacementCharSequence>(
                                                                            spanned,
                                                                            original, replacement));
            } else {
                return make_shared<ReplacementCharSequence>(source,
                                                   original,
                                                   replacement)->toString();
            }
        }
    }
    
    shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(source);
    
    if (spanned != NULL) {
        return make_shared<SpannedReplacementCharSequence>(spanned,
                                                  original, replacement);
    } else {
        return make_shared<ReplacementCharSequence>(source, original, replacement);
    }
}

void ReplacementTransformationMethod::onFocusChanged(View *view, shared_ptr<CharSequence> sourceText,
                    bool focused, int direction,
                    Rect previouslyFocusedRect) {
    // This callback isn't used.
}

ReplacementTransformationMethod::ReplacementCharSequence::ReplacementCharSequence(shared_ptr<CharSequence> source, UnicodeString original,
                        UnicodeString replacement) {
    mSource = source;
    mOriginal = original;
    mReplacement = replacement;
}

int ReplacementTransformationMethod::ReplacementCharSequence::length() {
    return mSource->length();
}

UChar ReplacementTransformationMethod::ReplacementCharSequence::charAt(int i) {
    UChar c = mSource->charAt(i);
    
    int n = mOriginal.length();
    for (int j = 0; j < n; j++) {
        if (c == mOriginal[j]) {
            c = mReplacement[j];
        }
    }
    
    return c;
}

shared_ptr<CharSequence> ReplacementTransformationMethod::ReplacementCharSequence::subSequence(int start, int end) {
    UnicodeString c;
    
    getChars(start, end, c, 0);
    return make_shared<String>(c);
}

shared_ptr<String> ReplacementTransformationMethod::ReplacementCharSequence::toString() {
    UnicodeString c;
    
    getChars(0, length(), c, 0);
    return make_shared<String>(c);
}

void ReplacementTransformationMethod::ReplacementCharSequence::getChars(int start, int end, UnicodeString &dest, int off) {
    TextUtils::getChars(mSource, start, end, dest, off);
    int offend = end - start + off;
    int n = mOriginal.length();
    
    for (int i = off; i < offend; i++) {
        char c = dest[i];
        
        for (int j = 0; j < n; j++) {
            if (c == mOriginal[j]) {
                dest.setCharAt(i, mReplacement[j]);
            }
        }
    }
}

ReplacementTransformationMethod::SpannedReplacementCharSequence::SpannedReplacementCharSequence(shared_ptr<Spanned> source, UnicodeString original,
                                                                                                UnicodeString replacement) : ReplacementCharSequence(source, original, replacement) {
    mSpanned = source;
}

shared_ptr<CharSequence> ReplacementTransformationMethod::SpannedReplacementCharSequence::subSequence(int start, int end) {
    return make_shared<SpannedString>(shared_from_this())->subSequence(start, end);
}

vector<shared_ptr<Object>> ReplacementTransformationMethod::SpannedReplacementCharSequence::getSpans(int start, int end, string type) {
    return mSpanned->getSpans(start, end, type);
}

int ReplacementTransformationMethod::SpannedReplacementCharSequence::getSpanStart(shared_ptr<Object> tag) {
    return mSpanned->getSpanStart(tag);
}

int ReplacementTransformationMethod::SpannedReplacementCharSequence::getSpanEnd(shared_ptr<Object> tag) {
    return mSpanned->getSpanEnd(tag);
}

int ReplacementTransformationMethod::SpannedReplacementCharSequence::getSpanFlags(shared_ptr<Object> tag) {
    return mSpanned->getSpanFlags(tag);
}

int ReplacementTransformationMethod::SpannedReplacementCharSequence::nextSpanTransition(int start, int end, string type) {
    return mSpanned->nextSpanTransition(start, end, type);
}

ANDROID_END
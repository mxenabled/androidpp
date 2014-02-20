//
//  ReplacementTransformationMethod.h
//  Androidpp
//
//  Created by Saul Howard on 1/30/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__ReplacementTransformationMethod__
#define __Androidpp__ReplacementTransformationMethod__

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/GetChars.h"
#include "Android/text/Spanned.h"
#include "Android/text/method/TransformationMethod.h"

#include <unicode/unistr.h>

#include <string>
#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class Rect;

/**
 * This transformation method causes the characters in the {@link #getOriginal}
 * array to be replaced by the corresponding characters in the
 * {@link #getReplacement} array.
 */
class ReplacementTransformationMethod : public TransformationMethod {
    
protected:
    
    /**
     * Returns the list of characters that are to be replaced by other
     * characters when displayed.
     */
    virtual UnicodeString getOriginal() = 0;
    /**
     * Returns a parallel array of replacement characters for the ones
     * that are to be replaced.
     */
    virtual UnicodeString getReplacement() = 0;
    
public:
    
    /**
     * Returns a CharSequence that will mirror the contents of the
     * source CharSequence but with the characters in {@link #getOriginal}
     * replaced by ones from {@link #getReplacement}.
     */
    shared_ptr<CharSequence> getTransformation(shared_ptr<CharSequence> source, View *v);
    
    void onFocusChanged(View *view, shared_ptr<CharSequence> sourceText,
                               bool focused, int direction,
                        Rect previouslyFocusedRect);
    
private:
    
    class ReplacementCharSequence : public virtual CharSequence, public virtual GetChars {
    
    private:
        
        UnicodeString mOriginal, mReplacement;
        
    public:
        
        ReplacementCharSequence(shared_ptr<CharSequence> source, UnicodeString original,
                                UnicodeString replacement);
        
        int length();
        
        UChar charAt(int i);
        
        shared_ptr<CharSequence> subSequence(int start, int end);
        
        shared_ptr<String> toString();
        
        void getChars(int start, int end, UnicodeString &dest, int off);
        
    private:
        
        shared_ptr<CharSequence> mSource;
    };
    
    class SpannedReplacementCharSequence : public enable_shared_from_this<SpannedReplacementCharSequence>, public ReplacementCharSequence, public virtual Spanned {
        
    public:
        
        SpannedReplacementCharSequence(shared_ptr<Spanned> source, UnicodeString original,
                                       UnicodeString replacement);
        
        shared_ptr<CharSequence> subSequence(int start, int end);
        
        vector<shared_ptr<Object>> getSpans(int start, int end, string type);
        
        int getSpanStart(shared_ptr<Object> tag);
        
        int getSpanEnd(shared_ptr<Object> tag);
        
        int getSpanFlags(shared_ptr<Object> tag);
        
        int nextSpanTransition(int start, int end, string type);
        
    private:
        
        shared_ptr<Spanned> mSpanned;
    };
};

ANDROID_END

#endif /* defined(__Androidpp__ReplacementTransformationMethod__) */

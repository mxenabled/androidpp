//
//  SpannablestringInternal.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__SpannableStringInternal__
#define __Androidpp__SpannableStringInternal__

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/Spannable.h"
#include "Android/text/String.h"
#include "Android/utils/Object.h"

#include <unicode/unistr.h>

#include <memory>
#include <string>
#include <vector>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class SpannableStringInternal {
    
private:
    
    shared_ptr<String> mText;
    vector<shared_ptr<Object> > mSpans;
    vector<int> mSpanData;
    int mSpanCount;
    
    static vector<shared_ptr<Object> > EMPTY;
    
    static const int START = 0;
    static const int END = 1;
    static const int FLAGS = 2;
    static const int COLUMNS = 3;
    
protected:
    
    SpannableStringInternal(Spannable *sharedThis, shared_ptr<CharSequence> source, int start, int end);
    
    void setSpan(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int start, int end, int flags);
    
    void removeSpan(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what);
    
    void sendSpanAdded(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int start, int end);
    
    void sendSpanRemoved(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int start, int end);
    
    void sendSpanChanged(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int s, int e, int st, int en);
    
    static string region(int start, int end);
    
    void checkRange(const string operation, int start, int end);
    
public:
    
    int length() {
        return mText->length();
    }
    
    UChar charAt(int i) {
        return mText->charAt(i);
    }
    
    shared_ptr<String> toString() {
        return mText;
    }
    
    /* subclasses must do subSequence() to preserve type */
    
    void getChars(int start, int end, UnicodeString &dest, int off);
    
    int getSpanStart(shared_ptr<Object> what);
    
    int getSpanEnd(shared_ptr<Object> what);
    
    int getSpanFlags(shared_ptr<Object> what);
    
    vector<shared_ptr<Object> > getSpans(int queryStart, int queryEnd, string kind);
    
    int nextSpanTransition(int start, int limit, string kind);
};

ANDROID_END

#endif /* defined(__Androidpp__SpannableStringInternal__) */

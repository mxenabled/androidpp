//
//  TextDirectionHeuristics.h
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__TextDirectionHeuristics__
#define __Androidpp__TextDirectionHeuristics__

#include "AndroidMacros.h"

#include "Android/text/TextDirectionHeuristic.h"
#include "android/text/TextUtils.h"
#include "Android/view/View.h"

#include <unicode/unistr.h>

#include <string>
#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class CharSequence;

/**
 * Some objects that implement TextDirectionHeuristic.
 *
 */
class TextDirectionHeuristics {
    
private:
    
    /**
     * State constants for taking care about true / false / unknown
     */
    static const int STATE_TRUE = 0;
    static const int STATE_FALSE = 1;
    static const int STATE_UNKNOWN = 2;
    
    static int isRtlText(int directionality);
    
    static int isRtlTextOrFormat(int directionality);
    
    
    /**
     * Interface for an algorithm to guess the direction of a paragraph of text.
     */
    class TextDirectionAlgorithm {
        
    public:
        
        /**
         * Returns whether the range of text is RTL according to the algorithm.
         */
        virtual int checkRtl(shared_ptr<CharSequence> cs, int start, int count) = 0;
    };
    
    /**
     * Computes the text direction based on an algorithm.  Subclasses implement
     * {@link #defaultIsRtl} to handle cases where the algorithm cannot determine the
     * direction from the text alone.
     */
    class TextDirectionHeuristicImpl : public TextDirectionHeuristic {
        
    private:
        
        shared_ptr<TextDirectionAlgorithm> mAlgorithm;
        
    public:
        
        TextDirectionHeuristicImpl(shared_ptr<TextDirectionAlgorithm> algorithm) {
            mAlgorithm = algorithm;
        }
        
    protected:
        
        /**
         * Return true if the default text direction is rtl.
         */
        virtual bool defaultIsRtl() = 0;
        
    public:
        
        bool isRtl(UnicodeString array, int start, int count);
        
        bool isRtl(shared_ptr<CharSequence> cs, int start, int count);
        
    private:
        
        bool doCheck(shared_ptr<CharSequence> cs, int start, int count);
    };
    
    class TextDirectionHeuristicInternal : public TextDirectionHeuristicImpl {
        
    public:
        
        bool isRtl(UnicodeString array, int start, int count) { return TextDirectionHeuristicImpl::isRtl(array, start, count); }
        
        bool isRtl(shared_ptr<CharSequence> cs, int start, int count) { return TextDirectionHeuristicImpl::isRtl(cs, start, count); }
        
        TextDirectionHeuristicInternal(bool defaultIsRtl, shared_ptr<TextDirectionAlgorithm> algorithm);
        
    private:
        
        const bool mDefaultIsRtl;
        
    protected:
        
        bool defaultIsRtl() {
            return mDefaultIsRtl;
        }
    };
    
    /**
     * Algorithm that uses the first strong directional character to determine the paragraph
     * direction. This is the standard Unicode Bidirectional algorithm.
     */
    class FirstStrong : public TextDirectionAlgorithm {
        
    public:
        
        int checkRtl(shared_ptr<CharSequence> cs, int start, int count);
        
        FirstStrong() {
        }
        
        static shared_ptr<FirstStrong> INSTANCE;
    };
    
    /**
     * Algorithm that uses the presence of any strong directional non-format
     * character (e.g. excludes LRE, LRO, RLE, RLO) to determine the
     * direction of text.
     */
    class AnyStrong : public TextDirectionAlgorithm {
        
    private:
        
        const bool mLookForRtl;
        
    public:
        
        int checkRtl(shared_ptr<CharSequence> cs, int start, int count);
        
        AnyStrong(bool lookForRtl) : mLookForRtl(lookForRtl) {
        }
        
        static shared_ptr<AnyStrong> INSTANCE_RTL;
        static shared_ptr<AnyStrong> INSTANCE_LTR;
    };
    
    /**
     * Algorithm that uses the Locale direction to force the direction of a paragraph.
     */
    class TextDirectionHeuristicLocale : public TextDirectionHeuristicImpl {
        
    public:
        
        TextDirectionHeuristicLocale() : TextDirectionHeuristicImpl(NULL) {
        }
        
        bool defaultIsRtl() {
            const int dir = TextUtils::getLayoutDirectionFromLocale();
            return (dir == View::LAYOUT_DIRECTION_RTL);
        }
        
        static shared_ptr<TextDirectionHeuristicLocale> INSTANCE;
    };
    
public:
    
    /**
     * Always decides that the direction is left to right.
     */
    static shared_ptr<TextDirectionHeuristic> LTR;
    
    /**
     * Always decides that the direction is right to left.
     */
    static shared_ptr<TextDirectionHeuristic> RTL;
    
    /**
     * Determines the direction based on the first strong directional character, including bidi
     * format chars, falling back to left to right if it finds none. This is the default behavior
     * of the Unicode Bidirectional Algorithm.
     */
    static shared_ptr<TextDirectionHeuristic> FIRSTSTRONG_LTR;
    
    /**
     * Determines the direction based on the first strong directional character, including bidi
     * format chars, falling back to right to left if it finds none. This is similar to the default
     * behavior of the Unicode Bidirectional Algorithm, just with different fallback behavior.
     */
    static shared_ptr<TextDirectionHeuristic> FIRSTSTRONG_RTL;
    
    /**
     * If the text contains any strong right to left non-format character, determines that the
     * direction is right to left, falling back to left to right if it finds none.
     */
    static shared_ptr<TextDirectionHeuristic> ANYRTL_LTR;
    
    /**
     * Force the paragraph direction to the Locale direction. Falls back to left to right.
     */
    static shared_ptr<TextDirectionHeuristic> LOCALE;
};

ANDROID_END

#endif /* defined(__Androidpp__TextDirectionHeuristics__) */

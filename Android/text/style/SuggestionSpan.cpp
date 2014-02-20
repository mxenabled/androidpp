//
//  SuggestionSpan.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "SuggestionSpan.h"

#include "Android/content/Context.h"
#include "Android/graphics/Color.h"
#include "Android/text/TextPaint.h"
#include "Android/utils/Object.h"
#include "Android/utils/System.h"

#include <algorithm>

ANDROID_BEGIN

const string SuggestionSpan::ACTION_SUGGESTION_PICKED = "android.text.style.SUGGESTION_PICKED";
const string SuggestionSpan::SUGGESTION_SPAN_PICKED_AFTER = "after";
const string SuggestionSpan::SUGGESTION_SPAN_PICKED_BEFORE = "before";
const string SuggestionSpan::SUGGESTION_SPAN_PICKED_HASHCODE = "hashcode";

const int SuggestionSpan::SUGGESTIONS_MAX_SIZE = 5;

SuggestionSpan::SuggestionSpan(Context *context, vector<string> suggestions, int flags) : SuggestionSpan(context, suggestions, flags, "") {
}

/**
 * @param locale Locale of the suggestions
 * @param suggestions Suggestions for the string under the span
 * @param flags Additional flags indicating how this span is handled in TextView
 */
SuggestionSpan::SuggestionSpan(vector<string> suggestions, int flags) : SuggestionSpan(NULL, suggestions, flags, "") {
}

/**
 * @param context Context for the application
 * @param locale locale Locale of the suggestions
 * @param suggestions Suggestions for the string under the span. Only the first up to
 * {@link android.text.style.SuggestionSpan#SUGGESTIONS_MAX_SIZE} will be considered. Null values not permitted.
 * @param flags Additional flags indicating how this span is handled in TextView
 * @param notificationTargetClass if not NULL, this class will get notified when the user
 * selects one of the suggestions.
 */
SuggestionSpan::SuggestionSpan(Context *context, vector<string> suggestions, int flags,
                      string notificationTargetClass) {
    
    const int N = min(SUGGESTIONS_MAX_SIZE, (int) suggestions.size());
    System::arraycopy(suggestions, 0, mSuggestions, 0, N);
    mFlags = flags;
//    if (context != NULL) {
//        mLocaleString = context->getResources().getConfiguration().locale.toString();
//    } else {
//        Log.e("SuggestionSpan", "No locale or context specified in SuggestionSpan constructor");
        mLocaleString = "";
//    }
//    
//    if (context != NULL) {
//        mNotificationTargetPackageName = context->getPackageName();
//    } else {
//        mNotificationTargetPackageName = "";
//    }
//    
//    if (notificationTargetClass != NULL) {
//        mNotificationTargetClassName = notificationTargetClass.getCanonicalName();
//    } else {
//        mNotificationTargetClassName = "";
//    }
//    mHashCode = hashCodeInternal(mSuggestions, mLocaleString, mNotificationTargetClassName);
    
    initStyle(context);
}

void SuggestionSpan::initStyle(Context *context) {
    if (context == NULL) {
        mMisspelledUnderlineThickness = 0;
        mEasyCorrectUnderlineThickness = 0;
        mAutoCorrectionUnderlineThickness = 0;
        mMisspelledUnderlineColor = Color::BLACK;
        mEasyCorrectUnderlineColor = Color::BLACK;
        mAutoCorrectionUnderlineColor = Color::BLACK;
        return;
    }
    
//    int defStyle = com.android.internal.R.attr.textAppearanceMisspelledSuggestion;
//    TypedArray typedArray = context->obtainStyledAttributes(
//                                                           NULL, com.android.internal.R.styleable.SuggestionSpan, defStyle, 0);
//    mMisspelledUnderlineThickness = typedArray.getDimension(
//                                                            com.android.internal.R.styleable.SuggestionSpan_textUnderlineThickness, 0);
//    mMisspelledUnderlineColor = typedArray.getColor(
//                                                    com.android.internal.R.styleable.SuggestionSpan_textUnderlineColor, Color.BLACK);
//    
//    defStyle = com.android.internal.R.attr.textAppearanceEasyCorrectSuggestion;
//    typedArray = context->obtainStyledAttributes(
//                                                NULL, com.android.internal.R.styleable.SuggestionSpan, defStyle, 0);
//    mEasyCorrectUnderlineThickness = typedArray.getDimension(
//                                                             com.android.internal.R.styleable.SuggestionSpan_textUnderlineThickness, 0);
//    mEasyCorrectUnderlineColor = typedArray.getColor(
//                                                     com.android.internal.R.styleable.SuggestionSpan_textUnderlineColor, Color.BLACK);
//    
//    defStyle = com.android.internal.R.attr.textAppearanceAutoCorrectionSuggestion;
//    typedArray = context->obtainStyledAttributes(
//                                                NULL, com.android.internal.R.styleable.SuggestionSpan, defStyle, 0);
//    mAutoCorrectionUnderlineThickness = typedArray.getDimension(
//                                                                com.android.internal.R.styleable.SuggestionSpan_textUnderlineThickness, 0);
//    mAutoCorrectionUnderlineColor = typedArray.getColor(
//                                                        com.android.internal.R.styleable.SuggestionSpan_textUnderlineColor, Color.BLACK);
}

//bool SuggestionSpan::equals(Object *o) {
//    if (o instanceof SuggestionSpan) {
//        return ((SuggestionSpan)o).hashCode() == mHashCode;
//    }
//    return false;
//}

int SuggestionSpan::hashCode() {
    return mHashCode;
}

//int SuggestionSpan::hashCodeInternal(vector<string> suggestions, string locale,
//                     string notificationTargetClassName) {
//    return Arrays.hashCode(new Object[] {Long.valueOf(SystemClock.uptimeMillis()), suggestions,
//        locale, notificationTargetClassName});
//}

void SuggestionSpan::updateDrawState(shared_ptr<TextPaint> tp) {
    const bool misspelled = (mFlags & FLAG_MISSPELLED) != 0;
    const bool easy = (mFlags & FLAG_EASY_CORRECT) != 0;
    const bool autoCorrection = (mFlags & FLAG_AUTO_CORRECTION) != 0;
    if (easy) {
        if (!misspelled) {
            tp->setUnderlineText(mEasyCorrectUnderlineColor, mEasyCorrectUnderlineThickness);
        } else if (tp->underlineColor == 0) {
            // Spans are rendered in an arbitrary order. Since misspelled is less prioritary
            // than just easy, do not apply misspelled if an easy (or a mispelled) has been set
            tp->setUnderlineText(mMisspelledUnderlineColor, mMisspelledUnderlineThickness);
        }
    } else if (autoCorrection) {
        tp->setUnderlineText(mAutoCorrectionUnderlineColor, mAutoCorrectionUnderlineThickness);
    }
}

/**
 * @return The color of the underline for that span, or 0 if there is no underline
 *
 * @hide
 */
int SuggestionSpan::getUnderlineColor() {
    // The order here should match what is used in updateDrawState
    const bool misspelled = (mFlags & FLAG_MISSPELLED) != 0;
    const bool easy = (mFlags & FLAG_EASY_CORRECT) != 0;
    const bool autoCorrection = (mFlags & FLAG_AUTO_CORRECTION) != 0;
    if (easy) {
        if (!misspelled) {
            return mEasyCorrectUnderlineColor;
        } else {
            return mMisspelledUnderlineColor;
        }
    } else if (autoCorrection) {
        return mAutoCorrectionUnderlineColor;
    }
    return 0;
}

/**
 * Notifies a suggestion selection.
 *
 * @hide
 */
//void SuggestionSpan::notifySelection(Context *context, string original, int index) {
//    const Intent intent = new Intent();
//    
//    if (context == NULL || mNotificationTargetClassName == NULL) {
//        return;
//    }
//    // Ensures that only a class in the original IME package will receive the
//    // notification.
//    if (mSuggestions == NULL || index < 0 || index >= mSuggestions.length) {
//        Log.w(TAG, "Unable to notify the suggestion as the index is out of range index=" + index
//              + " length=" + mSuggestions.length);
//        return;
//    }
//    
//    // The package name is not mandatory (legacy from JB), and if the package name
//    // is missing, we try to notify the suggestion through the input method manager.
//    if (mNotificationTargetPackageName != NULL) {
//        intent.setClassName(mNotificationTargetPackageName, mNotificationTargetClassName);
//        intent.setAction(SuggestionSpan.ACTION_SUGGESTION_PICKED);
//        intent.putExtra(SuggestionSpan.SUGGESTION_SPAN_PICKED_BEFORE, original);
//        intent.putExtra(SuggestionSpan.SUGGESTION_SPAN_PICKED_AFTER, mSuggestions[index]);
//        intent.putExtra(SuggestionSpan.SUGGESTION_SPAN_PICKED_HASHCODE, hashCode());
//        context->sendBroadcast(intent);
//    } else {
//        InputMethodManager imm = InputMethodManager.peekInstance();
//        if (imm != NULL) {
//            imm.notifySuggestionPicked(this, original, index);
//        }
//    }
//}

ANDROID_END
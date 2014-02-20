//
//  SuggestionSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__SuggestionSpan__
#define __Androidpp__SuggestionSpan__

#include "AndroidMacros.h"

#include "Android/text/style/CharacterStyle.h"

#include <string>
#include <vector>
#include <memory>

using namespace std;

ANDROID_BEGIN

class Context;
class Object;

/**
 * Holds suggestion candidates for the text enclosed in this span.
 *
 * When such a span is edited in an EditText, double tapping on the text enclosed in this span will
 * display a popup dialog listing suggestion replacement for that text. The user can then replace
 * the original text by one of the suggestions.
 *
 * These spans should typically be created by the input method to provide correction and alternates
 * for the text.
 *
 * @see android.widget.TextView#isSuggestionsEnabled()
 */
class SuggestionSpan : public CharacterStyle {
    
//    static const string TAG = "SuggestionSpan";
    
    /**
     * Sets this flag if the suggestions should be easily accessible with few interactions.
     * This flag should be set for every suggestions that the user is likely to use.
     */
public:
    
    static const int FLAG_EASY_CORRECT = 0x0001;
    
    /**
     * Sets this flag if the suggestions apply to a misspelled word/text. This type of suggestion is
     * rendered differently to highlight the error.
     */
    static const int FLAG_MISSPELLED = 0x0002;
    
    /**
     * Sets this flag if the auto correction is about to be applied to a word/text
     * that the user is typing/composing. This type of suggestion is rendered differently
     * to indicate the auto correction is happening.
     */
    static const int FLAG_AUTO_CORRECTION = 0x0004;
    
    static const string ACTION_SUGGESTION_PICKED;
    static const string SUGGESTION_SPAN_PICKED_AFTER;
    static const string SUGGESTION_SPAN_PICKED_BEFORE;
    static const string SUGGESTION_SPAN_PICKED_HASHCODE;
    
    static const int SUGGESTIONS_MAX_SIZE;
    
    virtual string getType() {
        return "SuggestionSpan";
    };
    
    /*
     * TODO: Needs to check the validity and add a feature that TextView will change
     * the current IME to the other IME which is specified in SuggestionSpan.
     * An IME needs to set the span by specifying the target IME and Subtype of SuggestionSpan.
     * And the current IME might want to specify any IME as the target IME including other IMEs.
     */
    
private:
    
    int mFlags;
    vector<string> mSuggestions;
    string mLocaleString;
    string mNotificationTargetClassName;
    string mNotificationTargetPackageName;
    int mHashCode = 0;
    
    float mEasyCorrectUnderlineThickness;
    int mEasyCorrectUnderlineColor;
    
    float mMisspelledUnderlineThickness;
    int mMisspelledUnderlineColor;
    
    float mAutoCorrectionUnderlineThickness;
    int mAutoCorrectionUnderlineColor;
    
public:
    
    /**
     * @param context Context for the application
     * @param suggestions Suggestions for the string under the span
     * @param flags Additional flags indicating how this span is handled in TextView
     */
    SuggestionSpan(Context *context, vector<string> suggestions, int flags);
    
    /**
     * @param locale Locale of the suggestions
     * @param suggestions Suggestions for the string under the span
     * @param flags Additional flags indicating how this span is handled in TextView
     */
    SuggestionSpan(vector<string> suggestions, int flags);
    
    /**
     * @param context Context for the application
     * @param locale locale Locale of the suggestions
     * @param suggestions Suggestions for the string under the span. Only the first up to
     * {@link android.text.style.SuggestionSpan#SUGGESTIONS_MAX_SIZE} will be considered. Null values not permitted.
     * @param flags Additional flags indicating how this span is handled in TextView
     * @param notificationTargetClass if not null, this class will get notified when the user
     * selects one of the suggestions.
     */
    SuggestionSpan(Context *context, vector<string> suggestions, int flags,
                          string notificationTargetClass);
    
private:
    
    void initStyle(Context *context);
    
public:
    
//    SuggestionSpan(Parcel src) {
//        mSuggestions = src.readStringArray();
//        mFlags = src.readInt();
//        mLocalestring = src.readString();
//        mNotificationTargetClassName = src.readString();
//        mNotificationTargetPackageName = src.readString();
//        mHashCode = src.readInt();
//        mEasyCorrectUnderlineColor = src.readInt();
//        mEasyCorrectUnderlineThickness = src.readFloat();
//        mMisspelledUnderlineColor = src.readInt();
//        mMisspelledUnderlineThickness = src.readFloat();
//        mAutoCorrectionUnderlineColor = src.readInt();
//        mAutoCorrectionUnderlineThickness = src.readFloat();
//    }
    
    /**
     * @return an array of suggestion texts for this span
     */
    vector<string> getSuggestions() {
        return mSuggestions;
    }
    
    /**
     * @return the locale of the suggestions
     */
    string getLocale() {
        return mLocaleString;
    }
    
    /**
     * @return The name of the class to notify. The class of the original IME package will receive
     * a notification when the user selects one of the suggestions. The notification will include
     * the original string, the suggested replacement string as well as the hashCode of this span.
     * The class will get notified by an intent that has those information.
     * This is an internal API because only the framework should know the class name.
     *
     * @hide
     */
    string getNotificationTargetClassName() {
        return mNotificationTargetClassName;
    }
    
    int getFlags() {
        return mFlags;
    }
    
    void setFlags(int flags) {
        mFlags = flags;
    }
    
//    @Override
//    int describeContents() {
//        return 0;
//    }
//    
//    @Override
//    void writeToParcel(Parcel dest, int flags) {
//        dest.writeStringArray(mSuggestions);
//        dest.writeInt(mFlags);
//        dest.writeString(mLocaleString);
//        dest.writeString(mNotificationTargetClassName);
//        dest.writeString(mNotificationTargetPackageName);
//        dest.writeInt(mHashCode);
//        dest.writeInt(mEasyCorrectUnderlineColor);
//        dest.writeFloat(mEasyCorrectUnderlineThickness);
//        dest.writeInt(mMisspelledUnderlineColor);
//        dest.writeFloat(mMisspelledUnderlineThickness);
//        dest.writeInt(mAutoCorrectionUnderlineColor);
//        dest.writeFloat(mAutoCorrectionUnderlineThickness);
//    }
//    
//    @Override
//    int getSpanTypeId() {
//        return TextUtils.SUGGESTION_SPAN;
//    }
    
//    bool equals(Object *o);
    
    int hashCode();
    
private:
    
//    static int hashCodeInternal(vector<string> suggestions, string locale,
//                                string notificationTargetClassName);
    
//    static const Parcelable.Creator<SuggestionSpan> CREATOR =
//    new Parcelable.Creator<SuggestionSpan>() {
//        @Override
//        SuggestionSpan createFromParcel(Parcel source) {
//            return new SuggestionSpan(source);
//        }
//        
//        @Override
//        SuggestionSpan[] newArray(int size) {
//            return new SuggestionSpan[size];
//        }
//    };
    
public:
    
    void updateDrawState(shared_ptr<TextPaint> tp);
    
    /**
     * @return The color of the underline for that span, or 0 if there is no underline
     *
     * @hide
     */
    int getUnderlineColor();
    
    /**
     * Notifies a suggestion selection.
     *
     * @hide
     */
//    void notifySelection(Context *context, string original, int index);
};

ANDROID_END

#endif /* defined(__Androidpp__SuggestionSpan__) */

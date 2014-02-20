//
//  InputType.h
//  Androidpp
//
//  Created by Saul Howard on 1/29/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_InputType_h
#define Androidpp_InputType_h

#include "AndroidMacros.h"

ANDROID_BEGIN

/**
 * Bit definitions for an integer defining the basic content type of text
 * held in an {@link android.text.Editable} object. Supported classes may be combined
 * with variations and flags to indicate desired behaviors.
 *
 * <h3>Examples</h3>
 *
 * <dl>
 * <dt>A password field with with the password visible to the user:
 * <dd>inputType = TYPE_CLASS_TEXT |
 *     TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
 *
 * <dt>A multi-line postal address with automatic capitalization:
 * <dd>inputType = TYPE_CLASS_TEXT |
 *     TYPE_TEXT_VARIATION_POSTAL_ADDRESS |
 *     TYPE_TEXT_FLAG_MULTI_LINE
 *
 * <dt>A time field:
 * <dd>inputType = TYPE_CLASS_DATETIME |
 *     TYPE_DATETIME_VARIATION_TIME
 * </dl>
 */
class InputType {
    
public:
    
    /**
     * Mask of bits that determine the overall class
     * of text being given.  Currently supported classes are:
     * {@link #TYPE_CLASS_TEXT}, {@link #TYPE_CLASS_NUMBER},
     * {@link #TYPE_CLASS_PHONE}, {@link #TYPE_CLASS_DATETIME}.
     * If the class is not one you
     * understand, assume {@link #TYPE_CLASS_TEXT} with NO variation
     * or flags.
     */
    static const int TYPE_MASK_CLASS = 0x0000000f;
    
    /**
     * Mask of bits that determine the variation of
     * the base content class.
     */
    static const int TYPE_MASK_VARIATION = 0x00000ff0;
    
    /**
     * Mask of bits that provide addition bit flags
     * of options.
     */
    static const int TYPE_MASK_FLAGS = 0x00fff000;
    
    /**
     * Special content type for when no explicit type has been specified.
     * This should be interpreted to mean that the target input connection
     * is not rich, it can not process and show things like candidate text nor
     * retrieve the current text, so the input method will need to run in a
     * limited "generate key events" mode.
     */
    static const int TYPE_NULL = 0x00000000;
    
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    
    /**
     * Class for normal text.  This class supports the following flags (only
     * one of which should be set):
     * {@link #TYPE_TEXT_FLAG_CAP_CHARACTERS},
     * {@link #TYPE_TEXT_FLAG_CAP_WORDS}, and.
     * {@link #TYPE_TEXT_FLAG_CAP_SENTENCES}.  It also supports the
     * following variations:
     * {@link #TYPE_TEXT_VARIATION_NORMAL}, and
     * {@link #TYPE_TEXT_VARIATION_URI}.  If you do not recognize the
     * variation, normal should be assumed.
     */
    static const int TYPE_CLASS_TEXT = 0x00000001;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: capitalize all characters.  Overrides
     * {@link #TYPE_TEXT_FLAG_CAP_WORDS} and
     * {@link #TYPE_TEXT_FLAG_CAP_SENTENCES}.  This value is explicitly defined
     * to be the same as {@link android.text.TextUtils#CAP_MODE_CHARACTERS}.
     */
    static const int TYPE_TEXT_FLAG_CAP_CHARACTERS = 0x00001000;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: capitalize first character of
     * all words.  Overrides {@link #TYPE_TEXT_FLAG_CAP_SENTENCES}.  This
     * value is explicitly defined
     * to be the same as {@link android.text.TextUtils#CAP_MODE_WORDS}.
     */
    static const int TYPE_TEXT_FLAG_CAP_WORDS = 0x00002000;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: capitalize first character of
     * each sentence.  This value is explicitly defined
     * to be the same as {@link android.text.TextUtils#CAP_MODE_SENTENCES}.
     */
    static const int TYPE_TEXT_FLAG_CAP_SENTENCES = 0x00004000;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: the user is entering free-form
     * text that should have auto-correction applied to it.
     */
    static const int TYPE_TEXT_FLAG_AUTO_CORRECT = 0x00008000;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: the text editor is performing
     * auto-completion of the text being entered based on its own semantics,
     * which it will present to the user as they type.  This generally means
     * that the input method should not be showing candidates itself, but can
     * expect for the editor to supply its own completions/candidates from
     * {@link android.view.inputmethod.InputMethodSession#displayCompletions
     * InputMethodSession.displayCompletions()} as a result of the editor calling
     * {@link android.view.inputmethod.InputMethodManager#displayCompletions
     * InputMethodManager.displayCompletions()}.
     */
    static const int TYPE_TEXT_FLAG_AUTO_COMPLETE = 0x00010000;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: multiple lines of text can be
     * entered into the field.  If this flag is not set, the text field
     * will be constrained to a single line.
     */
    static const int TYPE_TEXT_FLAG_MULTI_LINE = 0x00020000;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: the regular text view associated
     * with this should not be multi-line, but when a fullscreen input method
     * is providing text it should use multiple lines if it can.
     */
    static const int TYPE_TEXT_FLAG_IME_MULTI_LINE = 0x00040000;
    
    /**
     * Flag for {@link #TYPE_CLASS_TEXT}: the input method does not need to
     * display any dictionary-based candidates. This is useful for text views that
     * do not contain words from the language and do not benefit from any
     * dictionary-based completions or corrections. It overrides the
     * {@link #TYPE_TEXT_FLAG_AUTO_CORRECT} value when set.
     */
    static const int TYPE_TEXT_FLAG_NO_SUGGESTIONS = 0x00080000;
    
    // ----------------------------------------------------------------------
    
    /**
     * Default variation of {@link #TYPE_CLASS_TEXT}: plain old normal text.
     */
    static const int TYPE_TEXT_VARIATION_NORMAL = 0x00000000;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering a URI.
     */
    static const int TYPE_TEXT_VARIATION_URI = 0x00000010;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering an e-mail address.
     */
    static const int TYPE_TEXT_VARIATION_EMAIL_ADDRESS = 0x00000020;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering the subject line of
     * an e-mail.
     */
    static const int TYPE_TEXT_VARIATION_EMAIL_SUBJECT = 0x00000030;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering a short, possibly informal
     * message such as an instant message or a text message.
     */
    static const int TYPE_TEXT_VARIATION_SHORT_MESSAGE = 0x00000040;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering the content of a long, possibly
     * formal message such as the body of an e-mail.
     */
    static const int TYPE_TEXT_VARIATION_LONG_MESSAGE = 0x00000050;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering the name of a person.
     */
    static const int TYPE_TEXT_VARIATION_PERSON_NAME = 0x00000060;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering a postal mailing address.
     */
    static const int TYPE_TEXT_VARIATION_POSTAL_ADDRESS = 0x00000070;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering a password.
     */
    static const int TYPE_TEXT_VARIATION_PASSWORD = 0x00000080;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering a password, which should
     * be visible to the user.
     */
    static const int TYPE_TEXT_VARIATION_VISIBLE_PASSWORD = 0x00000090;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering text inside of a web form.
     */
    static const int TYPE_TEXT_VARIATION_WEB_EDIT_TEXT = 0x000000a0;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering text to filter contents
     * of a list etc.
     */
    static const int TYPE_TEXT_VARIATION_FILTER = 0x000000b0;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering text for phonetic
     * pronunciation, such as a phonetic name field in contacts.
     */
    static const int TYPE_TEXT_VARIATION_PHONETIC = 0x000000c0;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering e-mail address inside
     * of a web form.  This was added in
     * {@link android.os.Build.VERSION_CODES#HONEYCOMB}.  An IME must target
     * this API version or later to see this input type; if it doesn't, a request
     * for this type will be seen as {@link #TYPE_TEXT_VARIATION_EMAIL_ADDRESS}
     * when passed through {@link android.view.inputmethod.EditorInfo#makeCompatible(int)
     * EditorInfo.makeCompatible(int)}.
     */
    static const int TYPE_TEXT_VARIATION_WEB_EMAIL_ADDRESS = 0x000000d0;
    
    /**
     * Variation of {@link #TYPE_CLASS_TEXT}: entering password inside
     * of a web form.  This was added in
     * {@link android.os.Build.VERSION_CODES#HONEYCOMB}.  An IME must target
     * this API version or later to see this input type; if it doesn't, a request
     * for this type will be seen as {@link #TYPE_TEXT_VARIATION_PASSWORD}
     * when passed through {@link android.view.inputmethod.EditorInfo#makeCompatible(int)
     * EditorInfo.makeCompatible(int)}.
     */
    static const int TYPE_TEXT_VARIATION_WEB_PASSWORD = 0x000000e0;
    
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    
    /**
     * Class for numeric text.  This class supports the following flag:
     * {@link #TYPE_NUMBER_FLAG_SIGNED} and
     * {@link #TYPE_NUMBER_FLAG_DECIMAL}.  It also supports the following
     * variations: {@link #TYPE_NUMBER_VARIATION_NORMAL} and
     * {@link #TYPE_NUMBER_VARIATION_PASSWORD}.  If you do not recognize
     * the variation, normal should be assumed.
     */
    static const int TYPE_CLASS_NUMBER = 0x00000002;
    
    /**
     * Flag of {@link #TYPE_CLASS_NUMBER}: the number is signed, allowing
     * a positive or negative sign at the start.
     */
    static const int TYPE_NUMBER_FLAG_SIGNED = 0x00001000;
    
    /**
     * Flag of {@link #TYPE_CLASS_NUMBER}: the number is decimal, allowing
     * a decimal point to provide fractional values.
     */
    static const int TYPE_NUMBER_FLAG_DECIMAL = 0x00002000;
    
    // ----------------------------------------------------------------------
    
    /**
     * Default variation of {@link #TYPE_CLASS_NUMBER}: plain normal
     * numeric text.  This was added in
     * {@link android.os.Build.VERSION_CODES#HONEYCOMB}.  An IME must target
     * this API version or later to see this input type; if it doesn't, a request
     * for this type will be dropped when passed through
     * {@link android.view.inputmethod.EditorInfo#makeCompatible(int)
     * EditorInfo.makeCompatible(int)}.
     */
    static const int TYPE_NUMBER_VARIATION_NORMAL = 0x00000000;
    
    /**
     * Variation of {@link #TYPE_CLASS_NUMBER}: entering a numeric password.
     * This was added in {@link android.os.Build.VERSION_CODES#HONEYCOMB}.  An
     * IME must target this API version or later to see this input type; if it
     * doesn't, a request for this type will be dropped when passed
     * through {@link android.view.inputmethod.EditorInfo#makeCompatible(int)
     * EditorInfo.makeCompatible(int)}.
     */
    static const int TYPE_NUMBER_VARIATION_PASSWORD = 0x00000010;
    
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    
    /**
     * Class for a phone number.  This class currently supports no variations
     * or flags.
     */
    static const int TYPE_CLASS_PHONE = 0x00000003;
    
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    
    /**
     * Class for dates and times.  It supports the
     * following variations:
     * {@link #TYPE_DATETIME_VARIATION_NORMAL}
     * {@link #TYPE_DATETIME_VARIATION_DATE}, and
     * {@link #TYPE_DATETIME_VARIATION_TIME},.
     */
    static const int TYPE_CLASS_DATETIME = 0x00000004;
    
    /**
     * Default variation of {@link #TYPE_CLASS_DATETIME}: allows entering
     * both a date and time.
     */
    static const int TYPE_DATETIME_VARIATION_NORMAL = 0x00000000;
    
    /**
     * Default variation of {@link #TYPE_CLASS_DATETIME}: allows entering
     * only a date.
     */
    static const int TYPE_DATETIME_VARIATION_DATE = 0x00000010;
    
    /**
     * Default variation of {@link #TYPE_CLASS_DATETIME}: allows entering
     * only a time.
     */
    static const int TYPE_DATETIME_VARIATION_TIME = 0x00000020;
};

ANDROID_END

#endif

//
//  EditorInfo.h
//  Androidpp
//
//  Created by Saul Howard on 1/29/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_EditorInfo_h
#define Androidpp_EditorInfo_h

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/InputType.h"
#include "Android/text/String.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class EditorInfo : public InputType {
    
public:
    
    /**
     * The content type of the text box, whose bits are defined by
     * {@link android.text.InputType}.
     *
     * @see android.text.InputType
     * @see #TYPE_MASK_CLASS
     * @see #TYPE_MASK_VARIATION
     * @see #TYPE_MASK_FLAGS
     */
    int inputType = TYPE_NULL;
    
    /**
     * Set of bits in {@link #imeOptions} that provide alternative actions
     * associated with the "enter" key.  This both helps the IME provide
     * better feedback about what the enter key will do, and also allows it
     * to provide alternative mechanisms for providing that command.
     */
    static const int IME_MASK_ACTION = 0x000000ff;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: no specific action has been
     * associated with this editor, let the editor come up with its own if
     * it can.
     */
    static const int IME_ACTION_UNSPECIFIED = 0x00000000;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: there is no available action.
     */
    static const int IME_ACTION_NONE = 0x00000001;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: the action key performs a "go"
     * operation to take the user to the target of the text they typed.
     * Typically used, for example, when entering a URL.
     */
    static const int IME_ACTION_GO = 0x00000002;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: the action key performs a "search"
     * operation, taking the user to the results of searching for the text
     * the have typed (in whatever context is appropriate).
     */
    static const int IME_ACTION_SEARCH = 0x00000003;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: the action key performs a "send"
     * operation, delivering the text to its target.  This is typically used
     * when composing a message.
     */
    static const int IME_ACTION_SEND = 0x00000004;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: the action key performs a "next"
     * operation, taking the user to the next field that will accept text.
     */
    static const int IME_ACTION_NEXT = 0x00000005;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: the action key performs a "done"
     * operation, typically meaning the IME will be closed.
     */
    static const int IME_ACTION_DONE = 0x00000006;
    
    /**
     * Bits of {@link #IME_MASK_ACTION}: Like {@link #IME_ACTION_NEXT}, but
     * for moving to the previous field.  This will normally not be used to
     * specify an action (since it precludes {@link #IME_ACTION_NEXT}, but
     * can be returned to the app if it sets {@link #IME_FLAG_NAVIGATE_PREVIOUS}.
     */
    static const int IME_ACTION_PREVIOUS = 0x00000007;
    
    /**
     * Flag of {@link #imeOptions}: used to request that the IME never go
     * into fullscreen mode.  Applications need to be aware that the flag is not
     * a guarantee, and not all IMEs will respect it.
     */
    static const int IME_FLAG_NO_FULLSCREEN = 0x2000000;
    
    /**
     * Flag of {@link #imeOptions}: like {@link #IME_FLAG_NAVIGATE_NEXT}, but
     * specifies there is something interesting that a backward navigation
     * can focus on.  If the user selects the IME's facility to backward
     * navigate, this will show up in the application as an {@link #IME_ACTION_PREVIOUS}
     * at {@link InputConnection#performEditorAction(int)
     * InputConnection.performEditorAction(int)}.
     */
    static const int IME_FLAG_NAVIGATE_PREVIOUS = 0x4000000;
    
    /**
     * Flag of {@link #imeOptions}: used to specify that there is something
     * interesting that a forward navigation can focus on. This is like using
     * {@link #IME_ACTION_NEXT}, except allows the IME to be multiline (with
     * an enter key) as well as provide forward navigation.  Note that some
     * IMEs may not be able to do this, especially when running on a small
     * screen where there is little space.  In that case it does not need to
     * present a UI for this option.  Like {@link #IME_ACTION_NEXT}, if the
     * user selects the IME's facility to forward navigate, this will show up
     * in the application at {@link InputConnection#performEditorAction(int)
     * InputConnection.performEditorAction(int)}.
     */
    static const int IME_FLAG_NAVIGATE_NEXT = 0x8000000;
    
    /**
     * Flag of {@link #imeOptions}: used to specify that the IME does not need
     * to show its extracted text UI.  For input methods that may be fullscreen,
     * often when in landscape mode, this allows them to be smaller and let part
     * of the application be shown behind.  Though there will likely be limited
     * access to the application available from the user, it can make the
     * experience of a (mostly) fullscreen IME less jarring.  Note that when
     * this flag is specified the IME may <em>not</em> be set up to be able
     * to display text, so it should only be used in situations where this is
     * not needed.
     */
    static const int IME_FLAG_NO_EXTRACT_UI = 0x10000000;
    
    /**
     * Flag of {@link #imeOptions}: used in conjunction with
     * {@link #IME_MASK_ACTION}, this indicates that the action should not
     * be available as an accessory button when the input method is full-screen.
     * Note that by setting this flag, there can be cases where the action
     * is simply never available to the user.  Setting this generally means
     * that you think showing text being edited is more important than the
     * action you have supplied.
     */
    static const int IME_FLAG_NO_ACCESSORY_ACTION = 0x20000000;
    
    /**
     * Flag of {@link #imeOptions}: used in conjunction with
     * {@link #IME_MASK_ACTION}, this indicates that the action should not
     * be available in-line as a replacement for "enter" key.  Typically this is
     * because the action has such a significant impact or is not recoverable
     * enough that accidentally hitting it should be avoided, such as sending
     * a message.  Note that {@link android.widget.TextView} will automatically set this
     * flag for you on multi-line text views.
     */
    static const int IME_FLAG_NO_ENTER_ACTION = 0x40000000;
    
    /**
     * Flag of {@link #imeOptions}: used to request that the IME is capable of
     * inputting ASCII characters.  The intention of this flag is to ensure that
     * the user can type Roman alphabet characters in a {@link android.widget.TextView}
     * used for, typically, account ID or password input.  It is expected that IMEs
     * normally are able to input ASCII even without being told so (such IMEs
     * already respect this flag in a sense), but there could be some cases they
     * aren't when, for instance, only non-ASCII input languagaes like Arabic,
     * Greek, Hebrew, Russian are enabled in the IME.  Applications need to be
     * aware that the flag is not a guarantee, and not all IMEs will respect it.
     * However, it is strongly recommended for IME authors to respect this flag
     * especially when their IME could end up with a state that has only non-ASCII
     * input languages enabled.
     */
    static const int IME_FLAG_FORCE_ASCII = 0x80000000;
    
    /**
     * Generic unspecified type for {@link #imeOptions}.
     */
    static const int IME_NULL = 0x00000000;
    
    /**
     * Extended type information for the editor, to help the IME better
     * integrate with it.
     */
    int imeOptions = IME_NULL;
    
    /**
     * A string supplying additional information options that are
     * private to a particular IME implementation.  The string must be
     * scoped to a package owned by the implementation, to ensure there are
     * no conflicts between implementations, but other than that you can put
     * whatever you want in it to communicate with the IME.  For example,
     * you could have a string that supplies an argument like
     * <code>"com.example.myapp.SpecialMode=3"</code>.  This field is can be
     * filled in from the {@link android.R.attr#privateImeOptions}
     * attribute of a TextView.
     */
    shared_ptr<String> privateImeOptions;
    
    /**
     * In some cases an IME may be able to display an arbitrary label for
     * a command the user can perform, which you can specify here.  You can
     * not count on this being used.
     */
    shared_ptr<CharSequence> actionLabel;
    
    /**
     * If {@link #actionLabel} has been given, this is the id for that command
     * when the user presses its button that is delivered back with
     * {@link InputConnection#performEditorAction(int)
     * InputConnection.performEditorAction()}.
     */
    int actionId = 0;
    
    /**
     * The text offset of the start of the selection at the time editing
     * began; -1 if not known.
     */
    int initialSelStart = -1;
    
    /**
     * The text offset of the end of the selection at the time editing
     * began; -1 if not known.
     */
    int initialSelEnd = -1;
    
    /**
     * The capitalization mode of the first character being edited in the
     * text.  Values may be any combination of
     * {@link android.text.TextUtils#CAP_MODE_CHARACTERS TextUtils.CAP_MODE_CHARACTERS},
     * {@link android.text.TextUtils#CAP_MODE_WORDS TextUtils.CAP_MODE_WORDS}, and
     * {@link android.text.TextUtils#CAP_MODE_SENTENCES TextUtils.CAP_MODE_SENTENCES}, though
     * you should generally just take a non-zero value to mean start out in
     * caps mode.
     */
    int initialCapsMode = 0;
    
    /**
     * The "hint" text of the text view, typically shown in-line when the
     * text is empty to tell the user what to enter.
     */
    shared_ptr<CharSequence> hintText;
    
    /**
     * A label to show to the user describing the text they are writing.
     */
    shared_ptr<CharSequence> label;
};

ANDROID_END

#endif

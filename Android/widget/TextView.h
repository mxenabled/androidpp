//
//  TextView.h
//  Androidpp
//
//  Created by Saul Howard on 1/14/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__TextView__
#define __Androidpp__TextView__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/Drawable.h"
#include "Android/content/Context.h"
#include "Android/content/res/Resources.h"
#include "Android/text/BoringLayout.h"
#include "Android/text/CharSequence.h"
#include "Android/text/Editable.h"
#include "Android/text/GetChars.h"
#include "Android/text/GraphicsOperations.h"
#include "Android/text/Layout.h"
#include "Android/text/Spannable.h"
#include "Android/text/Spanned.h"
#include "Android/text/SpanWatcher.h"
#include "Android/text/TextPaint.h"
#include "Android/text/TextUtils.h"
#include "Android/text/TextWatcher.h"
#include "Android/utils/Object.h"
#include "Android/view/Gravity.h"
#include "Android/view/View.h"
#include "Android/view/ViewTreeObserver.h"

#include <string>
#include <vector>
#include <memory>
#include <math.h>

using namespace std;

ANDROID_BEGIN

class Canvas;
class ChangeWatcher;
class CharWrapper;
class ColorStateList;
class Editor;
class InputFilter;
class KeyEvent;
class KeyListener;
class Marquee;
class MovementMethod;
class Paint;
class Path;
class Rect;
class RectF;
class Scroller;
class String;
class TextPaint;
class TextDirectionHeuristic;
class TransformationMethod;
class Typeface;
class WordIterator;

class TextView : public View, public ViewTreeObserver::OnPreDrawListener {
    
public:
    
    enum BufferType {
        NORMAL, SPANNABLE, EDITABLE,
    };
    
private:
    
    static const bool DEBUG_EXTRACT = false;
    
    // Enum for the "typeface" XML parameter.
    // TODO: How can we get this from the XML instead of hardcoding it here?
    static const int SANS = 1;
    static const int SERIF = 2;
    static const int MONOSPACE = 3;
    
    // Bitfield for the "numeric" XML parameter.
    // TODO: How can we get this from the XML instead of hardcoding it here?
    static const int SIGNED = 2;
    static const int DECIMAL = 4;
    
    /**
     * Draw marquee text with fading edges as usual
     */
    static const int MARQUEE_FADE_NORMAL = 0;
    
    /**
     * Draw marquee text as ellipsize end while inactive instead of with the fade.
     * (Useful for devices where the fade can be expensive if overdone)
     */
    static const int MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS = 1;
    
    /**
     * Draw marquee text with fading edges because it is currently active/animating.
     */
    static const int MARQUEE_FADE_SWITCH_SHOW_FADE = 2;
    
    static const int LINES = 1;
    static const int EMS = LINES;
    static const int PIXELS = 2;
    
    static RectF TEMP_RECTF;
    
    // XXX should be much larger
    static const int VERY_WIDE = 1024 * 1024;
    static const int ANIMATED_SCROLL_GAP = 250;
    
    static const vector<shared_ptr<InputFilter>> NO_FILTERS;
    static const shared_ptr<Spanned> EMPTY_SPANNED;
    
    static const int CHANGE_WATCHER_PRIORITY = 100;
    
    // New state used to change background based on whether this TextView is multiline.
    static const vector<int> MULTILINE_STATE_SET;
    
    // System wide time for last cut or copy action.
    static long LAST_CUT_OR_COPY_TIME;
    
    class CharWrapper : public virtual CharSequence, public virtual GetChars, public virtual GraphicsOperations {
    
        friend class TextView;
        
    private:
        
        UnicodeString mChars;
        int mStart = 0, mLength = 0;
        
    public:
        
        CharWrapper(UnicodeString chars, int start, int len) {
            mChars = chars;
            mStart = start;
            mLength = len;
        }
        
        void set(UnicodeString chars, int start, int len) {
            mChars = chars;
            mStart = start;
            mLength = len;
        }
        
        int length() {
            return mLength;
        }
        
        UChar charAt(int off) {
            return mChars[off + mStart];
        }
        
        shared_ptr<String> toString();
        
        shared_ptr<CharSequence> subSequence(int start, int end);
        
        void getChars(int start, int end, UnicodeString &buf, int off);
        
        void drawText(shared_ptr<Canvas> c, int start, int end,
                      float x, float y, Paint *p);
        
        void drawTextRun(shared_ptr<Canvas> c, int start, int end,
                         int contextStart, int contextEnd, float x, float y, int flags, Paint *p);
        
        float measureText(int start, int end, Paint *p);
        
        int getTextWidths(int start, int end, vector<float> widths, Paint *p);
        
        float getTextRunAdvances(int start, int end, int contextStart,
                                        int contextEnd, int flags, vector<float> advances, int advancesIndex,
                                 Paint *p);
        
        int getTextRunCursor(int contextStart, int contextEnd, int flags,
                             int offset, int cursorOpt, Paint *p);
    };
    
//    static const class Marquee extends Handler {
//        // TODO: Add an option to configure this
//        static const float MARQUEE_DELTA_MAX = 0.07f;
//        static const int MARQUEE_DELAY = 1200;
//        static const int MARQUEE_RESTART_DELAY = 1200;
//        static const int MARQUEE_RESOLUTION = 1000 / 30;
//        static const int MARQUEE_PIXELS_PER_SECOND = 30;
//        
//        static const byte MARQUEE_STOPPED = 0x0;
//        static const byte MARQUEE_STARTING = 0x1;
//        static const byte MARQUEE_RUNNING = 0x2;
//        
//        static const int MESSAGE_START = 0x1;
//        static const int MESSAGE_TICK = 0x2;
//        static const int MESSAGE_RESTART = 0x3;
//        
//        const WeakReference<TextView> mView;
//        
//        byte mStatus = MARQUEE_STOPPED;
//        const float mScrollUnit;
//        float mMaxScroll;
//        float mMaxFadeScroll;
//        float mGhostStart;
//        float mGhostOffset;
//        float mFadeStop;
//        int mRepeatLimit;
//        
//        float mScroll;
//        
//        Marquee(TextView v) {
//            const float density = v.getContext().getResources().getDisplayMetrics().density;
//            mScrollUnit = (MARQUEE_PIXELS_PER_SECOND * density) / MARQUEE_RESOLUTION;
//            mView = new WeakReference<TextView>(v);
//        }
//        
//        @Override
//        public void handleMessage(Message msg) {
//            switch (msg.what) {
//                case MESSAGE_START:
//                    mStatus = MARQUEE_RUNNING;
//                    tick();
//                    break;
//                case MESSAGE_TICK:
//                    tick();
//                    break;
//                case MESSAGE_RESTART:
//                    if (mStatus == MARQUEE_RUNNING) {
//                        if (mRepeatLimit >= 0) {
//                            mRepeatLimit--;
//                        }
//                        start(mRepeatLimit);
//                    }
//                    break;
//            }
//        }
//        
//        void tick() {
//            if (mStatus != MARQUEE_RUNNING) {
//                return;
//            }
//            
//            removeMessages(MESSAGE_TICK);
//            
//            const TextView textView = mView.get();
//            if (textView != NULL && (textView.isFocused() || textView.isSelected())) {
//                mScroll += mScrollUnit;
//                if (mScroll > mMaxScroll) {
//                    mScroll = mMaxScroll;
//                    sendEmptyMessageDelayed(MESSAGE_RESTART, MARQUEE_RESTART_DELAY);
//                } else {
//                    sendEmptyMessageDelayed(MESSAGE_TICK, MARQUEE_RESOLUTION);
//                }
//                textView.invalidate();
//            }
//        }
//        
//        void stop() {
//            mStatus = MARQUEE_STOPPED;
//            removeMessages(MESSAGE_START);
//            removeMessages(MESSAGE_RESTART);
//            removeMessages(MESSAGE_TICK);
//            resetScroll();
//        }
//        
//        void resetScroll() {
//            mScroll = 0.0f;
//            const TextView textView = mView.get();
//            if (textView != NULL) textView.invalidate();
//        }
//        
//        void start(int repeatLimit) {
//            if (repeatLimit == 0) {
//                stop();
//                return;
//            }
//            mRepeatLimit = repeatLimit;
//            const TextView textView = mView.get();
//            if (textView != NULL && textView.mLayout != NULL) {
//                mStatus = MARQUEE_STARTING;
//                mScroll = 0.0f;
//                const int textWidth = textView.getWidth() - textView.getCompoundPaddingLeft() -
//                textView.getCompoundPaddingRight();
//                const float lineWidth = textView.mLayout::getLineWidth(0);
//                const float gap = textWidth / 3.0f;
//                mGhostStart = lineWidth - textWidth + gap;
//                mMaxScroll = mGhostStart + textWidth;
//                mGhostOffset = lineWidth + gap;
//                mFadeStop = lineWidth + textWidth / 6.0f;
//                mMaxFadeScroll = mGhostStart + lineWidth + lineWidth;
//                
//                textView.invalidate();
//                sendEmptyMessageDelayed(MESSAGE_START, MARQUEE_DELAY);
//            }
//        }
//        
//        float getGhostOffset() {
//            return mGhostOffset;
//        }
//        
//        float getScroll() {
//            return mScroll;
//        }
//        
//        float getMaxFadeScroll() {
//            return mMaxFadeScroll;
//        }
//        
//        bool shouldDrawLeftFade() {
//            return mScroll <= mFadeStop;
//        }
//        
//        bool shouldDrawGhost() {
//            return mStatus == MARQUEE_RUNNING && mScroll > mGhostStart;
//        }
//        
//        bool isRunning() {
//            return mStatus == MARQUEE_RUNNING;
//        }
//        
//        bool isStopped() {
//            return mStatus == MARQUEE_STOPPED;
//        }
//    };
    
    class ChangeWatcher : public TextWatcher, public SpanWatcher, public virtual Object {
        
    private:
        
        shared_ptr<CharSequence> mBeforeText;
        shared_ptr<TextView> mTextView = NULL;
        
    public:
        
        ChangeWatcher(shared_ptr<TextView> textView) {
            mTextView = textView;
        }
        
        void beforeTextChanged(shared_ptr<CharSequence> buffer, int start,
                                      int before, int after) {
//            if (AccessibilityManager.getInstance(mContext).isEnabled()
//                && ((!isPasswordInputType(getInputType()) && !hasPasswordTransformationMethod())
//                    || shouldSpeakPasswordsForAccessibility())) {
//                    mBeforeText = buffer.toString();
//                }
            
            mTextView->sendBeforeTextChanged(buffer, start, before, after);
        }
        
        void onTextChanged(shared_ptr<CharSequence> buffer, int start, int before, int after) {
            mTextView->handleTextChanged(buffer, start, before, after);
            
//            if (AccessibilityManager.getInstance(mContext).isEnabled() &&
//                (isFocused() || isSelected() && isShown())) {
//                sendAccessibilityEventTypeViewTextChanged(mBeforeText, start, before, after);
//                mBeforeText = NULL;
//            }
        }
        
        void afterTextChanged(shared_ptr<Editable> buffer) {
            mTextView->sendAfterTextChanged(buffer);
            
//            if (MetaKeyKeyListener::getMetaState(buffer, MetaKeyKeyListener::META_SELECTING) != 0) {
//                MetaKeyKeyListener::stopSelecting(mTextView, buffer);
//            }
        }
        
        void onSpanChanged(shared_ptr<Spannable> buf, shared_ptr<Object> what, int s, int e, int st, int en) {
            mTextView->spanChange(buf, what, s, st, e, en);
        }
        
        void onSpanAdded(shared_ptr<Spannable> buf, shared_ptr<Object> what, int s, int e) {
            mTextView->spanChange(buf, what, -1, s, -1, e);
        }
        
        void onSpanRemoved(shared_ptr<Spannable> buf, shared_ptr<Object> what, int s, int e) {
            mTextView->spanChange(buf, what, s, -1, e, -1);
        }
    };
    
    class Drawables {
        
        friend class TextView;
        
    private:
        
        const static int DRAWABLE_NONE = -1;
        const static int DRAWABLE_RIGHT = 0;
        const static int DRAWABLE_LEFT = 1;
        
        const Rect mCompoundRect;
        
        shared_ptr<Drawable> mDrawableTop, mDrawableBottom, mDrawableLeft, mDrawableRight,
        mDrawableStart, mDrawableEnd, mDrawableError, mDrawableTemp;
        
        shared_ptr<Drawable> mDrawableLeftInitial, mDrawableRightInitial;
        bool mIsRtlCompatibilityMode = false;
        bool mOverride = false;
        
        int mDrawableSizeTop = 0, mDrawableSizeBottom = 0, mDrawableSizeLeft = 0, mDrawableSizeRight = 0,
        mDrawableSizeStart = 0, mDrawableSizeEnd = 0, mDrawableSizeError = 0, mDrawableSizeTemp = 0;
        
        int mDrawableWidthTop = 0, mDrawableWidthBottom = 0, mDrawableHeightLeft = 0, mDrawableHeightRight = 0,
        mDrawableHeightStart = 0, mDrawableHeightEnd = 0, mDrawableHeightError = 0, mDrawableHeightTemp = 0;
        
        int mDrawablePadding = 0;
        
        int mDrawableSaved = DRAWABLE_NONE;
        
        void updateDrawablesLayoutDirection(int layoutDirection);
        
        void applyErrorDrawableIfNeeded(int layoutDirection);
        
    public:
        
        Drawables(Context *context);
        
        void resolveWithLayoutDirection(int layoutDirection);
        
        void setErrorDrawable(shared_ptr<Drawable> dr, shared_ptr<TextView> tv);
    };
    
    shared_ptr<ColorStateList> mTextColor;
    shared_ptr<ColorStateList> mHintTextColor;
    shared_ptr<ColorStateList> mLinkTextColor;
    int mCurTextColor = 0;
    int mCurHintTextColor = 0;
    bool mFreezesText = false;
    bool mTemporaryDetach = false;
    bool mDispatchTemporaryDetach = false;
    
    Editable::Factory mEditableFactory = Editable::Factory::getInstance();
    Spannable::Factory mSpannableFactory = Spannable::Factory::getInstance();
    
    float mShadowRadius = 0, mShadowDx = 0, mShadowDy = 0;
    
    bool mPreDrawRegistered = false;
    
    shared_ptr<TextUtils::TruncateAt> mEllipsize;
    
    shared_ptr<Drawables> mDrawables;
    
    shared_ptr<CharWrapper> mCharWrapper;
    
//    Marquee mMarquee;
    bool mRestartMarquee = false;
    
    int mMarqueeRepeatLimit = 3;
    
    int mLastLayoutDirection = -1;
    
    /**
     * On some devices the fading edges add a performance penalty if used
     * extensively in the same layout. This mode indicates how the marquee
     * is currently being shown, if applicable. (mEllipsize will == MARQUEE)
     */
    int mMarqueeFadeMode = MARQUEE_FADE_NORMAL;
    
    /**
     * When mMarqueeFadeMode is not MARQUEE_FADE_NORMAL, this stores
     * the layout that should be used when the mode switches.
     */
    shared_ptr<Layout> mSavedMarqueeModeLayout;
    
    shared_ptr<CharSequence> mText;
    shared_ptr<Spannable> mTextSpannable;
    shared_ptr<CharSequence> mTransformed;
    BufferType mBufferType = BufferType::NORMAL;
    
    shared_ptr<CharSequence> mHint;
    shared_ptr<Layout> mHintLayout;
    
    shared_ptr<MovementMethod> mMovement;
    
    shared_ptr<TransformationMethod> mTransformation;
    bool mAllowTransformationLengthChange = false;
    shared_ptr<ChangeWatcher> mChangeWatcher;
    
    vector<shared_ptr<TextWatcher>> mListeners;
    
    // display attributes
    shared_ptr<TextPaint> mTextPaint;
    bool mUserSetTextScaleX = false;
    shared_ptr<Layout> mLayout;
    
    int mGravity = Gravity::TOP | Gravity::START;
    bool mHorizontallyScrolling = false;
    
    int mAutoLinkMask = 0;
    bool mLinksClickable = false; //true;
    
    float mSpacingMult = 1.0f;
    float mSpacingAdd = 0.0f;
    
    int mMaximum = INT_MAX;
    int mMaxMode = LINES;
    int mMinimum = 0;
    int mMinMode = LINES;
    
    int mOldMaximum = mMaximum;
    int mOldMaxMode = mMaxMode;
    
    int mMaxWidth = INT_MAX;
    int mMaxWidthMode = PIXELS;
    int mMinWidth = 0;
    int mMinWidthMode = PIXELS;
    
    bool mSingleLine = false;
    int mDesiredHeightAtMeasure = -1;
    bool mIncludePad = true;
    int mDeferScroll = -1;
    
    // tmp primitives, so we don't alloc them on each draw
    Rect mTempRect;
    long mLastScroll = 0;
    shared_ptr<Scroller> mScroller;
    
    shared_ptr<BoringLayout::Metrics> mBoring, mHintBoring;
    shared_ptr<BoringLayout> mSavedLayout, mSavedHintLayout;
    
    shared_ptr<TextDirectionHeuristic> mTextDir;
    
    vector<shared_ptr<InputFilter>> mFilters = NO_FILTERS;
    
//    Locale mCurrentSpellCheckerLocaleCache;
//    const ReentrantLock mCurrentTextServicesLocaleLock = new ReentrantLock();
    
    // It is possible to have a selection even when mEditor is NULL (programmatically set, like when
    // a link is pressed). These highlight-related fields do not go in mEditor.
    int mHighlightColor = 0x6633B5E5;
    shared_ptr<Path> mHighlightPath;
    shared_ptr<Paint> mHighlightPaint;
    bool mHighlightPathBogus = true;
    
    // Although these fields are specific to editable text, they are not added to Editor because
    // they are defined by the TextView's style and are theme-dependent.
    int mCursorDrawableRes = 0;
    // These four fields, could be moved to Editor, since we know their default values and we
    // could condition the creation of the Editor to a non standard value. This is however
    // brittle since the hardcoded values here (such as
    // com.android.internal.R.drawable.text_select_handle_left) would have to be updated if the
    // default style is modified.
    int mTextSelectHandleLeftRes = 0;
    int mTextSelectHandleRightRes = 0;
    int mTextSelectHandleRes = 0;
    int mTextEditSuggestionItemLayout = 0;
    
    /**
     * EditText specific data, created on demand when one of the Editor fields is used.
     * See {@link #createEditorIfNeeded()}.
     */
//    Editor mEditor;
    
    /*
     * Kick-start the font cache for the zygote process (to pay the cost of
     * initializing freetype for our default font only once).
     */
//    static {
//        Paint p = new Paint();
//        p.setAntiAlias(true);
//        // We don't care about the result, just the side-effect of measuring.
//        p.measureText("H");
//    }
    
public:
    
    /**
     * Interface definition for a callback to be invoked when an action is
     * performed on the editor.
     */
    class OnEditorActionListener {
        
    public:
        
        /**
         * Called when an action is being performed.
         *
         * @param v The view that was clicked.
         * @param actionId Identifier of the action.  This will be either the
         * identifier you supplied, or {@link EditorInfo#IME_NULL
         * EditorInfo.IME_NULL} if being called due to the enter key
         * being pressed.
         * @param event If triggered by an enter key, this is the event;
         * otherwise, this is NULL.
         * @return Return true if you have consumed the action, else false.
         */
        virtual bool onEditorAction(shared_ptr<TextView> v, int actionId, KeyEvent *event) = 0;
    };
    
    TextView(Context *context) : TextView(context, NULL) {
    }
    
    TextView(Context *context, AttributeSet *attrs);
    
private:
    
    void setTypefaceFromAttrs(string familyName, int typefaceIndex, int styleIndex);
    
    void setRelativeDrawablesIfNeeded(shared_ptr<Drawable> start, shared_ptr<Drawable> end);
    
public:
    
    virtual void setEnabled(bool enabled);
    
    /**
     * Sets the typeface and style in which the text should be displayed,
     * and turns on the fake bold and italic bits in the Paint if the
     * Typeface that you provided does not have all the bits in the
     * style that you specified.
     *
     * @attr ref android.R.styleable#TextView_typeface
     * @attr ref android.R.styleable#TextView_textStyle
     */
    void setTypeface(Typeface *tf, int style);
    
protected:
    
    /**
     * Subclasses override this to specify that they have a KeyListener
     * by default even if not specifically called for in the XML options.
     */
    bool getDefaultEditable() {
        return false;
    }
    
    /**
     * Subclasses override this to specify a default movement method.
     */
    shared_ptr<MovementMethod> getDefaultMovementMethod() {
        return NULL;
    }
    
public:
    
    /**
     * Return the text the TextView is displaying. If setText() was called with
     * an argument of BufferType.SPANNABLE or BufferType.EDITABLE, you can cast
     * the return value from this method to Spannable or Editable, respectively.
     *
     * Note: The content of the return value should not be modified. If you want
     * a modifiable one, you should make your own copy first.
     *
     * @attr ref android.R.styleable#TextView_text
     */
    shared_ptr<CharSequence> getText() {
        return mText;
    }
    
    /**
     * Returns the length, in characters, of the text managed by this TextView
     */
    int length() {
        return mText->length();
    }
    
//    /**
//     * Return the text the TextView is displaying as an Editable object.  If
//     * the text is not editable, NULL is returned.
//     *
//     * @see #getText
//     */
//    Editable *getEditableText() {
//        return (mText instanceof Editable) ? (Editable)mText : NULL;
//    }
    
    /**
     * @return the height of one standard line in pixels.  Note that markup
     * within the text can cause individual lines to be taller or shorter
     * than this height, and the layout may contain additional first-
     * or last-line padding.
     */
    int getLineHeight() {
        return round(mTextPaint->getFontMetricsInt(NULL) * mSpacingMult + mSpacingAdd);
    }
    
    /**
     * @return the Layout that is currently being used to display the text.
     * This can be NULL if the text or width has recently changes.
     */
    shared_ptr<Layout> getLayout() {
        return mLayout;
    }
    
private:
    
    /**
     * @return the Layout that is currently being used to display the hint text.
     * This can be NULL.
     */
    shared_ptr<Layout> getHintLayout() {
        return mHintLayout;
    }
    
public:
    
    /**
     * @return the current key listener for this TextView.
     * This will frequently be NULL for non-EditText TextViews.
     *
     * @attr ref android.R.styleable#TextView_numeric
     * @attr ref android.R.styleable#TextView_digits
     * @attr ref android.R.styleable#TextView_phoneNumber
     * @attr ref android.R.styleable#TextView_inputMethod
     * @attr ref android.R.styleable#TextView_capitalize
     * @attr ref android.R.styleable#TextView_autoText
     */
    KeyListener *getKeyListener() {
        return NULL;//mEditor == NULL ? NULL : mEditor.mKeyListener;
    }
    
    /**
     * Sets the key listener to be used with this TextView.  This can be NULL
     * to disallow user input.  Note that this method has significant and
     * subtle interactions with soft keyboards and other input method:
     * see {@link KeyListener#getInputType() KeyListener.getContentType()}
     * for important details.  Calling this method will replace the current
     * content type of the text view with the content type returned by the
     * key listener.
     * <p>
     * Be warned that if you want a TextView with a key listener or movement
     * method not to be focusable, or if you want a TextView without a
     * key listener or movement method to be focusable, you must call
     * {@link #setFocusable} again after calling this to get the focusability
     * back the way you want it.
     *
     * @attr ref android.R.styleable#TextView_numeric
     * @attr ref android.R.styleable#TextView_digits
     * @attr ref android.R.styleable#TextView_phoneNumber
     * @attr ref android.R.styleable#TextView_inputMethod
     * @attr ref android.R.styleable#TextView_capitalize
     * @attr ref android.R.styleable#TextView_autoText
     */
    void setKeyListener(KeyListener *input);
    
private:
    
    void setKeyListenerOnly(KeyListener *input);
    
public:
    
    /**
     * @return the movement method being used for this TextView.
     * This will frequently be NULL for non-EditText TextViews.
     */
    shared_ptr<MovementMethod> getMovementMethod() {
        return mMovement;
    }
    
    /**
     * Sets the movement method (arrow key handler) to be used for
     * this TextView.  This can be NULL to disallow using the arrow keys
     * to move the cursor or scroll the view.
     * <p>
     * Be warned that if you want a TextView with a key listener or movement
     * method not to be focusable, or if you want a TextView without a
     * key listener or movement method to be focusable, you must call
     * {@link #setFocusable} again after calling this to get the focusability
     * back the way you want it.
     */
    void setMovementMethod(shared_ptr<MovementMethod> movement);
    
private:
    
    void fixFocusableAndClickableSettings();
    
public:
    
    /**
     * @return the current transformation method for this TextView.
     * This will frequently be NULL except for single-line and password
     * fields.
     *
     * @attr ref android.R.styleable#TextView_password
     * @attr ref android.R.styleable#TextView_singleLine
     */
    shared_ptr<TransformationMethod> getTransformationMethod() {
        return mTransformation;
    }
    
    /**
     * Sets the transformation that is applied to the text that this
     * TextView is displaying.
     *
     * @attr ref android.R.styleable#TextView_password
     * @attr ref android.R.styleable#TextView_singleLine
     */
    void setTransformationMethod(shared_ptr<TransformationMethod> method);
    
    /**
     * Returns the top padding of the view, plus space for the top
     * Drawable if any.
     */
    int getCompoundPaddingTop();
    
    /**
     * Returns the bottom padding of the view, plus space for the bottom
     * Drawable if any.
     */
    int getCompoundPaddingBottom();
    
    /**
     * Returns the left padding of the view, plus space for the left
     * Drawable if any.
     */
    int getCompoundPaddingLeft();
    
    /**
     * Returns the right padding of the view, plus space for the right
     * Drawable if any.
     */
    int getCompoundPaddingRight();
    
    /**
     * Returns the start padding of the view, plus space for the start
     * Drawable if any.
     */
    int getCompoundPaddingStart();
    
    /**
     * Returns the end padding of the view, plus space for the end
     * Drawable if any.
     */
    int getCompoundPaddingEnd();
    
    /**
     * Returns the extended top padding of the view, including both the
     * top Drawable if any and any extra space to keep more than maxLines
     * of text from showing.  It is only valid to call this after measuring.
     */
    int getExtendedPaddingTop();
    
    /**
     * Returns the extended bottom padding of the view, including both the
     * bottom Drawable if any and any extra space to keep more than maxLines
     * of text from showing.  It is only valid to call this after measuring.
     */
    int getExtendedPaddingBottom();
    
    /**
     * Returns the total left padding of the view, including the left
     * Drawable if any.
     */
    int getTotalPaddingLeft() {
        return getCompoundPaddingLeft();
    }
    
    /**
     * Returns the total right padding of the view, including the right
     * Drawable if any.
     */
    int getTotalPaddingRight() {
        return getCompoundPaddingRight();
    }
    
    /**
     * Returns the total start padding of the view, including the start
     * Drawable if any.
     */
    int getTotalPaddingStart() {
        return getCompoundPaddingStart();
    }
    
    /**
     * Returns the total end padding of the view, including the end
     * Drawable if any.
     */
    int getTotalPaddingEnd() {
        return getCompoundPaddingEnd();
    }
    
    /**
     * Returns the total top padding of the view, including the top
     * Drawable if any, the extra space to keep more than maxLines
     * from showing, and the vertical offset for gravity, if any.
     */
    int getTotalPaddingTop() {
        return getExtendedPaddingTop() + getVerticalOffset(true);
    }
    
    /**
     * Returns the total bottom padding of the view, including the bottom
     * Drawable if any, the extra space to keep more than maxLines
     * from showing, and the vertical offset for gravity, if any.
     */
    int getTotalPaddingBottom() {
        return getExtendedPaddingBottom() + getBottomVerticalOffset(true);
    }
    
    /**
     * Sets the Drawables (if any) to appear to the left of, above,
     * to the right of, and below the text.  Use NULL if you do not
     * want a Drawable there.  The Drawables must already have had
     * {@link Drawable#setBounds} called.
     *
     * @attr ref android.R.styleable#TextView_drawableLeft
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableRight
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    void setCompoundDrawables(shared_ptr<Drawable> left, shared_ptr<Drawable> top,
                                     shared_ptr<Drawable> right, shared_ptr<Drawable> bottom);
    
    /**
     * Sets the Drawables (if any) to appear to the left of, above,
     * to the right of, and below the text.  Use 0 if you do not
     * want a Drawable there. The Drawables' bounds will be set to
     * their intrinsic bounds.
     *
     * @param left Resource identifier of the left Drawable.
     * @param top Resource identifier of the top Drawable.
     * @param right Resource identifier of the right Drawable.
     * @param bottom Resource identifier of the bottom Drawable.
     *
     * @attr ref android.R.styleable#TextView_drawableLeft
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableRight
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    void setCompoundDrawablesWithIntrinsicBounds(int left, int top, int right, int bottom);
    
    /**
     * Sets the Drawables (if any) to appear to the left of, above,
     * to the right of, and below the text.  Use NULL if you do not
     * want a Drawable there. The Drawables' bounds will be set to
     * their intrinsic bounds.
     *
     * @attr ref android.R.styleable#TextView_drawableLeft
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableRight
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    void setCompoundDrawablesWithIntrinsicBounds(shared_ptr<Drawable> left, shared_ptr<Drawable> top,
                                                        shared_ptr<Drawable> right, shared_ptr<Drawable> bottom);
    
    /**
     * Sets the Drawables (if any) to appear to the start of, above,
     * to the end of, and below the text.  Use NULL if you do not
     * want a Drawable there.  The Drawables must already have had
     * {@link Drawable#setBounds} called.
     *
     * @attr ref android.R.styleable#TextView_drawableStart
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableEnd
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    void setCompoundDrawablesRelative(shared_ptr<Drawable> start, shared_ptr<Drawable> top,
                                             shared_ptr<Drawable> end, shared_ptr<Drawable> bottom);
    
    /**
     * Sets the Drawables (if any) to appear to the start of, above,
     * to the end of, and below the text.  Use 0 if you do not
     * want a Drawable there. The Drawables' bounds will be set to
     * their intrinsic bounds.
     *
     * @param start Resource identifier of the start Drawable.
     * @param top Resource identifier of the top Drawable.
     * @param end Resource identifier of the end Drawable.
     * @param bottom Resource identifier of the bottom Drawable.
     *
     * @attr ref android.R.styleable#TextView_drawableStart
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableEnd
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    void setCompoundDrawablesRelativeWithIntrinsicBounds(int start, int top, int end,
                                                                int bottom);
    
    /**
     * Sets the Drawables (if any) to appear to the start of, above,
     * to the end of, and below the text.  Use NULL if you do not
     * want a Drawable there. The Drawables' bounds will be set to
     * their intrinsic bounds.
     *
     * @attr ref android.R.styleable#TextView_drawableStart
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableEnd
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    void setCompoundDrawablesRelativeWithIntrinsicBounds(shared_ptr<Drawable> start, shared_ptr<Drawable> top,
                                                                shared_ptr<Drawable> end, shared_ptr<Drawable> bottom);
    
    /**
     * Returns drawables for the left, top, right, and bottom borders.
     *
     * @attr ref android.R.styleable#TextView_drawableLeft
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableRight
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    vector<shared_ptr<Drawable>> getCompoundDrawables();
    
    /**
     * Returns drawables for the start, top, end, and bottom borders.
     *
     * @attr ref android.R.styleable#TextView_drawableStart
     * @attr ref android.R.styleable#TextView_drawableTop
     * @attr ref android.R.styleable#TextView_drawableEnd
     * @attr ref android.R.styleable#TextView_drawableBottom
     */
    vector<shared_ptr<Drawable>> getCompoundDrawablesRelative();
    
    /**
     * Sets the size of the padding between the compound drawables and
     * the text.
     *
     * @attr ref android.R.styleable#TextView_drawablePadding
     */
    void setCompoundDrawablePadding(int pad);
    
    /**
     * Returns the padding between the compound drawables and the text.
     *
     * @attr ref android.R.styleable#TextView_drawablePadding
     */
    int getCompoundDrawablePadding();
    
    virtual void setPadding(int left, int top, int right, int bottom);
    
    virtual void setPaddingRelative(int start, int top, int end, int bottom);
    
    /**
     * Gets the autolink mask of the text.  See {@link
     * android.text.util.Linkify#ALL Linkify.ALL} and peers for
     * possible values.
     *
     * @attr ref android.R.styleable#TextView_autoLink
     */
    const int getAutoLinkMask() {
        return mAutoLinkMask;
    }
    
    /**
     * Sets the text color, size, style, hint color, and highlight color
     * from the specified TextAppearance resource.
     */
    void setTextAppearance(Context *context, int resid);
    
//    /**
//     * Get the default {@link Locale} of the text in this TextView.
//     * @return the default {@link Locale} of the text in this TextView.
//     */
//    Locale *getTextLocale() {
//        return mTextPaint->getTextLocale();
//    }
    
//    /**
//     * Set the default {@link Locale} of the text in this TextView to the given value. This value
//     * is used to choose appropriate typefaces for ambiguous characters. Typically used for CJK
//     * locales to disambiguate Hanzi/Kanji/Hanja characters.
//     *
//     * @param locale the {@link Locale} for drawing text, must not be NULL.
//     *
//     * @see Paint#setTextLocale
//     */
//    void setTextLocale(Locale *locale) {
//        mTextPaint->setTextLocale(locale);
//    }
    
    /**
     * @return the size (in pixels) of the default text size in this TextView.
     */
    float getTextSize() {
        return mTextPaint->getTextSize();
    }
    
    /**
     * Set the default text size to the given value, interpreted as "scaled
     * pixel" units.  This size is adjusted based on the current density and
     * user font size preference.
     *
     * @param size The scaled pixel size.
     *
     * @attr ref android.R.styleable#TextView_textSize
     */
    void setTextSize(float size);
    
    /**
     * Set the default text size to a given unit and value.  See {@link
     * TypedValue} for the possible dimension units.
     *
     * @param unit The desired dimension unit.
     * @param size The desired size in the given units.
     *
     * @attr ref android.R.styleable#TextView_textSize
     */
    void setTextSize(int unit, float size);
    
private:
    
    void setRawTextSize(float size);
    
public:
    
    /**
     * @return the extent by which text is currently being stretched
     * horizontally.  This will usually be 1.
     */
    float getTextScaleX() {
        return mTextPaint->getTextScaleX();
    }
    
    /**
     * Sets the extent by which text should be stretched horizontally.
     *
     * @attr ref android.R.styleable#TextView_textScaleX
     */
    void setTextScaleX(float size);
    
    /**
     * Sets the typeface and style in which the text should be displayed.
     * Note that not all Typeface families actually have bold and italic
     * variants, so you may need to use
     * {@link #setTypeface(Typeface, int)} to get the appearance
     * that you actually want.
     *
     * @see #getTypeface()
     *
     * @attr ref android.R.styleable#TextView_fontFamily
     * @attr ref android.R.styleable#TextView_typeface
     * @attr ref android.R.styleable#TextView_textStyle
     */
    void setTypeface(Typeface *tf);
    
    /**
     * @return the current typeface and style in which the text is being
     * displayed.
     *
     * @see #setTypeface(Typeface)
     *
     * @attr ref android.R.styleable#TextView_fontFamily
     * @attr ref android.R.styleable#TextView_typeface
     * @attr ref android.R.styleable#TextView_textStyle
     */
    Typeface *getTypeface() {
        return mTextPaint->getTypeface();
    }
    
    /**
     * Sets the text color for all the states (normal, selected,
     * focused) to be this color.
     *
     * @see #setTextColor(ColorStateList)
     * @see #getTextColors()
     *
     * @attr ref android.R.styleable#TextView_textColor
     */
    void setTextColor(int color);
    
    /**
     * Sets the text color.
     *
     * @see #setTextColor(int)
     * @see #getTextColors()
     * @see #setHintTextColor(ColorStateList)
     * @see #setLinkTextColor(ColorStateList)
     *
     * @attr ref android.R.styleable#TextView_textColor
     */
    void setTextColor(shared_ptr<ColorStateList> colors);
    
    /**
     * Gets the text colors for the different states (normal, selected, focused) of the TextView.
     *
     * @see #setTextColor(ColorStateList)
     * @see #setTextColor(int)
     *
     * @attr ref android.R.styleable#TextView_textColor
     */
    shared_ptr<ColorStateList> getTextColors() {
        return mTextColor;
    }
    
    /**
     * <p>Return the current color selected for normal text.</p>
     *
     * @return Returns the current text color.
     */
    int getCurrentTextColor() {
        return mCurTextColor;
    }
    
    /**
     * Sets the color used to display the selection highlight.
     *
     * @attr ref android.R.styleable#TextView_textColorHighlight
     */
    void setHighlightColor(int color);
    
    /**
     * @return the color used to display the selection highlight
     *
     * @see #setHighlightColor(int)
     *
     * @attr ref android.R.styleable#TextView_textColorHighlight
     */
    int getHighlightColor() {
        return mHighlightColor;
    }
    
    /**
     * Sets whether the soft input method will be made visible when this
     * TextView gets focused. The default is true.
     * @hide
     */
    void setShowSoftInputOnFocus(bool show);
    
    /**
     * Returns whether the soft input method will be made visible when this
     * TextView gets focused. The default is true.
     * @hide
     */
    bool getShowSoftInputOnFocus() {
        // When there is no Editor, return default true value
        return true;//mEditor == NULL || mEditor.mShowSoftInputOnFocus;
    }
    
    /**
     * Gives the text a shadow of the specified radius and color, the specified
     * distance from its normal position.
     *
     * @attr ref android.R.styleable#TextView_shadowColor
     * @attr ref android.R.styleable#TextView_shadowDx
     * @attr ref android.R.styleable#TextView_shadowDy
     * @attr ref android.R.styleable#TextView_shadowRadius
     */
    void setShadowLayer(float radius, float dx, float dy, int color);
    
    /**
     * Gets the radius of the shadow layer.
     *
     * @return the radius of the shadow layer. If 0, the shadow layer is not visible
     *
     * @see #setShadowLayer(float, float, float, int)
     *
     * @attr ref android.R.styleable#TextView_shadowRadius
     */
    float getShadowRadius() {
        return mShadowRadius;
    }
    
    /**
     * @return the horizontal offset of the shadow layer
     *
     * @see #setShadowLayer(float, float, float, int)
     *
     * @attr ref android.R.styleable#TextView_shadowDx
     */
    float getShadowDx() {
        return mShadowDx;
    }
    
    /**
     * @return the vertical offset of the shadow layer
     *
     * @see #setShadowLayer(float, float, float, int)
     *
     * @attr ref android.R.styleable#TextView_shadowDy
     */
    float getShadowDy() {
        return mShadowDy;
    }
    
    /**
     * @return the color of the shadow layer
     *
     * @see #setShadowLayer(float, float, float, int)
     *
     * @attr ref android.R.styleable#TextView_shadowColor
     */
    int getShadowColor() {
        return mTextPaint->shadowColor;
    }
    
    /**
     * @return the base paint used for the text.  Please use this only to
     * consult the Paint's properties and not to change them.
     */
    shared_ptr<TextPaint> getPaint() {
        return mTextPaint;
    }
    
    /**
     * Sets the autolink mask of the text.  See {@link
     * android.text.util.Linkify#ALL Linkify.ALL} and peers for
     * possible values.
     *
     * @attr ref android.R.styleable#TextView_autoLink
     */
    void setAutoLinkMask(int mask) {
        mAutoLinkMask = mask;
    }
    
    /**
     * Sets whether the movement method will automatically be set to
     * {@link LinkMovementMethod} if {@link #setAutoLinkMask} has been
     * set to nonzero and links are detected in {@link #setText}.
     * The default is true.
     *
     * @attr ref android.R.styleable#TextView_linksClickable
     */
    void setLinksClickable(bool whether) {
        mLinksClickable = whether;
    }
    
    /**
     * Returns whether the movement method will automatically be set to
     * {@link LinkMovementMethod} if {@link #setAutoLinkMask} has been
     * set to nonzero and links are detected in {@link #setText}.
     * The default is true.
     *
     * @attr ref android.R.styleable#TextView_linksClickable
     */
    bool getLinksClickable() {
        return mLinksClickable;
    }
    
    /**
     * Returns the list of URLSpans attached to the text
     * (by {@link Linkify} or otherwise) if any.  You can call
     * {@link URLSpan#getURL} on them to find where they link to
     * or use {@link Spanned#getSpanStart} and {@link Spanned#getSpanEnd}
     * to find the region of the text they are attached to.
     */
//    URLSpan[] getUrls() {
//        if (mText instanceof Spanned) {
//            return ((Spanned) mText).getSpans(0, mText.length(), URLSpan.class);
//        } else {
//            return new URLSpan[0];
//        }
//    }
    
    /**
     * Sets the color of the hint text for all the states (disabled, focussed, selected...) of this
     * TextView.
     *
     * @see #setHintTextColor(ColorStateList)
     * @see #getHintTextColors()
     * @see #setTextColor(int)
     *
     * @attr ref android.R.styleable#TextView_textColorHint
     */
    void setHintTextColor(int color);
    
    /**
     * Sets the color of the hint text.
     *
     * @see #getHintTextColors()
     * @see #setHintTextColor(int)
     * @see #setTextColor(ColorStateList)
     * @see #setLinkTextColor(ColorStateList)
     *
     * @attr ref android.R.styleable#TextView_textColorHint
     */
    void setHintTextColor(shared_ptr<ColorStateList> colors);
    
    /**
     * @return the color of the hint text, for the different states of this TextView.
     *
     * @see #setHintTextColor(ColorStateList)
     * @see #setHintTextColor(int)
     * @see #setTextColor(ColorStateList)
     * @see #setLinkTextColor(ColorStateList)
     *
     * @attr ref android.R.styleable#TextView_textColorHint
     */
    shared_ptr<ColorStateList> getHintTextColors() {
        return mHintTextColor;
    }
    
    /**
     * <p>Return the current color selected to paint the hint text.</p>
     *
     * @return Returns the current hint text color.
     */
    int getCurrentHintTextColor() {
        return mHintTextColor != NULL ? mCurHintTextColor : mCurTextColor;
    }
    
    /**
     * Sets the color of links in the text.
     *
     * @see #setLinkTextColor(ColorStateList)
     * @see #getLinkTextColors()
     *
     * @attr ref android.R.styleable#TextView_textColorLink
     */
    void setLinkTextColor(int color);
    
    /**
     * Sets the color of links in the text.
     *
     * @see #setLinkTextColor(int)
     * @see #getLinkTextColors()
     * @see #setTextColor(ColorStateList)
     * @see #setHintTextColor(ColorStateList)
     *
     * @attr ref android.R.styleable#TextView_textColorLink
     */
    void setLinkTextColor(shared_ptr<ColorStateList> colors);
    
    /**
     * @return the list of colors used to paint the links in the text, for the different states of
     * this TextView
     *
     * @see #setLinkTextColor(ColorStateList)
     * @see #setLinkTextColor(int)
     *
     * @attr ref android.R.styleable#TextView_textColorLink
     */
    shared_ptr<ColorStateList> getLinkTextColors() {
        return mLinkTextColor;
    }
    
    /**
     * Sets the horizontal alignment of the text and the
     * vertical gravity that will be used when there is extra space
     * in the TextView beyond what is required for the text itself.
     *
     * @see android.view.Gravity
     * @attr ref android.R.styleable#TextView_gravity
     */
    void setGravity(int gravity);
    
    /**
     * Returns the horizontal and vertical alignment of this TextView.
     *
     * @see android.view.Gravity
     * @attr ref android.R.styleable#TextView_gravity
     */
    int getGravity() {
        return mGravity;
    }
    
    /**
     * @return the flags on the Paint being used to display the text.
     * @see Paint#getFlags
     */
    int getPaintFlags() {
        return mTextPaint->getFlags();
    }
    
    /**
     * Sets flags on the Paint being used to display the text and
     * reflows the text if they are different from the old flags.
     * @see Paint#setFlags
     */
    void setPaintFlags(int flags);
    
    /**
     * Sets whether the text should be allowed to be wider than the
     * View is.  If false, it will be wrapped to the width of the View.
     *
     * @attr ref android.R.styleable#TextView_scrollHorizontally
     */
    void setHorizontallyScrolling(bool whether);
    
    /**
     * Returns whether the text is allowed to be wider than the View is.
     * If false, the text will be wrapped to the width of the View.
     *
     * @attr ref android.R.styleable#TextView_scrollHorizontally
     * @hide
     */
    bool getHorizontallyScrolling() {
        return mHorizontallyScrolling;
    }
    
    /**
     * Makes the TextView at least this many lines tall.
     *
     * Setting this value overrides any other (minimum) height setting. A single line TextView will
     * set this value to 1.
     *
     * @see #getMinLines()
     *
     * @attr ref android.R.styleable#TextView_minLines
     */
    void setMinLines(int minlines);
    
    /**
     * @return the minimum number of lines displayed in this TextView, or -1 if the minimum
     * height was set in pixels instead using {@link #setMinHeight(int) or #setHeight(int)}.
     *
     * @see #setMinLines(int)
     *
     * @attr ref android.R.styleable#TextView_minLines
     */
    int getMinLines() {
        return mMinMode == LINES ? mMinimum : -1;
    }
    
    /**
     * Makes the TextView at least this many pixels tall.
     *
     * Setting this value overrides any other (minimum) number of lines setting.
     *
     * @attr ref android.R.styleable#TextView_minHeight
     */
    void setMinHeight(int minHeight);
    
    /**
     * @return the minimum height of this TextView expressed in pixels, or -1 if the minimum
     * height was set in number of lines instead using {@link #setMinLines(int) or #setLines(int)}.
     *
     * @see #setMinHeight(int)
     *
     * @attr ref android.R.styleable#TextView_minHeight
     */
    int getMinHeight() {
        return mMinMode == PIXELS ? mMinimum : -1;
    }
    
    /**
     * Makes the TextView at most this many lines tall.
     *
     * Setting this value overrides any other (maximum) height setting.
     *
     * @attr ref android.R.styleable#TextView_maxLines
     */
    void setMaxLines(int maxlines);
    
    /**
     * @return the maximum number of lines displayed in this TextView, or -1 if the maximum
     * height was set in pixels instead using {@link #setMaxHeight(int) or #setHeight(int)}.
     *
     * @see #setMaxLines(int)
     *
     * @attr ref android.R.styleable#TextView_maxLines
     */
    int getMaxLines() {
        return mMaxMode == LINES ? mMaximum : -1;
    }
    
    /**
     * Makes the TextView at most this many pixels tall.  This option is mutually exclusive with the
     * {@link #setMaxLines(int)} method.
     *
     * Setting this value overrides any other (maximum) number of lines setting.
     *
     * @attr ref android.R.styleable#TextView_maxHeight
     */
    void setMaxHeight(int maxHeight);
    
    /**
     * @return the maximum height of this TextView expressed in pixels, or -1 if the maximum
     * height was set in number of lines instead using {@link #setMaxLines(int) or #setLines(int)}.
     *
     * @see #setMaxHeight(int)
     *
     * @attr ref android.R.styleable#TextView_maxHeight
     */
    int getMaxHeight() {
        return mMaxMode == PIXELS ? mMaximum : -1;
    }
    
    /**
     * Makes the TextView exactly this many lines tall.
     *
     * Note that setting this value overrides any other (minimum / maximum) number of lines or
     * height setting. A single line TextView will set this value to 1.
     *
     * @attr ref android.R.styleable#TextView_lines
     */
    void setLines(int lines);
    
    /**
     * Makes the TextView exactly this many pixels tall.
     * You could do the same thing by specifying this number in the
     * LayoutParams.
     *
     * Note that setting this value overrides any other (minimum / maximum) number of lines or
     * height setting.
     *
     * @attr ref android.R.styleable#TextView_height
     */
    void setHeight(int pixels);
    
    /**
     * Makes the TextView at least this many ems wide
     *
     * @attr ref android.R.styleable#TextView_minEms
     */
    void setMinEms(int minems);
    
    /**
     * @return the minimum width of the TextView, expressed in ems or -1 if the minimum width
     * was set in pixels instead (using {@link #setMinWidth(int)} or {@link #setWidth(int)}).
     *
     * @see #setMinEms(int)
     * @see #setEms(int)
     *
     * @attr ref android.R.styleable#TextView_minEms
     */
    int getMinEms() {
        return mMinWidthMode == EMS ? mMinWidth : -1;
    }
    
    /**
     * Makes the TextView at least this many pixels wide
     *
     * @attr ref android.R.styleable#TextView_minWidth
     */
    void setMinWidth(int minpixels);
    
    /**
     * @return the minimum width of the TextView, in pixels or -1 if the minimum width
     * was set in ems instead (using {@link #setMinEms(int)} or {@link #setEms(int)}).
     *
     * @see #setMinWidth(int)
     * @see #setWidth(int)
     *
     * @attr ref android.R.styleable#TextView_minWidth
     */
    int getMinWidth() {
        return mMinWidthMode == PIXELS ? mMinWidth : -1;
    }
    
    /**
     * Makes the TextView at most this many ems wide
     *
     * @attr ref android.R.styleable#TextView_maxEms
     */
    void setMaxEms(int maxems);
    
    /**
     * @return the maximum width of the TextView, expressed in ems or -1 if the maximum width
     * was set in pixels instead (using {@link #setMaxWidth(int)} or {@link #setWidth(int)}).
     *
     * @see #setMaxEms(int)
     * @see #setEms(int)
     *
     * @attr ref android.R.styleable#TextView_maxEms
     */
    int getMaxEms() {
        return mMaxWidthMode == EMS ? mMaxWidth : -1;
    }
    
    /**
     * Makes the TextView at most this many pixels wide
     *
     * @attr ref android.R.styleable#TextView_maxWidth
     */
    void setMaxWidth(int maxpixels);
    
    /**
     * @return the maximum width of the TextView, in pixels or -1 if the maximum width
     * was set in ems instead (using {@link #setMaxEms(int)} or {@link #setEms(int)}).
     *
     * @see #setMaxWidth(int)
     * @see #setWidth(int)
     *
     * @attr ref android.R.styleable#TextView_maxWidth
     */
    int getMaxWidth() {
        return mMaxWidthMode == PIXELS ? mMaxWidth : -1;
    }
    
    /**
     * Makes the TextView exactly this many ems wide
     *
     * @see #setMaxEms(int)
     * @see #setMinEms(int)
     * @see #getMinEms()
     * @see #getMaxEms()
     *
     * @attr ref android.R.styleable#TextView_ems
     */
    void setEms(int ems);
    
    /**
     * Makes the TextView exactly this many pixels wide.
     * You could do the same thing by specifying this number in the
     * LayoutParams.
     *
     * @see #setMaxWidth(int)
     * @see #setMinWidth(int)
     * @see #getMinWidth()
     * @see #getMaxWidth()
     *
     * @attr ref android.R.styleable#TextView_width
     */
    void setWidth(int pixels);
    
    /**
     * Sets line spacing for this TextView.  Each line will have its height
     * multiplied by <code>mult</code> and have <code>add</code> added to it.
     *
     * @attr ref android.R.styleable#TextView_lineSpacingExtra
     * @attr ref android.R.styleable#TextView_lineSpacingMultiplier
     */
    void setLineSpacing(float add, float mult);
    
    /**
     * Gets the line spacing multiplier
     *
     * @return the value by which each line's height is multiplied to get its actual height.
     *
     * @see #setLineSpacing(float, float)
     * @see #getLineSpacingExtra()
     *
     * @attr ref android.R.styleable#TextView_lineSpacingMultiplier
     */
    float getLineSpacingMultiplier() {
        return mSpacingMult;
    }
    
    /**
     * Gets the line spacing extra space
     *
     * @return the extra space that is added to the height of each lines of this TextView.
     *
     * @see #setLineSpacing(float, float)
     * @see #getLineSpacingMultiplier()
     *
     * @attr ref android.R.styleable#TextView_lineSpacingExtra
     */
    float getLineSpacingExtra() {
        return mSpacingAdd;
    }
    
    /**
     * Convenience method: Append the specified text to the TextView's
     * display buffer, upgrading it to BufferType.EDITABLE if it was
     * not already editable.
     */
    void append(string text) {
        append(text, 0, text.length());
    }
    
    /**
     * Convenience method: Append the specified text slice to the TextView's
     * display buffer, upgrading it to BufferType.EDITABLE if it was
     * not already editable.
     */
    void append(string text, int start, int end);
    
private:
    
    void updateTextColors();
    
protected:
    
    void drawableStateChanged();
    
//    @Override
//    public Parcelable onSaveInstanceState() {
//        Parcelable superState = View::onSaveInstanceState();
//        
//        // Save state if we are forced to
//        bool save = mFreezesText;
//        int start = 0;
//        int end = 0;
//        
//        if (mText != NULL) {
//            start = getSelectionStart();
//            end = getSelectionEnd();
//            if (start >= 0 || end >= 0) {
//                // Or save state if there is a selection
//                save = true;
//            }
//        }
//        
//        if (save) {
//            SavedState ss = new SavedState(superState);
//            // XXX Should also save the current scroll position!
//            ss.selStart = start;
//            ss.selEnd = end;
//            
//            if (mText instanceof Spanned) {
//                /*
//                 * Calling setText() strips off any ChangeWatchers;
//                 * strip them now to avoid leaking references.
//                 * But do it to a copy so that if there are any
//                 * further changes to the text of this view, it
//                 * won't get into an inconsistent state.
//                 */
//                
//                Spannable sp = new SpannableString(mText);
//                
//                for (ChangeWatcher cw : sp.getSpans(0, sp.length(), ChangeWatcher.class)) {
//                    sp.removeSpan(cw);
//                }
//                
//                if (mEditor != NULL) {
//                    removeMisspelledSpans(sp);
//                    sp.removeSpan(mEditor.mSuggestionRangeSpan);
//                }
//                
//                ss.text = sp;
//            } else {
//                ss.text = mText.toString();
//            }
//            
//            if (isFocused() && start >= 0 && end >= 0) {
//                ss.frozenWithFocus = true;
//            }
//            
//            ss.error = getError();
//            
//            return ss;
//        }
//        
//        return superState;
//    }
//    
//    void removeMisspelledSpans(Spannable spannable) {
//        SuggestionSpan[] suggestionSpans = spannable.getSpans(0, spannable.length(), SuggestionSpan.class);
//        for (int i = 0; i < suggestionSpans.length; i++) {
//            int flags = suggestionSpans[i].getFlags();
//            if ((flags & SuggestionSpan.FLAG_EASY_CORRECT) != 0
//                && (flags & SuggestionSpan.FLAG_MISSPELLED) != 0) {
//                spannable.removeSpan(suggestionSpans[i]);
//            }
//        }
//    }
//    
//    @Override
//    public void onRestoreInstanceState(Parcelable state) {
//        if (!(state instanceof SavedState)) {
//            View::onRestoreInstanceState(state);
//            return;
//        }
//        
//        SavedState ss = (SavedState)state;
//        View::onRestoreInstanceState(ss.getSuperState());
//        
//        // XXX restore buffer type too, as well as lots of other stuff
//        if (ss.text != NULL) {
//            setText(ss.text);
//        }
//        
//        if (ss.selStart >= 0 && ss.selEnd >= 0) {
//            if (mText instanceof Spannable) {
//                int len = mText.length();
//                
//                if (ss.selStart > len || ss.selEnd > len) {
//                    String restored = "";
//                    
//                    if (ss.text != NULL) {
//                        restored = "(restored) ";
//                    }
//                    
//                    Log.e(LOG_TAG, "Saved cursor position " + ss.selStart +
//                          "/" + ss.selEnd + " out of range for " + restored +
//                          "text " + mText);
//                } else {
//                    Selection.setSelection((Spannable) mText, ss.selStart, ss.selEnd);
//                    
//                    if (ss.frozenWithFocus) {
//                        createEditorIfNeeded();
//                        mEditor.mFrozenWithFocus = true;
//                    }
//                }
//            }
//        }
//        
//        if (ss.error != NULL) {
//            const shared_ptr<CharSequence> error = ss.error;
//            // Display the error later, after the first layout pass
//            post(new Runnable() {
//                public void run() {
//                    setError(error);
//                }
//            });
//        }
//    }
    
public:
    
    /**
     * Control whether this text view saves its entire text contents when
     * freezing to an icicle, in addition to dynamic state such as cursor
     * position.  By default this is false, not saving the text.  Set to true
     * if the text in the text view is not being saved somewhere else in
     * persistent storage (such as in a content provider) so that if the
     * view is later thawed the user will not lose their data.
     *
     * @param freezesText Controls whether a frozen icicle should include the
     * entire text data: true to include it, false to not.
     *
     * @attr ref android.R.styleable#TextView_freezesText
     */
    void setFreezesText(bool freezesText);
    
    /**
     * Return whether this text view is including its entire text contents
     * in frozen icicles.
     *
     * @return Returns true if text is included, false if it isn't.
     *
     * @see #setFreezesText
     */
    bool getFreezesText();
    
    ///////////////////////////////////////////////////////////////////////////
    
    /**
     * Sets the Factory used to create new Editables.
     */
    void setEditableFactory(Editable::Factory factory) {
        mEditableFactory = factory;
        setText(mText);
    }
    
    /**
     * Sets the Factory used to create new Spannables.
     */
    void setSpannableFactory(Spannable::Factory factory) {
        mSpannableFactory = factory;
        setText(mText);
    }
    
    /**
     * Sets the string value of the TextView. TextView <em>does not</em> accept
     * HTML-like formatting, which you can do with text strings in XML resource files.
     * To style your strings, attach android.text.style.* objects to a
     * {@link android.text.SpannableString SpannableString}, or see the
     * <a href="{@docRoot}guide/topics/resources/available-resources.html#stringresources">
     * Available Resource Types</a> documentation for an example of setting
     * formatted text in the XML resource file.
     *
     * @attr ref android.R.styleable#TextView_text
     */
    void setText(shared_ptr<CharSequence> text);
    
//    /**
//     * Like {@link #setText(shared_ptr<CharSequence>)},
//     * except that the cursor position (if any) is retained in the new text.
//     *
//     * @param text The new text to place in the text view.
//     *
//     * @see #setText(shared_ptr<CharSequence>)
//     */
//    public const void setTextKeepState(shared_ptr<CharSequence> text) {
//        setTextKeepState(text, mBufferType);
//    }
    
    /**
     * Sets the text that this TextView is to display (see
     * {@link #setText(shared_ptr<CharSequence>)}) and also sets whether it is stored
     * in a styleable/spannable buffer and whether it is editable.
     *
     * @attr ref android.R.styleable#TextView_text
     * @attr ref android.R.styleable#TextView_bufferType
     */
    void setText(shared_ptr<CharSequence> text, BufferType type);
    
private:
    
    void setText(shared_ptr<CharSequence> text, BufferType type,
                         bool notifyBefore, int oldlen);
    
public:
    
    /**
     * Sets the TextView to display the specified slice of the specified
     * char array.  You must promise that you will not change the contents
     * of the array except for right before another call to setText(),
     * since the TextView has no way to know that the text
     * has changed and that it needs to invalidate and re-layout.
     */
    void setText(UnicodeString text, int start, int len);
    
    /**
     * Like {@link #setText(shared_ptr<CharSequence>, android.widget.TextView.BufferType)},
     * except that the cursor position (if any) is retained in the new text.
     *
     * @see #setText(shared_ptr<CharSequence>, android.widget.TextView.BufferType)
     */
    void setTextKeepState(shared_ptr<CharSequence> text, BufferType type);
    
    void setText(string resid);
    
    void setText(string resid, BufferType type);
    
    /**
     * Sets the text to be displayed when the text of the TextView is empty.
     * Null means to use the normal empty text. The hint does not currently
     * participate in determining the size of the view.
     *
     * @attr ref android.R.styleable#TextView_hint
     */
    void setHint(shared_ptr<CharSequence> hint);
    
    /**
     * Sets the text to be displayed when the text of the TextView is empty,
     * from a resource.
     *
     * @attr ref android.R.styleable#TextView_hint
     */
    void setHint(string resid);
    
    /**
     * Returns the hint that is displayed when the text of the TextView
     * is empty.
     *
     * @attr ref android.R.styleable#TextView_hint
     */
    shared_ptr<CharSequence> getHint();
    
    bool isSingleLine();
    
private:
    
    static bool isMultilineInputType(int type);
    
    /**
     * Removes the suggestion spans.
     */
    shared_ptr<CharSequence> removeSuggestionSpans(shared_ptr<CharSequence> text);
    
public:
    
    /**
     * Set the type of the content with a constant as defined for {@link EditorInfo#inputType}. This
     * will take care of changing the key listener, by calling {@link #setKeyListener(KeyListener)},
     * to match the given content type.  If the given content type is {@link EditorInfo#TYPE_NULL}
     * then a soft keyboard will not be displayed for this text view.
     *
     * Note that the maximum number of displayed lines (see {@link #setMaxLines(int)}) will be
     * modified if you change the {@link EditorInfo#TYPE_TEXT_FLAG_MULTI_LINE} flag of the input
     * type.
     *
     * @see #getInputType()
     * @see #setRawInputType(int)
     * @see android.text.InputType
     * @attr ref android.R.styleable#TextView_inputType
     */
    void setInputType(int type);
    
private:
    
    /**
     * It would be better to rely on the input type for everything. A password inputType should have
     * a password transformation. We should hence use isPasswordInputType instead of this method.
     *
     * We should:
     * - Call setInputType in setKeyListener instead of changing the input type directly (which
     * would install the correct transformation).
     * - Refuse the installation of a non-password transformation in setTransformation if the input
     * type is password.
     *
     * However, this is like this for legacy reasons and we cannot break existing apps. This method
     * is useful since it matches what the user can see (obfuscated text or not).
     *
     * @return true if the current transformation method is of the password type.
     */
    bool hasPasswordTransformationMethod();
    
    static bool isPasswordInputType(int inputType);
    
    static bool isVisiblePasswordInputType(int inputType);
    
public:
    
    /**
     * Directly change the content type integer of the text view, without
     * modifying any other state.
     * @see #setInputType(int)
     * @see android.text.InputType
     * @attr ref android.R.styleable#TextView_inputType
     */
    void setRawInputType(int type);
    
private:
    
    void setInputType(int type, bool direct);
    
public:
    
//    /**
//     * Get the type of the editable content.
//     *
//     * @see #setInputType(int)
//     * @see android.text.InputType
//     */
//    int getInputType() {
//        return mEditor == NULL ? EditorInfo.TYPE_NULL : mEditor.mInputType;
//    }
//    
//    /**
//     * Change the editor type integer associated with the text view, which
//     * will be reported to an IME with {@link EditorInfo#imeOptions} when it
//     * has focus.
//     * @see #getImeOptions
//     * @see android.view.inputmethod.EditorInfo
//     * @attr ref android.R.styleable#TextView_imeOptions
//     */
//    void setImeOptions(int imeOptions) {
//        createEditorIfNeeded();
//        mEditor.createInputContentTypeIfNeeded();
//        mEditor.mInputContentType.imeOptions = imeOptions;
//    }
//    
//    /**
//     * Get the type of the IME editor.
//     *
//     * @see #setImeOptions(int)
//     * @see android.view.inputmethod.EditorInfo
//     */
//    int getImeOptions() {
//        return mEditor != NULL && mEditor.mInputContentType != NULL
//        ? mEditor.mInputContentType.imeOptions : EditorInfo.IME_NULL;
//    }
//    
//    /**
//     * Change the custom IME action associated with the text view, which
//     * will be reported to an IME with {@link EditorInfo#actionLabel}
//     * and {@link EditorInfo#actionId} when it has focus.
//     * @see #getImeActionLabel
//     * @see #getImeActionId
//     * @see android.view.inputmethod.EditorInfo
//     * @attr ref android.R.styleable#TextView_imeActionLabel
//     * @attr ref android.R.styleable#TextView_imeActionId
//     */
//    void setImeActionLabel(shared_ptr<CharSequence> label, int actionId) {
//        createEditorIfNeeded();
//        mEditor.createInputContentTypeIfNeeded();
//        mEditor.mInputContentType.imeActionLabel = label;
//        mEditor.mInputContentType.imeActionId = actionId;
//    }
//    
//    /**
//     * Get the IME action label previous set with {@link #setImeActionLabel}.
//     *
//     * @see #setImeActionLabel
//     * @see android.view.inputmethod.EditorInfo
//     */
//    public shared_ptr<CharSequence> getImeActionLabel() {
//        return mEditor != NULL && mEditor.mInputContentType != NULL
//        ? mEditor.mInputContentType.imeActionLabel : NULL;
//    }
//    
//    /**
//     * Get the IME action ID previous set with {@link #setImeActionLabel}.
//     *
//     * @see #setImeActionLabel
//     * @see android.view.inputmethod.EditorInfo
//     */
//    public int getImeActionId() {
//        return mEditor != NULL && mEditor.mInputContentType != NULL
//        ? mEditor.mInputContentType.imeActionId : 0;
//    }
//    
//    /**
//     * Set a special listener to be called when an action is performed
//     * on the text view.  This will be called when the enter key is pressed,
//     * or when an action supplied to the IME is selected by the user.  Setting
//     * this means that the normal hard key event will not insert a newline
//     * into the text view, even if it is multi-line; holding down the ALT
//     * modifier will, however, allow the user to insert a newline character.
//     */
//    public void setOnEditorActionListener(OnEditorActionListener l) {
//        createEditorIfNeeded();
//        mEditor.createInputContentTypeIfNeeded();
//        mEditor.mInputContentType.onEditorActionListener = l;
//    }
//    
//    /**
//     * Called when an attached input method calls
//     * {@link InputConnection#performEditorAction(int)
//     * InputConnection.performEditorAction()}
//     * for this text view.  The default implementation will call your action
//     * listener supplied to {@link #setOnEditorActionListener}, or perform
//     * a standard operation for {@link EditorInfo#IME_ACTION_NEXT
//     * EditorInfo.IME_ACTION_NEXT}, {@link EditorInfo#IME_ACTION_PREVIOUS
//     * EditorInfo.IME_ACTION_PREVIOUS}, or {@link EditorInfo#IME_ACTION_DONE
//     * EditorInfo.IME_ACTION_DONE}.
//     *
//     * <p>For backwards compatibility, if no IME options have been set and the
//     * text view would not normally advance focus on enter, then
//     * the NEXT and DONE actions received here will be turned into an enter
//     * key down/up pair to go through the normal key handling.
//     *
//     * @param actionCode The code of the action being performed.
//     *
//     * @see #setOnEditorActionListener
//     */
//    public void onEditorAction(int actionCode) {
//        const Editor.InputContentType ict = mEditor == NULL ? NULL : mEditor.mInputContentType;
//        if (ict != NULL) {
//            if (ict.onEditorActionListener != NULL) {
//                if (ict.onEditorActionListener.onEditorAction(this,
//                                                              actionCode, NULL)) {
//                    return;
//                }
//            }
//            
//            // This is the handling for some default action.
//            // Note that for backwards compatibility we don't do this
//            // default handling if explicit ime options have not been given,
//            // instead turning this into the normal enter key codes that an
//            // app may be expecting.
//            if (actionCode == EditorInfo.IME_ACTION_NEXT) {
//                View v = focusSearch(FOCUS_FORWARD);
//                if (v != NULL) {
//                    if (!v.requestFocus(FOCUS_FORWARD)) {
//                        throw new IllegalStateException("focus search returned a view " +
//                                                        "that wasn't able to take focus!");
//                    }
//                }
//                return;
//                
//            } else if (actionCode == EditorInfo.IME_ACTION_PREVIOUS) {
//                View v = focusSearch(FOCUS_BACKWARD);
//                if (v != NULL) {
//                    if (!v.requestFocus(FOCUS_BACKWARD)) {
//                        throw new IllegalStateException("focus search returned a view " +
//                                                        "that wasn't able to take focus!");
//                    }
//                }
//                return;
//                
//            } else if (actionCode == EditorInfo.IME_ACTION_DONE) {
//                InputMethodManager imm = InputMethodManager.peekInstance();
//                if (imm != NULL && imm.isActive(this)) {
//                    imm.hideSoftInputFromWindow(getWindowToken(), 0);
//                }
//                return;
//            }
//        }
//        
//        ViewRootImpl viewRootImpl = getViewRootImpl();
//        if (viewRootImpl != NULL) {
//            long eventTime = SystemClock.uptimeMillis();
//            viewRootImpl.dispatchKeyFromIme(
//                                            new KeyEvent(eventTime, eventTime,
//                                                         KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_ENTER, 0, 0,
//                                                         KeyCharacterMap.VIRTUAL_KEYBOARD, 0,
//                                                         KeyEvent.FLAG_SOFT_KEYBOARD | KeyEvent.FLAG_KEEP_TOUCH_MODE
//                                                         | KeyEvent.FLAG_EDITOR_ACTION));
//            viewRootImpl.dispatchKeyFromIme(
//                                            new KeyEvent(SystemClock.uptimeMillis(), eventTime,
//                                                         KeyEvent.ACTION_UP, KeyEvent.KEYCODE_ENTER, 0, 0,
//                                                         KeyCharacterMap.VIRTUAL_KEYBOARD, 0,
//                                                         KeyEvent.FLAG_SOFT_KEYBOARD | KeyEvent.FLAG_KEEP_TOUCH_MODE
//                                                         | KeyEvent.FLAG_EDITOR_ACTION));
//        }
//    }
//    
//    /**
//     * Set the content type of the text, which is the
//     * {@link EditorInfo#privateImeOptions EditorInfo.privateImeOptions}
//     * field that will be filled in when creating an input connection.
//     *
//     * @see #getPrivateImeOptions()
//     * @see EditorInfo#privateImeOptions
//     * @attr ref android.R.styleable#TextView_privateImeOptions
//     */
//    public void setPrivateImeOptions(String type) {
//        createEditorIfNeeded();
//        mEditor.createInputContentTypeIfNeeded();
//        mEditor.mInputContentType.privateImeOptions = type;
//    }
//    
//    /**
//     * Get the type of the content.
//     *
//     * @see #setPrivateImeOptions(String)
//     * @see EditorInfo#privateImeOptions
//     */
//    public String getPrivateImeOptions() {
//        return mEditor != NULL && mEditor.mInputContentType != NULL
//        ? mEditor.mInputContentType.privateImeOptions : NULL;
//    }
//    
//    /**
//     * Set the extra input data of the text, which is the
//     * {@link EditorInfo#extras TextBoxAttribute.extras}
//     * Bundle that will be filled in when creating an input connection.  The
//     * given integer is the resource ID of an XML resource holding an
//     * {@link android.R.styleable#InputExtras &lt;input-extras&gt;} XML tree.
//     *
//     * @see #getInputExtras(bool)
//     * @see EditorInfo#extras
//     * @attr ref android.R.styleable#TextView_editorExtras
//     */
//    public void setInputExtras(int xmlResId) throws XmlPullParserException, IOException {
//        createEditorIfNeeded();
//        XmlResourceParser parser = getResources().getXml(xmlResId);
//        mEditor.createInputContentTypeIfNeeded();
//        mEditor.mInputContentType.extras = new Bundle();
//        getResources().parseBundleExtras(parser, mEditor.mInputContentType.extras);
//    }
//    
//    /**
//     * Retrieve the input extras currently associated with the text view, which
//     * can be viewed as well as modified.
//     *
//     * @param create If true, the extras will be created if they don't already
//     * exist.  Otherwise, NULL will be returned if none have been created.
//     * @see #setInputExtras(int)
//     * @see EditorInfo#extras
//     * @attr ref android.R.styleable#TextView_editorExtras
//     */
//    public Bundle getInputExtras(bool create) {
//        if (mEditor == NULL && !create) return NULL;
//        createEditorIfNeeded();
//        if (mEditor.mInputContentType == NULL) {
//            if (!create) return NULL;
//            mEditor.createInputContentTypeIfNeeded();
//        }
//        if (mEditor.mInputContentType.extras == NULL) {
//            if (!create) return NULL;
//            mEditor.mInputContentType.extras = new Bundle();
//        }
//        return mEditor.mInputContentType.extras;
//    }
//    
//    /**
//     * Returns the error message that was set to be displayed with
//     * {@link #setError}, or <code>NULL</code> if no error was set
//     * or if it the error was cleared by the widget after user input.
//     */
//    shared_ptr<CharSequence> getError() {
//        return mEditor == NULL ? NULL : mEditor.mError;
//    }
    
    /**
     * Sets the right-hand compound drawable of the TextView to the "error"
     * icon and sets an error message that will be displayed in a popup when
     * the TextView has focus.  The icon and error message will be reset to
     * NULL when any key events cause changes to the TextView's text.  If the
     * <code>error</code> is <code>NULL</code>, the error message and icon
     * will be cleared.
     */
    void setError(shared_ptr<CharSequence> error);
//    
//    /**
//     * Sets the right-hand compound drawable of the TextView to the specified
//     * icon and sets an error message that will be displayed in a popup when
//     * the TextView has focus.  The icon and error message will be reset to
//     * NULL when any key events cause changes to the TextView's text.  The
//     * drawable must already have had {@link Drawable#setBounds} set on it.
//     * If the <code>error</code> is <code>NULL</code>, the error message will
//     * be cleared (and you should provide a <code>NULL</code> icon as well).
//     */
//    void setError(shared_ptr<CharSequence> error, Drawable icon) {
//        createEditorIfNeeded();
//        mEditor.setError(error, icon);
//    }
    
protected:
    
    virtual bool setFrame(int l, int t, int r, int b);
    
private:
    
    void restartMarqueeIfNeeded();
    
public:
    
    /**
     * Sets the list of input filters that will be used if the buffer is
     * Editable. Has no effect otherwise.
     *
     * @attr ref android.R.styleable#TextView_maxLength
     */
    void setFilters(vector<shared_ptr<InputFilter>> filters);
    
private:
    
    /**
     * Sets the list of input filters on the specified Editable,
     * and includes mInput in the list if it is an InputFilter.
     */
    void setFilters(shared_ptr<Editable> e, vector<shared_ptr<InputFilter>> filters);
    
public:
    
    /**
     * Returns the current list of input filters.
     *
     * @attr ref android.R.styleable#TextView_maxLength
     */
    vector<shared_ptr<InputFilter>> getFilters();
    
private:
    
    int getBoxHeight(shared_ptr<Layout> l);
    
    int getVerticalOffset(bool forceNormal);
    
    int getBottomVerticalOffset(bool forceNormal);
    
    void invalidateCursorPath();
    
    void invalidateCursor();
    
    void invalidateCursor(int a, int b, int c);
    
    /**
     * Invalidates the region of text enclosed between the start and end text offsets.
     */
    void invalidateRegion(int start, int end, bool invalidateCursor);
    
    void registerForPreDraw();
    
public:
    
    /**
     * {@inheritDoc}
     */
    bool onPreDraw();
    
protected:
    
    virtual void onAttachedToWindow();
    
    virtual void onDetachedFromWindow();
    
public:
    
    virtual void onScreenStateChanged(int screenState);
    
protected:
    
    virtual bool isPaddingOffsetRequired();
    
    virtual int getLeftPaddingOffset();
    
    virtual int getTopPaddingOffset();
    
    virtual int getBottomPaddingOffset();
    
    virtual int getRightPaddingOffset();
    
    virtual bool verifyDrawable(shared_ptr<Drawable> who);
    
public:
    
    virtual void jumpDrawablesToCurrentState();
    
    virtual void invalidateDrawable(shared_ptr<Drawable> drawable);
    
    virtual bool hasOverlappingRendering();
    
    /**
     *
     * Returns the state of the {@code textIsSelectable} flag (See
     * {@link #setTextIsSelectable setTextIsSelectable()}). Although you have to set this flag
     * to allow users to select and copy text in a non-editable TextView, the content of an
     * {@link EditText} can always be selected, independently of the value of this flag.
     * <p>
     *
     * @return True if the text displayed in this TextView can be selected by the user.
     *
     * @attr ref android.R.styleable#TextView_textIsSelectable
     */
    bool isTextSelectable();
    
    /**
     * Sets whether the content of this view is selectable by the user. The default is
     * {@code false}, meaning that the content is not selectable.
     * <p>
     * When you use a TextView to display a useful piece of information to the user (such as a
     * contact's address), make it selectable, so that the user can select and copy its
     * content. You can also use set the XML attribute
     * {@link android.R.styleable#TextView_textIsSelectable} to "true".
     * <p>
     * When you call this method to set the value of {@code textIsSelectable}, it sets
     * the flags {@code focusable}, {@code focusableInTouchMode}, {@code clickable},
     * and {@code longClickable} to the same value. These flags correspond to the attributes
     * {@link android.R.styleable#View_focusable android:focusable},
     * {@link android.R.styleable#View_focusableInTouchMode android:focusableInTouchMode},
     * {@link android.R.styleable#View_clickable android:clickable}, and
     * {@link android.R.styleable#View_longClickable android:longClickable}. To restore any of these
     * flags to a state you had set previously, call one or more of the following methods:
     * {@link #setFocusable(bool) setFocusable()},
     * {@link #setFocusableInTouchMode(bool) setFocusableInTouchMode()},
     * {@link #setClickable(bool) setClickable()} or
     * {@link #setLongClickable(bool) setLongClickable()}.
     *
     * @param selectable Whether the content of this TextView should be selectable.
     */
    void setTextIsSelectable(bool selectable);
    
protected:
    
    virtual vector<int> onCreateDrawableState(int extraSpace);
    
private:
    
    shared_ptr<Path> getUpdatedHighlightPath();
    
public:
    
    /**
     * @hide
     */
    int getHorizontalOffsetForDrawables();
    
protected:
    
    virtual void onDraw(shared_ptr<Canvas> canvas);
    
public:
    
    virtual void getFocusedRect(Rect r);
    
    /**
     * Return the number of lines of text, or 0 if the internal Layout has not
     * been built.
     */
    int getLineCount();
    
    /**
     * Return the baseline for the specified line (0...getLineCount() - 1)
     * If bounds is not NULL, return the top, left, right, bottom extents
     * of the specified line in it. If the internal Layout has not been built,
     * return 0 and set bounds to (0, 0, 0, 0)
     * @param line which line to examine (0..getLineCount() - 1)
     * @param bounds Optional. If not NULL, it returns the extent of the line
     * @return the Y-coordinate of the baseline
     */
    int getLineBounds(int line, Rect bounds);
    
    virtual int getBaseline();
    
protected:
    
    /**
     * @hide
     */
    virtual int getFadeTop(bool offsetRequired);
    
    /**
     * @hide
     */
    virtual int getFadeHeight(bool offsetRequired);
    
public:
    
    virtual bool onKeyPreIme(int keyCode, KeyEvent *event);
    
    virtual bool onKeyDown(int keyCode, KeyEvent *event);
    
    virtual bool onKeyMultiple(int keyCode, int repeatCount, KeyEvent *event);
    
private:
    
    /**
     * Returns true if pressing ENTER in this field advances focus instead
     * of inserting the character.  This is true mostly in single-line fields,
     * but also in mail addresses and subjects which will display on multiple
     * lines but where it doesn't make sense to insert newlines.
     */
    bool shouldAdvanceFocusOnEnter();
    
    /**
     * Returns true if pressing TAB in this field advances focus instead
     * of inserting the character.  Insert tabs only in multi-line editors.
     */
    bool shouldAdvanceFocusOnTab();
    
    int doKeyDown(int keyCode, KeyEvent *event, KeyEvent *otherEvent);
    
public:
    
    /**
     * Resets the mErrorWasChanged flag, so that future calls to {@link #setError(shared_ptr<CharSequence>)}
     * can be recorded.
     * @hide
     */
    void resetErrorChangedFlag();
    
    /**
     * @hide
     */
    void hideErrorIfUnchanged();
    
    virtual bool onKeyUp(int keyCode, KeyEvent *event);
    
//    @Override
//    bool onCheckIsTextEditor() {
//        return mEditor != NULL && mEditor.mInputType != EditorInfo.TYPE_NULL;
//    }
//    
//    @Override
//    InputConnection onCreateInputConnection(EditorInfo outAttrs) {
//        if (onCheckIsTextEditor() && isEnabled()) {
//            mEditor.createInputMethodStateIfNeeded();
//            outAttrs.inputType = getInputType();
//            if (mEditor.mInputContentType != NULL) {
//                outAttrs.imeOptions = mEditor.mInputContentType.imeOptions;
//                outAttrs.privateImeOptions = mEditor.mInputContentType.privateImeOptions;
//                outAttrs.actionLabel = mEditor.mInputContentType.imeActionLabel;
//                outAttrs.actionId = mEditor.mInputContentType.imeActionId;
//                outAttrs.extras = mEditor.mInputContentType.extras;
//            } else {
//                outAttrs.imeOptions = EditorInfo.IME_NULL;
//            }
//            if (focusSearch(FOCUS_DOWN) != NULL) {
//                outAttrs.imeOptions |= EditorInfo.IME_FLAG_NAVIGATE_NEXT;
//            }
//            if (focusSearch(FOCUS_UP) != NULL) {
//                outAttrs.imeOptions |= EditorInfo.IME_FLAG_NAVIGATE_PREVIOUS;
//            }
//            if ((outAttrs.imeOptions&EditorInfo.IME_MASK_ACTION)
//                == EditorInfo.IME_ACTION_UNSPECIFIED) {
//                if ((outAttrs.imeOptions&EditorInfo.IME_FLAG_NAVIGATE_NEXT) != 0) {
//                    // An action has not been set, but the enter key will move to
//                    // the next focus, so set the action to that.
//                    outAttrs.imeOptions |= EditorInfo.IME_ACTION_NEXT;
//                } else {
//                    // An action has not been set, and there is no focus to move
//                    // to, so let's just supply a "done" action.
//                    outAttrs.imeOptions |= EditorInfo.IME_ACTION_DONE;
//                }
//                if (!shouldAdvanceFocusOnEnter()) {
//                    outAttrs.imeOptions |= EditorInfo.IME_FLAG_NO_ENTER_ACTION;
//                }
//            }
//            if (isMultilineInputType(outAttrs.inputType)) {
//                // Multi-line text editors should always show an enter key.
//                outAttrs.imeOptions |= EditorInfo.IME_FLAG_NO_ENTER_ACTION;
//            }
//            outAttrs.hintText = mHint;
//            if (mText instanceof Editable) {
//                InputConnection ic = new EditableInputConnection(this);
//                outAttrs.initialSelStart = getSelectionStart();
//                outAttrs.initialSelEnd = getSelectionEnd();
//                outAttrs.initialCapsMode = ic.getCursorCapsMode(getInputType());
//                return ic;
//            }
//        }
//        return NULL;
//    }
//    
//    /**
//     * If this TextView contains editable content, extract a portion of it
//     * based on the information in <var>request</var> in to <var>outText</var>.
//     * @return Returns true if the text was successfully extracted, else false.
//     */
//    bool extractText(ExtractedTextRequest request, ExtractedText outText) {
//        createEditorIfNeeded();
//        return mEditor.extractText(request, outText);
//    }
//    
//    /**
//     * This is used to remove all style-impacting spans from text before new
//     * extracted text is being replaced into it, so that we don't have any
//     * lingering spans applied during the replace.
//     */
//    static void removeParcelableSpans(Spannable spannable, int start, int end) {
//        Object[] spans = spannable.getSpans(start, end, ParcelableSpan.class);
//        int i = spans.length;
//        while (i > 0) {
//            i--;
//            spannable.removeSpan(spans[i]);
//        }
//    }
//    
//    /**
//     * Apply to this text view the given extracted text, as previously
//     * returned by {@link #extractText(ExtractedTextRequest, ExtractedText)}.
//     */
//    void setExtractedText(ExtractedText text) {
//        Editable content = getEditableText();
//        if (text.text != NULL) {
//            if (content == NULL) {
//                setText(text.text, TextView.BufferType.EDITABLE);
//            } else if (text.partialStartOffset < 0) {
//                removeParcelableSpans(content, 0, content.length());
//                content.replace(0, content.length(), text.text);
//            } else {
//                const int N = content.length();
//                int start = text.partialStartOffset;
//                if (start > N) start = N;
//                int end = text.partialEndOffset;
//                if (end > N) end = N;
//                removeParcelableSpans(content, start, end);
//                content.replace(start, end, text.text);
//            }
//        }
//        
//        // Now set the selection position...  make sure it is in range, to
//        // avoid crashes.  If this is a partial update, it is possible that
//        // the underlying text may have changed, causing us problems here.
//        // Also we just don't want to trust clients to do the right thing.
//        Spannable sp = (Spannable)getText();
//        const int N = sp.length();
//        int start = text.selectionStart;
//        if (start < 0) start = 0;
//        else if (start > N) start = N;
//        int end = text.selectionEnd;
//        if (end < 0) end = 0;
//        else if (end > N) end = N;
//        Selection.setSelection(sp, start, end);
//        
//        // Finally, update the selection mode.
//        if ((text.flags&ExtractedText.FLAG_SELECTING) != 0) {
//            MetaKeyKeyListener.startSelecting(this, sp);
//        } else {
//            MetaKeyKeyListener.stopSelecting(this, sp);
//        }
//    }
//    
//    /**
//     * @hide
//     */
//    void setExtracting(ExtractedTextRequest req) {
//        if (mEditor.mInputMethodState != NULL) {
//            mEditor.mInputMethodState.mExtractedTextRequest = req;
//        }
//        // This would stop a possible selection mode, but no such mode is started in case
//        // extracted mode will start. Some text is selected though, and will trigger an action mode
//        // in the extracted view.
//        mEditor.hideControllers();
//    }
//    
//    /**
//     * Called by the framework in response to a text completion from
//     * the current input method, provided by it calling
//     * {@link InputConnection#commitCompletion
//     * InputConnection.commitCompletion()}.  The default implementation does
//     * nothing; text views that are supporting auto-completion should override
//     * this to do their desired behavior.
//     *
//     * @param text The auto complete text the user has selected.
//     */
//    void onCommitCompletion(CompletionInfo text) {
//        // intentionally empty
//    }
//    
//    /**
//     * Called by the framework in response to a text auto-correction (such as fixing a typo using a
//     * a dictionnary) from the current input method, provided by it calling
//     * {@link InputConnection#commitCorrection} InputConnection.commitCorrection()}. The default
//     * implementation flashes the background of the corrected word to provide feedback to the user.
//     *
//     * @param info The auto correct info about the text that was corrected.
//     */
//    void onCommitCorrection(CorrectionInfo info) {
//        if (mEditor != NULL) mEditor.onCommitCorrection(info);
//    }
//    
//    void beginBatchEdit() {
//        if (mEditor != NULL) mEditor.beginBatchEdit();
//    }
//    
//    void endBatchEdit() {
//        if (mEditor != NULL) mEditor.endBatchEdit();
//    }
//    
//    /**
//     * Called by the framework in response to a request to begin a batch
//     * of edit operations through a call to link {@link #beginBatchEdit()}.
//     */
//    void onBeginBatchEdit() {
//        // intentionally empty
//    }
//    
//    /**
//     * Called by the framework in response to a request to end a batch
//     * of edit operations through a call to link {@link #endBatchEdit}.
//     */
//    void onEndBatchEdit() {
//        // intentionally empty
//    }
//    
//    /**
//     * Called by the framework in response to a command from the
//     * current method, provided by it calling
//     * {@link InputConnection#performPrivateCommand
//     * InputConnection.performPrivateCommand()}.
//     *
//     * @param action The action name of the command.
//     * @param data Any additional data for the command.  This may be NULL.
//     * @return Return true if you handled the command, else false.
//     */
//    bool onPrivateIMECommand(String action, Bundle data) {
//        return false;
//    }
    
private:
    
    void nullLayouts();
    
    /**
     * Make a new Layout based on the already-measured size of the view,
     * on the assumption that it was measured correctly at some point.
     */
    void assumeLayout();
    
    Layout::Alignment getLayoutAlignment();
    
protected:
    
    /**
     * The width passed in is now the desired layout width,
     * not the full view width with padding.
     * {@hide}
     */
    void makeNewLayout(int wantWidth, int hintWidth,
                                 shared_ptr<BoringLayout::Metrics> boring,
                                 shared_ptr<BoringLayout::Metrics> hintBoring,
                                 int ellipsisWidth, bool bringIntoView);
    
private:
    
    shared_ptr<Layout> makeSingleLayout(int wantWidth, shared_ptr<BoringLayout::Metrics> boring, int ellipsisWidth,
                            Layout::Alignment alignment, bool shouldEllipsize, shared_ptr<TextUtils::TruncateAt> effectiveEllipsize,
                            bool useSaved);
    
    bool compressText(float width);
    
    static int desired(shared_ptr<Layout> layout);
    
public:
    
    /**
     * Set whether the TextView includes extra top and bottom padding to make
     * room for accents that go above the normal ascent and descent.
     * The default is true.
     *
     * @see #getIncludeFontPadding()
     *
     * @attr ref android.R.styleable#TextView_includeFontPadding
     */
    void setIncludeFontPadding(bool includepad);
    
    /**
     * Gets whether the TextView includes extra top and bottom padding to make
     * room for accents that go above the normal ascent and descent.
     *
     * @see #setIncludeFontPadding(bool)
     *
     * @attr ref android.R.styleable#TextView_includeFontPadding
     */
    bool getIncludeFontPadding();
    
private:
    
    static const shared_ptr<BoringLayout::Metrics> UNKNOWN_BORING;
    
protected:
    
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    
private:
    
    int getDesiredHeight();
    
    int getDesiredHeight(shared_ptr<Layout> layout, bool cap);
    
    /**
     * Check whether a change to the existing text layout requires a
     * new view layout.
     */
    void checkForResize();
    
    /**
     * Check whether entirely new text requires a new view layout
     * or merely a new text layout.
     */
    void checkForRelayout();
    
protected:
    
    virtual void onLayout(bool changed, int left, int top, int right, int bottom);
    
private:
    
    bool isShowingHint();
    
    /**
     * Returns true if anything changed.
     */
    bool bringTextIntoView();
    
public:
    
    /**
     * Move the point, specified by the offset, into the view if it is needed.
     * This has to be called after layout. Returns true if anything changed.
     */
    bool bringPointIntoView(int offset);
    
    /**
     * Move the cursor, if needed, so that it is at an offset that is visible
     * to the user.  This will not move the cursor if it represents more than
     * one character (a selection range).  This will only work if the
     * TextView contains spannable text; otherwise it will do nothing.
     *
     * @return True if the cursor was actually moved, false otherwise.
     */
    bool moveCursorToVisibleOffset();
    
    virtual void computeScroll();
    
private:
    
    void getInterestingRect(Rect r, int line);
    
    void convertFromViewportToContentCoordinates(Rect r);
    
    int viewportToContentHorizontalOffset();
    
    int viewportToContentVerticalOffset();
    
public:
    
//    @Override
//    void debug(int depth) {
//        View::debug(depth);
//        
//        String output = debugIndent(depth);
//        output += "frame={" + mLeft + ", " + mTop + ", " + mRight
//        + ", " + mBottom + "} scroll={" + mScrollX + ", " + mScrollY
//        + "} ";
//        
//        if (mText != NULL) {
//            
//            output += "mText=\"" + mText + "\" ";
//            if (mLayout != NULL) {
//                output += "mLayout width=" + mLayout::getWidth()
//                + " height=" + mLayout::getHeight();
//            }
//        } else {
//            output += "mText=NULL";
//        }
//        Log.d(VIEW_LOG_TAG, output);
//    }
    
    /**
     * Convenience for {@link Selection#getSelectionStart}.
     */
    int getSelectionStart();
    
    /**
     * Convenience for {@link Selection#getSelectionEnd}.
     */
    int getSelectionEnd();
    
    /**
     * Return true iff there is a selection inside this text view.
     */
    bool hasSelection();
    
    /**
     * Sets the properties of this field (lines, horizontally scrolling,
     * transformation method) to be for a single-line input.
     *
     * @attr ref android.R.styleable#TextView_singleLine
     */
    void setSingleLine();
    
    /**
     * Sets the properties of this field to transform input to ALL CAPS
     * display. This may use a "small caps" formatting if available.
     * This setting will be ignored if this field is editable or selectable.
     *
     * This call replaces the current transformation method. Disabling this
     * will not necessarily restore the previous behavior from before this
     * was enabled.
     *
     * @see #setTransformationMethod(TransformationMethod)
     * @attr ref android.R.styleable#TextView_textAllCaps
     */
    void setAllCaps(bool allCaps);
    
    /**
     * If true, sets the properties of this field (number of lines, horizontally scrolling,
     * transformation method) to be for a single-line input; if false, restores these to the default
     * conditions.
     *
     * Note that the default conditions are not necessarily those that were in effect prior this
     * method, and you may want to reset these properties to your custom values.
     *
     * @attr ref android.R.styleable#TextView_singleLine
     */
    void setSingleLine(bool singleLine);
    
private:
    
    /**
     * Adds or remove the EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE on the mInputType.
     * @param singleLine
     */
    void setInputTypeSingleLine(bool singleLine);
    
    void applySingleLine(bool singleLine, bool applyTransformation,
                                 bool changeMaxLines);
    
public:
    
    /**
     * Causes words in the text that are longer than the view is wide
     * to be ellipsized instead of broken in the middle.  You may also
     * want to {@link #setSingleLine} or {@link #setHorizontallyScrolling}
     * to constrain the text to a single line.  Use <code>NULL</code>
     * to turn off ellipsizing.
     *
     * If {@link #setMaxLines} has been used to set two or more lines,
     * {@link android.text.TextUtils.TruncateAt#END} and
     * {@link android.text.TextUtils.TruncateAt#MARQUEE}* are only supported
     * (other ellipsizing types will not do anything).
     *
     * @attr ref android.R.styleable#TextView_ellipsize
     */
    void setEllipsize(shared_ptr<TextUtils::TruncateAt> where);
    
    /**
     * Sets how many times to repeat the marquee animation. Only applied if the
     * TextView has marquee enabled. Set to -1 to repeat indefinitely.
     *
     * @see #getMarqueeRepeatLimit()
     *
     * @attr ref android.R.styleable#TextView_marqueeRepeatLimit
     */
    void setMarqueeRepeatLimit(int marqueeLimit);
    
    /**
     * Gets the number of times the marquee animation is repeated. Only meaningful if the
     * TextView has marquee enabled.
     *
     * @return the number of times the marquee animation is repeated. -1 if the animation
     * repeats indefinitely
     *
     * @see #setMarqueeRepeatLimit(int)
     *
     * @attr ref android.R.styleable#TextView_marqueeRepeatLimit
     */
    int getMarqueeRepeatLimit();
    
    /**
     * Returns where, if anywhere, words that are longer than the view
     * is wide should be ellipsized.
     */
    shared_ptr<TextUtils::TruncateAt> getEllipsize();
    
    /**
     * Set the TextView so that when it takes focus, all the text is
     * selected.
     *
     * @attr ref android.R.styleable#TextView_selectAllOnFocus
     */
    void setSelectAllOnFocus(bool selectAllOnFocus);
    
    /**
     * Set whether the cursor is visible. The default is true. Note that this property only
     * makes sense for editable TextView.
     *
     * @see #isCursorVisible()
     *
     * @attr ref android.R.styleable#TextView_cursorVisible
     */
    void setCursorVisible(bool visible);
    
    /**
     * @return whether or not the cursor is visible (assuming this TextView is editable)
     *
     * @see #setCursorVisible(bool)
     *
     * @attr ref android.R.styleable#TextView_cursorVisible
     */
    bool isCursorVisible();
    
private:
    
    bool canMarquee();
    
    void startMarquee();
    
    void stopMarquee();
    
    void startStopMarquee(bool start);
    
protected:
    
    /**
     * This method is called when the text is changed, in case any subclasses
     * would like to know.
     *
     * Within <code>text</code>, the <code>lengthAfter</code> characters
     * beginning at <code>start</code> have just replaced old text that had
     * length <code>lengthBefore</code>. It is an error to attempt to make
     * changes to <code>text</code> from this callback.
     *
     * @param text The text the TextView is displaying
     * @param start The offset of the start of the range of the text that was
     * modified
     * @param lengthBefore The length of the former text that has been replaced
     * @param lengthAfter The length of the replacement modified text
     */
    void onTextChanged(shared_ptr<CharSequence> text, int start, int lengthBefore, int lengthAfter);
    
    /**
     * This method is called when the selection has changed, in case any
     * subclasses would like to know.
     *
     * @param selStart The new selection start location.
     * @param selEnd The new selection end location.
     */
    void onSelectionChanged(int selStart, int selEnd);
    
public:
    
    /**
     * Adds a TextWatcher to the list of those whose methods are called
     * whenever this TextView's text changes.
     * <p>
     * In 1.0, the {@link TextWatcher#afterTextChanged} method was erroneously
     * not called after {@link #setText} calls.  Now, doing {@link #setText}
     * if there are any text changed listeners forces the buffer type to
     * Editable if it would not otherwise be and does call this method.
     */
    void addTextChangedListener(shared_ptr<TextWatcher> watcher);
    
    /**
     * Removes the specified TextWatcher from the list of those whose
     * methods are called
     * whenever this TextView's text changes.
     */
    void removeTextChangedListener(shared_ptr<TextWatcher> watcher);
    
private:
    
    void sendBeforeTextChanged(shared_ptr<CharSequence> text, int start, int before, int after);
    
    // Removes all spans that are inside or actually overlap the start..end range
    void removeIntersectingSpans(int start, int end, string type);
    
    /**
     * Not so it can be called from an inner class without going
     * through a thunk.
     */
    void sendOnTextChanged(shared_ptr<CharSequence> text, int start, int before, int after);
    
    /**
     * Not so it can be called from an inner class without going
     * through a thunk.
     */
    void sendAfterTextChanged(shared_ptr<Editable> text);
    
    void updateAfterEdit();
    
    /**
     * Not so it can be called from an inner class without going
     * through a thunk.
     */
    void handleTextChanged(shared_ptr<CharSequence> buffer, int start, int before, int after);
    
    /**
     * Not so it can be called from an inner class without going
     * through a thunk.
     */
    void spanChange(shared_ptr<Spanned> buf, shared_ptr<Object> what, int oldStart, int newStart, int oldEnd, int newEnd);
    
public:
    
    /**
     * @hide
     */
    virtual void dispatchFinishTemporaryDetach();
    
    virtual void onStartTemporaryDetach();
    
    virtual void onFinishTemporaryDetach();
    
//protected:
//    
//    @Override
//    void onFocusChanged(bool focused, int direction, Rect previouslyFocusedRect) {
//        if (mTemporaryDetach) {
//            // If we are temporarily in the detach state, then do nothing.
//            View::onFocusChanged(focused, direction, previouslyFocusedRect);
//            return;
//        }
//        
//        if (mEditor != NULL) mEditor.onFocusChanged(focused, direction);
//        
//        if (focused) {
//            if (mText instanceof Spannable) {
//                Spannable sp = (Spannable) mText;
//                MetaKeyKeyListener.resetMetaState(sp);
//            }
//        }
//        
//        startStopMarquee(focused);
//        
//        if (mTransformation != NULL) {
//            mTransformation.onFocusChanged(this, mText, focused, direction, previouslyFocusedRect);
//        }
//        
//        View::onFocusChanged(focused, direction, previouslyFocusedRect);
//    }
//    
//public:
//    
//    @Override
//    void onWindowFocusChanged(bool hasWindowFocus) {
//        View::onWindowFocusChanged(hasWindowFocus);
//        
//        if (mEditor != NULL) mEditor.onWindowFocusChanged(hasWindowFocus);
//        
//        startStopMarquee(hasWindowFocus);
//    }
//    
//protected:
//    
//    @Override
//    void onVisibilityChanged(View changedView, int visibility) {
//        View::onVisibilityChanged(changedView, visibility);
//        if (mEditor != NULL && visibility != VISIBLE) {
//            mEditor.hideControllers();
//        }
//    }
    
public:
    
//    /**
//     * Use {@link BaseInputConnection#removeComposingSpans
//     * BaseInputConnection.removeComposingSpans()} to remove any IME composing
//     * state from this text view.
//     */
//    void clearComposingText() {
//        if (mText instanceof Spannable) {
//            BaseInputConnection.removeComposingSpans((Spannable)mText);
//        }
//    }
//    
//    @Override
//    void setSelected(bool selected) {
//        bool wasSelected = isSelected();
//        
//        View::setSelected(selected);
//        
//        if (selected != wasSelected && mEllipsize == TextUtils.TruncateAt.MARQUEE) {
//            if (selected) {
//                startMarquee();
//            } else {
//                stopMarquee();
//            }
//        }
//    }
    
    virtual bool onTouchEvent(MotionEvent *event);
    
    virtual bool onGenericMotionEvent(MotionEvent *event);
    
private:
    
    /**
     * @return True iff this TextView contains a text that can be edited, or if this is
     * a selectable TextView.
     */
    bool isTextEditable();
    
public:
    
    /**
     * Returns true, only while processing a touch gesture, if the initial
     * touch down event caused focus to move to the text view and as a result
     * its selection changed.  Only valid while processing the touch gesture
     * of interest, in an editable text view.
     */
    bool didTouchFocusSelect();
    
    virtual void cancelLongPress();
    
//    @Override
//    bool onTrackballEvent(MotionEvent event) {
//        if (mMovement != NULL && mText instanceof Spannable && mLayout != NULL) {
//            if (mMovement.onTrackballEvent(this, (Spannable) mText, event)) {
//                return true;
//            }
//        }
//        
//        return View::onTrackballEvent(event);
//    }
//    
//    void setScroller(Scroller s) {
//        mScroller = s;
//    }
//    
//protected:
//    
//    @Override
//    float getLeftFadingEdgeStrength() {
//        if (mEllipsize == TextUtils.TruncateAt.MARQUEE &&
//            mMarqueeFadeMode != MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
//            if (mMarquee != NULL && !mMarquee.isStopped()) {
//                const Marquee marquee = mMarquee;
//                if (marquee.shouldDrawLeftFade()) {
//                    const float scroll = marquee.getScroll();
//                    return scroll / getHorizontalFadingEdgeLength();
//                } else {
//                    return 0.0f;
//                }
//            } else if (getLineCount() == 1) {
//                const int layoutDirection = getLayoutDirection();
//                const int absoluteGravity = Gravity::getAbsoluteGravity(mGravity, layoutDirection);
//                switch (absoluteGravity & Gravity::HORIZONTAL_GRAVITY_MASK) {
//                    case Gravity::LEFT:
//                        return 0.0f;
//                    case Gravity::RIGHT:
//                        return (mLayout::getLineRight(0) - (mRight - mLeft) -
//                                getCompoundPaddingLeft() - getCompoundPaddingRight() -
//                                mLayout::getLineLeft(0)) / getHorizontalFadingEdgeLength();
//                    case Gravity::CENTER_HORIZONTAL:
//                        return 0.0f;
//                }
//            }
//        }
//        return View::getLeftFadingEdgeStrength();
//    }
//    
//    @Override
//    float getRightFadingEdgeStrength() {
//        if (mEllipsize == TextUtils.TruncateAt.MARQUEE &&
//            mMarqueeFadeMode != MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
//            if (mMarquee != NULL && !mMarquee.isStopped()) {
//                const Marquee marquee = mMarquee;
//                const float maxFadeScroll = marquee.getMaxFadeScroll();
//                const float scroll = marquee.getScroll();
//                return (maxFadeScroll - scroll) / getHorizontalFadingEdgeLength();
//            } else if (getLineCount() == 1) {
//                const int layoutDirection = getLayoutDirection();
//                const int absoluteGravity = Gravity::getAbsoluteGravity(mGravity, layoutDirection);
//                switch (absoluteGravity & Gravity::HORIZONTAL_GRAVITY_MASK) {
//                    case Gravity::LEFT:
//                        const int textWidth = (mRight - mLeft) - getCompoundPaddingLeft() -
//                        getCompoundPaddingRight();
//                        const float lineWidth = mLayout::getLineWidth(0);
//                        return (lineWidth - textWidth) / getHorizontalFadingEdgeLength();
//                    case Gravity::RIGHT:
//                        return 0.0f;
//                    case Gravity::CENTER_HORIZONTAL:
//                    case Gravity::FILL_HORIZONTAL:
//                        return (mLayout::getLineWidth(0) - ((mRight - mLeft) -
//                                                           getCompoundPaddingLeft() - getCompoundPaddingRight())) /
//                        getHorizontalFadingEdgeLength();
//                }
//            }
//        }
//        return View::getRightFadingEdgeStrength();
//    }
//    
//    @Override
//    int computeHorizontalScrollRange() {
//        if (mLayout != NULL) {
//            return mSingleLine && (mGravity & Gravity::HORIZONTAL_GRAVITY_MASK) == Gravity::LEFT ?
//            (int) mLayout::getLineWidth(0) : mLayout::getWidth();
//        }
//        
//        return View::computeHorizontalScrollRange();
//    }
//    
//    @Override
//    int computeVerticalScrollRange() {
//        if (mLayout != NULL)
//            return mLayout::getHeight();
//        
//        return View::computeVerticalScrollRange();
//    }
//    
//    @Override
//    int computeVerticalScrollExtent() {
//        return getHeight() - getCompoundPaddingTop() - getCompoundPaddingBottom();
//    }
    
public:
    
//    @Override
//    void findViewsWithText(ArrayList<View> outViews, shared_ptr<CharSequence> searched, int flags) {
//        View::findViewsWithText(outViews, searched, flags);
//        if (!outViews.contains(this) && (flags & FIND_VIEWS_WITH_TEXT) != 0
//            && !TextUtils.isEmpty(searched) && !TextUtils.isEmpty(mText)) {
//            String searchedLowerCase = searched.toString().toLowerCase();
//            String textLowerCase = mText.toString().toLowerCase();
//            if (textLowerCase.contains(searchedLowerCase)) {
//                outViews.add(this);
//            }
//        }
//    }
    
    /**
     * Returns the TextView_textColor attribute from the
     * TypedArray, if set, or the TextAppearance_textColor
     * from the TextView_textAppearance attribute, if TextView_textColor
     * was not set directly.
     */
    static shared_ptr<ColorStateList> getTextColors(Context *context, AttributeSet *attrs);
    
    /**
     * Returns the default color from the TextView_textColor attribute
     * from the AttributeSet, if set, or the default color from the
     * TextAppearance_textColor from the TextView_textAppearance attribute,
     * if TextView_textColor was not set directly.
     */
    static int getTextColor(Context *context, AttributeSet *attrs, int def);
    
//    @Override
//    bool onKeyShortcut(int keyCode, KeyEvent event) {
//        const int filteredMetaState = event.getMetaState() & ~KeyEvent.META_CTRL_MASK;
//        if (KeyEvent.metaStateHasNoModifiers(filteredMetaState)) {
//            switch (keyCode) {
//                case KeyEvent.KEYCODE_A:
//                    if (canSelectText()) {
//                        return onTextContextMenuItem(ID_SELECT_ALL);
//                    }
//                    break;
//                case KeyEvent.KEYCODE_X:
//                    if (canCut()) {
//                        return onTextContextMenuItem(ID_CUT);
//                    }
//                    break;
//                case KeyEvent.KEYCODE_C:
//                    if (canCopy()) {
//                        return onTextContextMenuItem(ID_COPY);
//                    }
//                    break;
//                case KeyEvent.KEYCODE_V:
//                    if (canPaste()) {
//                        return onTextContextMenuItem(ID_PASTE);
//                    }
//                    break;
//            }
//        }
//        return View::onKeyShortcut(keyCode, event);
//    }
    
private:
    
    /**
     * Unlike {@link #textCanBeSelected()}, this method is based on the <i>current</i> state of the
     * TextView. {@link #textCanBeSelected()} has to be true (this is one of the conditions to have
     * a selection controller (see {@link Editor#prepareCursorControllers()}), but this is not
     * sufficient.
     */
    bool canSelectText();
    
    /**
     * Test based on the <i>intrinsic</i> charateristics of the TextView.
     * The text must be spannable and the movement method must allow for arbitary selection.
     *
     * See also {@link #canSelectText()}.
     */
    bool textCanBeSelected();
    
//    Locale getTextServicesLocale(bool allowNullLocale) {
//        // Start fetching the text services locale asynchronously.
//        updateTextServicesLocaleAsync();
//        // If !allowNullLocale and there is no cached text services locale, just return the default
//        // locale.
//        return (mCurrentSpellCheckerLocaleCache == NULL && !allowNullLocale) ? Locale.getDefault()
//        : mCurrentSpellCheckerLocaleCache;
//    }
//    
//    /**
//     * This is a temporary method. Future versions may support multi-locale text.
//     * Caveat: This method may not return the latest text services locale, but this should be
//     * acceptable and it's more important to make this method asynchronous.
//     *
//     * @return The locale that should be used for a word iterator
//     * in this TextView, based on the current spell checker settings,
//     * the current IME's locale, or the system default locale.
//     * Please note that a word iterator in this TextView is different from another word iterator
//     * used by SpellChecker.java of TextView. This method should be used for the former.
//     * @hide
//     */
//    // TODO: Support multi-locale
//    // TODO: Update the text services locale immediately after the keyboard locale is switched
//    // by catching intent of keyboard switch event
//    Locale getTextServicesLocale() {
//        return getTextServicesLocale(false /* allowNullLocale */);
//    }
//    
//    /**
//     * This is a temporary method. Future versions may support multi-locale text.
//     * Caveat: This method may not return the latest spell checker locale, but this should be
//     * acceptable and it's more important to make this method asynchronous.
//     *
//     * @return The locale that should be used for a spell checker in this TextView,
//     * based on the current spell checker settings, the current IME's locale, or the system default
//     * locale.
//     * @hide
//     */
//    Locale getSpellCheckerLocale() {
//        return getTextServicesLocale(true /* allowNullLocale */);
//    }
    
    void updateTextServicesLocaleAsync();
    
    void updateTextServicesLocaleLocked();
    
    void onLocaleChanged();
    
//public:
//    
//    /**
//     * This method is used by the ArrowKeyMovementMethod to jump from one word to the other.
//     * Made available to achieve a consistent behavior.
//     * @hide
//     */
//    WordIterator getWordIterator();
//    
//    @Override
//    void onPopulateAccessibilityEvent(AccessibilityEvent event) {
//        View::onPopulateAccessibilityEvent(event);
//        
//        const bool isPassword = hasPasswordTransformationMethod();
//        if (!isPassword || shouldSpeakPasswordsForAccessibility()) {
//            const shared_ptr<CharSequence> text = getTextForAccessibility();
//            if (!TextUtils.isEmpty(text)) {
//                event.getText().add(text);
//            }
//        }
//    }
//    
//    /**
//     * @return true if the user has explicitly allowed accessibility services
//     * to speak passwords.
//     */
//    bool shouldSpeakPasswordsForAccessibility() {
//        return (Settings.Secure.getInt(mContext.getContentResolver(),
//                                       Settings.Secure.ACCESSIBILITY_SPEAK_PASSWORD, 0) == 1);
//    }
//    
//    @Override
//    void onInitializeAccessibilityEvent(AccessibilityEvent event) {
//        View::onInitializeAccessibilityEvent(event);
//        
//        event.setClassName(TextView.class.getName());
//        const bool isPassword = hasPasswordTransformationMethod();
//        event.setPassword(isPassword);
//        
//        if (event.getEventType() == AccessibilityEvent.TYPE_VIEW_TEXT_SELECTION_CHANGED) {
//            event.setFromIndex(Selection.getSelectionStart(mText));
//            event.setToIndex(Selection.getSelectionEnd(mText));
//            event.setItemCount(mText.length());
//        }
//    }
//    
//    @Override
//    void onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo info) {
//        View::onInitializeAccessibilityNodeInfo(info);
//        
//        info.setClassName(TextView.class.getName());
//        const bool isPassword = hasPasswordTransformationMethod();
//        info.setPassword(isPassword);
//        
//        if (!isPassword) {
//            info.setText(getTextForAccessibility());
//        }
//        
//        if (mBufferType == BufferType.EDITABLE) {
//            info.setEditable(true);
//        }
//        
//        if (!TextUtils.isEmpty(mText)) {
//            info.addAction(AccessibilityNodeInfo.ACTION_NEXT_AT_MOVEMENT_GRANULARITY);
//            info.addAction(AccessibilityNodeInfo.ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY);
//            info.setMovementGranularities(AccessibilityNodeInfo.MOVEMENT_GRANULARITY_CHARACTER
//                                          | AccessibilityNodeInfo.MOVEMENT_GRANULARITY_WORD
//                                          | AccessibilityNodeInfo.MOVEMENT_GRANULARITY_LINE
//                                          | AccessibilityNodeInfo.MOVEMENT_GRANULARITY_PARAGRAPH
//                                          | AccessibilityNodeInfo.MOVEMENT_GRANULARITY_PAGE);
//        }
//        
//        if (isFocused()) {
//            if (canSelectText()) {
//                info.addAction(AccessibilityNodeInfo.ACTION_SET_SELECTION);
//            }
//            if (canCopy()) {
//                info.addAction(AccessibilityNodeInfo.ACTION_COPY);
//            }
//            if (canPaste()) {
//                info.addAction(AccessibilityNodeInfo.ACTION_PASTE);
//            }
//            if (canCut()) {
//                info.addAction(AccessibilityNodeInfo.ACTION_CUT);
//            }
//        }
//    }
//    
//    @Override
//    bool performAccessibilityAction(int action, Bundle arguments) {
//        switch (action) {
//            case AccessibilityNodeInfo.ACTION_COPY: {
//                if (isFocused() && canCopy()) {
//                    if (onTextContextMenuItem(ID_COPY)) {
//                        notifyAccessibilityStateChanged();
//                        return true;
//                    }
//                }
//            } return false;
//            case AccessibilityNodeInfo.ACTION_PASTE: {
//                if (isFocused() && canPaste()) {
//                    if (onTextContextMenuItem(ID_PASTE)) {
//                        notifyAccessibilityStateChanged();
//                        return true;
//                    }
//                }
//            } return false;
//            case AccessibilityNodeInfo.ACTION_CUT: {
//                if (isFocused() && canCut()) {
//                    if (onTextContextMenuItem(ID_CUT)) {
//                        notifyAccessibilityStateChanged();
//                        return true;
//                    }
//                }
//            } return false;
//            case AccessibilityNodeInfo.ACTION_SET_SELECTION: {
//                if (isFocused() && canSelectText()) {
//                    shared_ptr<CharSequence> text = getIterableTextForAccessibility();
//                    if (text == NULL) {
//                        return false;
//                    }
//                    const int start = (arguments != NULL) ? arguments.getInt(
//                                                                             AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_START_INT, -1) : -1;
//                    const int end = (arguments != NULL) ? arguments.getInt(
//                                                                           AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_END_INT, -1) : -1;
//                    if ((getSelectionStart() != start || getSelectionEnd() != end)) {
//                        // No arguments clears the selection.
//                        if (start == end && end == -1) {
//                            Selection.removeSelection((Spannable) text);
//                            notifyAccessibilityStateChanged();
//                            return true;
//                        }
//                        if (start >= 0 && start <= end && end <= text.length()) {
//                            Selection.setSelection((Spannable) text, start, end);
//                            // Make sure selection mode is engaged.
//                            if (mEditor != NULL) {
//                                mEditor.startSelectionActionMode();
//                            }
//                            notifyAccessibilityStateChanged();
//                            return true;
//                        }
//                    }
//                }
//            } return false;
//            default: {
//                return View::performAccessibilityAction(action, arguments);
//            }
//        }
//    }
//    
//    @Override
//    void sendAccessibilityEvent(int eventType) {
//        // Do not send scroll events since first they are not interesting for
//        // accessibility and second such events a generated too frequently.
//        // For details see the implementation of bringTextIntoView().
//        if (eventType == AccessibilityEvent.TYPE_VIEW_SCROLLED) {
//            return;
//        }
//        View::sendAccessibilityEvent(eventType);
//    }
//    
//    /**
//     * Gets the text reported for accessibility purposes.
//     *
//     * @return The accessibility text.
//     *
//     * @hide
//     */
//    shared_ptr<CharSequence> getTextForAccessibility() {
//        shared_ptr<CharSequence> text = getText();
//        if (TextUtils.isEmpty(text)) {
//            text = getHint();
//        }
//        return text;
//    }
//    
//    void sendAccessibilityEventTypeViewTextChanged(shared_ptr<CharSequence> beforeText,
//                                                   int fromIndex, int removedCount, int addedCount) {
//        AccessibilityEvent event =
//        AccessibilityEvent.obtain(AccessibilityEvent.TYPE_VIEW_TEXT_CHANGED);
//        event.setFromIndex(fromIndex);
//        event.setRemovedCount(removedCount);
//        event.setAddedCount(addedCount);
//        event.setBeforeText(beforeText);
//        sendAccessibilityEventUnchecked(event);
//    }
//    
//    /**
//     * Returns whether this text view is a current input method target.  The
//     * default implementation just checks with {@link InputMethodManager}.
//     */
//    bool isInputMethodTarget();
    
private:
    
    static const int ID_SELECT_ALL = 0;//android.R.id.selectAll;
    static const int ID_CUT = 0;//android.R.id.cut;
    static const int ID_COPY = 0;//android.R.id.copy;
    static const int ID_PASTE = 0;//android.R.id.paste;
    
public:
    
    /**
     * Called when a context menu option for the text view is selected.  Currently
     * this will be one of {@link android.R.id#selectAll}, {@link android.R.id#cut},
     * {@link android.R.id#copy} or {@link android.R.id#paste}.
     *
     * @return true if the context menu item action was performed.
     */
//    bool onTextContextMenuItem(int id);
    
private:
    
    shared_ptr<CharSequence> getTransformedText(int start, int end);
    
public:
    
    virtual bool performLongClick();
    
//protected:
//    
//    @Override
//    void onScrollChanged(int horiz, int vert, int oldHoriz, int oldVert) {
//        View::onScrollChanged(horiz, vert, oldHoriz, oldVert);
//        if (mEditor != NULL) {
//            mEditor.onScrollChanged();
//        }
//    }
    
public:
    
    /**
     * Return whether or not suggestions are enabled on this TextView. The suggestions are generated
     * by the IME or by the spell checker as the user types. This is done by adding
     * {@link SuggestionSpan}s to the text.
     *
     * When suggestions are enabled (default), this list of suggestions will be displayed when the
     * user asks for them on these parts of the text. This value depends on the inputType of this
     * TextView.
     *
     * The class of the input type must be {@link InputType#TYPE_CLASS_TEXT}.
     *
     * In addition, the type variation must be one of
     * {@link InputType#TYPE_TEXT_VARIATION_NORMAL},
     * {@link InputType#TYPE_TEXT_VARIATION_EMAIL_SUBJECT},
     * {@link InputType#TYPE_TEXT_VARIATION_LONG_MESSAGE},
     * {@link InputType#TYPE_TEXT_VARIATION_SHORT_MESSAGE} or
     * {@link InputType#TYPE_TEXT_VARIATION_WEB_EDIT_TEXT}.
     *
     * And constly, the {@link InputType#TYPE_TEXT_FLAG_NO_SUGGESTIONS} flag must <i>not</i> be set.
     *
     * @return true if the suggestions popup window is enabled, based on the inputType.
     */
    bool isSuggestionsEnabled();
    
//    /**
//     * If provided, this ActionMode.Callback will be used to create the ActionMode when text
//     * selection is initiated in this View.
//     *
//     * The standard implementation populates the menu with a subset of Select All, Cut, Copy and
//     * Paste actions, depending on what this View supports.
//     *
//     * A custom implementation can add new entries in the default menu in its
//     * {@link android.view.ActionMode.Callback#onPrepareActionMode(ActionMode, Menu)} method. The
//     * default actions can also be removed from the menu using {@link Menu#removeItem(int)} and
//     * passing {@link android.R.id#selectAll}, {@link android.R.id#cut}, {@link android.R.id#copy}
//     * or {@link android.R.id#paste} ids as parameters.
//     *
//     * Returning false from
//     * {@link android.view.ActionMode.Callback#onCreateActionMode(ActionMode, Menu)} will prevent
//     * the action mode from being started.
//     *
//     * Action click events should be handled by the custom implementation of
//     * {@link android.view.ActionMode.Callback#onActionItemClicked(ActionMode, MenuItem)}.
//     *
//     * Note that text selection mode is not started when a TextView receives focus and the
//     * {@link android.R.attr#selectAllOnFocus} flag has been set. The content is highlighted in
//     * that case, to allow for quick replacement.
//     */
//    void setCustomSelectionActionModeCallback(ActionMode::Callback actionModeCallback);
//    
//    /**
//     * Retrieves the value set in {@link #setCustomSelectionActionModeCallback}. Default is NULL.
//     *
//     * @return The current custom selection callback.
//     */
//    ActionMode::Callback getCustomSelectionActionModeCallback();
//    
//protected:
//    
//    /**
//     * @hide
//     */
//    void stopSelectionActionMode();
    
private:
    
    bool canCut();
    
    bool canCopy();
    
    bool canPaste();
    
    bool selectAllText();
    
    /**
     * Prepare text so that there are not zero or two spaces at beginning and end of region defined
     * by [min, max] when replacing this region by paste.
     * Note that if there were two spaces (or more) at that position before, they are kept. We just
     * make sure we do not add an extra one from the paste content.
     */
    long prepareSpacesAroundPaste(int min, int max, shared_ptr<CharSequence> paste);
    
    /**
     * Paste clipboard content between min and max positions.
     */
    void paste(int min, int max);
    
//    void setPrimaryClip(ClipData clip);
    
public:
    
    /**
     * Get the character offset closest to the specified absolute position. A typical use case is to
     * pass the result of {@link MotionEvent#getX()} and {@link MotionEvent#getY()} to this method.
     *
     * @param x The horizontal absolute position of a point on screen
     * @param y The vertical absolute position of a point on screen
     * @return the character offset for the character whose position is closest to the specified
     *  position. Returns -1 if there is no layout.
     */
    int getOffsetForPosition(float x, float y);
    
private:
    
    float convertToLocalHorizontalCoordinate(float x);
    
    int getLineAtCoordinate(float y);
    
    int getOffsetAtCoordinate(int line, float x);
    
//public:
//    
//    @Override
//    bool onDragEvent(DragEvent event) {
//        switch (event.getAction()) {
//            case DragEvent.ACTION_DRAG_STARTED:
//                return mEditor != NULL && mEditor.hasInsertionController();
//                
//            case DragEvent.ACTION_DRAG_ENTERED:
//                TextView.this.requestFocus();
//                return true;
//                
//            case DragEvent.ACTION_DRAG_LOCATION:
//                const int offset = getOffsetForPosition(event.getX(), event.getY());
//                Selection.setSelection((Spannable)mText, offset);
//                return true;
//                
//            case DragEvent.ACTION_DROP:
//                if (mEditor != NULL) mEditor.onDrop(event);
//                return true;
//                
//            case DragEvent.ACTION_DRAG_ENDED:
//            case DragEvent.ACTION_DRAG_EXITED:
//            default:
//                return true;
//        }
//    }
    
private:
    
//    bool isInBatchEditMode();
//    
//    @Override
//    public void onRtlPropertiesChanged(int layoutDirection) {
//        View::onRtlPropertiesChanged(layoutDirection);
//        
//        mTextDir = getTextDirectionHeuristic();
//    }
    
    shared_ptr<TextDirectionHeuristic> getTextDirectionHeuristic();

//    /**
//     * @hide
//     */
//    @Override
//    public void onResolveDrawables(int layoutDirection) {
//        // No need to resolve twice
//        if (mLastLayoutDirection == layoutDirection) {
//            return;
//        }
//        mLastLayoutDirection = layoutDirection;
//        
//        // Resolve drawables
//        if (mDrawables != NULL) {
//            mDrawables.resolveWithLayoutDirection(layoutDirection);
//        }
//    }
    
protected:
    
    /**
     * @hide
     */
    void resetResolvedDrawables();
    
//    /**
//     * @hide
//     */
//    void viewClicked(InputMethodManager imm);
//    }
    
    /**
     * Deletes the range of text [start, end[.
     * @hide
     */
    void deleteText_internal(int start, int end);
    
    /**
     * Replaces the range of text [start, end[ by replacement text
     * @hide
     */
    void replaceText_internal(int start, int end, shared_ptr<CharSequence> text);
    
    /**
     * Sets a span on the specified range of text
     * @hide
     */
    void setSpan_internal(Object span, int start, int end, int flags);
    
    /**
     * Moves the cursor to the specified offset position in text
     * @hide
     */
    void setCursorPosition_internal(int start, int end);

//private:
//    
//    /**
//     * An Editor should be created as soon as any of the editable-specific fields (grouped
//     * inside the Editor object) is assigned to a non-default value.
//     * This method will create the Editor if needed.
//     *
//     * A standard TextView (as well as buttons, checkboxes...) should not qualify and hence will
//     * have a NULL Editor, unlike an EditText. Inconsistent in-between states will have an
//     * Editor for backward compatibility, as soon as one of these fields is assigned.
//     *
//     * Also note that for performance reasons, the mEditor is created when needed, but not
//     * reset when no more edit-specific fields are needed.
//     */
//    void createEditorIfNeeded() {
//        if (mEditor == NULL) {
//            mEditor = new Editor(this);
//        }
//    }
//    
//public:
//    
//    /**
//     * @hide
//     */
//    @Override
//    shared_ptr<CharSequence> getIterableTextForAccessibility() {
//        if (!(mText instanceof Spannable)) {
//            setText(mText, BufferType.SPANNABLE);
//        }
//        return mText;
//    }
//    
//    /**
//     * @hide
//     */
//    @Override
//    TextSegmentIterator getIteratorForGranularity(int granularity) {
//        switch (granularity) {
//            case AccessibilityNodeInfo.MOVEMENT_GRANULARITY_LINE: {
//                Spannable text = (Spannable) getIterableTextForAccessibility();
//                if (!TextUtils.isEmpty(text) && getLayout() != NULL) {
//                    AccessibilityIterators.LineTextSegmentIterator iterator =
//                    AccessibilityIterators.LineTextSegmentIterator.getInstance();
//                    iterator.initialize(text, getLayout());
//                    return iterator;
//                }
//            } break;
//            case AccessibilityNodeInfo.MOVEMENT_GRANULARITY_PAGE: {
//                Spannable text = (Spannable) getIterableTextForAccessibility();
//                if (!TextUtils.isEmpty(text) && getLayout() != NULL) {
//                    AccessibilityIterators.PageTextSegmentIterator iterator =
//                    AccessibilityIterators.PageTextSegmentIterator.getInstance();
//                    iterator.initialize(this);
//                    return iterator;
//                }
//            } break;
//        }
//        return View::getIteratorForGranularity(granularity);
//    }
//    
//    /**
//     * @hide
//     */
//    @Override
//    int getAccessibilitySelectionStart() {
//        return getSelectionStart();
//    }
//    
//    /**
//     * @hide
//     */
//    bool isAccessibilitySelectionExtendable() {
//        return true;
//    }
//    
//    /**
//     * @hide
//     */
//    @Override
//    int getAccessibilitySelectionEnd() {
//        return getSelectionEnd();
//    }
//    
//    /**
//     * @hide
//     */
//    @Override
//    void setAccessibilitySelection(int start, int end) {
//        if (getAccessibilitySelectionStart() == start
//            && getAccessibilitySelectionEnd() == end) {
//            return;
//        }
//        // Hide all selection controllers used for adjusting selection
//        // since we are doing so explicitlty by other means and these
//        // controllers interact with how selection behaves.
//        if (mEditor != NULL) {
//            mEditor.hideControllers();
//        }
//        shared_ptr<CharSequence> text = getIterableTextForAccessibility();
//        if (Math.min(start, end) >= 0 && Math.max(start, end) <= text.length()) {
//            Selection.setSelection((Spannable) text, start, end);
//        } else {
//            Selection.removeSelection((Spannable) text);
//        }
//    }
//    
//    /**
//     * User interface state that is stored by TextView for implementing
//     * {@link View#onSaveInstanceState}.
//     */
//    public static class SavedState extends BaseSavedState {
//        int selStart;
//        int selEnd;
//        shared_ptr<CharSequence> text;
//        bool frozenWithFocus;
//        shared_ptr<CharSequence> error;
//        
//        SavedState(Parcelable superState) {
//            super(superState);
//        }
//        
//        @Override
//        public void writeToParcel(Parcel out, int flags) {
//            View::writeToParcel(out, flags);
//            out.writeInt(selStart);
//            out.writeInt(selEnd);
//            out.writeInt(frozenWithFocus ? 1 : 0);
//            TextUtils.writeToParcel(text, out, flags);
//            
//            if (error == NULL) {
//                out.writeInt(0);
//            } else {
//                out.writeInt(1);
//                TextUtils.writeToParcel(error, out, flags);
//            }
//        }
//        
//        @Override
//        public String toString() {
//            String str = "TextView.SavedState{"
//            + Integer.toHexString(System.identityHashCode(this))
//            + " start=" + selStart + " end=" + selEnd;
//            if (text != NULL) {
//                str += " text=" + text;
//            }
//            return str + "}";
//        }
//        
//        @SuppressWarnings("hiding")
//        public static const Parcelable.Creator<SavedState> CREATOR
//        = new Parcelable.Creator<SavedState>() {
//            public SavedState createFromParcel(Parcel in) {
//                return new SavedState(in);
//            }
//            
//            public SavedState[] newArray(int size) {
//                return new SavedState[size];
//            }
//        };
//        
//        SavedState(Parcel in) {
//            super(in);
//            selStart = in.readInt();
//            selEnd = in.readInt();
//            frozenWithFocus = (in.readInt() != 0);
//            text = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(in);
//            
//            if (in.readInt() != 0) {
//                error = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(in);
//            }
//        }
//    };
};

ANDROID_END

#endif /* defined(__Androidpp__TextView__) */

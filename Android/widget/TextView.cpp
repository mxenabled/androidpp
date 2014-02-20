//
//  TextView.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/14/14.
//  Copyright (c) 2014 MoneyDesktop-> All rights reserved.
//

#include "TextView.h"

#include "Android/content/res/ColorStateList.h"
#include "Android/graphics/Canvas.h"
#include "Android/graphics/Paint.h"
#include "Android/graphics/Path.h"
#include "Android/graphics/Rect.h"
#include "Android/graphics/Typeface.h"
#include "Android/internal/R.h"
#include "Android/text/Directions.h"
#include "Android/text/InputFilter.h"
#include "Android/text/InputType.h"
#include "Android/text/Selection.h"
#include "Android/text/SpannableString.h"
#include "Android/text/SpannedString.h"
#include "Android/text/StaticLayout.h"
#include "Android/text/String.h"
#include "Android/text/TextDirectionHeuristic.h"
#include "Android/text/TextDirectionHeuristics.h"
#include "Android/text/method/MovementMethod.h"
#include "Android/text/method/SingleLineTransformationMethod.h"
#include "Android/text/method/TransformationMethod.h"
#include "Android/text/method/TransformationMethod2.h"
#include "Android/utils/Exceptions.h"
#include "Android/utils/System.h"
#include "Android/utils/TypedValue.h"
#include "Android/view/MotionEvent.h"
#include "Android/view/inputmethod/EditorInfo.h"

#include <algorithm>
#include <sstream>

ANDROID_BEGIN

const shared_ptr<BoringLayout::Metrics> TextView::UNKNOWN_BORING = make_shared<BoringLayout::Metrics>();

const vector<int> TextView::MULTILINE_STATE_SET = { R::attr::state_multiline };
RectF TextView::TEMP_RECTF = RectF();
const vector<shared_ptr<InputFilter>> TextView::NO_FILTERS = vector<shared_ptr<InputFilter>>();
const shared_ptr<Spanned> TextView::EMPTY_SPANNED = make_shared<SpannedString>(make_shared<String>());

void TextView::Drawables::updateDrawablesLayoutDirection(int layoutDirection) {
    if (mDrawableLeft != NULL) {
        mDrawableLeft->setLayoutDirection(layoutDirection);
    }
    if (mDrawableRight != NULL) {
        mDrawableRight->setLayoutDirection(layoutDirection);
    }
    if (mDrawableTop != NULL) {
        mDrawableTop->setLayoutDirection(layoutDirection);
    }
    if (mDrawableBottom != NULL) {
        mDrawableBottom->setLayoutDirection(layoutDirection);
    }
}

void TextView::Drawables::applyErrorDrawableIfNeeded(int layoutDirection) {
    // first restore the initial state if needed
    switch (mDrawableSaved) {
        case DRAWABLE_LEFT:
            mDrawableLeft = mDrawableTemp;
            mDrawableSizeLeft = mDrawableSizeTemp;
            mDrawableHeightLeft = mDrawableHeightTemp;
            break;
        case DRAWABLE_RIGHT:
            mDrawableRight = mDrawableTemp;
            mDrawableSizeRight = mDrawableSizeTemp;
            mDrawableHeightRight = mDrawableHeightTemp;
            break;
        case DRAWABLE_NONE:
        default:
        {
        }
    }
    // then, if needed, assign the Error drawable to the correct location
    if (mDrawableError != NULL) {
        switch(layoutDirection) {
            case LAYOUT_DIRECTION_RTL:
                mDrawableSaved = DRAWABLE_LEFT;
                
                mDrawableTemp = mDrawableLeft;
                mDrawableSizeTemp = mDrawableSizeLeft;
                mDrawableHeightTemp = mDrawableHeightLeft;
                
                mDrawableLeft = mDrawableError;
                mDrawableSizeLeft = mDrawableSizeError;
                mDrawableHeightLeft = mDrawableHeightError;
                break;
            case LAYOUT_DIRECTION_LTR:
            default:
                mDrawableSaved = DRAWABLE_RIGHT;
                
                mDrawableTemp = mDrawableRight;
                mDrawableSizeTemp = mDrawableSizeRight;
                mDrawableHeightTemp = mDrawableHeightRight;
                
                mDrawableRight = mDrawableError;
                mDrawableSizeRight = mDrawableSizeError;
                mDrawableHeightRight = mDrawableHeightError;
                break;
        }
    }
}

TextView::Drawables::Drawables(Context *context) {
    mIsRtlCompatibilityMode = false;
    mOverride = false;
}

void TextView::Drawables::resolveWithLayoutDirection(int layoutDirection) {
    // First reset "left" and "right" drawables to their initial values
    mDrawableLeft = mDrawableLeftInitial;
    mDrawableRight = mDrawableRightInitial;
    
    if (mIsRtlCompatibilityMode) {
        // Use "start" drawable as "left" drawable if the "left" drawable was not defined
        if (mDrawableStart != NULL && mDrawableLeft == NULL) {
            mDrawableLeft = mDrawableStart;
            mDrawableSizeLeft = mDrawableSizeStart;
            mDrawableHeightLeft = mDrawableHeightStart;
        }
        // Use "end" drawable as "right" drawable if the "right" drawable was not defined
        if (mDrawableEnd != NULL && mDrawableRight == NULL) {
            mDrawableRight = mDrawableEnd;
            mDrawableSizeRight = mDrawableSizeEnd;
            mDrawableHeightRight = mDrawableHeightEnd;
        }
    } else {
        // JB-MR1+ normal case: "start" / "end" drawables are overriding "left" / "right"
        // drawable if and only if they have been defined
        switch(layoutDirection) {
            case LAYOUT_DIRECTION_RTL:
                if (mOverride) {
                    mDrawableRight = mDrawableStart;
                    mDrawableSizeRight = mDrawableSizeStart;
                    mDrawableHeightRight = mDrawableHeightStart;
                }
                
                if (mOverride) {
                    mDrawableLeft = mDrawableEnd;
                    mDrawableSizeLeft = mDrawableSizeEnd;
                    mDrawableHeightLeft = mDrawableHeightEnd;
                }
                break;
                
            case LAYOUT_DIRECTION_LTR:
            default:
                if (mOverride) {
                    mDrawableLeft = mDrawableStart;
                    mDrawableSizeLeft = mDrawableSizeStart;
                    mDrawableHeightLeft = mDrawableHeightStart;
                }
                
                if (mOverride) {
                    mDrawableRight = mDrawableEnd;
                    mDrawableSizeRight = mDrawableSizeEnd;
                    mDrawableHeightRight = mDrawableHeightEnd;
                }
                break;
        }
    }
    applyErrorDrawableIfNeeded(layoutDirection);
    updateDrawablesLayoutDirection(layoutDirection);
}

void TextView::Drawables::setErrorDrawable(shared_ptr<Drawable> dr, shared_ptr<TextView> tv) {
    if (mDrawableError != dr && mDrawableError != NULL) {
        mDrawableError->setCallback(NULL);
    }
    mDrawableError = dr;
    
    Rect compoundRect = mCompoundRect;
    vector<int> state = tv->getDrawableState();
    
    if (mDrawableError != NULL) {
        mDrawableError->setState(state);
        mDrawableError->copyBounds(compoundRect);
        mDrawableError->setCallback(tv.get());
        mDrawableSizeError = compoundRect.width();
        mDrawableHeightError = compoundRect.height();
    } else {
        mDrawableSizeError = mDrawableHeightError = 0;
    }
}

shared_ptr<String> TextView::CharWrapper::toString() {
    return make_shared<String>(mChars);
}

shared_ptr<CharSequence> TextView::CharWrapper::subSequence(int start, int end) {
    if (start < 0 || end < 0 || start > mLength || end > mLength) {
        stringstream str;
        str << start << ", " << end;
        throw IndexOutOfBoundsException(str.str());
    }
    
    return make_shared<String>(mChars, start + mStart, end - start);
}

void TextView::CharWrapper::getChars(int start, int end, UnicodeString &buf, int off) {
    if (start < 0 || end < 0 || start > mLength || end > mLength) {
        stringstream str;
        str << start << ", " << end;
        throw IndexOutOfBoundsException(str.str());
    }
    
    buf.insert(off, mChars.tempSubString(start+ mStart));
//    System::arraycopy(mChars, start + mStart, buf, off, end - start);
}

void TextView::CharWrapper::drawText(shared_ptr<Canvas> c, int start, int end,
              float x, float y, Paint *p) {
    c->drawText(mChars, start + mStart, end - start, x, y, p);
}

void TextView::CharWrapper::drawTextRun(shared_ptr<Canvas> c, int start, int end,
                 int contextStart, int contextEnd, float x, float y, int flags, Paint *p) {
    int count = end - start;
    int contextCount = contextEnd - contextStart;
    c->drawTextRun(mChars.getBuffer(), start + mStart, count, contextStart + mStart,
                  contextCount, x, y, flags, p);
}

float TextView::CharWrapper::measureText(int start, int end, Paint *p) {
    return p->measureText(mChars, start + mStart, end - start);
}

int TextView::CharWrapper::getTextWidths(int start, int end, vector<float> widths, Paint *p) {
    return p->getTextWidths(mChars, start + mStart, end - start, widths);
}

float TextView::CharWrapper::getTextRunAdvances(int start, int end, int contextStart,
                         int contextEnd, int flags, vector<float> advances, int advancesIndex,
                         Paint *p) {
    int count = end - start;
    int contextCount = contextEnd - contextStart;
    return p->getTextRunAdvances(mChars, start + mStart, count,
                                 contextStart + mStart, contextCount, flags, &advances,
                                 advancesIndex);
}

int TextView::CharWrapper::getTextRunCursor(int contextStart, int contextEnd, int flags,
                     int offset, int cursorOpt, Paint *p) {
    int contextCount = contextEnd - contextStart;
    return p->getTextRunCursor(mChars, contextStart + mStart,
                               contextCount, flags, offset + mStart, cursorOpt);
}

TextView::TextView(Context *context, AttributeSet *attrs) : View(context, attrs) {
    
    mText = make_shared<String>();
    
    Resources *res = getResources();
//    CompatibilityInfo *compat = res->getCompatibilityInfo();
    
    mTextPaint = make_shared<TextPaint>(Paint::kAntiAlias_Flag);
    mTextPaint->density = res->getDisplayMetrics().density;
    mTextPaint->setCompatibilityScaling(1.0f); //compat->applicationScale);
    
//    mHighlightPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
//    mHighlightPaint.setCompatibilityScaling(compat->applicationScale);
    
    mMovement = getDefaultMovementMethod();
    
    int textColorHighlight = 0;
    shared_ptr<ColorStateList> textColor;
    shared_ptr<ColorStateList> textColorHint;
    shared_ptr<ColorStateList> textColorLink;
    int textSize = 15;
    string fontFamily;
    int typefaceIndex = -1;
    int styleIndex = 0;
    bool allCaps = false;
    int shadowcolor = 0;
    float dx = 0, dy = 0, r = 0;
    
    bool editable = getDefaultEditable();
    string inputMethod;
    int numeric = 0;
    string digits;
    bool phone = false;
    bool autotext = false;
    int autocap = -1;
    int buffertype = 0;
    bool selectallonfocus = false;
    shared_ptr<Drawable> drawableLeft, drawableTop, drawableRight, drawableBottom, drawableStart, drawableEnd;
    int drawablePadding = 0;
    int ellipsize = -1;
    bool singleLine = false;
    int maxlength = -1;
    shared_ptr<CharSequence> text;
    shared_ptr<CharSequence> hint;
    bool password = false;
    
    bool focusable = mMovement != NULL || getKeyListener() != NULL;
    bool clickable = focusable;
    bool longClickable = focusable;
    
    attrs->loadAttributes();
    
    for (int i = 0; i < attrs->getAttributeCount(); i++) {
        
        const char *name = attrs->getAttributeName(i);
        
		if (strcmp(R::styleable::textColorHighlight, name) == 0) {
            textColorHighlight = attrs->getAttributeColorValue(res, name, textColorHighlight);
		}
        
        if (strcmp(R::styleable::textColor, name) == 0) {
            textColor = attrs->getAttributeColorStateList(res, name);
		}
        
        if (strcmp(R::styleable::textColorHint, name) == 0) {
            textColorHint = attrs->getAttributeColorStateList(res, name);
		}
        
        if (strcmp(R::styleable::textColorLink, name) == 0) {
            textColorLink = attrs->getAttributeColorStateList(res, name);
		}
        
        if (strcmp(R::styleable::textSize, name) == 0) {
            textSize = attrs->getDimensionValue(res, name, textSize);
		}
        
        if (strcmp(R::styleable::typeface, name) == 0) {
            typefaceIndex = attrs->getAttributeIntValue(name, -1);
		}
        
        if (strcmp(R::styleable::fontFamily, name) == 0) {
            fontFamily = string(attrs->getAttributeValue(name));
		}
        
        if (strcmp(R::styleable::textStyle, name) == 0) {
            std::string style(attrs->getAttributeValue(name));
            styleIndex = Typeface::getStyle(style, Typeface::NORMAL);
		}
        
        if (strcmp(R::styleable::textAllCaps, name) == 0) {
            allCaps = attrs->getAttributeBooleanValue(name, false);
		}
        
        if (strcmp(R::styleable::shadowColor, name) == 0) {
            shadowcolor = attrs->getAttributeColorValue(res, name, 0);
		}
        
        if (strcmp(R::styleable::shadowDx, name) == 0) {
            dx = attrs->getAttributeFloatValue(name, 0.0f);
		}
        
        if (strcmp(R::styleable::shadowDy, name) == 0) {
            dy = attrs->getAttributeFloatValue(name, 0.0f);
		}
        
        if (strcmp(R::styleable::shadowRadius, name) == 0) {
            r = attrs->getAttributeFloatValue(name, 0.0f);
		}
        
        if (strcmp(R::styleable::text, name) == 0) {
            text = make_shared<String>(attrs->getAttributeUnicodeString(name));
		}
        
        if (strcmp(R::styleable::hint, name) == 0) {
            hint = make_shared<String>(attrs->getAttributeUnicodeString(name));
		}
        
        if (strcmp(R::styleable::maxLines, name) == 0) {
            setMaxLines(attrs->getAttributeIntValue(name, -1));
        }
        
        if (strcmp(R::styleable::lines, name) == 0) {
            setLines(attrs->getAttributeIntValue(name, -1));
        }
        
        if (strcmp(R::styleable::singleLine, name) == 0) {
            singleLine = attrs->getAttributeBooleanValue(name, singleLine);
        }
        
        if (strcmp(R::styleable::gravity, name) == 0) {
            std::string gravity(attrs->getAttributeValue(name));
            setGravity(Gravity::getGravity(gravity, -1));
        }
	}
//    int inputType = EditorInfo::TYPE_NULL;
//    
//    int n = a.getIndexCount();
//    for (int i = 0; i < n; i++) {
//        int attr = a.getIndex(i);
//        
//        switch (attr) {
//            case com.android.internal.R.styleable.TextView_editable:
//                editable = a.getBoolean(attr, editable);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_inputMethod:
//                inputMethod = a.getText(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_numeric:
//                numeric = a.getInt(attr, numeric);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_digits:
//                digits = a.getText(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_phoneNumber:
//                phone = a.getBoolean(attr, phone);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_autoText:
//                autotext = a.getBoolean(attr, autotext);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_capitalize:
//                autocap = a.getInt(attr, autocap);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_bufferType:
//                buffertype = a.getInt(attr, buffertype);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_selectAllOnFocus:
//                selectallonfocus = a.getBoolean(attr, selectallonfocus);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_autoLink:
//                mAutoLinkMask = a.getInt(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_linksClickable:
//                mLinksClickable = a.getBoolean(attr, true);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_drawableLeft:
//                drawableLeft = a.getDrawable(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_drawableTop:
//                drawableTop = a.getDrawable(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_drawableRight:
//                drawableRight = a.getDrawable(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_drawableBottom:
//                drawableBottom = a.getDrawable(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_drawableStart:
//                drawableStart = a.getDrawable(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_drawableEnd:
//                drawableEnd = a.getDrawable(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_drawablePadding:
//                drawablePadding = a.getDimensionPixelSize(attr, drawablePadding);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_maxLines:
//                setMaxLines(a.getInt(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_maxHeight:
//                setMaxHeight(a.getDimensionPixelSize(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_lines:
//                setLines(a.getInt(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_height:
//                setHeight(a.getDimensionPixelSize(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_minLines:
//                setMinLines(a.getInt(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_minHeight:
//                setMinHeight(a.getDimensionPixelSize(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_maxEms:
//                setMaxEms(a.getInt(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_maxWidth:
//                setMaxWidth(a.getDimensionPixelSize(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_ems:
//                setEms(a.getInt(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_width:
//                setWidth(a.getDimensionPixelSize(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_minEms:
//                setMinEms(a.getInt(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_minWidth:
//                setMinWidth(a.getDimensionPixelSize(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_gravity:
//                setGravity(a.getInt(attr, -1));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_hint:
//                hint = a.getText(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_text:
//                text = a.getText(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_scrollHorizontally:
//                if (a.getBoolean(attr, false)) {
//                    setHorizontallyScrolling(true);
//                }
//                break;
//                
//            case com.android.internal.R.styleable.TextView_singleLine:
//                singleLine = a.getBoolean(attr, singleLine);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_ellipsize:
//                ellipsize = a.getInt(attr, ellipsize);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_marqueeRepeatLimit:
//                setMarqueeRepeatLimit(a.getInt(attr, mMarqueeRepeatLimit));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_includeFontPadding:
//                if (!a.getBoolean(attr, true)) {
//                    setIncludeFontPadding(false);
//                }
//                break;
//                
//            case com.android.internal.R.styleable.TextView_cursorVisible:
//                if (!a.getBoolean(attr, true)) {
//                    setCursorVisible(false);
//                }
//                break;
//                
//            case com.android.internal.R.styleable.TextView_maxLength:
//                maxlength = a.getInt(attr, -1);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textScaleX:
//                setTextScaleX(a.getFloat(attr, 1.0f));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_freezesText:
//                mFreezesText = a.getBoolean(attr, false);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_shadowColor:
//                shadowcolor = a.getInt(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_shadowDx:
//                dx = a.getFloat(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_shadowDy:
//                dy = a.getFloat(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_shadowRadius:
//                r = a.getFloat(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_enabled:
//                setEnabled(a.getBoolean(attr, isEnabled()));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textColorHighlight:
//                textColorHighlight = a.getColor(attr, textColorHighlight);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textColor:
//                textColor = a.getColorStateList(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textColorHint:
//                textColorHint = a.getColorStateList(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textColorLink:
//                textColorLink = a.getColorStateList(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textSize:
//                textSize = a.getDimensionPixelSize(attr, textSize);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_typeface:
//                typefaceIndex = a.getInt(attr, typefaceIndex);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textStyle:
//                styleIndex = a.getInt(attr, styleIndex);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_fontFamily:
//                fontFamily = a.getstring(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_password:
//                password = a.getBoolean(attr, password);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_lineSpacingExtra:
//                mSpacingAdd = a.getDimensionPixelSize(attr, (int) mSpacingAdd);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_lineSpacingMultiplier:
//                mSpacingMult = a.getFloat(attr, mSpacingMult);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_inputType:
//                inputType = a.getInt(attr, EditorInfo::TYPE_NULL);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_imeOptions:
//                createEditorIfNeeded();
//                mEditor.createInputContentTypeIfNeeded();
//                mEditor.mInputContentType.imeOptions = a.getInt(attr,
//                                                                mEditor.mInputContentType.imeOptions);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_imeActionLabel:
//                createEditorIfNeeded();
//                mEditor.createInputContentTypeIfNeeded();
//                mEditor.mInputContentType.imeActionLabel = a.getText(attr);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_imeActionId:
//                createEditorIfNeeded();
//                mEditor.createInputContentTypeIfNeeded();
//                mEditor.mInputContentType.imeActionId = a.getInt(attr,
//                                                                 mEditor.mInputContentType.imeActionId);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_privateImeOptions:
//                setPrivateImeOptions(a.getstring(attr));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_editorExtras:
//                try {
//                    setInputExtras(a.getResourceId(attr, 0));
//                } catch (XmlPullParserException e) {
//                    Log.w(LOG_TAG, "Failure reading input extras", e);
//                } catch (IOException e) {
//                    Log.w(LOG_TAG, "Failure reading input extras", e);
//                }
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textCursorDrawable:
//                mCursorDrawableRes = a.getResourceId(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textSelectHandleLeft:
//                mTextSelectHandleLeftRes = a.getResourceId(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textSelectHandleRight:
//                mTextSelectHandleRightRes = a.getResourceId(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textSelectHandle:
//                mTextSelectHandleRes = a.getResourceId(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textEditSuggestionItemLayout:
//                mTextEditSuggestionItemLayout = a.getResourceId(attr, 0);
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textIsSelectable:
//                setTextIsSelectable(a.getBoolean(attr, false));
//                break;
//                
//            case com.android.internal.R.styleable.TextView_textAllCaps:
//                allCaps = a.getBoolean(attr, false);
//                break;
//        }
//    }
//    a.recycle();
//    
//    BufferType bufferType = BufferType::EDITABLE;
//    
//    const int variation = inputType & (EditorInfo::TYPE_MASK_CLASS | EditorInfo::TYPE_MASK_VARIATION);
//    const bool passwordInputType = variation == (EditorInfo::TYPE_CLASS_TEXT | EditorInfo::TYPE_TEXT_VARIATION_PASSWORD);
//    const bool webPasswordInputType = variation == (EditorInfo::TYPE_CLASS_TEXT | EditorInfo::TYPE_TEXT_VARIATION_WEB_PASSWORD);
//    const bool numberPasswordInputType = variation == (EditorInfo::TYPE_CLASS_NUMBER | EditorInfo::TYPE_NUMBER_VARIATION_PASSWORD);
//    
//    if (inputMethod != NULL) {
//        Class<?> c;
//        
//        try {
//            c = Class.forName(inputMethod.tostring());
//        } catch (ClassNotFoundException ex) {
//            throw new RuntimeException(ex);
//        }
//        
//        try {
//            createEditorIfNeeded();
//            mEditor.mKeyListener = (KeyListener) c.newInstance();
//        } catch (InstantiationException ex) {
//            throw new RuntimeException(ex);
//        } catch (IllegalAccessException ex) {
//            throw new RuntimeException(ex);
//        }
//        try {
//            mEditor.mInputType = inputType != EditorInfo::TYPE_NULL
//            ? inputType
//            : mEditor.mKeyListener.getInputType();
//        } catch (IncompatibleClassChangeError e) {
//            mEditor.mInputType = EditorInfo::TYPE_CLASS_TEXT;
//        }
//    } else if (digits != NULL) {
//        createEditorIfNeeded();
//        mEditor.mKeyListener = DigitsKeyListener.getInstance(digits.tostring());
//        // If no input type was specified, we will default to generic
//        // text, since we can't tell the IME about the set of digits
//        // that was selected.
//        mEditor.mInputType = inputType != EditorInfo::TYPE_NULL
//        ? inputType : EditorInfo::TYPE_CLASS_TEXT;
//    } else if (inputType != EditorInfo::TYPE_NULL) {
//        setInputType(inputType, true);
//        // If set, the input type overrides what was set using the deprecated singleLine flag.
//        singleLine = !isMultilineInputType(inputType);
//    } else if (phone) {
//        createEditorIfNeeded();
//        mEditor.mKeyListener = DialerKeyListener.getInstance();
//        mEditor.mInputType = inputType = EditorInfo::TYPE_CLASS_PHONE;
//    } else if (numeric != 0) {
//        createEditorIfNeeded();
//        mEditor.mKeyListener = DigitsKeyListener.getInstance((numeric & SIGNED) != 0,
//                                                             (numeric & DECIMAL) != 0);
//        inputType = EditorInfo::TYPE_CLASS_NUMBER;
//        if ((numeric & SIGNED) != 0) {
//            inputType |= EditorInfo::TYPE_NUMBER_FLAG_SIGNED;
//        }
//        if ((numeric & DECIMAL) != 0) {
//            inputType |= EditorInfo::TYPE_NUMBER_FLAG_DECIMAL;
//        }
//        mEditor.mInputType = inputType;
//    } else if (autotext || autocap != -1) {
//        TextKeyListener.Capitalize cap;
//        
//        inputType = EditorInfo::TYPE_CLASS_TEXT;
//        
//        switch (autocap) {
//            case 1:
//                cap = TextKeyListener.Capitalize.SENTENCES;
//                inputType |= EditorInfo::TYPE_TEXT_FLAG_CAP_SENTENCES;
//                break;
//                
//            case 2:
//                cap = TextKeyListener.Capitalize.WORDS;
//                inputType |= EditorInfo::TYPE_TEXT_FLAG_CAP_WORDS;
//                break;
//                
//            case 3:
//                cap = TextKeyListener.Capitalize.CHARACTERS;
//                inputType |= EditorInfo::TYPE_TEXT_FLAG_CAP_CHARACTERS;
//                break;
//                
//            default:
//                cap = TextKeyListener.Capitalize.NONE;
//                break;
//        }
//        
//        createEditorIfNeeded();
//        mEditor.mKeyListener = TextKeyListener.getInstance(autotext, cap);
//        mEditor.mInputType = inputType;
//    } else if (isTextSelectable()) {
//        // Prevent text changes from keyboard.
//        if (mEditor != NULL) {
//            mEditor.mKeyListener = NULL;
//            mEditor.mInputType = EditorInfo::TYPE_NULL;
//        }
//        bufferType = BufferType::SPANNABLE;
//        // So that selection can be changed using arrow keys and touch is handled.
//        setMovementMethod(ArrowKeyMovementMethod.getInstance());
//    } else if (editable) {
//        createEditorIfNeeded();
//        mEditor.mKeyListener = TextKeyListener.getInstance();
//        mEditor.mInputType = EditorInfo::TYPE_CLASS_TEXT;
//    } else {
//        if (mEditor != NULL) mEditor.mKeyListener = NULL;
//        
//        switch (buffertype) {
//            case 0:
//                bufferType = BufferType::NORMAL;
//                break;
//            case 1:
//                bufferType = BufferType::SPANNABLE;
//                break;
//            case 2:
//                bufferType = BufferType::EDITABLE;
//                break;
//        }
//    }
//    
//    if (mEditor != NULL) mEditor.adjustInputType(password, passwordInputType,
//                                                 webPasswordInputType, numberPasswordInputType);
//    
//    if (selectallonfocus) {
//        createEditorIfNeeded();
//        mEditor.mSelectAllOnFocus = true;
//        
//        if (bufferType == BufferType::NORMAL)
//            bufferType = BufferType::SPANNABLE;
//    }
    
    // This call will save the initial left/right drawables
    setCompoundDrawablesWithIntrinsicBounds(drawableLeft, drawableTop, drawableRight, drawableBottom);
    setRelativeDrawablesIfNeeded(drawableStart, drawableEnd);
    setCompoundDrawablePadding(drawablePadding);
    
    // Same as setSingleLine(), but make sure the transformation method and the maximum number
    // of lines of height are unchanged for multi-line TextViews.
    setInputTypeSingleLine(singleLine);
    applySingleLine(singleLine, singleLine, singleLine);
    
    if (singleLine && getKeyListener() == NULL && ellipsize < 0) {
        ellipsize = 3; // END
    }
    
    switch (ellipsize) {
        case 1:
            setEllipsize(TextUtils::TruncateAt::START);
            break;
        case 2:
            setEllipsize(TextUtils::TruncateAt::MIDDLE);
            break;
        case 3:
            setEllipsize(TextUtils::TruncateAt::END);
            break;
        case 4:
//            if (ViewConfiguration::get(context).isFadingMarqueeEnabled()) {
//                setHorizontalFadingEdgeEnabled(true);
//                mMarqueeFadeMode = MARQUEE_FADE_NORMAL;
//            } else {
//                setHorizontalFadingEdgeEnabled(false);
//                mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS;
//            }
//            setEllipsize(TextUtils::TruncateAt::MARQUEE);
            break;
    }
    
    setTextColor(textColor != NULL ? textColor : ColorStateList::valueOf(0xFF000000));
    setHintTextColor(textColorHint);
    setLinkTextColor(textColorLink);
    if (textColorHighlight != 0) {
        setHighlightColor(textColorHighlight);
    }
    setRawTextSize(textSize);
    
//    if (allCaps) {
//        setTransformationMethod(new AllCapsTransformationMethod(getContext()));
//    }
//    
//    if (password || passwordInputType || webPasswordInputType || numberPasswordInputType) {
//        setTransformationMethod(PasswordTransformationMethod.getInstance());
//        typefaceIndex = MONOSPACE;
//    } else if (mEditor != NULL &&
//               (mEditor.mInputType & (EditorInfo::TYPE_MASK_CLASS | EditorInfo::TYPE_MASK_VARIATION))
//               == (EditorInfo::TYPE_CLASS_TEXT | EditorInfo::TYPE_TEXT_VARIATION_PASSWORD)) {
//        typefaceIndex = MONOSPACE;
//    }
    
    setTypefaceFromAttrs(fontFamily, typefaceIndex, styleIndex);
    
    if (shadowcolor != 0) {
        setShadowLayer(r, dx, dy, shadowcolor);
    }
    
    if (maxlength >= 0) {
        setFilters({ make_shared<InputFilter::LengthFilter>(maxlength) });
    } else {
        setFilters(NO_FILTERS);
    }
    
    setText(text); //setText(text, bufferType);
    if (hint != NULL) setHint(hint);
    
//    n = a.getIndexCount();
//    for (int i = 0; i < n; i++) {
//        int attr = a.getIndex(i);
//        
//        switch (attr) {
//            case com.android.internal.R.styleable.View_focusable:
//                focusable = a.getBoolean(attr, focusable);
//                break;
//                
//            case com.android.internal.R.styleable.View_clickable:
//                clickable = a.getBoolean(attr, clickable);
//                break;
//                
//            case com.android.internal.R.styleable.View_longClickable:
//                longClickable = a.getBoolean(attr, longClickable);
//                break;
//        }
//    }
//    a.recycle();
    
//    setFocusable(focusable);
    setClickable(clickable);
    setLongClickable(longClickable);
    
//    if (mEditor != NULL) mEditor.prepareCursorControllers();
    
    // If not explicitly specified this view is important for accessibility.
//    if (getImportantForAccessibility() == IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
//        setImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_YES);
//    }
}

void TextView::setTypefaceFromAttrs(string familyName, int typefaceIndex, int styleIndex) {
    Typeface *tf = NULL;
    if (familyName != NULL) {
        tf = Typeface::create(familyName.c_str(), (Typeface::Style) styleIndex);
        if (tf != NULL) {
            setTypeface(tf);
            return;
        }
    }
    
    Typeface::initialize();
    
    switch (typefaceIndex) {
        case SANS:
            tf = Typeface::SANS_SERIF;
            break;
            
        case SERIF:
            tf = Typeface::SERIF;
            break;
            
        case MONOSPACE:
            tf = Typeface::MONOSPACE;
            break;
    }
    
    setTypeface(tf, styleIndex);
}

void TextView::setRelativeDrawablesIfNeeded(shared_ptr<Drawable> start, shared_ptr<Drawable> end) {
    bool hasRelativeDrawables = (start != NULL) || (end != NULL);
    if (hasRelativeDrawables) {
        shared_ptr<Drawables> &dr = mDrawables;
        if (dr == NULL) {
            mDrawables = dr = make_shared<Drawables>(getContext());
        }
        mDrawables->mOverride = true;
        Rect compoundRect = dr->mCompoundRect;
        vector<int> state = getDrawableState();
        if (start != NULL) {
            start->setBounds(0, 0, start->getIntrinsicWidth(), start->getIntrinsicHeight());
            start->setState(state);
            start->copyBounds(compoundRect);
            start->setCallback(this);
            
            dr->mDrawableStart = start;
            dr->mDrawableSizeStart = compoundRect.width();
            dr->mDrawableHeightStart = compoundRect.height();
        } else {
            dr->mDrawableSizeStart = dr->mDrawableHeightStart = 0;
        }
        if (end != NULL) {
            end->setBounds(0, 0, end->getIntrinsicWidth(), end->getIntrinsicHeight());
            end->setState(state);
            end->copyBounds(compoundRect);
            end->setCallback(this);
            
            dr->mDrawableEnd = end;
            dr->mDrawableSizeEnd = compoundRect.width();
            dr->mDrawableHeightEnd = compoundRect.height();
        } else {
            dr->mDrawableSizeEnd = dr->mDrawableHeightEnd = 0;
        }
    }
}

void TextView::setKeyListener(KeyListener *input) {
    setKeyListenerOnly(input);
    fixFocusableAndClickableSettings();
    
//    if (input != NULL) {
//        createEditorIfNeeded();
//        try {
//            mEditor.mInputType = mEditor.mKeyListener.getInputType();
//        } catch (IncompatibleClassChangeError e) {
//            mEditor.mInputType = EditorInfo::TYPE_CLASS_TEXT;
//        }
//        // Change inputType, without affecting transformation.
//        // No need to applySingleLine since mSingleLine is unchanged.
//        setInputTypeSingleLine(mSingleLine);
//    } else {
//        if (mEditor != NULL) mEditor.mInputType = EditorInfo::TYPE_NULL;
//    }
//    
//    InputMethodManager imm = InputMethodManager.peekInstance();
//    if (imm != NULL) imm.restartInput(this);
}

void TextView::setKeyListenerOnly(KeyListener *input) {
//    if (mEditor == NULL && input == NULL) return; // NULL is the default value
//    
//    createEditorIfNeeded();
//    if (mEditor.mKeyListener != input) {
//        mEditor.mKeyListener = input;
//        if (input != NULL && !(mText instanceof Editable)) {
//            setText(mText);
//        }
//        
//        setFilters((Editable) mText, mFilters);
//    }
}

void TextView::setMovementMethod(shared_ptr<MovementMethod> movement) {
    if (mMovement != movement) {
        mMovement = movement;
        
        if (movement != NULL && mTextSpannable == NULL) {
            setText(mText);
        }
        
        fixFocusableAndClickableSettings();
        
        // SelectionModifierCursorController depends on textCanBeSelected, which depends on
        // mMovement
//        if (mEditor != NULL) mEditor.prepareCursorControllers();
    }
}

void TextView::fixFocusableAndClickableSettings() {
//    if (mMovement != NULL || (mEditor != NULL && mEditor.mKeyListener != NULL)) {
//        setFocusable(true);
//        setClickable(true);
//        setLongClickable(true);
//    } else {
//        setFocusable(false);
        setClickable(false);
        setLongClickable(false);
//    }
}

void TextView::setTransformationMethod(shared_ptr<TransformationMethod> method) {
    if (method == mTransformation) {
        // Avoid TextView::the setText() below if the transformation is
        // the same.
        return;
    }
    if (mTransformation != NULL) {
        if (mTextSpannable != NULL) {
            mTextSpannable->removeSpan(mTransformation);
        }
    }
    
    mTransformation = method;
    
    shared_ptr<TransformationMethod2> method2 = dynamic_pointer_cast<TransformationMethod2>(method);
    if (method2 != NULL) {
        mAllowTransformationLengthChange = !isTextSelectable() && dynamic_pointer_cast<Editable>(mText) == NULL;
        method2->setLengthChangesAllowed(mAllowTransformationLengthChange);
    } else {
        mAllowTransformationLengthChange = false;
    }
    
    setText(mText);
    
    if (hasPasswordTransformationMethod()) {
//        notifyAccessibilityStateChanged();
    }
}

int TextView::getCompoundPaddingTop() {
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr == NULL || dr->mDrawableTop == NULL) {
        return mPaddingTop;
    } else {
        return mPaddingTop + dr->mDrawablePadding + dr->mDrawableSizeTop;
    }
}

int TextView::getCompoundPaddingBottom() {
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr == NULL || dr->mDrawableBottom == NULL) {
        return mPaddingBottom;
    } else {
        return mPaddingBottom + dr->mDrawablePadding + dr->mDrawableSizeBottom;
    }
}

void TextView::setEnabled(bool enabled) {
    if (enabled == isEnabled()) {
        return;
    }
    
    if (!enabled) {
        // Hide the soft input if the currently active TextView is disabled
//        InputMethodManager imm = InputMethodManager.peekInstance();
//        if (imm != NULL && imm.isActive(this)) {
//            imm.hideSoftInputFromWindow(getWindowToken(), 0);
//        }
    }
    
    View::setEnabled(enabled);
    
    if (enabled) {
        // Make sure IME is updated with current editor info.
//        InputMethodManager imm = InputMethodManager.peekInstance();
//        if (imm != NULL) imm.restartInput(this);
    }
    
    // Will change text color
//    if (mEditor != NULL) {
//        mEditor.invalidateTextDisplayList();
//        mEditor.prepareCursorControllers();
//        
//        // start or stop the cursor blinking as appropriate
//        mEditor.makeBlink();
//    }
}

void TextView::setTypeface(Typeface *tf, int style) {
    if (style > 0) {
        if (tf == NULL) {
            tf = Typeface::defaultFromStyle(style);
        } else {
            tf = Typeface::create(tf, style);
        }
        
        setTypeface(tf);
        // now compute what (if any) algorithmic styling is needed
        int typefaceStyle = tf != NULL ? tf->getStyle() : 0;
        int need = style & ~typefaceStyle;
        mTextPaint->setFakeBoldText((need & Typeface::BOLD) != 0);
        mTextPaint->setTextSkewX((need & Typeface::ITALIC) != 0 ? -0.25f : 0);
    } else {
        mTextPaint->setFakeBoldText(false);
        mTextPaint->setTextSkewX(0);
        setTypeface(tf);
    }
}

int TextView::getCompoundPaddingLeft() {
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr == NULL || dr->mDrawableLeft == NULL) {
        return mPaddingLeft;
    } else {
        return mPaddingLeft + dr->mDrawablePadding + dr->mDrawableSizeLeft;
    }
}

int TextView::getCompoundPaddingRight() {
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr == NULL || dr->mDrawableRight == NULL) {
        return mPaddingRight;
    } else {
        return mPaddingRight + dr->mDrawablePadding + dr->mDrawableSizeRight;
    }
}

int TextView::getCompoundPaddingStart() {
    resolveDrawables();
    switch(getLayoutDirection()) {
        default:
        case LAYOUT_DIRECTION_LTR:
            return getCompoundPaddingLeft();
        case LAYOUT_DIRECTION_RTL:
            return getCompoundPaddingRight();
    }
}

int TextView::getCompoundPaddingEnd() {
    resolveDrawables();
    switch(getLayoutDirection()) {
        default:
        case LAYOUT_DIRECTION_LTR:
            return getCompoundPaddingRight();
        case LAYOUT_DIRECTION_RTL:
            return getCompoundPaddingLeft();
    }
}

int TextView::getExtendedPaddingTop() {
    if (mMaxMode != LINES) {
        return getCompoundPaddingTop();
    }
    
    if (mLayout->getLineCount() <= mMaximum) {
        return getCompoundPaddingTop();
    }
    
    int top = getCompoundPaddingTop();
    int bottom = getCompoundPaddingBottom();
    int viewht = getHeight() - top - bottom;
    int layoutht = mLayout->getLineTop(mMaximum);
    
    if (layoutht >= viewht) {
        return top;
    }
    
    const int gravity = mGravity & Gravity::VERTICAL_GRAVITY_MASK;
    if (gravity == Gravity::TOP) {
        return top;
    } else if (gravity == Gravity::BOTTOM) {
        return top + viewht - layoutht;
    } else { // (gravity == Gravity::CENTER_VERTICAL)
        return top + (viewht - layoutht) / 2;
    }
}

int TextView::getExtendedPaddingBottom() {
    if (mMaxMode != LINES) {
        return getCompoundPaddingBottom();
    }
    
    if (mLayout->getLineCount() <= mMaximum) {
        return getCompoundPaddingBottom();
    }
    
    int top = getCompoundPaddingTop();
    int bottom = getCompoundPaddingBottom();
    int viewht = getHeight() - top - bottom;
    int layoutht = mLayout->getLineTop(mMaximum);
    
    if (layoutht >= viewht) {
        return bottom;
    }
    
    const int gravity = mGravity & Gravity::VERTICAL_GRAVITY_MASK;
    if (gravity == Gravity::TOP) {
        return bottom + viewht - layoutht;
    } else if (gravity == Gravity::BOTTOM) {
        return bottom;
    } else { // (gravity == Gravity::CENTER_VERTICAL)
        return bottom + (viewht - layoutht) / 2;
    }
}

void TextView::setCompoundDrawables(shared_ptr<Drawable> left, shared_ptr<Drawable> top,
                                 shared_ptr<Drawable> right, shared_ptr<Drawable> bottom) {
    shared_ptr<Drawables> &dr = mDrawables;
    
    const bool drawables = left != NULL || top != NULL
    || right != NULL || bottom != NULL;
    
    if (!drawables) {
        // Clearing drawables->..  can we free the data structure?
        if (dr != NULL) {
            if (dr->mDrawablePadding == 0) {
                mDrawables = NULL;
            } else {
                // We need to retain the last set padding, so just clear
                // out all of the fields in the existing structure.
                if (dr->mDrawableLeft != NULL) dr->mDrawableLeft->setCallback(NULL);
                dr->mDrawableLeft = NULL;
                if (dr->mDrawableTop != NULL) dr->mDrawableTop->setCallback(NULL);
                dr->mDrawableTop = NULL;
                if (dr->mDrawableRight != NULL) dr->mDrawableRight->setCallback(NULL);
                dr->mDrawableRight = NULL;
                if (dr->mDrawableBottom != NULL) dr->mDrawableBottom->setCallback(NULL);
                dr->mDrawableBottom = NULL;
                dr->mDrawableSizeLeft = dr->mDrawableHeightLeft = 0;
                dr->mDrawableSizeRight = dr->mDrawableHeightRight = 0;
                dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
                dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
            }
        }
    } else {
        if (dr == NULL) {
            mDrawables = dr = make_shared<Drawables>(getContext());
        }
        
        mDrawables->mOverride = false;
        
        if (dr->mDrawableLeft != left && dr->mDrawableLeft != NULL) {
            dr->mDrawableLeft->setCallback(NULL);
        }
        dr->mDrawableLeft = left;
        
        if (dr->mDrawableTop != top && dr->mDrawableTop != NULL) {
            dr->mDrawableTop->setCallback(NULL);
        }
        dr->mDrawableTop = top;
        
        if (dr->mDrawableRight != right && dr->mDrawableRight != NULL) {
            dr->mDrawableRight->setCallback(NULL);
        }
        dr->mDrawableRight = right;
        
        if (dr->mDrawableBottom != bottom && dr->mDrawableBottom != NULL) {
            dr->mDrawableBottom->setCallback(NULL);
        }
        dr->mDrawableBottom = bottom;
        
        Rect compoundRect = dr->mCompoundRect;
        vector<int> state;
        
        state = getDrawableState();
        
        if (left != NULL) {
            left->setState(state);
            left->copyBounds(compoundRect);
            left->setCallback(this);
            dr->mDrawableSizeLeft = compoundRect.width();
            dr->mDrawableHeightLeft = compoundRect.height();
        } else {
            dr->mDrawableSizeLeft = dr->mDrawableHeightLeft = 0;
        }
        
        if (right != NULL) {
            right->setState(state);
            right->copyBounds(compoundRect);
            right->setCallback(this);
            dr->mDrawableSizeRight = compoundRect.width();
            dr->mDrawableHeightRight = compoundRect.height();
        } else {
            dr->mDrawableSizeRight = dr->mDrawableHeightRight = 0;
        }
        
        if (top != NULL) {
            top->setState(state);
            top->copyBounds(compoundRect);
            top->setCallback(this);
            dr->mDrawableSizeTop = compoundRect.height();
            dr->mDrawableWidthTop = compoundRect.width();
        } else {
            dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
        }
        
        if (bottom != NULL) {
            bottom->setState(state);
            bottom->copyBounds(compoundRect);
            bottom->setCallback(this);
            dr->mDrawableSizeBottom = compoundRect.height();
            dr->mDrawableWidthBottom = compoundRect.width();
        } else {
            dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
        }
    }
    
    // Save initial left/right drawables
    if (dr != NULL) {
        dr->mDrawableLeftInitial = left;
        dr->mDrawableRightInitial = right;
    }
    
    invalidate();
    requestLayout();
}

void TextView::setCompoundDrawablesWithIntrinsicBounds(int left, int top, int right, int bottom) {
//    Resources *resources = getContext()->getResources();
//    setCompoundDrawablesWithIntrinsicBounds(left != 0 ? resources->getDrawable(left) : NULL,
//                                            top != 0 ? resources->getDrawable(top) : NULL,
//                                            right != 0 ? resources->getDrawable(right) : NULL,
//                                            bottom != 0 ? resources->getDrawable(bottom) : NULL);
}

void TextView::setCompoundDrawablesWithIntrinsicBounds(shared_ptr<Drawable> left, shared_ptr<Drawable> top,
                                                    shared_ptr<Drawable> right, shared_ptr<Drawable> bottom) {
    
    if (left != NULL) {
        left->setBounds(0, 0, left->getIntrinsicWidth(), left->getIntrinsicHeight());
    }
    if (right != NULL) {
        right->setBounds(0, 0, right->getIntrinsicWidth(), right->getIntrinsicHeight());
    }
    if (top != NULL) {
        top->setBounds(0, 0, top->getIntrinsicWidth(), top->getIntrinsicHeight());
    }
    if (bottom != NULL) {
        bottom->setBounds(0, 0, bottom->getIntrinsicWidth(), bottom->getIntrinsicHeight());
    }
    setCompoundDrawables(left, top, right, bottom);
}

void TextView::setCompoundDrawablesRelative(shared_ptr<Drawable> start, shared_ptr<Drawable> top,
                                         shared_ptr<Drawable> end, shared_ptr<Drawable> bottom) {
    shared_ptr<Drawables> &dr = mDrawables;
    
    const bool drawables = start != NULL || top != NULL
    || end != NULL || bottom != NULL;
    
    if (!drawables) {
        // Clearing drawables->..  can we free the data structure?
        if (dr != NULL) {
            if (dr->mDrawablePadding == 0) {
                mDrawables = NULL;
            } else {
                // We need to retain the last set padding, so just clear
                // out all of the fields in the existing structure.
                if (dr->mDrawableStart != NULL) dr->mDrawableStart->setCallback(NULL);
                dr->mDrawableStart = NULL;
                if (dr->mDrawableTop != NULL) dr->mDrawableTop->setCallback(NULL);
                dr->mDrawableTop = NULL;
                if (dr->mDrawableEnd != NULL) dr->mDrawableEnd->setCallback(NULL);
                dr->mDrawableEnd = NULL;
                if (dr->mDrawableBottom != NULL) dr->mDrawableBottom->setCallback(NULL);
                dr->mDrawableBottom = NULL;
                dr->mDrawableSizeStart = dr->mDrawableHeightStart = 0;
                dr->mDrawableSizeEnd = dr->mDrawableHeightEnd = 0;
                dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
                dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
            }
        }
    } else {
        if (dr == NULL) {
            mDrawables = dr = make_shared<Drawables>(getContext());
        }
        
        mDrawables->mOverride = true;
        
        if (dr->mDrawableStart != start && dr->mDrawableStart != NULL) {
            dr->mDrawableStart->setCallback(NULL);
        }
        dr->mDrawableStart = start;
        
        if (dr->mDrawableTop != top && dr->mDrawableTop != NULL) {
            dr->mDrawableTop->setCallback(NULL);
        }
        dr->mDrawableTop = top;
        
        if (dr->mDrawableEnd != end && dr->mDrawableEnd != NULL) {
            dr->mDrawableEnd->setCallback(NULL);
        }
        dr->mDrawableEnd = end;
        
        if (dr->mDrawableBottom != bottom && dr->mDrawableBottom != NULL) {
            dr->mDrawableBottom->setCallback(NULL);
        }
        dr->mDrawableBottom = bottom;
        
        Rect compoundRect = dr->mCompoundRect;
        vector<int> state;
        
        state = getDrawableState();
        
        if (start != NULL) {
            start->setState(state);
            start->copyBounds(compoundRect);
            start->setCallback(this);
            dr->mDrawableSizeStart = compoundRect.width();
            dr->mDrawableHeightStart = compoundRect.height();
        } else {
            dr->mDrawableSizeStart = dr->mDrawableHeightStart = 0;
        }
        
        if (end != NULL) {
            end->setState(state);
            end->copyBounds(compoundRect);
            end->setCallback(this);
            dr->mDrawableSizeEnd = compoundRect.width();
            dr->mDrawableHeightEnd = compoundRect.height();
        } else {
            dr->mDrawableSizeEnd = dr->mDrawableHeightEnd = 0;
        }
        
        if (top != NULL) {
            top->setState(state);
            top->copyBounds(compoundRect);
            top->setCallback(this);
            dr->mDrawableSizeTop = compoundRect.height();
            dr->mDrawableWidthTop = compoundRect.width();
        } else {
            dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
        }
        
        if (bottom != NULL) {
            bottom->setState(state);
            bottom->copyBounds(compoundRect);
            bottom->setCallback(this);
            dr->mDrawableSizeBottom = compoundRect.height();
            dr->mDrawableWidthBottom = compoundRect.width();
        } else {
            dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
        }
    }
    
    resetResolvedDrawables();
    resolveDrawables();
    invalidate();
    requestLayout();
}

void TextView::setCompoundDrawablesRelativeWithIntrinsicBounds(int start, int top, int end,
                                                            int bottom) {
//    Resources *resources = getContext()->getResources();
//    setCompoundDrawablesRelativeWithIntrinsicBounds(
//                                                    start != 0 ? resources->getDrawable(start) : NULL,
//                                                    top != 0 ? resources->getDrawable(top) : NULL,
//                                                    end != 0 ? resources->getDrawable(end) : NULL,
//                                                    bottom != 0 ? resources->getDrawable(bottom) : NULL);
}

void TextView::setCompoundDrawablesRelativeWithIntrinsicBounds(shared_ptr<Drawable> start, shared_ptr<Drawable> top,
                                                            shared_ptr<Drawable> end, shared_ptr<Drawable> bottom) {
    
    if (start != NULL) {
        start->setBounds(0, 0, start->getIntrinsicWidth(), start->getIntrinsicHeight());
    }
    if (end != NULL) {
        end->setBounds(0, 0, end->getIntrinsicWidth(), end->getIntrinsicHeight());
    }
    if (top != NULL) {
        top->setBounds(0, 0, top->getIntrinsicWidth(), top->getIntrinsicHeight());
    }
    if (bottom != NULL) {
        bottom->setBounds(0, 0, bottom->getIntrinsicWidth(), bottom->getIntrinsicHeight());
    }
    setCompoundDrawablesRelative(start, top, end, bottom);
}

vector<shared_ptr<Drawable>> TextView::getCompoundDrawables() {
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr != NULL) {
        return {
            dr->mDrawableLeft, dr->mDrawableTop, dr->mDrawableRight, dr->mDrawableBottom
        };
    } else {
        return { NULL, NULL, NULL, NULL };
    }
}

vector<shared_ptr<Drawable>> TextView::getCompoundDrawablesRelative() {
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr != NULL) {
        return {
            dr->mDrawableStart, dr->mDrawableTop, dr->mDrawableEnd, dr->mDrawableBottom
        };
    } else {
        return { NULL, NULL, NULL, NULL };
    }
}

void TextView::setCompoundDrawablePadding(int pad) {
    shared_ptr<Drawables> &dr = mDrawables;
    if (pad == 0) {
        if (dr != NULL) {
            dr->mDrawablePadding = pad;
        }
    } else {
        if (dr == NULL) {
            mDrawables = dr = make_shared<Drawables>(getContext());
        }
        dr->mDrawablePadding = pad;
    }
    
    invalidate();
    requestLayout();
}

int TextView::getCompoundDrawablePadding() {
    shared_ptr<Drawables> &dr = mDrawables;
    return dr != NULL ? dr->mDrawablePadding : 0;
}

void TextView::setPadding(int left, int top, int right, int bottom) {
    if (left != mPaddingLeft ||
        right != mPaddingRight ||
        top != mPaddingTop ||
        bottom != mPaddingBottom) {
        nullLayouts();
    }
    
    // the super call will requestLayout()
    View::setPadding(left, top, right, bottom);
    invalidate();
}

void TextView::setPaddingRelative(int start, int top, int end, int bottom) {
    if (start != getPaddingStart() ||
        end != getPaddingEnd() ||
        top != mPaddingTop ||
        bottom != mPaddingBottom) {
        nullLayouts();
    }
    
    // the super call will requestLayout()
    View::setPaddingRelative(start, top, end, bottom);
    invalidate();
}

void TextView::setTextAppearance(Context *context, int resid) {
//    TypedArray appearance =
//    context->obtainStyledAttributes(resid,
//                                   com.android.internal.R.styleable.TextAppearance);
//    
//    int color;
//    ColorStateList colors;
//    int ts;
//    
//    color = appearance.getColor(
//                                com.android.internal.R.styleable.TextAppearance_textColorHighlight, 0);
//    if (color != 0) {
//        setHighlightColor(color);
//    }
//    
//    colors = appearance.getColorStateList(com.android.internal.R.styleable.
//                                          TextAppearance_textColor);
//    if (colors != NULL) {
//        setTextColor(colors);
//    }
//    
//    ts = appearance.getDimensionPixelSize(com.android.internal.R.styleable.
//                                          TextAppearance_textSize, 0);
//    if (ts != 0) {
//        setRawTextSize(ts);
//    }
//    
//    colors = appearance.getColorStateList(com.android.internal.R.styleable.
//                                          TextAppearance_textColorHint);
//    if (colors != NULL) {
//        setHintTextColor(colors);
//    }
//    
//    colors = appearance.getColorStateList(com.android.internal.R.styleable.
//                                          TextAppearance_textColorLink);
//    if (colors != NULL) {
//        setLinkTextColor(colors);
//    }
//    
//    string familyName;
//    int typefaceIndex, styleIndex;
//    
//    familyName = appearance.getstring(com.android.internal.R.styleable.
//                                      TextAppearance_fontFamily);
//    typefaceIndex = appearance.getInt(com.android.internal.R.styleable.
//                                      TextAppearance_typeface, -1);
//    styleIndex = appearance.getInt(com.android.internal.R.styleable.
//                                   TextAppearance_textStyle, -1);
//    
//    setTypefaceFromAttrs(familyName, typefaceIndex, styleIndex);
//    
//    const int shadowcolor = appearance.getInt(
//                                              com.android.internal.R.styleable.TextAppearance_shadowColor, 0);
//    if (shadowcolor != 0) {
//        const float dx = appearance.getFloat(
//                                             com.android.internal.R.styleable.TextAppearance_shadowDx, 0);
//        const float dy = appearance.getFloat(
//                                             com.android.internal.R.styleable.TextAppearance_shadowDy, 0);
//        const float r = appearance.getFloat(
//                                            com.android.internal.R.styleable.TextAppearance_shadowRadius, 0);
//        
//        setShadowLayer(r, dx, dy, shadowcolor);
//    }
//    
//    if (appearance.getBoolean(com.android.internal.R.styleable.TextAppearance_textAllCaps,
//                              false)) {
//        setTransformationMethod(new AllCapsTransformationMethod(getContext()));
//    }
//    
//    appearance.recycle();
}

void TextView::setTextSize(float size) {
    setTextSize(TypedValue::COMPLEX_UNIT_SP, size);
}

void TextView::setTextSize(int unit, float size) {
    Context *c = getContext();
    Resources *r = NULL;
    
//    if (c == NULL)
//        r = Resources.getSystem();
//    else
        r = c->getResources();
    
    setRawTextSize(TypedValue::applyDimension(unit, size, r->getDisplayMetrics()));
}

void TextView::setRawTextSize(float size) {
    if (size != mTextPaint->getTextSize()) {
        mTextPaint->setTextSize(size);
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

void TextView::setTextScaleX(float size) {
    if (size != mTextPaint->getTextScaleX()) {
        mUserSetTextScaleX = true;
        mTextPaint->setTextScaleX(size);
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

void TextView::setTypeface(Typeface *tf) {
    if (mTextPaint->getTypeface() != tf) {
        mTextPaint->setTypeface(tf);
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

void TextView::setTextColor(int color) {
    mTextColor = ColorStateList::valueOf(color);
    updateTextColors();
}

void TextView::setTextColor(shared_ptr<ColorStateList> colors) {
    if (colors == NULL) {
        throw NullPointerException();
    }
    
    mTextColor = colors;
    updateTextColors();
}

void TextView::setHighlightColor(int color) {
    if (mHighlightColor != color) {
        mHighlightColor = color;
        invalidate();
    }
}

void TextView::setShowSoftInputOnFocus(bool show) {
//    createEditorIfNeeded();
//    mEditor.mShowSoftInputOnFocus = show;
}

void TextView::setShadowLayer(float radius, float dx, float dy, int color) {
    mTextPaint->setShadowLayer(radius, dx, dy, color);
    
    mShadowRadius = radius;
    mShadowDx = dx;
    mShadowDy = dy;
    
    // Will change text clip region
//    if (mEditor != NULL) mEditor.invalidateTextDisplayList();
    invalidate();
}

void TextView::setHintTextColor(int color) {
    mHintTextColor = ColorStateList::valueOf(color);
    updateTextColors();
}

void TextView::setHintTextColor(shared_ptr<ColorStateList> colors) {
    mHintTextColor = colors;
    updateTextColors();
}

void TextView::setLinkTextColor(int color) {
    mLinkTextColor = ColorStateList::valueOf(color);
    updateTextColors();
}

void TextView::setLinkTextColor(shared_ptr<ColorStateList> colors) {
    mLinkTextColor = colors;
    updateTextColors();
}

void TextView::setGravity(int gravity) {
    if ((gravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) == 0) {
        gravity |= Gravity::START;
    }
    if ((gravity & Gravity::VERTICAL_GRAVITY_MASK) == 0) {
        gravity |= Gravity::TOP;
    }
    
    bool newLayout = false;
    
    if ((gravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) !=
        (mGravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK)) {
        newLayout = true;
    }
    
    if (gravity != mGravity) {
        invalidate();
    }
    
    mGravity = gravity;
    
    if (mLayout != NULL && newLayout) {
        // XXX this is heavy-handed because no actual content changes.
        int want = mLayout->getWidth();
        int hintWant = mHintLayout == NULL ? 0 : mHintLayout->getWidth();
        
        makeNewLayout(want, hintWant, UNKNOWN_BORING, UNKNOWN_BORING,
                      mRight - mLeft - getCompoundPaddingLeft() -
                      getCompoundPaddingRight(), true);
    }
}

void TextView::setPaintFlags(int flags) {
    if (mTextPaint->getFlags() != flags) {
        mTextPaint->setFlags(flags);
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

void TextView::setHorizontallyScrolling(bool whether) {
    if (mHorizontallyScrolling != whether) {
        mHorizontallyScrolling = whether;
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

void TextView::setMinLines(int minlines) {
    mMinimum = minlines;
    mMinMode = LINES;
    
    requestLayout();
    invalidate();
}

void TextView::setMinHeight(int minHeight) {
    mMinimum = minHeight;
    mMinMode = PIXELS;
    
    requestLayout();
    invalidate();
}

void TextView::setMaxLines(int maxlines) {
    mMaximum = maxlines;
    mMaxMode = LINES;
    
    requestLayout();
    invalidate();
}

void TextView::setMaxHeight(int maxHeight) {
    mMaximum = maxHeight;
    mMaxMode = PIXELS;
    
    requestLayout();
    invalidate();
}

void TextView::setLines(int lines) {
    mMaximum = mMinimum = lines;
    mMaxMode = mMinMode = LINES;
    
    requestLayout();
    invalidate();
}

void TextView::setHeight(int pixels) {
    mMaximum = mMinimum = pixels;
    mMaxMode = mMinMode = PIXELS;
    
    requestLayout();
    invalidate();
}

void TextView::setMinEms(int minems) {
    mMinWidth = minems;
    mMinWidthMode = EMS;
    
    requestLayout();
    invalidate();
}

void TextView::setMinWidth(int minpixels) {
    mMinWidth = minpixels;
    mMinWidthMode = PIXELS;
    
    requestLayout();
    invalidate();
}

void TextView::setMaxEms(int maxems) {
    mMaxWidth = maxems;
    mMaxWidthMode = EMS;
    
    requestLayout();
    invalidate();
}

void TextView::setMaxWidth(int maxpixels) {
    mMaxWidth = maxpixels;
    mMaxWidthMode = PIXELS;
    
    requestLayout();
    invalidate();
}

void TextView::setEms(int ems) {
    mMaxWidth = mMinWidth = ems;
    mMaxWidthMode = mMinWidthMode = EMS;
    
    requestLayout();
    invalidate();
}

void TextView::setWidth(int pixels) {
    mMaxWidth = mMinWidth = pixels;
    mMaxWidthMode = mMinWidthMode = PIXELS;
    
    requestLayout();
    invalidate();
}

void TextView::setLineSpacing(float add, float mult) {
    if (mSpacingAdd != add || mSpacingMult != mult) {
        mSpacingAdd = add;
        mSpacingMult = mult;
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

void TextView::append(string text, int start, int end) {
//    if (!(mText instanceof Editable)) {
//        setText(mText, BufferType::EDITABLE);
//    }
//    
//    ((Editable) mText).append(text, start, end);
}

void TextView::updateTextColors() {
    bool inval = false;
    int color = mTextColor->getColorForState(getDrawableState(), 0);
    if (color != mCurTextColor) {
        mCurTextColor = color;
        inval = true;
    }
    if (mLinkTextColor != NULL) {
        color = mLinkTextColor->getColorForState(getDrawableState(), 0);
        if (color != mTextPaint->linkColor) {
            mTextPaint->linkColor = color;
            inval = true;
        }
    }
    if (mHintTextColor != NULL) {
        color = mHintTextColor->getColorForState(getDrawableState(), 0);
        if (color != mCurHintTextColor && mText->length() == 0) {
            mCurHintTextColor = color;
            inval = true;
        }
    }
    if (inval) {
        // Text needs to be redrawn with the new color
//        if (mEditor != NULL) mEditor.invalidateTextDisplayList();
        invalidate();
    }
}

void TextView::drawableStateChanged() {
    View::drawableStateChanged();
    if (mTextColor != NULL && (mTextColor->isStateful()
        || (mHintTextColor != NULL && mHintTextColor->isStateful())
        || (mLinkTextColor != NULL && mLinkTextColor->isStateful()))) {
        updateTextColors();
    }
    
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr != NULL) {
        vector<int> state = getDrawableState();
        if (dr->mDrawableTop != NULL && dr->mDrawableTop->isStateful()) {
            dr->mDrawableTop->setState(state);
        }
        if (dr->mDrawableBottom != NULL && dr->mDrawableBottom->isStateful()) {
            dr->mDrawableBottom->setState(state);
        }
        if (dr->mDrawableLeft != NULL && dr->mDrawableLeft->isStateful()) {
            dr->mDrawableLeft->setState(state);
        }
        if (dr->mDrawableRight != NULL && dr->mDrawableRight->isStateful()) {
            dr->mDrawableRight->setState(state);
        }
        if (dr->mDrawableStart != NULL && dr->mDrawableStart->isStateful()) {
            dr->mDrawableStart->setState(state);
        }
        if (dr->mDrawableEnd != NULL && dr->mDrawableEnd->isStateful()) {
            dr->mDrawableEnd->setState(state);
        }
    }
}

void TextView::setFreezesText(bool freezesText) {
    mFreezesText = freezesText;
}

bool TextView::getFreezesText() {
    return mFreezesText;
}

void TextView::setText(shared_ptr<CharSequence> text) {
    setText(text, mBufferType);
}

void TextView::setText(shared_ptr<CharSequence> text, BufferType type) {
    setText(text, type, true, 0);
    
    if (mCharWrapper != NULL) {
        mCharWrapper->mChars.remove();
    }
}

void TextView::setText(shared_ptr<CharSequence> text, BufferType type,
             bool notifyBefore, int oldlen) {
    if (text == NULL) {
        text = make_shared<String>();
    }
    
    // If suggestions are not enabled, remove the suggestion spans from the text
    if (!isSuggestionsEnabled()) {
//        text = removeSuggestionSpans(text);
    }
    
    if (!mUserSetTextScaleX) mTextPaint->setTextScaleX(1.0f);
    
    shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(text);
//    if (spanned != NULL &&
//        spanned->getSpanStart(TextUtils::TruncateAt::MARQUEE) >= 0) {
//        if (ViewConfiguration.get(mContext).isFadingMarqueeEnabled()) {
//            setHorizontalFadingEdgeEnabled(true);
//            mMarqueeFadeMode = MARQUEE_FADE_NORMAL;
//        } else {
//            setHorizontalFadingEdgeEnabled(false);
//            mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS;
//        }
//        setEllipsize(TextUtils::TruncateAt::MARQUEE);
//    }
    
    int n = mFilters.size();
    for (int i = 0; i < n; i++) {
        shared_ptr<CharSequence> out = mFilters[i]->filter(text, 0, text->length(), EMPTY_SPANNED, 0, 0);
        if (out != NULL) {
            text = out;
        }
    }
    
    if (notifyBefore) {
        if (mText != NULL) {
            oldlen = mText->length();
            sendBeforeTextChanged(mText, 0, oldlen, text->length());
        } else {
            sendBeforeTextChanged(make_shared<String>(), 0, 0, text->length());
        }
    }
    
    bool needEditableForNotification = false;
    
    if (mListeners.size() != 0) {
        needEditableForNotification = true;
    }
    
    if (type == BufferType::EDITABLE || getKeyListener() != NULL ||
        needEditableForNotification) {
//        createEditorIfNeeded();
//        Editable t = mEditableFactory.newEditable(text);
//        text = t;
//        setFilters(t, mFilters);
//        InputMethodManager imm = InputMethodManager.peekInstance();
//        if (imm != NULL) imm.restartInput(this);
    } else if (type == BufferType::SPANNABLE || mMovement != NULL) {
        text = mSpannableFactory.newSpannable(text);
    } else if (dynamic_pointer_cast<CharWrapper>(text) == NULL) {
        text = TextUtils::stringOrSpannedString(text);
    }
    
    if (mAutoLinkMask != 0) {
        shared_ptr<Spannable> s2 = dynamic_pointer_cast<Spannable>(text);
        
        if (type != BufferType::EDITABLE || s2 == NULL) {
            s2 = mSpannableFactory.newSpannable(text);
        }
        
//        if (Linkify::addLinks(s2, mAutoLinkMask)) {
//            text = s2;
//            type = (type == BufferType::EDITABLE) ? BufferType::EDITABLE : BufferType::SPANNABLE;
//            
//                        mText = text;
//            
//            // Do not change the movement method for text that support text selection as it
//            // would prevent an arbitrary cursor displacement.
//            if (mLinksClickable && !textCanBeSelected()) {
//                setMovementMethod(LinkMovementMethod::getInstance());
//            }
//        }
    }
    
    mBufferType = type;
    mText = text;
    mTextSpannable = dynamic_pointer_cast<Spannable>(mText);
    
    if (mTransformation == NULL) {
        mTransformed = text;
    } else {
        mTransformed = mTransformation->getTransformation(text, this);
    }
    
    const int textLength = text->length();
    
    shared_ptr<Spannable> sp = dynamic_pointer_cast<Spannable>(text);
    if (sp != NULL && !mAllowTransformationLengthChange) {
        
        // Remove any ChangeWatchers that might have come from other TextViews.
        const vector<shared_ptr<Object>> watchers = sp->getSpans(0, sp->length(), "ChangeWatcher");
        const int count = watchers.size();
        for (int i = 0; i < count; i++) {
            sp->removeSpan(watchers[i]);
        }
        
        if (mChangeWatcher == NULL) mChangeWatcher = make_shared<ChangeWatcher>(static_pointer_cast<TextView>(shared_from_this()));
        
        sp->setSpan(mChangeWatcher, 0, textLength, Spanned::SPAN_INCLUSIVE_INCLUSIVE |
                    (CHANGE_WATCHER_PRIORITY << Spanned::SPAN_PRIORITY_SHIFT));
        
//        if (mEditor != NULL) mEditor.addSpanWatchers(sp);
        
        if (mTransformation != NULL) {
            sp->setSpan(mTransformation, 0, textLength, Spanned::SPAN_INCLUSIVE_INCLUSIVE);
        }
        
        if (mMovement != NULL) {
            mMovement->initialize(this, sp);
            
//            if (mEditor != NULL) mEditor.mSelectionMoved = false;
        }
    }
    
    if (mLayout != NULL) {
        checkForRelayout();
    }
    
    sendOnTextChanged(text, 0, oldlen, textLength);
    onTextChanged(text, 0, oldlen, textLength);
    
    if (needEditableForNotification) {
        sendAfterTextChanged(dynamic_pointer_cast<Editable>(text));
    }
    
    // SelectionModifierCursorController depends on textCanBeSelected, which depends on text
//    if (mEditor != NULL) mEditor.prepareCursorControllers();
}

void TextView::setText(UnicodeString text, int start, int len) {
    int oldlen = 0;
    
    if (start < 0 || len < 0 || start + len > text.length()) {
        stringstream str;
        str << start << ", " << len;
        throw IndexOutOfBoundsException(str.str());
    }
    
    if (mText != NULL) {
        oldlen = mText->length();
        sendBeforeTextChanged(mText, 0, oldlen, len);
    } else {
        sendBeforeTextChanged(make_shared<String>(), 0, 0, len);
    }
    
    if (mCharWrapper == NULL) {
        mCharWrapper = make_shared<CharWrapper>(text, start, len);
    } else {
        mCharWrapper->set(text, start, len);
    }
    
    setText(mCharWrapper, mBufferType, false, oldlen);
}

void TextView::setTextKeepState(shared_ptr<CharSequence> text, BufferType type) {
    int start = getSelectionStart();
    int end = getSelectionEnd();
    int len = text->length();
    
    setText(text, type);
    
    if (start >= 0 || end >= 0) {
        if (mTextSpannable != NULL) {
            Selection::setSelection(mTextSpannable,
                                   max(0, min(start, len)),
                                   max(0, min(end, len)));
        }
    }
}

void TextView::setText(string resid) {
    setText(getContext()->getResources()->getText(resid));
}

void TextView::setText(string resid, BufferType type) {
    setText(getContext()->getResources()->getText(resid), type);
}

void TextView::setHint(shared_ptr<CharSequence> hint) {
    mHint = TextUtils::stringOrSpannedString(hint);
    
    if (mLayout != NULL) {
        checkForRelayout();
    }
    
    if (mText->length() == 0) {
        invalidate();
    }
    
    // Invalidate display list if hint is currently used
//    if (mEditor != NULL && mText->length() == 0 && mHint != NULL) {
//        mEditor.invalidateTextDisplayList();
//    }
}

void TextView::setHint(string resid) {
    setHint(getContext()->getResources()->getText(resid));
}

shared_ptr<CharSequence> TextView::getHint() {
    return mHint;
}

bool TextView::isSingleLine() {
    return mSingleLine;
}

bool TextView::isMultilineInputType(int type) {
    return (type & (EditorInfo::TYPE_MASK_CLASS | EditorInfo::TYPE_TEXT_FLAG_MULTI_LINE)) ==
    (EditorInfo::TYPE_CLASS_TEXT | EditorInfo::TYPE_TEXT_FLAG_MULTI_LINE);
}

shared_ptr<CharSequence> TextView::removeSuggestionSpans(shared_ptr<CharSequence> text) {
    
    if (dynamic_pointer_cast<Spanned>(text) != NULL) {
        shared_ptr<Spannable> spannable = dynamic_pointer_cast<Spannable>(text);
        if (spannable == NULL) {
            spannable = make_shared<SpannableString>(text);
            text = spannable;
        }
        
        vector<shared_ptr<Object>> spans = spannable->getSpans(0, text->length(), "SuggestionSpan");
        
        for (int i = 0; i < spans.size(); i++) {
            spannable->removeSpan(spans[i]);
        }
    }
    
    return text;
}

void TextView::setInputType(int type) {
//    const bool wasPassword = isPasswordInputType(getInputType());
//    const bool wasVisiblePassword = isVisiblePasswordInputType(getInputType());
//    setInputType(type, false);
//    const bool isPassword = isPasswordInputType(type);
//    const bool isVisiblePassword = isVisiblePasswordInputType(type);
//    bool forceUpdate = false;
//    if (isPassword) {
//        setTransformationMethod(PasswordTransformationMethod.getInstance());
//        setTypefaceFromAttrs(NULL bool hasPasswordTransformationMethod() {
//    } else if (isVisiblePassword) {
//        if (mTransformation == PasswordTransformationMethod.getInstance()) {
//            forceUpdate = true;
//        }
//        setTypefaceFromAttrs(null /* fontFamily */, MONOSPACE, 0);
//    } else if (wasPassword || wasVisiblePassword) {
//        // not in password mode, clean up typeface and transformation
//        setTypefaceFromAttrs(null /* fontFamily */, -1, -1);
//        if (mTransformation == PasswordTransformationMethod.getInstance()) {
//            forceUpdate = true;
//        }
//    }
//                             
//    bool singleLine = !isMultilineInputType(type);
//
//    // We need to update the single line mode if it has changed or we
//    // were previously in password mode.
//    if (mSingleLine != singleLine || forceUpdate) {
//        // Change single line mode, but only change the transformation if
//        // we are not in password mode.
//        applySingleLine(singleLine, !isPassword, true);
//    }
//
//    if (!isSuggestionsEnabled()) {
//        mText = removeSuggestionSpans(mText);
//    }
//
//    InputMethodManager imm = InputMethodManager.peekInstance();
//    if (imm != NULL) imm.restartInput(this);
}

bool TextView::hasPasswordTransformationMethod() {
    return false; //mTransformation instanceof PasswordTransformationMethod;
}

bool TextView::isPasswordInputType(int inputType) {
    const int variation =
    inputType & (EditorInfo::TYPE_MASK_CLASS | EditorInfo::TYPE_MASK_VARIATION);
    return variation
    == (EditorInfo::TYPE_CLASS_TEXT | EditorInfo::TYPE_TEXT_VARIATION_PASSWORD)
    || variation
    == (EditorInfo::TYPE_CLASS_TEXT | EditorInfo::TYPE_TEXT_VARIATION_WEB_PASSWORD)
    || variation
    == (EditorInfo::TYPE_CLASS_NUMBER | EditorInfo::TYPE_NUMBER_VARIATION_PASSWORD);
}

bool TextView::isVisiblePasswordInputType(int inputType) {
    const int variation =
    inputType & (EditorInfo::TYPE_MASK_CLASS | EditorInfo::TYPE_MASK_VARIATION);
    return variation
    == (EditorInfo::TYPE_CLASS_TEXT | EditorInfo::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
}

void TextView::setRawInputType(int type) {
//    if (type == InputType::TYPE_NULL && mEditor == NULL) return; //TYPE_NULL is the default value
//    createEditorIfNeeded();
//    mEditor.mInputType = type;
}

void TextView::setInputType(int type, bool direct) {
//    const int cls = type & EditorInfo::TYPE_MASK_CLASS;
//    KeyListener input;
//    if (cls == EditorInfo::TYPE_CLASS_TEXT) {
//        bool autotext = (type & EditorInfo::TYPE_TEXT_FLAG_AUTO_CORRECT) != 0;
//        TextKeyListener.Capitalize cap;
//        if ((type & EditorInfo::TYPE_TEXT_FLAG_CAP_CHARACTERS) != 0) {
//            cap = TextKeyListener.Capitalize.CHARACTERS;
//        } else if ((type & EditorInfo::TYPE_TEXT_FLAG_CAP_WORDS) != 0) {
//            cap = TextKeyListener.Capitalize.WORDS;
//        } else if ((type & EditorInfo::TYPE_TEXT_FLAG_CAP_SENTENCES) != 0) {
//            cap = TextKeyListener.Capitalize.SENTENCES;
//        } else {
//            cap = TextKeyListener.Capitalize.NONE;
//        }
//        input = TextKeyListener.getInstance(autotext, cap);
//    } else if (cls == EditorInfo::TYPE_CLASS_NUMBER) {
//        input = DigitsKeyListener.getInstance(
//                                              (type & EditorInfo::TYPE_NUMBER_FLAG_SIGNED) != 0,
//                                              (type & EditorInfo::TYPE_NUMBER_FLAG_DECIMAL) != 0);
//    } else if (cls == EditorInfo::TYPE_CLASS_DATETIME) {
//        switch (type & EditorInfo::TYPE_MASK_VARIATION) {
//            case EditorInfo::TYPE_DATETIME_VARIATION_DATE:
//                input = DateKeyListener.getInstance();
//                break;
//            case EditorInfo::TYPE_DATETIME_VARIATION_TIME:
//                input = TimeKeyListener.getInstance();
//                break;
//            default:
//                input = DateTimeKeyListener.getInstance();
//                break;
//        }
//    } else if (cls == EditorInfo::TYPE_CLASS_PHONE) {
//        input = DialerKeyListener.getInstance();
//    } else {
//        input = TextKeyListener.getInstance();
//    }
//    setRawInputType(type);
//    if (direct) {
//        createEditorIfNeeded();
//        mEditor.mKeyListener = input;
//    } else {
//        setKeyListenerOnly(input);
//    }
}

void TextView::setError(shared_ptr<CharSequence> error) {
//    if (error == NULL) {
//        setError(NULL, NULL);
//    } else {
//        Drawable dr = getContext()->getResources()->getDrawable(com.android.internal.R.drawable.indicator_input_error);
//        
//        dr->setBounds(0, 0, dr->getIntrinsicWidth(), dr->getIntrinsicHeight());
//        setError(error, dr);
//    }
}

bool TextView::setFrame(int l, int t, int r, int b) {
    bool result = View::setFrame(l, t, r, b);
    
//    if (mEditor != NULL) mEditor.setFrame();
    
    restartMarqueeIfNeeded();
    
    return result;
}

void TextView::restartMarqueeIfNeeded() {
    if (mRestartMarquee && mEllipsize == TextUtils::TruncateAt::MARQUEE) {
        mRestartMarquee = false;
        startMarquee();
    }
}

void TextView::setFilters(vector<shared_ptr<InputFilter>> filters) {
    
    mFilters = filters;
    
    shared_ptr<Editable> editable = dynamic_pointer_cast<Editable>(mText);
    if (editable != NULL) {
        setFilters(editable, filters);
    }
}

void TextView::setFilters(shared_ptr<Editable> e, vector<shared_ptr<InputFilter>> filters) {
//    if (mEditor != NULL && mEditor.mKeyListener instanceof InputFilter) {
//        vector<shared_ptr<InputFilter>> nf = vector<shared_ptr<InputFilter>>(filters.length + 1);
//        
//        System::arraycopy(filters, 0, nf, 0, filters.length);
//        nf[filters.length] = (InputFilter) mEditor.mKeyListener;
//        
//        e.setFilters(nf);
//    } else {
        e->setFilters(filters);
//    }
}

vector<shared_ptr<InputFilter>> TextView::getFilters() {
    return mFilters;
}

int TextView::getBoxHeight(shared_ptr<Layout> l) {
    Insets opticalInsets = /*isLayoutModeOptical(mParent) ? getOpticalInsets() :*/ Insets::NONE;
    int padding = (l == mHintLayout) ?
    getCompoundPaddingTop() + getCompoundPaddingBottom() :
    getExtendedPaddingTop() + getExtendedPaddingBottom();
    return getMeasuredHeight() - padding + opticalInsets.top + opticalInsets.bottom;
}

int TextView::getVerticalOffset(bool forceNormal) {
    int voffset = 0;
    const int gravity = mGravity & Gravity::VERTICAL_GRAVITY_MASK;
    
    shared_ptr<Layout> &l = mLayout;
    if (!forceNormal && mText->length() == 0 && mHintLayout != NULL) {
        l = mHintLayout;
    }
    
    if (gravity != Gravity::TOP) {
        int boxht = getBoxHeight(l);
        int textht = l->getHeight();
        
        if (textht < boxht) {
            if (gravity == Gravity::BOTTOM)
                voffset = boxht - textht;
            else // (gravity == Gravity::CENTER_VERTICAL)
                voffset = (boxht - textht) >> 1;
        }
    }
    return voffset;
}

int TextView::getBottomVerticalOffset(bool forceNormal) {
    int voffset = 0;
    const int gravity = mGravity & Gravity::VERTICAL_GRAVITY_MASK;
    
    shared_ptr<Layout> &l = mLayout;
    if (!forceNormal && mText->length() == 0 && mHintLayout != NULL) {
        l = mHintLayout;
    }
    
    if (gravity != Gravity::BOTTOM) {
        int boxht = getBoxHeight(l);
        int textht = l->getHeight();
        
        if (textht < boxht) {
            if (gravity == Gravity::TOP)
                voffset = boxht - textht;
            else // (gravity == Gravity::CENTER_VERTICAL)
                voffset = (boxht - textht) >> 1;
        }
    }
    return voffset;
}

void TextView::invalidateCursorPath() {
//    if (mHighlightPathBogus) {
//        invalidateCursor();
//    } else {
//        const int horizontalPadding = getCompoundPaddingLeft();
//        const int verticalPadding = getExtendedPaddingTop() + getVerticalOffset(true);
//        
//        if (mEditor.mCursorCount == 0) {
//            synchronized (TEMP_RECTF) {
//                                float thick = ceil(mTextPaint->getStrokeWidth());
//                if (thick < 1.0f) {
//                    thick = 1.0f;
//                }
//                
//                thick /= 2.0f;
//                
//                // mHighlightPath is guaranteed to be non NULL at that point.
//                mHighlightPath.computeBounds(TEMP_RECTF, false);
//                
//                invalidate((int) floor(horizontalPadding + TEMP_RECTF.left - thick),
//                           (int) floor(verticalPadding + TEMP_RECTF.top - thick),
//                           (int) ceil(horizontalPadding + TEMP_RECTF.right + thick),
//                           (int) ceil(verticalPadding + TEMP_RECTF.bottom + thick));
//            }
//        } else {
//            for (int i = 0; i < mEditor.mCursorCount; i++) {
//                Rect bounds = mEditor.mCursorDrawable[i].getBounds();
//                invalidate(bounds.left + horizontalPadding, bounds.top + verticalPadding,
//                           bounds.right + horizontalPadding, bounds.bottom + verticalPadding);
//            }
//        }
//    }
}

void TextView::invalidateCursor() {
    int where = getSelectionEnd();
    
    invalidateCursor(where, where, where);
}

void TextView::invalidateCursor(int a, int b, int c) {
    if (a >= 0 || b >= 0 || c >= 0) {
        int start = min(min(a, b), c);
        int end = max(max(a, b), c);
        invalidateRegion(start, end, true);
    }
}

void TextView::invalidateRegion(int start, int end, bool invalidateCursor) {
    if (mLayout == NULL) {
        invalidate();
    } else {
        int lineStart = mLayout->getLineForOffset(start);
        int top = mLayout->getLineTop(lineStart);
        
        // This is ridiculous, but the descent from the line above
        // can hang down into the line we really want to redraw,
        // so we have to invalidate part of the line above to make
        // sure everything that needs to be redrawn really is.
        // (But not the whole line above, because that would cause
        // the same problem with the descenders on the line above it!)
        if (lineStart > 0) {
            top -= mLayout->getLineDescent(lineStart - 1);
        }
        
        int lineEnd;
        
        if (start == end)
            lineEnd = lineStart;
        else
            lineEnd = mLayout->getLineForOffset(end);
        
        int bottom = mLayout->getLineBottom(lineEnd);
        
        // mEditor can be NULL in case selection is set programmatically.
//        if (invalidateCursor && mEditor != NULL) {
//            for (int i = 0; i < mEditor.mCursorCount; i++) {
//                Rect bounds = mEditor.mCursorDrawable[i].getBounds();
//                top = min(top, bounds.top);
//                bottom = max(bottom, bounds.bottom);
//            }
//        }
        
        const int compoundPaddingLeft = getCompoundPaddingLeft();
        const int verticalPadding = getExtendedPaddingTop() + getVerticalOffset(true);
        
        int left, right;
        if (lineStart == lineEnd && !invalidateCursor) {
            left = (int) mLayout->getPrimaryHorizontal(start);
            right = (int) (mLayout->getPrimaryHorizontal(end) + 1.0);
            left += compoundPaddingLeft;
            right += compoundPaddingLeft;
        } else {
            // Rectangle bounding box when the region spans several lines
            left = compoundPaddingLeft;
            right = getWidth() - getCompoundPaddingRight();
        }
        
        invalidate(mScrollX + left, verticalPadding + top,
                   mScrollX + right, verticalPadding + bottom);
    }
}

void TextView::registerForPreDraw() {
    if (!mPreDrawRegistered) {
//        getViewTreeObserver().addOnPreDrawListener(this);
        mPreDrawRegistered = true;
    }
}

bool TextView::onPreDraw() {
    if (mLayout == NULL) {
        assumeLayout();
    }
    
    bool changed = false;
    
    if (mMovement != NULL) {
                int curs = getSelectionEnd();
        // Do not create the controller if it is not already created.
//        if (mEditor != NULL && mEditor.mSelectionModifierCursorController != NULL &&
//            mEditor.mSelectionModifierCursorController.isSelectionStartDragged()) {
//            curs = getSelectionStart();
//        }
        
        if (curs < 0 && (mGravity & Gravity::VERTICAL_GRAVITY_MASK) == Gravity::BOTTOM) {
            curs = mText->length();
        }
        
        if (curs >= 0) {
            changed = bringPointIntoView(curs);
        }
    } else {
        changed = bringTextIntoView();
    }
    
    // This has to be checked here since:
    // - onFocusChanged cannot start it when focus is given to a view with selected text (after
    //   a screen rotation) since layout is not yet initialized at that point.
//    if (mEditor != NULL && mEditor.mCreatedWithASelection) {
//        mEditor.startSelectionActionMode();
//        mEditor.mCreatedWithASelection = false;
//    }
    
    // Phone specific code (there is no ExtractEditText on tablets).
    // ExtractEditText does not call onFocus when it is displayed, and mHasSelectionOnFocus can
    // not be set. Do the test here instead.
//    if (this instanceof ExtractEditText && hasSelection() && mEditor != NULL) {
//        mEditor.startSelectionActionMode();
//    }
    
//    getViewTreeObserver()->removeOnPreDrawListener(this);
    mPreDrawRegistered = false;
    
    return !changed;
}

void TextView::onAttachedToWindow() {
    View::onAttachedToWindow();
    
    mTemporaryDetach = false;
    
//    if (mEditor != NULL) mEditor.onAttachedToWindow();
}

void TextView::onDetachedFromWindow() {
    View::onDetachedFromWindow();
    
    if (mPreDrawRegistered) {
//        getViewTreeObserver().removeOnPreDrawListener(this);
        mPreDrawRegistered = false;
    }
    
    resetResolvedDrawables();
    
//    if (mEditor != NULL) mEditor.onDetachedFromWindow();
}

void TextView::onScreenStateChanged(int screenState) {
    View::onScreenStateChanged(screenState);
//    if (mEditor != NULL) mEditor.onScreenStateChanged(screenState);
}

bool TextView::isPaddingOffsetRequired() {
    return mShadowRadius != 0 || mDrawables != NULL;
}

int TextView::getLeftPaddingOffset() {
    return getCompoundPaddingLeft() - mPaddingLeft + min(0, (int) (mShadowDx - mShadowRadius));
}

int TextView::getTopPaddingOffset() {
    return min(0, (int) (mShadowDy - mShadowRadius));
}

int TextView::getBottomPaddingOffset() {
    return max(0, (int) (mShadowDy + mShadowRadius));
}

int TextView::getRightPaddingOffset() {
    return -(getCompoundPaddingRight() - mPaddingRight) +
     max(0, (int) (mShadowDx + mShadowRadius));
}

bool TextView::verifyDrawable(shared_ptr<Drawable> who) {
    const bool verified = View::verifyDrawable(who);
    if (!verified && mDrawables != NULL) {
        return who == mDrawables->mDrawableLeft || who == mDrawables->mDrawableTop ||
        who == mDrawables->mDrawableRight || who == mDrawables->mDrawableBottom ||
        who == mDrawables->mDrawableStart || who == mDrawables->mDrawableEnd;
    }
    return verified;
}

void TextView::jumpDrawablesToCurrentState() {
    View::jumpDrawablesToCurrentState();
    if (mDrawables != NULL) {
        if (mDrawables->mDrawableLeft != NULL) {
            mDrawables->mDrawableLeft->jumpToCurrentState();
        }
        if (mDrawables->mDrawableTop != NULL) {
            mDrawables->mDrawableTop->jumpToCurrentState();
        }
        if (mDrawables->mDrawableRight != NULL) {
            mDrawables->mDrawableRight->jumpToCurrentState();
        }
        if (mDrawables->mDrawableBottom != NULL) {
            mDrawables->mDrawableBottom->jumpToCurrentState();
        }
        if (mDrawables->mDrawableStart != NULL) {
            mDrawables->mDrawableStart->jumpToCurrentState();
        }
        if (mDrawables->mDrawableEnd != NULL) {
            mDrawables->mDrawableEnd->jumpToCurrentState();
        }
    }
}

void TextView::invalidateDrawable(shared_ptr<Drawable> drawable) {
    if (verifyDrawable(drawable)) {
        Rect dirty = drawable->getBounds();
        int scrollX = mScrollX;
        int scrollY = mScrollY;
        
        // IMPORTANT: The coordinates below are based on the coordinates computed
        // for each compound drawable in onDraw(). Make sure to update each section
        // accordingly.
        shared_ptr<Drawables> &drawables = mDrawables;
        if (drawables != NULL) {
            if (drawable == drawables->mDrawableLeft) {
                const int compoundPaddingTop = getCompoundPaddingTop();
                const int compoundPaddingBottom = getCompoundPaddingBottom();
                const int vspace = mBottom - mTop - compoundPaddingBottom - compoundPaddingTop;
                
                scrollX += mPaddingLeft;
                scrollY += compoundPaddingTop + (vspace - drawables->mDrawableHeightLeft) / 2;
            } else if (drawable == drawables->mDrawableRight) {
                const int compoundPaddingTop = getCompoundPaddingTop();
                const int compoundPaddingBottom = getCompoundPaddingBottom();
                const int vspace = mBottom - mTop - compoundPaddingBottom - compoundPaddingTop;
                
                scrollX += (mRight - mLeft - mPaddingRight - drawables->mDrawableSizeRight);
                scrollY += compoundPaddingTop + (vspace - drawables->mDrawableHeightRight) / 2;
            } else if (drawable == drawables->mDrawableTop) {
                const int compoundPaddingLeft = getCompoundPaddingLeft();
                const int compoundPaddingRight = getCompoundPaddingRight();
                const int hspace = mRight - mLeft - compoundPaddingRight - compoundPaddingLeft;
                
                scrollX += compoundPaddingLeft + (hspace - drawables->mDrawableWidthTop) / 2;
                scrollY += mPaddingTop;
            } else if (drawable == drawables->mDrawableBottom) {
                const int compoundPaddingLeft = getCompoundPaddingLeft();
                const int compoundPaddingRight = getCompoundPaddingRight();
                const int hspace = mRight - mLeft - compoundPaddingRight - compoundPaddingLeft;
                
                scrollX += compoundPaddingLeft + (hspace - drawables->mDrawableWidthBottom) / 2;
                scrollY += (mBottom - mTop - mPaddingBottom - drawables->mDrawableSizeBottom);
            }
        }
        
        invalidate(dirty.left() + scrollX, dirty.top() + scrollY,
                   dirty.right() + scrollX, dirty.bottom() + scrollY);
    }
}

bool TextView::hasOverlappingRendering() {
    return ((getBackground() != NULL && getBackground()->getCurrent() != NULL)
            || mTextSpannable != NULL || hasSelection());
}

bool TextView::isTextSelectable() {
    return false; //mEditor == NULL ? false : mEditor.mTextIsSelectable;
}

void TextView::setTextIsSelectable(bool selectable) {
//    if (!selectable && mEditor == NULL) return; // false is default value with no edit data
//    
//    createEditorIfNeeded();
//    if (mEditor.mTextIsSelectable == selectable) return;
//    
//    mEditor.mTextIsSelectable = selectable;
//    setFocusableInTouchMode(selectable);
//    setFocusable(selectable);
//    setClickable(selectable);
//    setLongClickable(selectable);
//    
//    // mInputType should already be EditorInfo::TYPE_NULL and mInput should be NULL
//    
//    setMovementMethod(selectable ? ArrowKeyMovementMethod.getInstance() : NULL);
//    setText(mText, selectable ? BufferType::SPANNABLE : BufferType::NORMAL);
//    
//    // Called by setText above, but safer in case of future code changes
//    mEditor.prepareCursorControllers();
}

vector<int> TextView::onCreateDrawableState(int extraSpace) {
    vector<int> drawableState;
    
    if (mSingleLine) {
        drawableState = View::onCreateDrawableState(extraSpace);
    } else {
        drawableState = View::onCreateDrawableState(extraSpace + 1);
        mergeDrawableStates(drawableState, MULTILINE_STATE_SET);
    }
    
    if (isTextSelectable()) {
        // Disable pressed state, which was introduced when TextView was made clickable.
        // Prevents text color change.
        // setClickable(false) would have a similar effect, but it also disables focus changes
        // and long press actions, which are both needed by text selection.
        const int length = drawableState.size();
        for (int i = 0; i < length; i++) {
            if (drawableState[i] == R::attr::state_pressed) {
                vector<int> nonPressedState = vector<int>(length - 1);
                System::arraycopy(drawableState, 0, nonPressedState, 0, i);
                System::arraycopy(drawableState, i + 1, nonPressedState, i, length - i - 1);
                return nonPressedState;
            }
        }
    }
    
    return drawableState;
}

shared_ptr<Path> TextView::getUpdatedHighlightPath() {
    shared_ptr<Path> highlight;
//    Paint &highlightPaint = mHighlightPaint;
//    
//    const int selStart = getSelectionStart();
//    const int selEnd = getSelectionEnd();
//    if (mMovement != NULL && (isFocused() || isPressed()) && selStart >= 0) {
//        if (selStart == selEnd) {
//            if (mEditor != NULL && mEditor.isCursorVisible() &&
//                (SystemClock.uptimeMillis() - mEditor.mShowCursor) %
//                (2 * Editor.BLINK) < Editor.BLINK) {
//                if (mHighlightPathBogus) {
//                    if (mHighlightPath == NULL) mHighlightPath = new Path();
//                    mHighlightPath.reset();
//                    mLayout->getCursorPath(selStart, mHighlightPath, mText);
//                    mEditor.updateCursorsPositions();
//                    mHighlightPathBogus = false;
//                }
//                
//                // XXX should pass to skin instead of drawing directly
//                highlightPaint.setColor(mCurTextColor);
//                highlightPaint.setStyle(Paint.Style.STROKE);
//                highlight = mHighlightPath;
//            }
//        } else {
//            if (mHighlightPathBogus) {
//                if (mHighlightPath == NULL) mHighlightPath = new Path();
//                mHighlightPath.reset();
//                mLayout->getSelectionPath(selStart, selEnd, mHighlightPath);
//                mHighlightPathBogus = false;
//            }
//            
//            // XXX should pass to skin instead of drawing directly
//            highlightPaint.setColor(mHighlightColor);
//            highlightPaint.setStyle(Paint.Style.FILL);
//            
//            highlight = mHighlightPath;
//        }
//    }
    return highlight;
}

int TextView::getHorizontalOffsetForDrawables() {
    return 0;
}

void TextView::onDraw(shared_ptr<Canvas> canvas) {
    restartMarqueeIfNeeded();
    
    // Draw the background for this view
    View::onDraw(canvas);
    
    const int compoundPaddingLeft = getCompoundPaddingLeft();
    const int compoundPaddingTop = getCompoundPaddingTop();
    const int compoundPaddingRight = getCompoundPaddingRight();
    const int compoundPaddingBottom = getCompoundPaddingBottom();
    const int scrollX = mScrollX;
    const int scrollY = mScrollY;
    const int right = mRight;
    const int left = mLeft;
    const int bottom = mBottom;
    const int top = mTop;
    const bool isLayoutRtol = isLayoutRtl();
    const int offset = getHorizontalOffsetForDrawables();
    const int leftOffset = isLayoutRtol ? 0 : offset;
    const int rightOffset = isLayoutRtol ? offset : 0 ;
    
    shared_ptr<Drawables> &dr = mDrawables;
    if (dr != NULL) {
                
        int vspace = bottom - top - compoundPaddingBottom - compoundPaddingTop;
        int hspace = right - left - compoundPaddingRight - compoundPaddingLeft;
        
        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableLeft != NULL) {
            canvas->save();
            canvas->translate(scrollX + mPaddingLeft + leftOffset,
                             scrollY + compoundPaddingTop +
                             (vspace - dr->mDrawableHeightLeft) / 2);
            dr->mDrawableLeft->draw(canvas);
            canvas->restore();
        }
        
        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableRight != NULL) {
            canvas->save();
            canvas->translate(scrollX + right - left - mPaddingRight
                             - dr->mDrawableSizeRight - rightOffset,
                             scrollY + compoundPaddingTop + (vspace - dr->mDrawableHeightRight) / 2);
            dr->mDrawableRight->draw(canvas);
            canvas->restore();
        }
        
        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableTop != NULL) {
            canvas->save();
            canvas->translate(scrollX + compoundPaddingLeft +
                             (hspace - dr->mDrawableWidthTop) / 2, scrollY + mPaddingTop);
            dr->mDrawableTop->draw(canvas);
            canvas->restore();
        }
        
        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableBottom != NULL) {
            canvas->save();
            canvas->translate(scrollX + compoundPaddingLeft +
                             (hspace - dr->mDrawableWidthBottom) / 2,
                             scrollY + bottom - top - mPaddingBottom - dr->mDrawableSizeBottom);
            dr->mDrawableBottom->draw(canvas);
            canvas->restore();
        }
    }
    
    int color = mCurTextColor;
    
    if (mLayout == NULL) {
        assumeLayout();
    }
    
    shared_ptr<Layout> layout = mLayout;
    
    if (mHint != NULL && mText->length() == 0) {
        if (mHintTextColor != NULL) {
            color = mCurHintTextColor;
        }
        
        layout = mHintLayout;
    }
    
    mTextPaint->setColor(color);
    mTextPaint->drawableState = getDrawableState();
    
    canvas->save();
        
    int extendedPaddingTop = getExtendedPaddingTop();
    int extendedPaddingBottom = getExtendedPaddingBottom();
    
    const int vspace = mBottom - mTop - compoundPaddingBottom - compoundPaddingTop;
    const int maxScrollY = mLayout->getHeight() - vspace;
    
    float clipLeft = compoundPaddingLeft + scrollX;
    float clipTop = (scrollY == 0) ? 0 : extendedPaddingTop + scrollY;
    float clipRight = right - left - compoundPaddingRight + scrollX;
    float clipBottom = bottom - top + scrollY -
    ((scrollY == maxScrollY) ? 0 : extendedPaddingBottom);
    
    if (mShadowRadius != 0) {
        clipLeft += min(0.0f, mShadowDx - mShadowRadius);
        clipRight += max(0.0f, mShadowDx + mShadowRadius);
        
        clipTop += min(0.0f, mShadowDy - mShadowRadius);
        clipBottom += max(0.0f, mShadowDy + mShadowRadius);
    }
    
    canvas->clipRect(clipLeft, clipTop, clipRight, clipBottom);
    
    int voffsetText = 0;
    int voffsetCursor = 0;
    
    // translate in by our padding
        if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) != Gravity::TOP) {
        voffsetText = getVerticalOffset(false);
        voffsetCursor = getVerticalOffset(true);
    }
    canvas->translate(compoundPaddingLeft, extendedPaddingTop + voffsetText);
    
    const int layoutDirection = getLayoutDirection();
    const int absoluteGravity = Gravity::getAbsoluteGravity(mGravity, layoutDirection);
    if (mEllipsize == TextUtils::TruncateAt::MARQUEE &&
        mMarqueeFadeMode != MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
        if (!mSingleLine && getLineCount() == 1 && canMarquee() &&
            (absoluteGravity & Gravity::HORIZONTAL_GRAVITY_MASK) != Gravity::LEFT) {
            const int width = mRight - mLeft;
            const int padding = getCompoundPaddingLeft() + getCompoundPaddingRight();
            const float dx = mLayout->getLineRight(0) - (width - padding);
            canvas->translate(isLayoutRtol ? -dx : +dx, 0.0f);
        }
        
//        if (mMarquee != NULL && mMarquee->isRunning()) {
//            const float dx = -mMarquee->getScroll();
//            canvas->translate(isLayoutRtl ? -dx : +dx, 0.0f);
//        }
    }
    
    const int cursorOffsetVertical = voffsetCursor - voffsetText;
    
    shared_ptr<Path> highlight = getUpdatedHighlightPath();
//    if (mEditor != NULL) {
//        mEditor.onDraw(canvas, layout, highlight, mHighlightPaint, cursorOffsetVertical);
//    } else {
        layout->draw(canvas, highlight.get(), mHighlightPaint.get(), cursorOffsetVertical);
//    }
    
//    if (mMarquee != NULL && mMarquee->shouldDrawGhost()) {
//        const int dx = (int) mMarquee->getGhostOffset();
//        canvas->translate(isLayoutRtl ? -dx : dx, 0.0f);
//        layout->draw(canvas, highlight, mHighlightPaint, cursorOffsetVertical);
//    }
    
    canvas->restore();
}

void TextView::getFocusedRect(Rect r) {
    if (mLayout == NULL) {
        View::getFocusedRect(r);
        return;
    }
    
    int selEnd = getSelectionEnd();
    if (selEnd < 0) {
        View::getFocusedRect(r);
        return;
    }
    
    int selStart = getSelectionStart();
    if (selStart < 0 || selStart >= selEnd) {
        int line = mLayout->getLineForOffset(selEnd);
        r.fTop = mLayout->getLineTop(line);
        r.fBottom = mLayout->getLineBottom(line);
        r.fLeft = (int) mLayout->getPrimaryHorizontal(selEnd) - 2;
        r.fRight = r.fLeft + 4;
    } else {
        int lineStart = mLayout->getLineForOffset(selStart);
        int lineEnd = mLayout->getLineForOffset(selEnd);
        r.fTop = mLayout->getLineTop(lineStart);
        r.fBottom = mLayout->getLineBottom(lineEnd);
        if (lineStart == lineEnd) {
            r.fLeft = (int) mLayout->getPrimaryHorizontal(selStart);
            r.fRight = (int) mLayout->getPrimaryHorizontal(selEnd);
        } else {
            // Selection extends across multiple lines -- make the focused
            // rect cover the entire width.
            if (mHighlightPathBogus) {
                if (mHighlightPath == NULL) mHighlightPath = make_shared<Path>();
                mHighlightPath->reset();
                mLayout->getSelectionPath(selStart, selEnd, mHighlightPath.get());
                mHighlightPathBogus = false;
            }
//            synchronized (TEMP_RECTF) {
                mHighlightPath->computeBounds(TEMP_RECTF, true);
                r.fLeft = (int) TEMP_RECTF.fLeft - 1;
                r.fRight = (int) TEMP_RECTF.fRight + 1;
//            }
        }
    }
    
    // Adjust for padding and gravity.
    int paddingLeft = getCompoundPaddingLeft();
    int paddingTop = getExtendedPaddingTop();
    if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) != Gravity::TOP) {
        paddingTop += getVerticalOffset(false);
    }
    r.offset(paddingLeft, paddingTop);
    int paddingBottom = getExtendedPaddingBottom();
    r.fBottom += paddingBottom;
}

int TextView::getLineCount() {
    return mLayout != NULL ? mLayout->getLineCount() : 0;
}

int TextView::getLineBounds(int line, Rect bounds) {
    if (mLayout == NULL) {
        bounds.set(0, 0, 0, 0);
        return 0;
    } else {
        int baseline = mLayout->getLineBounds(line, bounds);
        
        int voffset = getExtendedPaddingTop();
        if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) != Gravity::TOP) {
            voffset += getVerticalOffset(true);
        }
        
        bounds.offset(getCompoundPaddingLeft(), voffset);
        
        return baseline + voffset;
    }
}

int TextView::getBaseline() {
    if (mLayout == NULL) {
        return View::getBaseline();
    }
    
    int voffset = 0;
    if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) != Gravity::TOP) {
        voffset = getVerticalOffset(true);
    }
    
//    if (isLayoutModeOptical(mParent)) {
//        voffset -= getOpticalInsets().top;
//    }
    
    return getExtendedPaddingTop() + voffset + mLayout->getLineBaseline(0);
}

int TextView::getFadeTop(bool offsetRequired) {
    if (mLayout == NULL) return 0;
    
    int voffset = 0;
    if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) != Gravity::TOP) {
        voffset = getVerticalOffset(true);
    }
    
    if (offsetRequired) voffset += getTopPaddingOffset();
    
    return getExtendedPaddingTop() + voffset;
}

int TextView::getFadeHeight(bool offsetRequired) {
    return mLayout != NULL ? mLayout->getHeight() : 0;
}

bool TextView::onKeyPreIme(int keyCode, KeyEvent *event) {
//    if (keyCode == KeyEvent.KEYCODE_BACK) {
//        bool isInSelectionMode = mEditor != NULL && mEditor.mSelectionActionMode != NULL;
//        
//        if (isInSelectionMode) {
//            if (event->getAction() == KeyEvent.ACTION_DOWN && event->getRepeatCount() == 0) {
//                KeyEvent.DispatcherState state = getKeyDispatcherState();
//                if (state != NULL) {
//                    state.startTracking(event, this);
//                }
//                return true;
//            } else if (event->getAction() == KeyEvent.ACTION_UP) {
//                KeyEvent.DispatcherState state = getKeyDispatcherState();
//                if (state != NULL) {
//                    state.handleUpEvent(event);
//                }
//                if (event->isTracking() && !event->isCanceled()) {
//                    stopSelectionActionMode();
//                    return true;
//                }
//            }
//        }
//    }
    return View::onKeyPreIme(keyCode, event);
}

bool TextView::onKeyDown(int keyCode, KeyEvent *event) {
    int which = doKeyDown(keyCode, event, NULL);
    if (which == 0) {
        // Go through default dispatching.
        return View::onKeyDown(keyCode, event);
    }
    
    return true;
}

bool TextView::onKeyMultiple(int keyCode, int repeatCount, KeyEvent *event) {
//    KeyEvent *down = KeyEvent::changeAction(event, KeyEvent::ACTION_DOWN);
//    
//    int which = doKeyDown(keyCode, down, event);
//    if (which == 0) {
//        // Go through default dispatching.
//        return View::onKeyMultiple(keyCode, repeatCount, event);
//    }
//    if (which == -1) {
//        // Consumed the whole thing.
//        return true;
//    }
//    
//    repeatCount--;
//    
//    // We are going to dispatch the remaining events to either the input
//    // or movement method.  To do this, we will just send a repeated stream
//    // of down and up events until we have done the complete repeatCount.
//    // It would be nice if those interfaces had an onKeyMultiple() method,
//    // but adding that is a more complicated change.
//    KeyEvent *up = KeyEvent::changeAction(event, KeyEvent::ACTION_UP);
//    if (which == 1) {
//        // mEditor and mEditor.mInput are not NULL from doKeyDown
//        mEditor.mKeyListener.onKeyUp(this, (Editable)mText, keyCode, up);
//        while (--repeatCount > 0) {
//            mEditor.mKeyListener.onKeyDown(this, (Editable)mText, keyCode, down);
//            mEditor.mKeyListener.onKeyUp(this, (Editable)mText, keyCode, up);
//        }
//        hideErrorIfUnchanged();
//        
//    } else if (which == 2) {
//        // mMovement is not NULL from doKeyDown
//        mMovement->onKeyUp(this, mTextSpannable, keyCode, up);
//        while (--repeatCount > 0) {
//            mMovement->onKeyDown(this, mTextSpannable, keyCode, down);
//            mMovement->onKeyUp(this, mTextSpannable, keyCode, up);
//        }
//    }
    
    return true;
}

bool TextView::shouldAdvanceFocusOnEnter() {
    if (getKeyListener() == NULL) {
        return false;
    }
    
    if (mSingleLine) {
        return true;
    }
    
//    if (mEditor != NULL &&
//        (mEditor.mInputType & EditorInfo::TYPE_MASK_CLASS) == EditorInfo::TYPE_CLASS_TEXT) {
//        int variation = mEditor.mInputType & EditorInfo::TYPE_MASK_VARIATION;
//        if (variation == EditorInfo::TYPE_TEXT_VARIATION_EMAIL_ADDRESS
//            || variation == EditorInfo::TYPE_TEXT_VARIATION_EMAIL_SUBJECT) {
//            return true;
//        }
//    }
    
    return false;
}

bool TextView::shouldAdvanceFocusOnTab() {
//    if (getKeyListener() != NULL && !mSingleLine && mEditor != NULL &&
//        (mEditor.mInputType & EditorInfo::TYPE_MASK_CLASS) == EditorInfo::TYPE_CLASS_TEXT) {
//        int variation = mEditor.mInputType & EditorInfo::TYPE_MASK_VARIATION;
//        if (variation == EditorInfo::TYPE_TEXT_FLAG_IME_MULTI_LINE
//            || variation == EditorInfo::TYPE_TEXT_FLAG_MULTI_LINE) {
//            return false;
//        }
//    }
    return true;
}

int TextView::doKeyDown(int keyCode, KeyEvent *event, KeyEvent *otherEvent) {
    if (!isEnabled()) {
        return 0;
    }
    
//    switch (keyCode) {
//        case KeyEvent::KEYCODE_ENTER:
//            if (event->hasNoModifiers()) {
//                // When mInputContentType is set, we know that we are
//                // running in a "modern" cupcake environment, so don't need
//                // to worry about the application trying to capture
//                // enter key events.
//                if (mEditor != NULL && mEditor.mInputContentType != NULL) {
//                    // If there is an action listener, given them a
//                    // chance to consume the event->
//                    if (mEditor.mInputContentType.onEditorActionListener != NULL &&
//                        mEditor.mInputContentType.onEditorActionListener.onEditorAction(
//                                                                                        this, EditorInfo::IME_NULL, event)) {
//                            mEditor.mInputContentType.enterDown = true;
//                            // We are consuming the enter key for them.
//                            return -1;
//                        }
//                }
//                
//                // If our editor should move focus when enter is pressed, or
//                // this is a generated event from an IME action button, then
//                // don't let it be inserted into the text->
//                if ((event->getFlags() & KeyEvent.FLAG_EDITOR_ACTION) != 0
//                    || shouldAdvanceFocusOnEnter()) {
//                    if (hasOnClickListeners()) {
//                        return 0;
//                    }
//                    return -1;
//                }
//            }
//            break;
//            
//        case KeyEvent.KEYCODE_DPAD_CENTER:
//            if (event->hasNoModifiers()) {
//                if (shouldAdvanceFocusOnEnter()) {
//                    return 0;
//                }
//            }
//            break;
//            
//        case KeyEvent.KEYCODE_TAB:
//            if (event->hasNoModifiers() || event->hasModifiers(KeyEvent.META_SHIFT_ON)) {
//                if (shouldAdvanceFocusOnTab()) {
//                    return 0;
//                }
//            }
//            break;
//            
//            // Has to be done on key down (and not on key up) to correctly be intercepted.
//        case KeyEvent.KEYCODE_BACK:
//            if (mEditor != NULL && mEditor.mSelectionActionMode != NULL) {
//                stopSelectionActionMode();
//                return -1;
//            }
//            break;
//    }
//    
//    if (mEditor != NULL && mEditor.mKeyListener != NULL) {
//        resetErrorChangedFlag();
//        
//        bool doDown = true;
//        if (otherEvent != NULL) {
//            try {
//                beginBatchEdit();
//                const bool handled = mEditor.mKeyListener.onKeyOther(this, (Editable) mText,
//                                                                     otherEvent);
//                hideErrorIfUnchanged();
//                doDown = false;
//                if (handled) {
//                    return -1;
//                }
//            } catch (AbstractMethodError e) {
//                // onKeyOther was added after 1.0, so if it isn't
//                // implemented we need to try to dispatch as a regular down.
//            } constly {
//                endBatchEdit();
//            }
//        }
//        
//        if (doDown) {
//            beginBatchEdit();
//            const bool handled = mEditor.mKeyListener.onKeyDown(this, (Editable) mText,
//                                                                keyCode, event);
//            endBatchEdit();
//            hideErrorIfUnchanged();
//            if (handled) return 1;
//        }
//    }
//    
//    // bug 650865: sometimes we get a key event before a layout->
//    // don't try to move around if we don't know the layout->
//    
//    if (mMovement != NULL && mLayout != NULL) {
//        bool doDown = true;
//        if (otherEvent != NULL) {
//            try {
//                bool handled = mMovement->onKeyOther(this, mTextSpannable,
//                                                    otherEvent);
//                doDown = false;
//                if (handled) {
//                    return -1;
//                }
//            } catch (AbstractMethodError e) {
//                // onKeyOther was added after 1.0, so if it isn't
//                // implemented we need to try to dispatch as a regular down.
//            }
//        }
//        if (doDown) {
//            if (mMovement->onKeyDown(this, mTextSpannable, keyCode, event))
//                return 2;
//        }
//    }
    
    return 0;
}

void TextView::resetErrorChangedFlag() {
//    if (mEditor != NULL) mEditor.mErrorWasChanged = false;
}

void TextView::hideErrorIfUnchanged() {
//    if (mEditor != NULL && mEditor.mError != NULL && !mEditor.mErrorWasChanged) {
//        setError(NULL, NULL);
//    }
}

bool TextView::onKeyUp(int keyCode, KeyEvent *event) {
    if (!isEnabled()) {
        return View::onKeyUp(keyCode, event);
    }
    
//    switch (keyCode) {
//        case KeyEvent.KEYCODE_DPAD_CENTER:
//            if (event->hasNoModifiers()) {
//                                if (!hasOnClickListeners()) {
//                    if (mMovement != NULL && mText instanceof Editable
//                        && mLayout != NULL && onCheckIsTextEditor()) {
//                        InputMethodManager imm = InputMethodManager.peekInstance();
//                        viewClicked(imm);
//                        if (imm != NULL && getShowSoftInputOnFocus()) {
//                            imm.showSoftInput(this, 0);
//                        }
//                    }
//                }
//            }
//            return View::onKeyUp(keyCode, event);
//            
//        case KeyEvent.KEYCODE_ENTER:
//            if (event->hasNoModifiers()) {
//                if (mEditor != NULL && mEditor.mInputContentType != NULL
//                    && mEditor.mInputContentType.onEditorActionListener != NULL
//                    && mEditor.mInputContentType.enterDown) {
//                    mEditor.mInputContentType.enterDown = false;
//                    if (mEditor.mInputContentType.onEditorActionListener.onEditorAction(
//                                                                                        this, EditorInfo::IME_NULL, event)) {
//                        return true;
//                    }
//                }
//                
//                if ((event->getFlags() & KeyEvent.FLAG_EDITOR_ACTION) != 0
//                    || shouldAdvanceFocusOnEnter()) {
//                                        if (!hasOnClickListeners()) {
//                        View v = focusSearch(FOCUS_DOWN);
//                        
//                        if (v != NULL) {
//                            if (!v.requestFocus(FOCUS_DOWN)) {
//                                throw new IllegalStateException(
//                                                                "focus search returned a view " +
//                                                                "that wasn't able to take focus!");
//                            }
//                            
//                                                        View::onKeyUp(keyCode, event);
//                            return true;
//                        } else if ((event->getFlags()
//                                    & KeyEvent.FLAG_EDITOR_ACTION) != 0) {
//                            // No target for next focus, but make sure the IME
//                            // if this came from it.
//                            InputMethodManager imm = InputMethodManager.peekInstance();
//                            if (imm != NULL && imm.isActive(this)) {
//                                imm.hideSoftInputFromWindow(getWindowToken(), 0);
//                            }
//                        }
//                    }
//                }
//                return View::onKeyUp(keyCode, event);
//            }
//            break;
//    }
//    
//    if (mEditor != NULL && mEditor.mKeyListener != NULL)
//        if (mEditor.mKeyListener.onKeyUp(this, (Editable) mText, keyCode, event))
//            return true;
//    
//    if (mMovement != NULL && mLayout != NULL)
//        if (mMovement->onKeyUp(this, mTextSpannable, keyCode, event))
//            return true;
    
    return View::onKeyUp(keyCode, event);
}

void TextView::nullLayouts() {
    
    shared_ptr<BoringLayout> boring = dynamic_pointer_cast<BoringLayout>(mLayout);
    
    if (boring != NULL && mSavedLayout == NULL) {
        mSavedLayout = boring;
    }
    
    boring = dynamic_pointer_cast<BoringLayout>(mHintLayout);
    
    if (boring != NULL && mSavedHintLayout == NULL) {
        mSavedHintLayout = boring;
    }
    
    mSavedMarqueeModeLayout = NULL;
    mLayout = NULL;
    mHintLayout = NULL;
    
    mBoring = NULL;
    mHintBoring = NULL;
    
    // Since it depends on the value of mLayout
//    if (mEditor != NULL) mEditor.prepareCursorControllers();
}

void TextView::assumeLayout() {
    int width = mRight - mLeft - getCompoundPaddingLeft() - getCompoundPaddingRight();
    
    if (width < 1) {
        width = 0;
    }
    
    int physicalWidth = width;
    
    if (mHorizontallyScrolling) {
        width = VERY_WIDE;
    }
    
    makeNewLayout(width, physicalWidth, UNKNOWN_BORING, UNKNOWN_BORING,
                  physicalWidth, false);
}

Layout::Alignment TextView::getLayoutAlignment() {
    Layout::Alignment alignment;
    switch (getTextAlignment()) {
        case TEXT_ALIGNMENT_GRAVITY:
            switch (mGravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) {
                case Gravity::START:
                    alignment = Layout::Alignment::ALIGN_NORMAL;
                    break;
                case Gravity::END:
                    alignment = Layout::Alignment::ALIGN_OPPOSITE;
                    break;
                case Gravity::LEFT:
                    alignment = Layout::Alignment::ALIGN_LEFT;
                    break;
                case Gravity::RIGHT:
                    alignment = Layout::Alignment::ALIGN_RIGHT;
                    break;
                case Gravity::CENTER_HORIZONTAL:
                    alignment = Layout::Alignment::ALIGN_CENTER;
                    break;
                default:
                    alignment = Layout::Alignment::ALIGN_NORMAL;
                    break;
            }
            break;
        case TEXT_ALIGNMENT_TEXT_START:
            alignment = Layout::Alignment::ALIGN_NORMAL;
            break;
        case TEXT_ALIGNMENT_TEXT_END:
            alignment = Layout::Alignment::ALIGN_OPPOSITE;
            break;
        case TEXT_ALIGNMENT_CENTER:
            alignment = Layout::Alignment::ALIGN_CENTER;
            break;
        case TEXT_ALIGNMENT_VIEW_START:
            alignment = (getLayoutDirection() == LAYOUT_DIRECTION_RTL) ?
            Layout::Alignment::ALIGN_RIGHT : Layout::Alignment::ALIGN_LEFT;
            break;
        case TEXT_ALIGNMENT_VIEW_END:
            alignment = (getLayoutDirection() == LAYOUT_DIRECTION_RTL) ?
            Layout::Alignment::ALIGN_LEFT : Layout::Alignment::ALIGN_RIGHT;
            break;
        case TEXT_ALIGNMENT_INHERIT:
            // This should never happen as we have already resolved the text alignment
            // but better safe than sorry so we just fall through
        default:
            alignment = Layout::Alignment::ALIGN_NORMAL;
            break;
    }
    return alignment;
}

void TextView::makeNewLayout(int wantWidth, int hintWidth,
                   shared_ptr<BoringLayout::Metrics> boring,
                   shared_ptr<BoringLayout::Metrics> hintBoring,
                   int ellipsisWidth, bool bringIntoView) {
    stopMarquee();
    
    // Update "old" cached values
    mOldMaximum = mMaximum;
    mOldMaxMode = mMaxMode;
    
    mHighlightPathBogus = true;
    
    if (wantWidth < 0) {
        wantWidth = 0;
    }
    if (hintWidth < 0) {
        hintWidth = 0;
    }
    
    Layout::Alignment alignment = getLayoutAlignment();
    
    const bool testDirChange = mSingleLine && mLayout != NULL &&
    (alignment == Layout::Alignment::ALIGN_NORMAL ||
     alignment == Layout::Alignment::ALIGN_OPPOSITE);
    
    int oldDir = 0;
    if (testDirChange) oldDir = mLayout->getParagraphDirection(0);
    bool shouldEllipsize = mEllipsize != NULL && getKeyListener() == NULL;
    const bool switchEllipsize = mEllipsize == TextUtils::TruncateAt::MARQUEE &&
    mMarqueeFadeMode != MARQUEE_FADE_NORMAL;
    
    shared_ptr<TextUtils::TruncateAt> effectiveEllipsize = mEllipsize;
    
    if (mEllipsize == TextUtils::TruncateAt::MARQUEE &&
        mMarqueeFadeMode == MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
        effectiveEllipsize = TextUtils::TruncateAt::END_SMALL;
    }
    
    if (mTextDir == NULL) {
        mTextDir = getTextDirectionHeuristic();
    }
    
    mLayout = makeSingleLayout(wantWidth, boring, ellipsisWidth, alignment, shouldEllipsize,
                               effectiveEllipsize, effectiveEllipsize == mEllipsize);
    if (switchEllipsize) {
        shared_ptr<TextUtils::TruncateAt> oppositeEllipsize = effectiveEllipsize == TextUtils::TruncateAt::MARQUEE ?
        TextUtils::TruncateAt::END : TextUtils::TruncateAt::MARQUEE;
        mSavedMarqueeModeLayout = makeSingleLayout(wantWidth, boring, ellipsisWidth, alignment,
                                                   shouldEllipsize, oppositeEllipsize, effectiveEllipsize != mEllipsize);
    }
    
    shouldEllipsize = mEllipsize != NULL;
    mHintLayout = NULL;
    
    if (mHint != NULL) {
        if (shouldEllipsize) hintWidth = wantWidth;
        
        if (hintBoring == UNKNOWN_BORING) {
            hintBoring = BoringLayout::isBoring(mHint, mTextPaint, mTextDir,
                                                mHintBoring);
            if (hintBoring != NULL) {
                mHintBoring = hintBoring;
            }
        }
        
        if (hintBoring != NULL) {
            if (hintBoring->width <= hintWidth &&
                (!shouldEllipsize || hintBoring->width <= ellipsisWidth)) {
                if (mSavedHintLayout != NULL) {
                    mHintLayout = mSavedHintLayout->replaceOrMake(mHint, mTextPaint,
                                  hintWidth, alignment, mSpacingMult, mSpacingAdd,
                                  hintBoring, mIncludePad);
                } else {
                    mHintLayout = BoringLayout::make(mHint, mTextPaint,
                                                     hintWidth, alignment, mSpacingMult, mSpacingAdd,
                                                     hintBoring, mIncludePad);
                }
                
                mSavedHintLayout = dynamic_pointer_cast<BoringLayout>(mHintLayout);
            } else if (shouldEllipsize && hintBoring->width <= hintWidth) {
                if (mSavedHintLayout != NULL) {
                    mHintLayout = mSavedHintLayout->replaceOrMake(mHint, mTextPaint,
                                  hintWidth, alignment, mSpacingMult, mSpacingAdd,
                                  hintBoring, mIncludePad, mEllipsize,
                                  ellipsisWidth);
                } else {
                    mHintLayout = BoringLayout::make(mHint, mTextPaint,
                                                     hintWidth, alignment, mSpacingMult, mSpacingAdd,
                                                     hintBoring, mIncludePad, mEllipsize,
                                                     ellipsisWidth);
                }
            } else if (shouldEllipsize) {
                mHintLayout = make_shared<StaticLayout>(mHint,
                                               0, mHint->length(),
                                               mTextPaint, hintWidth, alignment, mTextDir, mSpacingMult,
                                               mSpacingAdd, mIncludePad, mEllipsize,
                                               ellipsisWidth, mMaxMode == LINES ? mMaximum : INT_MAX);
            } else {
                mHintLayout = make_shared<StaticLayout>(mHint, mTextPaint,
                                               hintWidth, alignment, mTextDir, mSpacingMult, mSpacingAdd,
                                               mIncludePad);
            }
        } else if (shouldEllipsize) {
            mHintLayout = make_shared<StaticLayout>(mHint,
                                           0, mHint->length(),
                                           mTextPaint, hintWidth, alignment, mTextDir, mSpacingMult,
                                           mSpacingAdd, mIncludePad, mEllipsize,
                                           ellipsisWidth, mMaxMode == LINES ? mMaximum : INT_MAX);
        } else {
            mHintLayout = make_shared<StaticLayout>(mHint, mTextPaint,
                                           hintWidth, alignment, mTextDir, mSpacingMult, mSpacingAdd,
                                           mIncludePad);
        }
    }
    
    if (bringIntoView || (testDirChange && oldDir != mLayout->getParagraphDirection(0))) {
        registerForPreDraw();
    }
    
    if (mEllipsize == TextUtils::TruncateAt::MARQUEE) {
        if (!compressText(ellipsisWidth)) {
            const int height = mLayoutParams->height;
            // If the size of the view does not depend on the size of the text, try to
            // start the marquee immediately
            if (height != LayoutParams::WRAP_CONTENT && height != LayoutParams::MATCH_PARENT) {
                startMarquee();
            } else {
                // Defer the start of the marquee until we know our width (see setFrame())
                mRestartMarquee = true;
            }
        }
    }
    
    // CursorControllers need a non-NULL mLayout
//    if (mEditor != NULL) mEditor.prepareCursorControllers();
}

shared_ptr<Layout> TextView::makeSingleLayout(int wantWidth, shared_ptr<BoringLayout::Metrics> boring, int ellipsisWidth,
                                              Layout::Alignment alignment, bool shouldEllipsize, shared_ptr<TextUtils::TruncateAt> effectiveEllipsize,
                        bool useSaved) {
    shared_ptr<Layout> result;
    
    if (mTextSpannable != NULL) {
//        result = make_shared<DynamicLayout>(mText, mTransformed, mTextPaint, wantWidth,
//                                   alignment, mTextDir, mSpacingMult,
//                                   mSpacingAdd, mIncludePad, getKeyListener() == NULL ? effectiveEllipsize : NULL,
//                                   ellipsisWidth);
    } else {
        if (boring == UNKNOWN_BORING) {
            boring = BoringLayout::isBoring(mTransformed, mTextPaint, mTextDir, mBoring);
            if (boring != NULL) {
                mBoring = boring;
            }
        }
        
        if (boring != NULL) {
            if (boring->width <= wantWidth &&
                (effectiveEllipsize == NULL || boring->width <= ellipsisWidth)) {
                if (useSaved && mSavedLayout != NULL) {
                    result = mSavedLayout->replaceOrMake(mTransformed, mTextPaint,
                                                         wantWidth, alignment, mSpacingMult, mSpacingAdd,
                                                         boring, mIncludePad);
                } else {
                    result = BoringLayout::make(mTransformed, mTextPaint,
                                                wantWidth, alignment, mSpacingMult, mSpacingAdd,
                                                boring, mIncludePad);
                }
                
                if (useSaved) {
                    mSavedLayout = dynamic_pointer_cast<BoringLayout>(result);
                }
            } else if (shouldEllipsize && boring->width <= wantWidth) {
                if (useSaved && mSavedLayout != NULL) {
                    result = mSavedLayout->replaceOrMake(mTransformed, mTextPaint,
                                                         wantWidth, alignment, mSpacingMult, mSpacingAdd,
                                                         boring, mIncludePad, effectiveEllipsize,
                                                         ellipsisWidth);
                } else {
                    result = BoringLayout::make(mTransformed, mTextPaint,
                                                wantWidth, alignment, mSpacingMult, mSpacingAdd,
                                                boring, mIncludePad, effectiveEllipsize,
                                                ellipsisWidth);
                }
            } else if (shouldEllipsize) {
                result = make_shared<StaticLayout>(mTransformed,
                                          0, mTransformed->length(),
                                          mTextPaint, wantWidth, alignment, mTextDir, mSpacingMult,
                                          mSpacingAdd, mIncludePad, effectiveEllipsize,
                                          ellipsisWidth, mMaxMode == LINES ? mMaximum : INT_MAX);
            } else {
                result = make_shared<StaticLayout>(mTransformed, mTextPaint,
                                          wantWidth, alignment, mTextDir, mSpacingMult, mSpacingAdd,
                                          mIncludePad);
            }
        } else if (shouldEllipsize) {
            result = make_shared<StaticLayout>(mTransformed,
                                      0, mTransformed->length(),
                                      mTextPaint, wantWidth, alignment, mTextDir, mSpacingMult,
                                      mSpacingAdd, mIncludePad, effectiveEllipsize,
                                      ellipsisWidth, mMaxMode == LINES ? mMaximum : INT_MAX);
        } else {
            result = make_shared<StaticLayout>(mTransformed, mTextPaint,
                                      wantWidth, alignment, mTextDir, mSpacingMult, mSpacingAdd,
                                      mIncludePad);
        }
    }
    return result;
}

bool TextView::compressText(float width) {
    
//    if (isHardwareAccelerated()) return false;
//    
//    // Only compress the text if it hasn't been compressed by the previous pass
//    if (width > 0.0f && mLayout != NULL && getLineCount() == 1 && !mUserSetTextScaleX &&
//        mTextPaint->getTextScaleX() == 1.0f) {
//        const float textWidth = mLayout->getLineWidth(0);
//        const float overflow = (textWidth + 1.0f - width) / width;
//        if (overflow > 0.0f && overflow <= Marquee::MARQUEE_DELTA_MAX) {
//            mTextPaint->setTextScaleX(1.0f - overflow - 0.005f);
//            post(new Runnable() {
//                public void run() {
//                    requestLayout();
//                }
//            });
//            return true;
//        }
//    }
    
    return false;
}

int TextView::desired(shared_ptr<Layout> layout) {
    int n = layout->getLineCount();
    shared_ptr<CharSequence> text = layout->getText();
    float maxx = 0;
    
    // if any line was wrapped, we can't use it.
    // but it's ok for the last line not to have a newline
    
    for (int i = 0; i < n - 1; i++) {
        if (text->charAt(layout->getLineEnd(i) - 1) != '\n')
            return -1;
    }
    
    for (int i = 0; i < n; i++) {
        maxx = max(maxx, layout->getLineWidth(i));
    }
    
    return (int) ceil(maxx);
}

void TextView::setIncludeFontPadding(bool includepad) {
    if (mIncludePad != includepad) {
        mIncludePad = includepad;
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

bool TextView::getIncludeFontPadding() {
    return mIncludePad;
}

void TextView::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    int widthMode = MeasureSpec::getMode(widthMeasureSpec);
    int heightMode = MeasureSpec::getMode(heightMeasureSpec);
    int widthSize = MeasureSpec::getSize(widthMeasureSpec);
    int heightSize = MeasureSpec::getSize(heightMeasureSpec);
    
    int width = 0;
    int height = 0;
    
    shared_ptr<BoringLayout::Metrics> boring = UNKNOWN_BORING;
    shared_ptr<BoringLayout::Metrics> hintBoring = UNKNOWN_BORING;
    
    if (mTextDir == NULL) {
        mTextDir = getTextDirectionHeuristic();
    }
    
    int des = -1;
    bool fromexisting = false;
    
    if (widthMode == MeasureSpec::EXACTLY) {
        // Parent has told us how big to be. So be it.
        width = widthSize;
    } else {
        if (mLayout != NULL && mEllipsize == NULL) {
            des = desired(mLayout);
        }
        
        if (des < 0) {
            boring = BoringLayout::isBoring(mTransformed, mTextPaint, mTextDir, mBoring);
            if (boring != NULL) {
                mBoring = boring;
            }
        } else {
            fromexisting = true;
        }
        
        if (boring == NULL || boring == UNKNOWN_BORING) {
            if (des < 0) {
                des = (int) ceil(Layout::getDesiredWidth(mTransformed, mTextPaint));
            }
            width = des;
        } else {
            width = boring->width;
        }
        
        const shared_ptr<Drawables> &dr = mDrawables;
        if (dr != NULL) {
            width = max(width, dr->mDrawableWidthTop);
            width = max(width, dr->mDrawableWidthBottom);
        }
        
        if (mHint != NULL) {
            int hintDes = -1;
            int hintWidth;
            
            if (mHintLayout != NULL && mEllipsize == NULL) {
                hintDes = desired(mHintLayout);
            }
            
            if (hintDes < 0) {
                hintBoring = BoringLayout::isBoring(mHint, mTextPaint, mTextDir, mHintBoring);
                if (hintBoring != NULL) {
                    mHintBoring = hintBoring;
                }
            }
            
            if (hintBoring == NULL || hintBoring == UNKNOWN_BORING) {
                if (hintDes < 0) {
                    hintDes = (int) ceil(Layout::getDesiredWidth(mHint, mTextPaint));
                }
                hintWidth = hintDes;
            } else {
                hintWidth = hintBoring->width;
            }
            
            if (hintWidth > width) {
                width = hintWidth;
            }
        }
        
        width += getCompoundPaddingLeft() + getCompoundPaddingRight();
        
        if (mMaxWidthMode == EMS) {
            width = min(width, mMaxWidth * getLineHeight());
        } else {
            width = min(width, mMaxWidth);
        }
        
        if (mMinWidthMode == EMS) {
            width = max(width, mMinWidth * getLineHeight());
        } else {
            width = max(width, mMinWidth);
        }
        
        // Check against our minimum width
        width = max(width, getSuggestedMinimumWidth());
        
        if (widthMode == MeasureSpec::AT_MOST) {
            width = min(widthSize, width);
        }
    }
    
    int want = width - getCompoundPaddingLeft() - getCompoundPaddingRight();
    int unpaddedWidth = want;
    
    if (mHorizontallyScrolling) want = VERY_WIDE;
    
    int hintWant = want;
    int hintWidth = (mHintLayout == NULL) ? hintWant : mHintLayout->getWidth();
    
    if (mLayout == NULL) {
        makeNewLayout(want, hintWant, boring, hintBoring,
                      width - getCompoundPaddingLeft() - getCompoundPaddingRight(), false);
    } else {
        const bool layoutChanged = (mLayout->getWidth() != want) ||
        (hintWidth != hintWant) ||
        (mLayout->getEllipsizedWidth() !=
         width - getCompoundPaddingLeft() - getCompoundPaddingRight());
        
        const bool widthChanged = (mHint == NULL) &&
        (mEllipsize == NULL) &&
        (want > mLayout->getWidth()) &&
        (dynamic_pointer_cast<BoringLayout>(mLayout) != NULL || (fromexisting && des >= 0 && des <= want));
        
        const bool maximumChanged = (mMaxMode != mOldMaxMode) || (mMaximum != mOldMaximum);
        
        if (layoutChanged || maximumChanged) {
            if (!maximumChanged && widthChanged) {
                mLayout->increaseWidthTo(want);
            } else {
                makeNewLayout(want, hintWant, boring, hintBoring,
                              width - getCompoundPaddingLeft() - getCompoundPaddingRight(), false);
            }
        } else {
            // Nothing has changed
        }
    }
    
    if (heightMode == MeasureSpec::EXACTLY) {
        // Parent has told us how big to be. So be it.
        height = heightSize;
        mDesiredHeightAtMeasure = -1;
    } else {
        int desired = getDesiredHeight();
        
        height = desired;
        mDesiredHeightAtMeasure = desired;
        
        if (heightMode == MeasureSpec::AT_MOST) {
            height = min(desired, heightSize);
        }
    }
    
    int unpaddedHeight = height - getCompoundPaddingTop() - getCompoundPaddingBottom();
    if (mMaxMode == LINES && mLayout->getLineCount() > mMaximum) {
        unpaddedHeight = min(unpaddedHeight, mLayout->getLineTop(mMaximum));
    }
    
        if (mMovement != NULL ||
        mLayout->getWidth() > unpaddedWidth ||
        mLayout->getHeight() > unpaddedHeight) {
        registerForPreDraw();
    } else {
//        scrollTo(0, 0);
    }
    
    setMeasuredDimension(width, height);
}

int TextView::getDesiredHeight() {
    return max(
                    getDesiredHeight(mLayout, true),
                    getDesiredHeight(mHintLayout, mEllipsize != NULL));
}

int TextView::getDesiredHeight(shared_ptr<Layout> layout, bool cap) {
    if (layout == NULL) {
        return 0;
    }
    
    int linecount = layout->getLineCount();
    int pad = getCompoundPaddingTop() + getCompoundPaddingBottom();
    int desired = layout->getLineTop(linecount);
    
    const shared_ptr<Drawables> &dr = mDrawables;
    if (dr != NULL) {
        desired = max(desired, dr->mDrawableHeightLeft);
        desired = max(desired, dr->mDrawableHeightRight);
    }
    
    desired += pad;
    
    if (mMaxMode == LINES) {
                if (cap) {
            if (linecount > mMaximum) {
                desired = layout->getLineTop(mMaximum);
                
                if (dr != NULL) {
                    desired = max(desired, dr->mDrawableHeightLeft);
                    desired = max(desired, dr->mDrawableHeightRight);
                }
                
                desired += pad;
                linecount = mMaximum;
            }
        }
    } else {
        desired = min(desired, mMaximum);
    }
    
    if (mMinMode == LINES) {
        if (linecount < mMinimum) {
            desired += getLineHeight() * (mMinimum - linecount);
        }
    } else {
        desired = max(desired, mMinimum);
    }
    
    // Check against our minimum height
    desired = max(desired, getSuggestedMinimumHeight());
    
    return desired;
}

void TextView::checkForResize() {
    bool sizeChanged = false;
    
    if (mLayout != NULL) {
        // Check if our width changed
        if (mLayoutParams->width == LayoutParams::WRAP_CONTENT) {
            sizeChanged = true;
            invalidate();
        }
        
        // Check if our height changed
        if (mLayoutParams->height == LayoutParams::WRAP_CONTENT) {
            int desiredHeight = getDesiredHeight();
            
            if (desiredHeight != getHeight()) {
                sizeChanged = true;
            }
        } else if (mLayoutParams->height == LayoutParams::MATCH_PARENT) {
            if (mDesiredHeightAtMeasure >= 0) {
                int desiredHeight = getDesiredHeight();
                
                if (desiredHeight != mDesiredHeightAtMeasure) {
                    sizeChanged = true;
                }
            }
        }
    }
    
    if (sizeChanged) {
        requestLayout();
        // caller will have already invalidated
    }
}

void TextView::checkForRelayout() {
    // If we have a fixed width, we can just swap in a new text layout
    // if the text height stays the same or if the view height is fixed.
    
    if ((mLayoutParams->width != LayoutParams::WRAP_CONTENT ||
         (mMaxWidthMode == mMinWidthMode && mMaxWidth == mMinWidth)) &&
        (mHint == NULL || mHintLayout != NULL) &&
        (mRight - mLeft - getCompoundPaddingLeft() - getCompoundPaddingRight() > 0)) {
        // Static width, so try making a new text layout->
        
        int oldht = mLayout->getHeight();
        int want = mLayout->getWidth();
        int hintWant = mHintLayout == NULL ? 0 : mHintLayout->getWidth();
        
                makeNewLayout(want, hintWant, UNKNOWN_BORING, UNKNOWN_BORING,
                      mRight - mLeft - getCompoundPaddingLeft() - getCompoundPaddingRight(),
                      false);
        
        if (mEllipsize != TextUtils::TruncateAt::MARQUEE) {
            // In a fixed-height view, so use our new text layout->
            if (mLayoutParams->height != LayoutParams::WRAP_CONTENT &&
                mLayoutParams->height != LayoutParams::MATCH_PARENT) {
                invalidate();
                return;
            }
            
            // Dynamic height, but height has stayed the same,
            // so use our new text layout->
            if (mLayout->getHeight() == oldht &&
                (mHintLayout == NULL || mHintLayout->getHeight() == oldht)) {
                invalidate();
                return;
            }
        }
        
        // We lose: the height has changed and we have a dynamic height.
        // Request a new view layout using our new text layout->
        requestLayout();
        invalidate();
    } else {
        // Dynamic width, so we have no choice but to request a new
        // view layout with a new text layout->
        nullLayouts();
        requestLayout();
        invalidate();
    }
}

void TextView::onLayout(bool changed, int left, int top, int right, int bottom) {
    View::onLayout(changed, left, top, right, bottom);
    if (mDeferScroll >= 0) {
        int curs = mDeferScroll;
        mDeferScroll = -1;
        bringPointIntoView(min(curs, mText->length()));
    }
}

bool TextView::isShowingHint() {
    return TextUtils::isEmpty(mText) && !TextUtils::isEmpty(mHint);
}

bool TextView::bringTextIntoView() {
    shared_ptr<Layout> &layout = isShowingHint() ? mHintLayout : mLayout;
    int line = 0;
    if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) == Gravity::BOTTOM) {
        line = layout->getLineCount() - 1;
    }
    
    Layout::Alignment a = layout->getParagraphAlignment(line);
    int dir = layout->getParagraphDirection(line);
    int hspace = mRight - mLeft - getCompoundPaddingLeft() - getCompoundPaddingRight();
    int vspace = mBottom - mTop - getExtendedPaddingTop() - getExtendedPaddingBottom();
    int ht = layout->getHeight();
    
    int scrollx, scrolly;
    
    // Convert to left, center, or right alignment.
    if (a == Layout::Alignment::ALIGN_NORMAL) {
        a = dir == Directions::DIR_LEFT_TO_RIGHT ? Layout::Alignment::ALIGN_LEFT :
        Layout::Alignment::ALIGN_RIGHT;
    } else if (a == Layout::Alignment::ALIGN_OPPOSITE){
        a = dir == Directions::DIR_LEFT_TO_RIGHT ? Layout::Alignment::ALIGN_RIGHT :
        Layout::Alignment::ALIGN_LEFT;
    }
    
    if (a == Layout::Alignment::ALIGN_CENTER) {
                
        int left = (int) floor(layout->getLineLeft(line));
        int right = (int) ceil(layout->getLineRight(line));
        
        if (right - left < hspace) {
            scrollx = (right + left) / 2 - hspace / 2;
        } else {
            if (dir < 0) {
                scrollx = right - hspace;
            } else {
                scrollx = left;
            }
        }
    } else if (a == Layout::Alignment::ALIGN_RIGHT) {
        int right = (int) ceil(layout->getLineRight(line));
        scrollx = right - hspace;
    } else { // a == Layout::Alignment::ALIGN_LEFT (will also be the default)
        scrollx = (int) floor(layout->getLineLeft(line));
    }
    
    if (ht < vspace) {
        scrolly = 0;
    } else {
        if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) == Gravity::BOTTOM) {
            scrolly = ht - vspace;
        } else {
            scrolly = 0;
        }
    }
    
    if (scrollx != mScrollX || scrolly != mScrollY) {
        scrollTo(scrollx, scrolly);
        return true;
    } else {
        return false;
    }
}

bool TextView::bringPointIntoView(int offset) {
    if (isLayoutRequested()) {
        mDeferScroll = offset;
        return false;
    }
    bool changed = false;
    
    shared_ptr<Layout> layout = isShowingHint() ? mHintLayout: mLayout;
    
    if (layout == NULL) return changed;
    
    int line = layout->getLineForOffset(offset);
    
    int grav;
    
    switch (layout->getParagraphAlignment(line)) {
        case Layout::Alignment::ALIGN_LEFT:
            grav = 1;
            break;
        case Layout::Alignment::ALIGN_RIGHT:
            grav = -1;
            break;
        case Layout::Alignment::ALIGN_NORMAL:
            grav = layout->getParagraphDirection(line);
            break;
        case Layout::Alignment::ALIGN_OPPOSITE:
            grav = -layout->getParagraphDirection(line);
            break;
        case Layout::Alignment::ALIGN_CENTER:
        default:
            grav = 0;
            break;
    }
    
    // We only want to clamp the cursor to fit within the layout width
    // in left-to-right modes, because in a right to left alignment,
    // we want to scroll to keep the line-right on the screen, as other
    // lines are likely to have text flush with the right margin, which
    // we want to keep visible.
    // A better long-term solution would probably be to measure both
    // the full line and a blank-trimmed version, and, for example, use
    // the latter measurement for centering and right alignment, but for
    // the time being we only implement the cursor clamping in left to
    // right where it is most likely to be annoying.
    const bool clamped = grav > 0;
    // FIXME: Is it okay to truncate this, or should we round?
    const int x = (int)layout->getPrimaryHorizontal(offset, clamped);
    const int top = layout->getLineTop(line);
    const int bottom = layout->getLineTop(line + 1);
    
    int left = (int) floor(layout->getLineLeft(line));
    int right = (int) ceil(layout->getLineRight(line));
    int ht = layout->getHeight();
    
    int hspace = mRight - mLeft - getCompoundPaddingLeft() - getCompoundPaddingRight();
    int vspace = mBottom - mTop - getExtendedPaddingTop() - getExtendedPaddingBottom();
    if (!mHorizontallyScrolling && right - left > hspace && right > x) {
        // If cursor has been clamped, make sure we don't scroll.
        right = max(x, left + hspace);
    }
    
    int hslack = (bottom - top) / 2;
    int vslack = hslack;
    
    if (vslack > vspace / 4)
        vslack = vspace / 4;
    if (hslack > hspace / 4)
        hslack = hspace / 4;
    
    int hs = mScrollX;
    int vs = mScrollY;
    
    if (top - vs < vslack)
        vs = top - vslack;
    if (bottom - vs > vspace - vslack)
        vs = bottom - (vspace - vslack);
    if (ht - vs < vspace)
        vs = ht - vspace;
    if (0 - vs > 0)
        vs = 0;
    
    if (grav != 0) {
        if (x - hs < hslack) {
            hs = x - hslack;
        }
        if (x - hs > hspace - hslack) {
            hs = x - (hspace - hslack);
        }
    }
    
    if (grav < 0) {
        if (left - hs > 0)
            hs = left;
        if (right - hs < hspace)
            hs = right - hspace;
    } else if (grav > 0) {
        if (right - hs < hspace)
            hs = right - hspace;
        if (left - hs > 0)
            hs = left;
    } else {
        if (right - left <= hspace) {
                        hs = left - (hspace - (right - left)) / 2;
        } else if (x > right - hslack) {
                        hs = right - hspace;
        } else if (x < left + hslack) {
                        hs = left;
        } else if (left > hs) {
                        hs = left;
        } else if (right < hs + hspace) {
                        hs = right - hspace;
        } else {
                        if (x - hs < hslack) {
                hs = x - hslack;
            }
            if (x - hs > hspace - hslack) {
                hs = x - (hspace - hslack);
            }
        }
    }
    
    if (hs != mScrollX || vs != mScrollY) {
        if (mScroller == NULL) {
            scrollTo(hs, vs);
        } /*else {
            long duration = AnimationUtils.currentAnimationTimeMillis() - mLastScroll;
            int dx = hs - mScrollX;
            int dy = vs - mScrollY;
            
            if (duration > ANIMATED_SCROLL_GAP) {
                mScroller.startScroll(mScrollX, mScrollY, dx, dy);
                awakenScrollBars(mScroller.getDuration());
                invalidate();
            } else {
                if (!mScroller.isFinished()) {
                    mScroller.abortAnimation();
                }
                
                scrollBy(dx, dy);
            }
            
            mLastScroll = AnimationUtils::currentAnimationTimeMillis();
        }*/
        
        changed = true;
    }
    
    if (isFocused()) {
        // This offsets because getInterestingRect() is in terms of viewport coordinates, but
        // requestRectangleOnScreen() is in terms of content coordinates.
        
        // The offsets here are to ensure the rectangle we are using is
        // within our view bounds, in case the cursor is on the far left
        // or right->  If it isn't withing the bounds, then this request
        // will be ignored.
        mTempRect.set(x - 2, top, x + 2, bottom);
        getInterestingRect(mTempRect, line);
        mTempRect.offset(mScrollX, mScrollY);
        
//        if (requestRectangleOnScreen(mTempRect)) {
//            changed = true;
//        }
    }
    
    return changed;
}

bool TextView::moveCursorToVisibleOffset() {
    if (mTextSpannable == NULL) {
        return false;
    }
    int start = getSelectionStart();
    int end = getSelectionEnd();
    if (start != end) {
        return false;
    }
    
    // First: make sure the line is visible on screen:
    
    int line = mLayout->getLineForOffset(start);
    
    const int top = mLayout->getLineTop(line);
    const int bottom = mLayout->getLineTop(line + 1);
    const int vspace = mBottom - mTop - getExtendedPaddingTop() - getExtendedPaddingBottom();
    int vslack = (bottom - top) / 2;
    if (vslack > vspace / 4)
        vslack = vspace / 4;
    const int vs = mScrollY;
    
    if (top < (vs + vslack)) {
        line = mLayout->getLineForVertical(vs + vslack + (bottom - top));
    } else if (bottom > (vspace + vs - vslack)) {
        line = mLayout->getLineForVertical(vspace + vs - vslack - (bottom - top));
    }
    
    // Next: make sure the character is visible on screen:
    
    const int hspace = mRight - mLeft - getCompoundPaddingLeft() - getCompoundPaddingRight();
    const int hs = mScrollX;
    const int leftChar = mLayout->getOffsetForHorizontal(line, hs);
    const int rightChar = mLayout->getOffsetForHorizontal(line, hspace+hs);
    
    // line might contain bidirectional text
    const int lowChar = leftChar < rightChar ? leftChar : rightChar;
    const int highChar = leftChar > rightChar ? leftChar : rightChar;
    
    int newStart = start;
    if (newStart < lowChar) {
        newStart = lowChar;
    } else if (newStart > highChar) {
        newStart = highChar;
    }
    
    if (newStart != start) {
        Selection::setSelection(mTextSpannable, newStart);
        return true;
    }
    
    return false;
}

void TextView::computeScroll() {
    if (mScroller != NULL) {
//        if (mScroller.computeScrollOffset()) {
//            mScrollX = mScroller.getCurrX();
//            mScrollY = mScroller.getCurrY();
//            invalidateParentCaches();
//            postInvalidate();  // So we draw again
//        }
    }
}

void TextView::getInterestingRect(Rect r, int line) {
    convertFromViewportToContentCoordinates(r);
    
    // Rectangle can can be expanded on first and last line to take
    // padding into account.
    // TODO Take left/right padding into account too?
    if (line == 0) r.fTop -= getExtendedPaddingTop();
    if (line == mLayout->getLineCount() - 1) r.fBottom += getExtendedPaddingBottom();
}

void TextView::convertFromViewportToContentCoordinates(Rect r) {
    const int horizontalOffset = viewportToContentHorizontalOffset();
    r.fLeft += horizontalOffset;
    r.fRight += horizontalOffset;
    
    const int verticalOffset = viewportToContentVerticalOffset();
    r.fTop += verticalOffset;
    r.fBottom += verticalOffset;
}

int TextView::viewportToContentHorizontalOffset() {
    return getCompoundPaddingLeft() - mScrollX;
}

int TextView::viewportToContentVerticalOffset() {
    int offset = getExtendedPaddingTop() - mScrollY;
    if ((mGravity & Gravity::VERTICAL_GRAVITY_MASK) != Gravity::TOP) {
        offset += getVerticalOffset(false);
    }
    return offset;
}

int TextView::getSelectionStart() {
    return Selection::getSelectionStart(getText());
}

int TextView::getSelectionEnd() {
    return Selection::getSelectionEnd(getText());
}

bool TextView::hasSelection() {
    const int selectionStart = getSelectionStart();
    const int selectionEnd = getSelectionEnd();
    
    return selectionStart >= 0 && selectionStart != selectionEnd;
}

void TextView::setSingleLine() {
    setSingleLine(true);
}

void TextView::setAllCaps(bool allCaps) {
//    if (allCaps) {
//        setTransformationMethod(make_shared<AllCapsTransformationMethod>(getContext()));
//    } else {
        setTransformationMethod(NULL);
//    }
}

void TextView::setSingleLine(bool singleLine) {
    // Could be used, but may break backward compatibility.
    // if (mSingleLine == singleLine) return;
    setInputTypeSingleLine(singleLine);
    applySingleLine(singleLine, true, true);
}

void TextView::setInputTypeSingleLine(bool singleLine) {
//    if (mEditor != NULL &&
//        (mEditor.mInputType & EditorInfo::TYPE_MASK_CLASS) == EditorInfo::TYPE_CLASS_TEXT) {
//        if (singleLine) {
//            mEditor.mInputType &= ~EditorInfo::TYPE_TEXT_FLAG_MULTI_LINE;
//        } else {
//            mEditor.mInputType |= EditorInfo::TYPE_TEXT_FLAG_MULTI_LINE;
//        }
//    }
}

void TextView::applySingleLine(bool singleLine, bool applyTransformation,
                     bool changeMaxLines) {
    mSingleLine = singleLine;
    if (singleLine) {
        setLines(1);
        setHorizontallyScrolling(true);
        if (applyTransformation) {
            setTransformationMethod(SingleLineTransformationMethod::getInstance());
        }
    } else {
        if (changeMaxLines) {
            setMaxLines(INT_MAX);
        }
        setHorizontallyScrolling(false);
        if (applyTransformation) {
            setTransformationMethod(NULL);
        }
    }
}

void TextView::setEllipsize(shared_ptr<TextUtils::TruncateAt> where) {
    // TruncateAt is an enum. != comparison is ok between these singleton objects.
    if (mEllipsize != where) {
        mEllipsize = where;
        
        if (mLayout != NULL) {
            nullLayouts();
            requestLayout();
            invalidate();
        }
    }
}

void TextView::setMarqueeRepeatLimit(int marqueeLimit) {
    mMarqueeRepeatLimit = marqueeLimit;
}

int TextView::getMarqueeRepeatLimit() {
    return mMarqueeRepeatLimit;
}

shared_ptr<TextUtils::TruncateAt> TextView::getEllipsize() {
    return mEllipsize;
}

void TextView::setSelectAllOnFocus(bool selectAllOnFocus) {
//    createEditorIfNeeded();
//    mEditor.mSelectAllOnFocus = selectAllOnFocus;
    
    if (selectAllOnFocus && mTextSpannable == NULL) {
        setText(mText, BufferType::SPANNABLE);
    }
}

void TextView::setCursorVisible(bool visible) {
//    if (visible && mEditor == NULL) return; // visible is the default value with no edit data
//    createEditorIfNeeded();
//    if (mEditor.mCursorVisible != visible) {
//        mEditor.mCursorVisible = visible;
//        invalidate();
//        
//        mEditor.makeBlink();
//        
//        // InsertionPointCursorController depends on mCursorVisible
//        mEditor.prepareCursorControllers();
//    }
}

bool TextView::isCursorVisible() {
    // true is the default value
    return false; //mEditor == NULL ? true : mEditor.mCursorVisible;
}

bool TextView::canMarquee() {
    int width = (mRight - mLeft - getCompoundPaddingLeft() - getCompoundPaddingRight());
    return width > 0 && (mLayout->getLineWidth(0) > width ||
                         (mMarqueeFadeMode != MARQUEE_FADE_NORMAL && mSavedMarqueeModeLayout != NULL &&
                          mSavedMarqueeModeLayout->getLineWidth(0) > width));
}

void TextView::startMarquee() {
    // Do not ellipsize EditText
    if (getKeyListener() != NULL) return;
    
    if (compressText(getWidth() - getCompoundPaddingLeft() - getCompoundPaddingRight())) {
        return;
    }
    
//    if ((mMarquee == NULL || mMarquee->isStopped()) && (isFocused() || isSelected()) &&
//        getLineCount() == 1 && canMarquee()) {
//        
//        if (mMarqueeFadeMode == MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
//            mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_FADE;
//            const shared_ptr<Layout> &tmp = mLayout;
//            mLayout = mSavedMarqueeModeLayout;
//            mSavedMarqueeModeLayout = tmp;
//            setHorizontalFadingEdgeEnabled(true);
//            requestLayout();
//            invalidate();
//        }
//        
//        if (mMarquee == NULL) mMarquee = new Marquee(this);
//        mMarquee->start(mMarqueeRepeatLimit);
//    }
}

void TextView::stopMarquee() {
//    if (mMarquee != NULL && !mMarquee->isStopped()) {
//        mMarquee->stop();
//    }
    
    if (mMarqueeFadeMode == MARQUEE_FADE_SWITCH_SHOW_FADE) {
        mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS;
        const shared_ptr<Layout> &tmp = mSavedMarqueeModeLayout;
        mSavedMarqueeModeLayout = mLayout;
        mLayout = tmp;
        setHorizontalFadingEdgeEnabled(false);
        requestLayout();
        invalidate();
    }
}

void TextView::startStopMarquee(bool start) {
    if (mEllipsize == TextUtils::TruncateAt::MARQUEE) {
        if (start) {
            startMarquee();
        } else {
            stopMarquee();
        }
    }
}

void TextView::onTextChanged(shared_ptr<CharSequence> text, int start, int lengthBefore, int lengthAfter) {
    // intentionally empty, template pattern method can be overridden by subclasses
}

void TextView::onSelectionChanged(int selStart, int selEnd) {
//    sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_TEXT_SELECTION_CHANGED);
//    notifyAccessibilityStateChanged();
}

void TextView::addTextChangedListener(shared_ptr<TextWatcher> watcher) {
    mListeners.push_back(watcher);
}

void TextView::removeTextChangedListener(shared_ptr<TextWatcher> watcher) {
    mListeners.erase(remove(mListeners.begin(), mListeners.end(), watcher), mListeners.end());
}

void TextView::sendBeforeTextChanged(shared_ptr<CharSequence> text, int start, int before, int after) {
    
    const vector<shared_ptr<TextWatcher>> &list = mListeners;
    const int count = list.size();
    for (int i = 0; i < count; i++) {
        list[i]->beforeTextChanged(text, start, before, after);
    }
    
    // The spans that are inside or intersect the modified region no longer make sense
    removeIntersectingSpans(start, start + before, "SpellCheckSpan");
    removeIntersectingSpans(start, start + before, "SuggestionSpan");
}

// Removes all spans that are inside or actually overlap the start->.end range
void TextView::removeIntersectingSpans(int start, int end, string type) {
//    if (!(mText instanceof Editable)) return;
//    Editable text = (Editable) mText;
//    
//    vector<shared_ptr<Object>> spans = text->getSpans(start, end, type);
//    const int length = spans.size();
//    for (int i = 0; i < length; i++) {
//        const int s = text->getSpanStart(spans[i]);
//        const int e = text->getSpanEnd(spans[i]);
//        // Spans that are adjacent to the edited region will be handled in
//        // updateSpellCheckSpans. Result depends on what will be added (space or text)
//        if (e == start || s == end) break;
//        text->removeSpan(spans[i]);
//    }
}

void TextView::sendOnTextChanged(shared_ptr<CharSequence> text, int start, int before, int after) {

    const vector<shared_ptr<TextWatcher>> &list = mListeners;
    const int count = list.size();
    for (int i = 0; i < count; i++) {
        list[i]->onTextChanged(text, start, before, after);
    }
    
//    if (mEditor != NULL) mEditor.sendOnTextChanged(start, after);
}

void TextView::sendAfterTextChanged(shared_ptr<Editable> text) {

    const vector<shared_ptr<TextWatcher>> &list = mListeners;
    const int count = list.size();
    for (int i = 0; i < count; i++) {
        list[i]->afterTextChanged(text);
    }
}

void TextView::updateAfterEdit() {
    invalidate();
    int curs = getSelectionStart();
    
    if (curs >= 0 || (mGravity & Gravity::VERTICAL_GRAVITY_MASK) == Gravity::BOTTOM) {
        registerForPreDraw();
    }
    
    checkForResize();
    
    if (curs >= 0) {
        mHighlightPathBogus = true;
//        if (mEditor != NULL) mEditor.makeBlink();
        bringPointIntoView(curs);
    }
}

void TextView::handleTextChanged(shared_ptr<CharSequence> buffer, int start, int before, int after) {
//    const Editor.InputMethodState ims = mEditor == NULL ? NULL : mEditor.mInputMethodState;
//    if (ims == NULL || ims.mBatchEditNesting == 0) {
//        updateAfterEdit();
//    }
//    if (ims != NULL) {
//        ims.mContentChanged = true;
//        if (ims.mChangedStart < 0) {
//            ims.mChangedStart = start;
//            ims.mChangedEnd = start+before;
//        } else {
//            ims.mChangedStart = min(ims.mChangedStart, start);
//            ims.mChangedEnd = max(ims.mChangedEnd, start + before - ims.mChangedDelta);
//        }
//        ims.mChangedDelta += after-before;
//    }
    
    sendOnTextChanged(buffer, start, before, after);
    onTextChanged(buffer, start, before, after);
}

void TextView::spanChange(shared_ptr<Spanned> buf, shared_ptr<Object> what, int oldStart, int newStart, int oldEnd, int newEnd) {
//    // XXX Make the start and end move together if this ends up
//    // spending too much time invalidating.
//    
//    bool selChanged = false;
//    int newSelStart=-1, newSelEnd=-1;
//    
//    const Editor.InputMethodState ims = mEditor == NULL ? NULL : mEditor.mInputMethodState;
//    
//    if (what == Selection::SELECTION_END) {
//        selChanged = true;
//        newSelEnd = newStart;
//        
//        if (oldStart >= 0 || newStart >= 0) {
//            invalidateCursor(Selection::getSelectionStart(buf), oldStart, newStart);
//            checkForResize();
//            registerForPreDraw();
//            if (mEditor != NULL) mEditor.makeBlink();
//        }
//    }
//    
//    if (what == Selection::SELECTION_START) {
//        selChanged = true;
//        newSelStart = newStart;
//        
//        if (oldStart >= 0 || newStart >= 0) {
//            int end = Selection::getSelectionEnd(buf);
//            invalidateCursor(end, oldStart, newStart);
//        }
//    }
//    
//    if (selChanged) {
//        mHighlightPathBogus = true;
//        if (mEditor != NULL && !isFocused()) mEditor.mSelectionMoved = true;
//        
//        if ((buf.getSpanFlags(what)&Spanned.SPAN_INTERMEDIATE) == 0) {
//            if (newSelStart < 0) {
//                newSelStart = Selection::getSelectionStart(buf);
//            }
//            if (newSelEnd < 0) {
//                newSelEnd = Selection::getSelectionEnd(buf);
//            }
//            onSelectionChanged(newSelStart, newSelEnd);
//        }
//    }
//    
//    if (what instanceof UpdateAppearance || what instanceof ParagraphStyle ||
//        what instanceof CharacterStyle) {
//        if (ims == NULL || ims.mBatchEditNesting == 0) {
//            invalidate();
//            mHighlightPathBogus = true;
//            checkForResize();
//        } else {
//            ims.mContentChanged = true;
//        }
//        if (mEditor != NULL) {
//            if (oldStart >= 0) mEditor.invalidateTextDisplayList(mLayout, oldStart, oldEnd);
//            if (newStart >= 0) mEditor.invalidateTextDisplayList(mLayout, newStart, newEnd);
//        }
//    }
//    
//    if (MetaKeyKeyListener.isMetaTracker(buf, what)) {
//        mHighlightPathBogus = true;
//        if (ims != NULL && MetaKeyKeyListener.isSelectingMetaTracker(buf, what)) {
//            ims.mSelectionModeChanged = true;
//        }
//        
//        if (Selection::getSelectionStart(buf) >= 0) {
//            if (ims == NULL || ims.mBatchEditNesting == 0) {
//                invalidateCursor();
//            } else {
//                ims.mCursorChanged = true;
//            }
//        }
//    }
//    
//    if (what instanceof ParcelableSpan) {
//        // If this is a span that can be sent to a remote process,
//        // the current extract editor would be interested in it.
//        if (ims != NULL && ims.mExtractedTextRequest != NULL) {
//            if (ims.mBatchEditNesting != 0) {
//                if (oldStart >= 0) {
//                    if (ims.mChangedStart > oldStart) {
//                        ims.mChangedStart = oldStart;
//                    }
//                    if (ims.mChangedStart > oldEnd) {
//                        ims.mChangedStart = oldEnd;
//                    }
//                }
//                if (newStart >= 0) {
//                    if (ims.mChangedStart > newStart) {
//                        ims.mChangedStart = newStart;
//                    }
//                    if (ims.mChangedStart > newEnd) {
//                        ims.mChangedStart = newEnd;
//                    }
//                }
//            } else {
//                if (DEBUG_EXTRACT) Log.v(LOG_TAG, "Span change outside of batch: "
//                                         + oldStart + "-" + oldEnd + ","
//                                         + newStart + "-" + newEnd + " " + what);
//                ims.mContentChanged = true;
//            }
//        }
//    }
//    
//    if (mEditor != NULL && mEditor.mSpellChecker != NULL && newStart < 0 &&
//        what instanceof SpellCheckSpan) {
//        mEditor.mSpellChecker.onSpellCheckSpanRemoved((SpellCheckSpan) what);
//    }
}

void TextView::dispatchFinishTemporaryDetach() {
    mDispatchTemporaryDetach = true;
    View::dispatchFinishTemporaryDetach();
    mDispatchTemporaryDetach = false;
}

void TextView::onStartTemporaryDetach() {
    View::onStartTemporaryDetach();
    // Only track when onStartTemporaryDetach() is called directly,
    // usually because this instance is an editable field in a list
    if (!mDispatchTemporaryDetach) mTemporaryDetach = true;
    
    // Tell the editor that we are temporarily detached. It can use this to preserve
    // selection state as needed.
//    if (mEditor != NULL) mEditor.mTemporaryDetach = true;
}

void TextView::onFinishTemporaryDetach() {
    View::onFinishTemporaryDetach();
    // Only track when onStartTemporaryDetach() is called directly,
    // usually because this instance is an editable field in a list
    if (!mDispatchTemporaryDetach) mTemporaryDetach = false;
//    if (mEditor != NULL) mEditor.mTemporaryDetach = false;
}

bool TextView::onTouchEvent(MotionEvent *event) {
    const int action = event->getActionMasked();
    
//    if (mEditor != NULL) mEditor.onTouchEvent(event);
    
    const bool superResult = View::onTouchEvent(event);
    
//    if (mEditor != NULL && mEditor.mDiscardNextActionUp && action == MotionEvent.ACTION_UP) {
//        mEditor.mDiscardNextActionUp = false;
//        return superResult;
//    }
    
    const bool touchIsFinished = (action == MotionEvent::ACTION_UP) &&
    (true /*mEditor == NULL || !mEditor.mIgnoreActionUpEvent */) && isFocused();
    
    if ((mMovement != NULL || onCheckIsTextEditor()) && isEnabled()
        && mTextSpannable != NULL && mLayout != NULL) {
        bool handled = false;
        
        if (mMovement != NULL) {
            handled |= mMovement->onTouchEvent(this, mTextSpannable, event);
        }
        
        const bool textIsSelectable = isTextSelectable();
        if (touchIsFinished && mLinksClickable && mAutoLinkMask != 0 && textIsSelectable) {
            // The LinkMovementMethod which should handle taps on links has not been installed
            // on non editable text that support text selection.
            // We reproduce its behavior here to open links for these.
//            vector<shared_ptr<Object>> links = mTextSpannable->getSpans(getSelectionStart(),
//                                                                        getSelectionEnd(), "ClickableSpan");
//            
//            if (links.length > 0) {
//                dynamic_pointer_cast<ClickableSpan>(links[0])->onClick(this);
//                handled = true;
//            }
        }
        
        if (touchIsFinished && (isTextEditable() || textIsSelectable)) {
            // Show the IME, except when selecting in read-only text->
//            const InputMethodManager imm = InputMethodManager.peekInstance();
//            viewClicked(imm);
//            if (!textIsSelectable && mEditor.mShowSoftInputOnFocus) {
//                handled |= imm != NULL && imm.showSoftInput(this, 0);
//            }
//            
//            // The above condition ensures that the mEditor is not NULL
//            mEditor.onTouchUpEvent(event);
            
            handled = true;
        }
        
        if (handled) {
            return true;
        }
    }
    
    return superResult;
}

bool TextView::onGenericMotionEvent(MotionEvent *event) {
    if (mMovement != NULL && mTextSpannable != NULL && mLayout != NULL) {
        if (mMovement->onGenericMotionEvent(this, mTextSpannable, event)) {
            return true;
        }
    }
    return View::onGenericMotionEvent(event);
}

bool TextView::isTextEditable() {
    return false; //mText instanceof Editable && onCheckIsTextEditor() && isEnabled();
}

bool TextView::didTouchFocusSelect() {
    return false; //mEditor != NULL && mEditor.mTouchFocusSelected;
}

void TextView::cancelLongPress() {
    View::cancelLongPress();
//    if (mEditor != NULL) mEditor.mIgnoreActionUpEvent = true;
}

shared_ptr<ColorStateList> TextView::getTextColors(Context *context, AttributeSet *attrs) {
    shared_ptr<ColorStateList> colors;
//    attrs->getAttributeColorStateList(context->getResources(), <#const char *attribute#>)
//    colors = attrs.getColorStateList(R::styleable::textColor);
//    
//    if (colors == NULL) {
//        int ap = attrs.getResourceId(com.android.internal.R.styleable.
//                                     TextView_textAppearance, -1);
//        if (ap != -1) {
//            TypedArray appearance;
//            appearance = context->obtainStyledAttributes(ap,
//                                                        com.android.internal.R.styleable.TextAppearance);
//            colors = appearance.getColorStateList(com.android.internal.R.styleable.
//                                                  TextAppearance_textColor);
//            appearance.recycle();
//        }
//    }
    
    return colors;
}

int TextView::getTextColor(Context *context, AttributeSet *attrs, int def) {
    shared_ptr<ColorStateList> colors = getTextColors(context, attrs);
    
    if (colors == NULL) {
        return def;
    } else {
        return colors->getDefaultColor();
    }
}

bool TextView::canSelectText() {
    return false; //mText->length() != 0 && mEditor != NULL && mEditor.hasSelectionController();
}

bool TextView::textCanBeSelected() {
    // prepareCursorController() relies on this method.
    // If you change this condition, make sure prepareCursorController is called anywhere
    // the value of this condition might be changed.
    if (mMovement == NULL || !mMovement->canSelectArbitrarily()) return false;
    return isTextEditable() || (isTextSelectable() && mTextSpannable != NULL && isEnabled());
}

void TextView::updateTextServicesLocaleAsync() {
//    AsyncTask.execute(new Runnable() {
//        @Override
//        void run() {
//            if (mCurrentTextServicesLocaleLock.tryLock()) {
//                try {
//                    updateTextServicesLocaleLocked();
//                } constly {
//                    mCurrentTextServicesLocaleLock.unlock();
//                }
//            }
//        }
//    });
}

void TextView::updateTextServicesLocaleLocked() {
//    const TextServicesManager textServicesManager = (TextServicesManager)
//    mContext->getSystemService(Context->TEXT_SERVICES_MANAGER_SERVICE);
//    const SpellCheckerSubtype subtype = textServicesManager.getCurrentSpellCheckerSubtype(true);
//    const Locale locale;
//    if (subtype != NULL) {
//        locale = SpellCheckerSubtype.constructLocaleFromString(subtype.getLocale());
//    } else {
//        locale = NULL;
//    }
//    mCurrentSpellCheckerLocaleCache = locale;
}

void TextView::onLocaleChanged() {
    // Will be re-created on demand in getWordIterator with the proper new locale
//    mEditor.mWordIterator = NULL;
}

//WordIterator TextView::getWordIterator() {
//    if (mEditor != NULL) {
//        return mEditor.getWordIterator();
//    } else {
//        return NULL;
//    }
//}
//
//bool TextView::isInputMethodTarget() {
//    InputMethodManager imm = InputMethodManager.peekInstance();
//    return imm != NULL && imm.isActive(this);
//}
//
//bool onTextContextMenuItem(int id) {
//    int min = 0;
//    int max = mText->length();
//    
//    if (isFocused()) {
//        const int selStart = getSelectionStart();
//        const int selEnd = getSelectionEnd();
//        
//        min = max(0, min(selStart, selEnd));
//        max = max(0, max(selStart, selEnd));
//    }
//    
//    switch (id) {
//        case ID_SELECT_ALL:
//            // This does not enter text selection mode. Text is highlighted, so that it can be
//            // bulk edited, like selectAllOnFocus does. Returns true even if text is empty.
//            selectAllText();
//            return true;
//            
//        case ID_PASTE:
//            paste(min, max);
//            return true;
//            
//        case ID_CUT:
//            setPrimaryClip(ClipData.newPlainText(NULL, getTransformedText(min, max)));
//            deleteText_internal(min, max);
//            stopSelectionActionMode();
//            return true;
//            
//        case ID_COPY:
//            setPrimaryClip(ClipData.newPlainText(NULL, getTransformedText(min, max)));
//            stopSelectionActionMode();
//            return true;
//    }
//    return false;
//}

shared_ptr<CharSequence> TextView::getTransformedText(int start, int end) {
    return removeSuggestionSpans(mTransformed->subSequence(start, end));
}

bool TextView::performLongClick() {
    bool handled = false;
    
    if (View::performLongClick()) {
        handled = true;
    }
    
//    if (mEditor != NULL) {
//        handled |= mEditor.performLongClick(handled);
//    }
    
    if (handled) {
//        performHapticFeedback(HapticFeedbackConstants.LONG_PRESS);
//        if (mEditor != NULL) mEditor.mDiscardNextActionUp = true;
    }
    
    return handled;
}

bool TextView::isSuggestionsEnabled() {
//    if (mEditor == NULL) return false;
//    if ((mEditor.mInputType & InputType::TYPE_MASK_CLASS) != InputType::TYPE_CLASS_TEXT) {
//        return false;
//    }
//    if ((mEditor.mInputType & InputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS) > 0) return false;
//    
//    const int variation = mEditor.mInputType & EditorInfo::TYPE_MASK_VARIATION;
//    return (variation == EditorInfo::TYPE_TEXT_VARIATION_NORMAL ||
//            variation == EditorInfo::TYPE_TEXT_VARIATION_EMAIL_SUBJECT ||
//            variation == EditorInfo::TYPE_TEXT_VARIATION_LONG_MESSAGE ||
//            variation == EditorInfo::TYPE_TEXT_VARIATION_SHORT_MESSAGE ||
//            variation == EditorInfo::TYPE_TEXT_VARIATION_WEB_EDIT_TEXT);
    return false;
}

//void setCustomSelectionActionModeCallback(ActionMode.Callback actionModeCallback) {
//    createEditorIfNeeded();
//    mEditor.mCustomSelectionActionModeCallback = actionModeCallback;
//}
//
//ActionMode.Callback getCustomSelectionActionModeCallback() {
//    return mEditor == NULL ? NULL : mEditor.mCustomSelectionActionModeCallback;
//}
//
//
//
//void stopSelectionActionMode() {
//    mEditor.stopSelectionActionMode();
//}

bool TextView::canCut() {
//    if (hasPasswordTransformationMethod()) {
//        return false;
//    }
//    
//    if (mText->length() > 0 && hasSelection() && mText instanceof Editable && mEditor != NULL &&
//        mEditor.mKeyListener != NULL) {
//        return true;
//    }
    
    return false;
}

bool TextView::canCopy() {
//    if (hasPasswordTransformationMethod()) {
//        return false;
//    }
//    
//    if (mText->length() > 0 && hasSelection()) {
//        return true;
//    }
    
    return false;
}

bool TextView::canPaste() {
    return false; /*(mText instanceof Editable &&
            mEditor != NULL && mEditor.mKeyListener != NULL &&
            getSelectionStart() >= 0 &&
            getSelectionEnd() >= 0 &&
            ((ClipboardManager)getContext()->getSystemService(Context->CLIPBOARD_SERVICE)).
            hasPrimaryClip()); */
}

bool TextView::selectAllText() {
    const int length = mText->length();
    Selection::setSelection(mTextSpannable, 0, length);
    return length > 0;
}

long TextView::prepareSpacesAroundPaste(int min, int max, shared_ptr<CharSequence> paste) {
//    if (paste->length() > 0) {
//        if (min > 0) {
//            const char charBefore = mTransformed.charAt(min - 1);
//            const char charAfter = paste->charAt(0);
//            
//            if (Character::isSpaceChar(charBefore) && Character::isSpaceChar(charAfter)) {
//                // Two spaces at beginning of paste: remove one
//                const int originalLength = mText->length();
//                deleteText_internal(min - 1, min);
//                // Due to filters, there is no guarantee that exactly one character was
//                // removed: count instead.
//                const int delta = mText->length() - originalLength;
//                min += delta;
//                max += delta;
//            } else if (!Character::isSpaceChar(charBefore) && charBefore != '\n' &&
//                       !Character::isSpaceChar(charAfter) && charAfter != '\n') {
//                // No space at beginning of paste: add one
//                const int originalLength = mText->length();
//                replaceText_internal(min, min, " ");
//                // Taking possible filters into account as above.
//                const int delta = mText->length() - originalLength;
//                min += delta;
//                max += delta;
//            }
//        }
//        
//        if (max < mText->length()) {
//            const char charBefore = paste->charAt(paste->length() - 1);
//            const char charAfter = mTransformed.charAt(max);
//            
//            if (Character::isSpaceChar(charBefore) && Character::isSpaceChar(charAfter)) {
//                // Two spaces at end of paste: remove one
//                deleteText_internal(max, max + 1);
//            } else if (!Character::isSpaceChar(charBefore) && charBefore != '\n' &&
//                       !Character::isSpaceChar(charAfter) && charAfter != '\n') {
//                // No space at end of paste: add one
//                replaceText_internal(max, max, " ");
//            }
//        }
//    }
    
    return TextUtils::packRangeInLong(min, max);
}

void TextView::paste(int min, int max) {
//    ClipboardManager clipboard =
//    (ClipboardManager) getContext()->getSystemService(Context->CLIPBOARD_SERVICE);
//    ClipData clip = clipboard.getPrimaryClip();
//    if (clip != NULL) {
//        bool didFirst = false;
//        for (int i=0; i<clip.getItemCount(); i++) {
//            shared_ptr<CharSequence> paste = clip.getItemAt(i).coerceToStyledText(getContext());
//            if (paste != NULL) {
//                if (!didFirst) {
//                    long minMax = prepareSpacesAroundPaste(min, max, paste);
//                    min = TextUtils::unpackRangeStartFromLong(minMax);
//                    max = TextUtils::unpackRangeEndFromLong(minMax);
//                    Selection::setSelection(mTextSpannable, max);
//                    ((Editable) mText).replace(min, max, paste);
//                    didFirst = true;
//                } else {
//                    ((Editable) mText).insert(getSelectionEnd(), "\n");
//                    ((Editable) mText).insert(getSelectionEnd(), paste);
//                }
//            }
//        }
//        stopSelectionActionMode();
//        LAST_CUT_OR_COPY_TIME = 0;
//    }
}

//void setPrimaryClip(ClipData clip) {
//    ClipboardManager clipboard = (ClipboardManager) getContext()->
//    getSystemService(Context->CLIPBOARD_SERVICE);
//    clipboard.setPrimaryClip(clip);
//    LAST_CUT_OR_COPY_TIME = SystemClock.uptimeMillis();
//}

int TextView::getOffsetForPosition(float x, float y) {
    if (getLayout() == NULL) return -1;
    const int line = getLineAtCoordinate(y);
    const int offset = getOffsetAtCoordinate(line, x);
    return offset;
}

float TextView::convertToLocalHorizontalCoordinate(float x) {
    x -= getTotalPaddingLeft();
    // Clamp the position to inside of the view.
    x = max(0.0f, x);
    x = min((float)(getWidth() - getTotalPaddingRight() - 1), x);
    x += getScrollX();
    return x;
}

int TextView::getLineAtCoordinate(float y) {
    y -= getTotalPaddingTop();
    // Clamp the position to inside of the view.
    y = max(0.0f, y);
    y = min((float)(getHeight() - getTotalPaddingBottom() - 1), y);
    y += getScrollY();
    return getLayout()->getLineForVertical((int) y);
}

int TextView::getOffsetAtCoordinate(int line, float x) {
    x = convertToLocalHorizontalCoordinate(x);
    return getLayout()->getOffsetForHorizontal(line, x);
}

//bool TextView::isInBatchEditMode() {
//    if (mEditor == NULL) return false;
//    const Editor.InputMethodState ims = mEditor.mInputMethodState;
//    if (ims != NULL) {
//        return ims.mBatchEditNesting > 0;
//    }
//    return mEditor.mInBatchEditControllers;
//}

shared_ptr<TextDirectionHeuristic> TextView::getTextDirectionHeuristic() {
    if (hasPasswordTransformationMethod()) {
        // passwords fields should be LTR
        return TextDirectionHeuristics::LTR;
    }
    
    // Always need to resolve layout direction first
    const bool defaultIsRtl = (getLayoutDirection() == LAYOUT_DIRECTION_RTL);
    
    // Now, we can select the heuristic
    switch (getTextDirection()) {
        default:
        case TEXT_DIRECTION_FIRST_STRONG:
            return (defaultIsRtl ? TextDirectionHeuristics::FIRSTSTRONG_RTL :
                    TextDirectionHeuristics::FIRSTSTRONG_LTR);
        case TEXT_DIRECTION_ANY_RTL:
            return TextDirectionHeuristics::ANYRTL_LTR;
        case TEXT_DIRECTION_LTR:
            return TextDirectionHeuristics::LTR;
        case TEXT_DIRECTION_RTL:
            return TextDirectionHeuristics::RTL;
        case TEXT_DIRECTION_LOCALE:
            return TextDirectionHeuristics::LOCALE;
            
    }
}

void TextView::resetResolvedDrawables() {
    View::resetResolvedDrawables();
    mLastLayoutDirection = -1;
}

//void viewClicked(InputMethodManager imm) {
//    if (imm != NULL) {
//        imm.viewClicked(this);
//    }
//}

void TextView::deleteText_internal(int start, int end) {
//    ((Editable) mText).delete(start, end);
}

void TextView::replaceText_internal(int start, int end, shared_ptr<CharSequence> text) {
//    ((Editable) mText).replace(start, end, text);
}

void TextView::setSpan_internal(Object span, int start, int end, int flags) {
//    ((Editable) mText).setSpan(span, start, end, flags);
}

void TextView::setCursorPosition_internal(int start, int end) {
//    Selection::setSelection(((Editable) mText), start, end);
}

ANDROID_END
/*
 * File:   FrameLayout.h
 * Author: saulhoward
 *
 * Created on October 17, 2013, 11:19 PM
 */

#ifndef FRAMELAYOUT_H
#define	FRAMELAYOUT_H

#include "AndroidMacros.h"

#include "Android/view/Gravity.h"
#include "Android/view/ViewGroup.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class FrameLayout : public ViewGroup {
private:
    bool m_foregroundBoundsChanged = false;
    int m_foregroundGravity = Gravity::FILL;
    int m_foregroundPaddingLeft = 0;
    int m_foregroundPaddingTop = 0;
    int m_foregroundPaddingRight = 0;
    int m_foregroundPaddingBottom = 0;
    std::vector<shared_ptr<View>> m_matchParentChildren;
    bool m_measureAllChildren = false;

    int getPaddingLeftWithForeground();
    int getPaddingRightWithForeground();
    int getPaddingTopWithForeground();
    int getPaddingBottomWithForeground();

    static const int DEFAULT_CHILD_GRAVITY = Gravity::TOP | Gravity::START;

protected:
    bool m_foregroundInPadding = true;

    virtual bool checkLayoutParams(LayoutParams *params);
    virtual LayoutParams *generateDefaultLayoutParams();
    virtual LayoutParams *generateLayoutParams(LayoutParams *p);
    virtual void onLayout(bool changed, int left, int top, int right, int bottom);
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    virtual void onSizeChanged(int w, int h, int oldw, int oldh);
public:
    FrameLayout(Context *context);
    FrameLayout(Context *context, AttributeSet *attrs);
    virtual ~FrameLayout();

    virtual LayoutParams *generateLayoutParams(AttributeSet *attrs);
    int getForegroundGravity() { return m_foregroundGravity; }
    void setForegroundGravity(int foregroundGravity);
    bool getMeasureAllChildren();
    void setMeasureAllChildren(bool measureAll);
};

ANDROID_END

#endif	/* FRAMELAYOUT_H */


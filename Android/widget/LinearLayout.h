/*
 * File:   LinearLayout.h
 * Author: saulhoward
 *
 * Created on October 9, 2013, 11:40 AM
 */

#ifndef LINEARLAYOUT_H
#define	LINEARLAYOUT_H

#include "AndroidMacros.h"

#include "Android/view/Gravity.h"
#include "Android/view/ViewGroup.h"

ANDROID_BEGIN

class LinearLayout : public ViewGroup {
public:

	const static int HORIZONTAL = 0;
	const static int VERTICAL = 1;

	LinearLayout(Context *context);
    LinearLayout(Context *context, AttributeSet *attrs);
    virtual ~LinearLayout();
    
    virtual bool canResolveTextAlignment() { return View::canResolveTextAlignment(); }
    virtual LayoutParams *generateLayoutParams(AttributeSet *attrs);
    void setOrientation(int orientation);
    virtual bool shouldDelayChildPressedState() { return false; }
    
private:

    static const int INDEX_CENTER_VERTICAL = 0;
    static const int INDEX_TOP = 1;
    static const int INDEX_BOTTOM = 2;
    static const int INDEX_FILL = 3;
    static const int VERTICAL_GRAVITY_COUNT = 4;

	bool m_baselineAligned = true;
    int m_baselineAlignedChildIndex = -1;
    int m_baselineChildTop = 0;
    int m_gravity = Gravity::START | Gravity::TOP;
    int *m_maxAscent = NULL;
    int *m_maxDescent = NULL;
	int m_orientation = 0;
	int m_totalLength = 0;
	bool m_useLargestChild = false;
	float m_weightSum = -1;

    void forceUniformHeight(int count, int widthMeasureSpec);
    void forceUniformWidth(int count, int heightMeasureSpec);
	virtual int getChildrenSkipCount(shared_ptr<View> child, int index) { return 0; }
    virtual int getLocationOffset(shared_ptr<View> child) { return 0; }
    virtual int getNextLocationOffset(shared_ptr<View> child) { return 0; }
	virtual int getVirtualChildCount();
	virtual shared_ptr<View> getVirtualChildAt(int index);
    void layoutHorizontal(int left, int top, int right, int bottom);
    void layoutVertical(int left, int top, int right, int bottom);
    void measureChildBeforeLayout(shared_ptr<View> child, int childIndex,
                            int widthMeasureSpec, int totalWidth, int heightMeasureSpec,
                            int totalHeight);
	void measureHorizontal(int widthMeasureSpec, int heightMeasureSpec);
	virtual int measureNullChild(int childIndex) { return 0; }
	void measureVertical(int widthMeasureSpec, int heightMeasureSpec);
    void setChildFrame(shared_ptr<View> child, int left, int top, int width, int height);

protected:
    virtual bool checkLayoutParams(LayoutParams *params);
    virtual LayoutParams *generateDefaultLayoutParams();
    virtual LayoutParams *generateLayoutParams(LayoutParams *params);
    void measureChildWithMargins(shared_ptr<View> child,
                            int parentWidthMeasureSpec, int widthUsed,
                            int parentHeightMeasureSpec, int heightUsed);
    virtual void onLayout(bool changed, int left, int top, int right, int bottom);
	virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
};

ANDROID_END

#endif	/* LINEARLAYOUT_H */


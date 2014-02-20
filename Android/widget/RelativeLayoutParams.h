//
//  RelativeLayoutParams.h
//  cocos2dx
//
//  Created by Saul Howard on 11/8/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_RelativeLayoutParams_h
#define cocos2dx_RelativeLayoutParams_h

#include "AndroidMacros.h"

#include "Android/view/MarginLayoutParams.h"

using namespace std;

ANDROID_BEGIN

class RelativeLayoutParams : public MarginLayoutParams {
    
public:
    
    static const int sTRUE = -1;
    
    /**
     * Rule that aligns a child's right edge with another child's left edge.
     */
    static const int LEFT_OF                  = 0;
    /**
     * Rule that aligns a child's left edge with another child's right edge.
     */
    static const int RIGHT_OF                 = 1;
    /**
     * Rule that aligns a child's bottom edge with another child's top edge.
     */
    static const int ABOVE                    = 2;
    /**
     * Rule that aligns a child's top edge with another child's bottom edge.
     */
    static const int BELOW                    = 3;
    
    /**
     * Rule that aligns a child's baseline with another child's baseline.
     */
    static const int ALIGN_BASELINE           = 4;
    /**
     * Rule that aligns a child's left edge with another child's left edge.
     */
    static const int ALIGN_LEFT               = 5;
    /**
     * Rule that aligns a child's top edge with another child's top edge.
     */
    static const int ALIGN_TOP                = 6;
    /**
     * Rule that aligns a child's right edge with another child's right edge.
     */
    static const int ALIGN_RIGHT              = 7;
    /**
     * Rule that aligns a child's bottom edge with another child's bottom edge.
     */
    static const int ALIGN_BOTTOM             = 8;
    
    /**
     * Rule that aligns the child's left edge with its RelativeLayout
     * parent's left edge.
     */
    static const int ALIGN_PARENT_LEFT        = 9;
    /**
     * Rule that aligns the child's top edge with its RelativeLayout
     * parent's top edge.
     */
    static const int ALIGN_PARENT_TOP         = 10;
    /**
     * Rule that aligns the child's right edge with its RelativeLayout
     * parent's right edge.
     */
    static const int ALIGN_PARENT_RIGHT       = 11;
    /**
     * Rule that aligns the child's bottom edge with its RelativeLayout
     * parent's bottom edge.
     */
    static const int ALIGN_PARENT_BOTTOM      = 12;
    
    /**
     * Rule that centers the child with respect to the bounds of its
     * RelativeLayout parent.
     */
    static const int CENTER_IN_PARENT         = 13;
    /**
     * Rule that centers the child horizontally with respect to the
     * bounds of its RelativeLayout parent.
     */
    static const int CENTER_HORIZONTAL        = 14;
    /**
     * Rule that centers the child vertically with respect to the
     * bounds of its RelativeLayout parent.
     */
    static const int CENTER_VERTICAL          = 15;
    /**
     * Rule that aligns a child's end edge with another child's start edge.
     */
    static const int START_OF                 = 16;
    /**
     * Rule that aligns a child's start edge with another child's end edge.
     */
    static const int END_OF                   = 17;
    /**
     * Rule that aligns a child's start edge with another child's start edge.
     */
    static const int ALIGN_START              = 18;
    /**
     * Rule that aligns a child's end edge with another child's end edge.
     */
    static const int ALIGN_END                = 19;
    /**
     * Rule that aligns the child's start edge with its RelativeLayout
     * parent's start edge.
     */
    static const int ALIGN_PARENT_START       = 20;
    /**
     * Rule that aligns the child's end edge with its RelativeLayout
     * parent's end edge.
     */
    static const int ALIGN_PARENT_END         = 21;
    
    static const int VERB_COUNT               = 22;
    
    RelativeLayoutParams(Context *context, AttributeSet *attrs) : MarginLayoutParams(context, attrs),
        m_rules(VERB_COUNT),
        m_initialRules(VERB_COUNT) {
        
        vector<int64_t> rules = m_rules;
        
        attrs->loadAttributes();
        
        for (int i = 0; i < attrs->getAttributeCount(); i++) {
            
            const char *name = attrs->getAttributeName(i);
            
            if (strcmp(R::styleable::layout_alignWithParentIfMissing, name) == 0) {
                alignWithParent = attrs->getAttributeBooleanValue(name, false);
            } else if (strcmp(R::styleable::layout_toLeftOf, name) == 0) {
                rules[LEFT_OF] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_toRightOf, name) == 0) {
                rules[RIGHT_OF] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_above, name) == 0) {
                rules[ABOVE] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_below, name) == 0) {
                rules[BELOW] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignBaseline, name) == 0) {
                rules[ALIGN_BASELINE] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignLeft, name) == 0) {
                rules[ALIGN_LEFT] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignTop, name) == 0) {
                rules[ALIGN_TOP] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignRight, name) == 0) {
                rules[ALIGN_RIGHT] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignBottom, name) == 0) {
                rules[ALIGN_BOTTOM] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignParentLeft, name) == 0) {
                rules[ALIGN_PARENT_LEFT] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_alignParentTop, name) == 0) {
                rules[ALIGN_PARENT_TOP] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_alignParentRight, name) == 0) {
                rules[ALIGN_PARENT_RIGHT] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_alignParentBottom, name) == 0) {
                rules[ALIGN_PARENT_BOTTOM] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_centerInParent, name) == 0) {
                rules[CENTER_IN_PARENT] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_centerHorizontal, name) == 0) {
                rules[CENTER_HORIZONTAL] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_centerVertical, name) == 0) {
                rules[CENTER_VERTICAL] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_toStartOf, name) == 0) {
                rules[START_OF] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_toEndOf, name) == 0) {
                rules[END_OF] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignStart, name) == 0) {
                rules[ALIGN_START] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignEnd, name) == 0) {
                rules[ALIGN_END] = hash<string>()(attrs->getAttributeId(name));
            } else if (strcmp(R::styleable::layout_alignParentStart, name) == 0) {
                rules[ALIGN_PARENT_START] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            } else if (strcmp(R::styleable::layout_alignParentEnd, name) == 0) {
                rules[ALIGN_PARENT_END] = attrs->getAttributeBooleanValue(name, false) ? sTRUE : 0;
            }
        }
        
        m_rulesChanged = true;
        m_rules = m_initialRules = rules;
    }
    
    RelativeLayoutParams(int width, int height) : MarginLayoutParams(width, height) {
    }
    
    RelativeLayoutParams(LayoutParams *p) : MarginLayoutParams(p) {
    }
    
    RelativeLayoutParams(MarginLayoutParams *source) : MarginLayoutParams(source) {
    }
    
    virtual ~RelativeLayoutParams() {}
    
    bool alignWithParent = false;
    
    vector<int64_t> m_rules;
    vector<int64_t> m_initialRules;
    
    int mLeft = 0, mTop = 0, mRight = 0, mBottom = 0;
    
    int m_start = DEFAULT_MARGIN_RELATIVE;
    int m_end = DEFAULT_MARGIN_RELATIVE;
    
    bool m_rulesChanged = false;
    bool m_isRtlCompatibilityMode = false;
    
    vector<int64_t> getRules(int layoutDirection) {
        if (hasRelativeRules() &&
            (m_rulesChanged || layoutDirection != getLayoutDirection())) {
            resolveRules(layoutDirection);
            if (layoutDirection != getLayoutDirection()) {
                setLayoutDirection(layoutDirection);
            }
        }
        return m_rules;
    }
    
    vector<int64_t> getRules() {
        return m_rules;
    }
    
private:
    
    bool hasRelativeRules() {
        return (m_initialRules[START_OF] != 0 || m_initialRules[END_OF] != 0 ||
                m_initialRules[ALIGN_START] != 0 || m_initialRules[ALIGN_END] != 0 ||
                m_initialRules[ALIGN_PARENT_START] != 0 || m_initialRules[ALIGN_PARENT_END] != 0);
    }
    
    void resolveRules(int layoutDirection) {
        const bool isLayoutRtl = (layoutDirection == View::LAYOUT_DIRECTION_RTL);
        
        // Reset to initial state
        m_rules = m_initialRules;
        
        // Apply rules depending on direction and if we are in RTL compatibility mode
        if (m_isRtlCompatibilityMode) {
            if (m_rules[ALIGN_START] != 0) {
                if (m_rules[ALIGN_LEFT] == 0) {
                    // "left" rule is not defined but "start" rule is: use the "start" rule as
                    // the "left" rule
                    m_rules[ALIGN_LEFT] = m_rules[ALIGN_START];
                }
                m_rules[ALIGN_START] = 0;
            }
            
            if (m_rules[ALIGN_END] != 0) {
                if (m_rules[ALIGN_RIGHT] == 0) {
                    // "right" rule is not defined but "end" rule is: use the "end" rule as the
                    // "right" rule
                    m_rules[ALIGN_RIGHT] = m_rules[ALIGN_END];
                }
                m_rules[ALIGN_END] = 0;
            }
            
            if (m_rules[START_OF] != 0) {
                if (m_rules[LEFT_OF] == 0) {
                    // "left" rule is not defined but "start" rule is: use the "start" rule as
                    // the "left" rule
                    m_rules[LEFT_OF] = m_rules[START_OF];
                }
                m_rules[START_OF] = 0;
            }
            
            if (m_rules[END_OF] != 0) {
                if (m_rules[RIGHT_OF] == 0) {
                    // "right" rule is not defined but "end" rule is: use the "end" rule as the
                    // "right" rule
                    m_rules[RIGHT_OF] = m_rules[END_OF];
                }
                m_rules[END_OF] = 0;
            }
            
            if (m_rules[ALIGN_PARENT_START] != 0) {
                if (m_rules[ALIGN_PARENT_LEFT] == 0) {
                    // "left" rule is not defined but "start" rule is: use the "start" rule as
                    // the "left" rule
                    m_rules[ALIGN_PARENT_LEFT] = m_rules[ALIGN_PARENT_START];
                }
                m_rules[ALIGN_PARENT_START] = 0;
            }
            
            if (m_rules[ALIGN_PARENT_RIGHT] == 0) {
                if (m_rules[ALIGN_PARENT_RIGHT] == 0) {
                    // "right" rule is not defined but "end" rule is: use the "end" rule as the
                    // "right" rule
                    m_rules[ALIGN_PARENT_RIGHT] = m_rules[ALIGN_PARENT_END];
                }
                m_rules[ALIGN_PARENT_END] = 0;
            }
        } else {
            // JB MR1+ case
            if ((m_rules[ALIGN_START] != 0 || m_rules[ALIGN_END] != 0) &&
                (m_rules[ALIGN_LEFT] != 0 || m_rules[ALIGN_RIGHT] != 0)) {
                // "start"/"end" rules take precedence over "left"/"right" rules
                m_rules[ALIGN_LEFT] = 0;
                m_rules[ALIGN_RIGHT] = 0;
            }
            if (m_rules[ALIGN_START] != 0) {
                // "start" rule resolved to "left" or "right" depending on the direction
                m_rules[isLayoutRtl ? ALIGN_RIGHT : ALIGN_LEFT] = m_rules[ALIGN_START];
                m_rules[ALIGN_START] = 0;
            }
            if (m_rules[ALIGN_END] != 0) {
                // "end" rule resolved to "left" or "right" depending on the direction
                m_rules[isLayoutRtl ? ALIGN_LEFT : ALIGN_RIGHT] = m_rules[ALIGN_END];
                m_rules[ALIGN_END] = 0;
            }
            
            if ((m_rules[START_OF] != 0 || m_rules[END_OF] != 0) &&
                (m_rules[LEFT_OF] != 0 || m_rules[RIGHT_OF] != 0)) {
                // "start"/"end" rules take precedence over "left"/"right" rules
                m_rules[LEFT_OF] = 0;
                m_rules[RIGHT_OF] = 0;
            }
            if (m_rules[START_OF] != 0) {
                // "start" rule resolved to "left" or "right" depending on the direction
                m_rules[isLayoutRtl ? RIGHT_OF : LEFT_OF] = m_rules[START_OF];
                m_rules[START_OF] = 0;
            }
            if (m_rules[END_OF] != 0) {
                // "end" rule resolved to "left" or "right" depending on the direction
                m_rules[isLayoutRtl ? LEFT_OF : RIGHT_OF] = m_rules[END_OF];
                m_rules[END_OF] = 0;
            }
            
            if ((m_rules[ALIGN_PARENT_START] != 0 || m_rules[ALIGN_PARENT_END] != 0) &&
                (m_rules[ALIGN_PARENT_LEFT] != 0 || m_rules[ALIGN_PARENT_RIGHT] != 0)) {
                // "start"/"end" rules take precedence over "left"/"right" rules
                m_rules[ALIGN_PARENT_LEFT] = 0;
                m_rules[ALIGN_PARENT_RIGHT] = 0;
            }
            if (m_rules[ALIGN_PARENT_START] != 0) {
                // "start" rule resolved to "left" or "right" depending on the direction
                m_rules[isLayoutRtl ? ALIGN_PARENT_RIGHT : ALIGN_PARENT_LEFT] = m_rules[ALIGN_PARENT_START];
                m_rules[ALIGN_PARENT_START] = 0;
            }
            if (m_rules[ALIGN_PARENT_END] != 0) {
                // "end" rule resolved to "left" or "right" depending on the direction
                m_rules[isLayoutRtl ? ALIGN_PARENT_LEFT : ALIGN_PARENT_RIGHT] = m_rules[ALIGN_PARENT_END];
                m_rules[ALIGN_PARENT_END] = 0;
            }
        }
        m_rulesChanged = false;
    }
};

ANDROID_END

#endif

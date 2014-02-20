/*
 * File:   R.h
 * Author: saulhoward
 *
 * Created on October 10, 2013, 10:20 AM
 */

#ifndef R_H
#define	R_H

#include "AndroidMacros.h"

#include <vector>

ANDROID_BEGIN

class R {

public:
    
    class attr {
    public:
        static const int state_window_focused   = 0x0101009D;
        static const int state_selected         = 0x010100A1;
        static const int state_focused          = 0x0101009C;
        static const int state_enabled          = 0x0101009E;
        static const int state_multiline        = 0x00000000;
        static const int state_pressed          = 0x010100A7;
        static const int state_activated        = 0x010102FE;
        static const int state_accelerated      = 0x0101031B;
        static const int state_hovered          = 0x01010367;
        static const int state_drag_can_accept  = 0x01010368;
        static const int state_drag_hovered     = 0x01010369;
        static const int state_checked          = 0x01010370;
    };

    class layout {

    public:

        static const char* match_parent;
        static const char* wrap_content;
        static const char* layout_width;
        static const char* layout_height;
    };
    
    class string {
        
    public:
        
        static const char16_t ellipsis;
        static const char16_t ellipsis_two_dots;
    };

    class styleable {

    public:

        static std::vector<int> ViewDrawableStates;
        
        static const char* background;
        static const char* gravity;
        static const char* horizontal;
        static const char* id;
        static const char* layout_gravity;
        static const char* layout_margin;
        static const char* layout_marginBottom;
        static const char* layout_marginEnd;
        static const char* layout_marginLeft;
        static const char* layout_marginRight;
        static const char* layout_marginStart;
        static const char* layout_marginTop;
        static const char* layout_weight;
        static const char* orientation;
        static const char* padding;
        static const char* paddingBottom;
        static const char* paddingLeft;
        static const char* paddingRight;
        static const char* paddingTop;
        static const char* vertical;
        static const char* visibility;
        static const char* weightSum;
        static const char* textAlignment;
        
        // RelativeLayout
        
        static const char* layout_alignWithParentIfMissing;
        static const char* layout_toLeftOf;
        static const char* layout_toRightOf;
        static const char* layout_above;
        static const char* layout_below;
        static const char* layout_alignBaseline;
        static const char* layout_alignLeft;
        static const char* layout_alignTop;
        static const char* layout_alignRight;
        static const char* layout_alignBottom;
        static const char* layout_alignParentLeft;
        static const char* layout_alignParentTop;
        static const char* layout_alignParentRight;
        static const char* layout_alignParentBottom;
        static const char* layout_centerInParent;
        static const char* layout_centerHorizontal;
        static const char* layout_centerVertical;
        static const char* layout_toStartOf;
        static const char* layout_toEndOf;
        static const char* layout_alignStart;
        static const char* layout_alignEnd;
        static const char* layout_alignParentStart;
        static const char* layout_alignParentEnd;
        
        // Drawable
        
        static const char* visible;
        static const char* shape;
        static const char* innerRadius;
        static const char* innerRadiusRatio;
        static const char* dither;
        static const char* thickness;
        static const char* thicknessRatio;
        static const char* useLevel;
        static const char* width;
        static const char* height;
        static const char* startColor;
        static const char* centerColor;
        static const char* endColor;
        static const char* type;
        static const char* centerX;
        static const char* centerY;
        static const char* angle;
        static const char* gradientRadius;
        static const char* color;
        static const char* dashWidth;
        static const char* dashGap;
        static const char* radius;
        static const char* topLeftRadius;
        static const char* topRightRadius;
        static const char* bottomLeftRadius;
        static const char* bottomRightRadius;
        static const char* left;
        static const char* top;
        static const char* right;
        static const char* bottom;
        static const char* src;
        static const char* filter;
        static const char* antialias;
        static const char* tileMode;
        static const char* variablePadding;
        static const char* constantSize;
        static const char* enterFadeDuration;
        static const char* exitFadeDuration;
        static const char* opacity;
        static const char* drawable;
        
        // ImageView
        
        static const char* baselineAlignBottom;
        static const char* baseline;
        static const char* adjustViewBounds;
        static const char* maxWidth;
        static const char* maxHeight;
        static const char* scaleType;
        static const char* tint;
        static const char* drawableAlpha;
        static const char* cropToPadding;
        
        // ListView
        
        static const char* entries;
        static const char* divider;
        static const char* overScrollHeader;
        static const char* overScrollFooter;
        static const char* dividerHeight;
        static const char* headerDividersEnabled;
        static const char* footerDividersEnabled;
        
        // TextView
        
        static const char* textColorHighlight;
        static const char* textColor;
        static const char* textColorHint;
        static const char* textColorLink;
        static const char* textSize;
        static const char* typeface;
        static const char* fontFamily;
        static const char* textStyle;
        static const char* textAllCaps;
        static const char* shadowColor;
        static const char* shadowDx;
        static const char* shadowDy;
        static const char* shadowRadius;
        static const char* text;
        static const char* hint;
        static const char* maxLines;
        static const char* lines;
        static const char* singleLine;
    };
};

ANDROID_END

#endif	/* R_H */


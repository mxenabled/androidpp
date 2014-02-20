//
//  R.c
//  cocos2dx
//
//  Created by Saul Howard on 11/14/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "R.h"

ANDROID_BEGIN

std::vector<int> R::styleable::ViewDrawableStates = std::vector<int>();

const char* R::layout::layout_width = "layout_width";
const char* R::layout::layout_height = "layout_height";
const char* R::layout::match_parent = "match_parent";
const char* R::layout::wrap_content = "wrap_content";

const char16_t R::string::ellipsis = 0x2026;
const char16_t R::string::ellipsis_two_dots = 0x2026;

const char* R::styleable::background = "background";
const char* R::styleable::gravity = "gravity";
const char* R::styleable::horizontal = "horizontal";
const char* R::styleable::id = "id";
const char* R::styleable::layout_gravity = "layout_gravity";
const char* R::styleable::layout_margin = "layout_margin";
const char* R::styleable::layout_marginBottom = "layout_marginBottom";
const char* R::styleable::layout_marginEnd = "layout_marginEnd";
const char* R::styleable::layout_marginLeft = "layout_marginLeft";
const char* R::styleable::layout_marginRight = "layout_marginRight";
const char* R::styleable::layout_marginStart = "layout_marginStart";
const char* R::styleable::layout_marginTop = "layout_marginTop";
const char* R::styleable::layout_weight = "layout_weight";
const char* R::styleable::orientation = "orientation";
const char* R::styleable::padding = "padding";
const char* R::styleable::paddingBottom = "paddingBottom";
const char* R::styleable::paddingLeft = "paddingLeft";
const char* R::styleable::paddingRight = "paddingRight";
const char* R::styleable::paddingTop = "paddingTop";
const char* R::styleable::vertical = "vertical";
const char* R::styleable::visibility = "visibility";
const char* R::styleable::weightSum = "weightSum";
const char* R::styleable::textAlignment = "textAlignment";

// RelativeLayout

const char* R::styleable::layout_alignWithParentIfMissing = "layout_alignWithParentIfMissing";
const char* R::styleable::layout_toLeftOf = "layout_toLeftOf";
const char* R::styleable::layout_toRightOf = "layout_toRightOf";
const char* R::styleable::layout_above = "layout_above";
const char* R::styleable::layout_below = "layout_below";
const char* R::styleable::layout_alignBaseline = "layout_alignBaseline";
const char* R::styleable::layout_alignLeft = "layout_alignLeft";
const char* R::styleable::layout_alignTop = "layout_alignTop";
const char* R::styleable::layout_alignRight = "layout_alignRight";
const char* R::styleable::layout_alignBottom = "layout_alignBottom";
const char* R::styleable::layout_alignParentLeft = "layout_alignParentLeft";
const char* R::styleable::layout_alignParentTop = "layout_alignParentTop";
const char* R::styleable::layout_alignParentRight = "layout_alignParentRight";
const char* R::styleable::layout_alignParentBottom = "layout_alignParentBottom";
const char* R::styleable::layout_centerInParent = "layout_centerInParent";
const char* R::styleable::layout_centerHorizontal = "layout_centerHorizontal";
const char* R::styleable::layout_centerVertical = "layout_centerVertical";
const char* R::styleable::layout_toStartOf = "layout_toStartOf";
const char* R::styleable::layout_toEndOf = "layout_toEndOf";
const char* R::styleable::layout_alignStart = "layout_alignStart";
const char* R::styleable::layout_alignEnd = "layout_alignEnd";
const char* R::styleable::layout_alignParentStart = "layout_alignParentStart";
const char* R::styleable::layout_alignParentEnd = "layout_alignParentEnd";

// Drawable

const char* R::styleable::visible = "visible";
const char* R::styleable::shape = "shape";
const char* R::styleable::innerRadius = "innerRadius";
const char* R::styleable::innerRadiusRatio = "innerRadiusRatio";
const char* R::styleable::dither = "dither";
const char* R::styleable::thickness = "thickness";
const char* R::styleable::thicknessRatio = "thicknessRatio";
const char* R::styleable::useLevel = "useLevel";
const char* R::styleable::width = "width";
const char* R::styleable::height = "height";
const char* R::styleable::startColor = "startColor";
const char* R::styleable::centerColor = "centerColor";
const char* R::styleable::endColor = "endColor";
const char* R::styleable::type = "type";
const char* R::styleable::centerX = "centerX";
const char* R::styleable::centerY = "centerY";
const char* R::styleable::angle = "angle";
const char* R::styleable::gradientRadius = "gradientRadius";
const char* R::styleable::color = "color";
const char* R::styleable::dashWidth = "dashWidth";
const char* R::styleable::dashGap = "dashGap";
const char* R::styleable::radius = "radius";
const char* R::styleable::topLeftRadius = "topLeftRadius";
const char* R::styleable::topRightRadius = "topRightRadius";
const char* R::styleable::bottomLeftRadius = "bottomLeftRadius";
const char* R::styleable::bottomRightRadius = "bottomRightRadius";
const char* R::styleable::left = "left";
const char* R::styleable::top = "top";
const char* R::styleable::right = "right";
const char* R::styleable::bottom = "bottom";
const char* R::styleable::src = "src";
const char* R::styleable::filter = "filter";
const char* R::styleable::antialias = "antialias";
const char* R::styleable::tileMode = "tileMode";
const char* R::styleable::variablePadding = "variablePadding";
const char* R::styleable::constantSize = "constantSize";
const char* R::styleable::enterFadeDuration = "enterFadeDuration";
const char* R::styleable::exitFadeDuration = "exitFadeDuration";
const char* R::styleable::opacity = "opacity";
const char* R::styleable::drawable = "drawable";

// ImageView

const char* R::styleable::baselineAlignBottom = "baselineAlignBottom";
const char* R::styleable::baseline = "baseline";
const char* R::styleable::adjustViewBounds = "adjustViewBounds";
const char* R::styleable::maxWidth = "maxWidth";
const char* R::styleable::maxHeight = "maxHeight";
const char* R::styleable::scaleType = "scaleType";
const char* R::styleable::tint = "tint";
const char* R::styleable::drawableAlpha = "drawableAlpha";
const char* R::styleable::cropToPadding = "cropToPadding";

// ListView

const char* R::styleable::entries = "entries";
const char* R::styleable::divider = "divider";
const char* R::styleable::overScrollHeader = "overScrollHeader";
const char* R::styleable::overScrollFooter = "overScrollFooter";
const char* R::styleable::dividerHeight = "dividerHeight";
const char* R::styleable::headerDividersEnabled = "headerDividersEnabled";
const char* R::styleable::footerDividersEnabled = "footerDividersEnabled";

// TextView

const char* R::styleable::textColorHighlight = "textColorHighlight";
const char* R::styleable::textColor = "textColor";
const char* R::styleable::textColorHint = "textColorHint";
const char* R::styleable::textColorLink = "textColorLink";
const char* R::styleable::textSize = "textSize";
const char* R::styleable::typeface = "typeface";
const char* R::styleable::fontFamily = "fontFamily";
const char* R::styleable::textStyle = "textStyle";
const char* R::styleable::textAllCaps = "textAllCaps";
const char* R::styleable::shadowColor = "shadowColor";
const char* R::styleable::shadowDx = "shadowDx";
const char* R::styleable::shadowDy = "shadowDy";
const char* R::styleable::shadowRadius = "shadowRadius";
const char* R::styleable::text = "text";
const char* R::styleable::hint = "hint";
const char* R::styleable::maxLines = "maxLines";
const char* R::styleable::lines = "lines";
const char* R::styleable::singleLine = "singleLine";


ANDROID_END

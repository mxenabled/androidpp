//
//  Configuration.c
//  Androidpp
//
//  Created by Saul Howard on 2/4/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Configuration.h"

#include "Android/view/View.h"

ANDROID_BEGIN

int Configuration::getLayoutDirection() {
    return (screenLayout & SCREENLAYOUT_LAYOUTDIR_MASK) == SCREENLAYOUT_LAYOUTDIR_RTL
    ? View::LAYOUT_DIRECTION_RTL : View::LAYOUT_DIRECTION_LTR;
}

ANDROID_END
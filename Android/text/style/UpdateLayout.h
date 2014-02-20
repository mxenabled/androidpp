//
//  UpdateLayout.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_UpdateLayout_h
#define Androidpp_UpdateLayout_h

#include "AndroidMacros.h"

#include "Android/text/style/UpdateAppearance.h"

ANDROID_BEGIN

/**
 * The classes that affect character-level text in a way that modifies their
 * appearance when one is added or removed must implement this interface.  Note
 * that if the class also impacts size or other metrics, it should instead
 * implement {@link UpdateLayout}.
 */
class UpdateLayout : public UpdateAppearance {
};

ANDROID_END

#endif

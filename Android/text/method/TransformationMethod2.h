//
//  TransformationMethod2.h
//  Androidpp
//
//  Created by Saul Howard on 1/28/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_TransformationMethod2_h
#define Androidpp_TransformationMethod2_h

#include "AndroidMacros.h"

#include "Android/text/method/TransformationMethod.h"

ANDROID_BEGIN

/**
 * TransformationMethod2 extends the TransformationMethod interface
 * and adds the ability to relax restrictions of TransformationMethod.
 *
 * @hide
 */
class TransformationMethod2 : public TransformationMethod {
    
public:
    
    /**
     * Relax the contract of TransformationMethod to allow length changes,
     * or revert to the length-restricted behavior.
     *
     * @param allowLengthChanges true to allow the transformation to change the length
     *                           of the input string.
     */
    virtual void setLengthChangesAllowed(bool allowLengthChanges) = 0;
};

ANDROID_END

#endif

//
//  Transformation.h
//  cocos2dx
//
//  Created by Saul Howard on 11/25/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Transformation_h
#define cocos2dx_Transformation_h

#include "AndroidMacros.h"

#include "Android/graphics/Matrix.h"

ANDROID_BEGIN

class Transformation {
    
protected:
    
    Matrix mMatrix;
    float mAlpha;
    int mTransformationType;
    
public:
    /**
     * Indicates a transformation that has no effect (alpha = 1 and identity matrix.)
     */
    const static int TYPE_IDENTITY = 0x0;
    /**
     * Indicates a transformation that applies an alpha only (uses an identity matrix.)
     */
    const static int TYPE_ALPHA = 0x1;
    /**
     * Indicates a transformation that applies a matrix only (alpha = 1.)
     */
    const static int TYPE_MATRIX = 0x2;
    /**
     * Indicates a transformation that applies an alpha and a matrix.
     */
    const static int TYPE_BOTH = TYPE_ALPHA | TYPE_MATRIX;
    
    /**
     * Creates a new transformation with alpha = 1 and the identity matrix.
     */
    Transformation() {
        clear();
    }
    
    /**
     * Reset the transformation to a state that leaves the object
     * being animated in an unmodified state. The transformation type is
     * {@link #TYPE_BOTH} by default.
     */
    inline void clear() {
        mMatrix.reset();
        mAlpha = 1.0f;
        mTransformationType = TYPE_BOTH;
    }
    
    /**
     * Indicates the nature of this transformation.
     *
     * @return {@link #TYPE_ALPHA}, {@link #TYPE_MATRIX},
     *         {@link #TYPE_BOTH} or {@link #TYPE_IDENTITY}.
     */
    inline int getTransformationType() {
        return mTransformationType;
    }
    
    /**
     * Sets the transformation type.
     *
     * @param transformationType One of {@link #TYPE_ALPHA},
     *        {@link #TYPE_MATRIX}, {@link #TYPE_BOTH} or
     *        {@link #TYPE_IDENTITY}.
     */
    inline void setTransformationType(int transformationType) {
        mTransformationType = transformationType;
    }
    
    /**
     * Clones the specified transformation.
     *
     * @param t The transformation to clone.
     */
    inline void set(Transformation t) {
        mAlpha = t.getAlpha();
        mMatrix.reset();
        mMatrix = t.getMatrix();
        mTransformationType = t.getTransformationType();
    }
    
    /**
     * Apply this Transformation to an existing Transformation, e.g. apply
     * a scale effect to something that has already been rotated.
     * @param t
     */
    inline void compose(Transformation t) {
        mAlpha *= t.getAlpha();
        mMatrix.preConcat(t.getMatrix());
    }
    
    /**
     * Like {@link #compose(Transformation)} but does this.postConcat(t) of
     * the transformation matrix.
     * @hide
     */
    inline void postCompose(Transformation t) {
        mAlpha *= t.getAlpha();
        mMatrix.postConcat(t.getMatrix());
    }
    
    /**
     * @return The 3x3 Matrix representing the trnasformation to apply to the
     * coordinates of the object being animated
     */
    inline Matrix getMatrix() {
        return mMatrix;
    }
    
    /**
     * Sets the degree of transparency
     * @param alpha 1.0 means fully opaqe and 0.0 means fully transparent
     */
    inline void setAlpha(float alpha) {
        mAlpha = alpha;
    }
    
    /**
     * @return The degree of transparency
     */
    inline float getAlpha() {
        return mAlpha;
    }
};

ANDROID_END

#endif

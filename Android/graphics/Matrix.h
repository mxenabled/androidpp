//
//  Matrix.h
//  cocos2dx
//
//  Created by Saul Howard on 11/25/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Matrix_h
#define cocos2dx_Matrix_h

#include "AndroidMacros.h"
#include "Android/graphics/Rect.h"
#include "Android/utils/Exceptions.h"

#include <SkMatrix.h>

#include <vector>

using namespace std;

ANDROID_BEGIN

class Matrix : public SkMatrix {
public:
    
    void mapRect(RectF *rect) {
        SkMatrix::mapRect(rect);
    }
    
    void postConcat(Matrix matrix) {
        SkMatrix::postConcat((SkMatrix) matrix);
    }
    
    bool setRectToRect(const RectF &src, const RectF &dst, ScaleToFit stf) {
        return SkMatrix::setRectToRect(src, dst, stf);
    }
    
    /**
     * Apply this matrix to the array of 2D points specified by src, and write
     * the transformed points into the array of points specified by dst. The
     * two arrays represent their "points" as pairs of floats [x, y].
     *
     * @param dst   The array of dst points (x,y pairs)
     * @param dstIndex The index of the first [x,y] pair of dst floats
     * @param src   The array of src points (x,y pairs)
     * @param srcIndex The index of the first [x,y] pair of src floats
     * @param pointCount The number of points (x,y pairs) to transform
     */
    void mapPoints(vector<float> &dst, int dstIndex, vector<float> &src, int srcIndex,
                          int pointCount) {
        checkPointArrays(src, srcIndex, dst, dstIndex, pointCount);
        mapPointsInternal(dst, dstIndex, src, srcIndex,
                         pointCount, true);
    }
    
    /**
     * Apply this matrix to the array of 2D points, and write the transformed
     * points back into the array
     *
     * @param pts The array [x0, y0, x1, y1, ...] of points to transform.
     */
    void mapPoints(vector<float> &pts) {
        mapPoints(pts, 0, pts, 0, pts.size() >> 1);
    }
    
private:
    
    void mapPointsInternal(vector<float> &dst, int dstIndex,
                   vector<float> &src, int srcIndex,
                   int ptCount, bool isPts) {
        SkASSERT(ptCount >= 0);
        
#ifdef SK_SCALAR_IS_FIXED
        // we allocate twice the count, 1 set for src, 1 for dst
        SkAutoSTMalloc<32, SkPoint> storage(ptCount * 2);
        SkPoint* pts = storage.get();
        SkPoint* srcPt = pts;
        SkPoint* dstPt = pts + ptCount;
        
        int i;
        for (i = 0; i < ptCount; i++) {
            srcPt[i].set(SkFloatToScalar(src[i << 1]),
                         SkFloatToScalar(src[(i << 1) + 1]));
        }
        
        if (isPts)
            matrix->mapPoints(dstPt, srcPt, ptCount);
        else
            matrix->mapVectors(dstPt, srcPt, ptCount);
        
        for (i = 0; i < ptCount; i++) {
            dst[i << 1]  = SkScalarToFloat(dstPt[i].fX);
            dst[(i << 1) + 1]  = SkScalarToFloat(dstPt[i].fY);
        }
#else
        if (isPts)
            SkMatrix::mapPoints((SkPoint*) &dst[0], (const SkPoint*) &src[0], ptCount);
        else
            SkMatrix::mapVectors((SkVector*) &dst[0], (const SkVector*) &src[0],
                               ptCount);
#endif
    }
    
    // private helper to perform range checks on arrays of "points"
    static void checkPointArrays(vector<float> &src, int srcIndex,
                                         vector<float> &dst, int dstIndex,
                                         int pointCount) {
        // check for too-small and too-big indices
        int srcStop = srcIndex + (pointCount << 1);
        int dstStop = dstIndex + (pointCount << 1);
        if ((pointCount | srcIndex | dstIndex | srcStop | dstStop) < 0 ||
            srcStop > src.size() || dstStop > dst.size()) {
            throw ArrayIndexOutOfBoundsException();
        }
    }
    
};

ANDROID_END

#endif

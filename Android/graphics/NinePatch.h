//
//  NinePatch.h
//  cocos2dx
//
//  Created by Saul Howard on 12/3/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__NinePatch__
#define __cocos2dx__NinePatch__

#include "AndroidMacros.h"

#include <stdint.h>
#include <sys/types.h>

ANDROID_BEGIN

/** ********************************************************************
 *  PNG Extensions
 *
 *  New private chunks that may be placed in PNG images.
 *
 *********************************************************************** */

/**
 * This chunk specifies how to split an image into segments for
 * scaling.
 *
 * There are J horizontal and K vertical segments.  These segments divide
 * the image into J*K regions as follows (where J=4 and K=3):
 *
 *      F0   S0    F1     S1
 *   +-----+----+------+-------+
 * S2|  0  |  1 |  2   |   3   |
 *   +-----+----+------+-------+
 *   |     |    |      |       |
 *   |     |    |      |       |
 * F2|  4  |  5 |  6   |   7   |
 *   |     |    |      |       |
 *   |     |    |      |       |
 *   +-----+----+------+-------+
 * S3|  8  |  9 |  10  |   11  |
 *   +-----+----+------+-------+
 *
 * Each horizontal and vertical segment is considered to by either
 * stretchable (marked by the Sx labels) or fixed (marked by the Fy
 * labels), in the horizontal or vertical axis, respectively. In the
 * above example, the first is horizontal segment (F0) is fixed, the
 * next is stretchable and then they continue to alternate. Note that
 * the segment list for each axis can begin or end with a stretchable
 * or fixed segment.
 *
 * The relative sizes of the stretchy segments indicates the relative
 * amount of stretchiness of the regions bordered by the segments.  For
 * example, regions 3, 7 and 11 above will take up more horizontal space
 * than regions 1, 5 and 9 since the horizontal segment associated with
 * the first set of regions is larger than the other set of regions.  The
 * ratios of the amount of horizontal (or vertical) space taken by any
 * two stretchable slices is exactly the ratio of their corresponding
 * segment lengths.
 *
 * xDivs and yDivs point to arrays of horizontal and vertical pixel
 * indices.  The first pair of Divs (in either array) indicate the
 * starting and ending points of the first stretchable segment in that
 * axis. The next pair specifies the next stretchable segment, etc. So
 * in the above example xDiv[0] and xDiv[1] specify the horizontal
 * coordinates for the regions labeled 1, 5 and 9.  xDiv[2] and
 * xDiv[3] specify the coordinates for regions 3, 7 and 11. Note that
 * the leftmost slices always start at x=0 and the rightmost slices
 * always end at the end of the image. So, for example, the regions 0,
 * 4 and 8 (which are fixed along the X axis) start at x value 0 and
 * go to xDiv[0] and slices 2, 6 and 10 start at xDiv[1] and end at
 * xDiv[2].
 *
 * The array pointed to by the colors field lists contains hints for
 * each of the regions.  They are ordered according left-to-right and
 * top-to-bottom as indicated above. For each segment that is a solid
 * color the array entry will contain that color value; otherwise it
 * will contain NO_COLOR.  Segments that are completely transparent
 * will always have the value TRANSPARENT_COLOR.
 *
 * The PNG chunk type is "npTc".
 */
struct Res_png_9patch
{
    Res_png_9patch() : wasDeserialized(false), xDivs(nullptr),
    yDivs(nullptr), colors(nullptr) { }
    
    int8_t wasDeserialized;
    int8_t numXDivs;
    int8_t numYDivs;
    int8_t numColors;
    
    // These tell where the next section of a patch starts.
    // For example, the first patch includes the pixels from
    // 0 to xDivs[0]-1 and the second patch includes the pixels
    // from xDivs[0] to xDivs[1]-1.
    // Note: allocation/free of these pointers is left to the caller.
    int32_t* xDivs;
    int32_t* yDivs;
    
    int32_t paddingLeft, paddingRight;
    int32_t paddingTop, paddingBottom;
    
    enum {
        // The 9 patch segment is not a solid color.
        NO_COLOR = 0x00000001,
        
        // The 9 patch segment is completely transparent.
        TRANSPARENT_COLOR = 0x00000000
    };
    // Note: allocation/free of this pointer is left to the caller.
    uint32_t* colors;
    
    // Convert data from device representation to PNG file representation.
    void deviceToFile();
    // Convert data from PNG file representation to device representation.
    void fileToDevice();
    // Serialize/Marshall the patch data into a newly malloc-ed block
    void* serialize();
    // Serialize/Marshall the patch data
    void serialize(void* outData);
    // Deserialize/Unmarshall the patch data
    static Res_png_9patch* deserialize(const void* data);
    // Compute the size of the serialized data structure
    size_t serializedSize();
    
    static void deserializeInternal(const void* inData, Res_png_9patch* outData);
};

class NinePatch {
    
};

ANDROID_END

#endif /* defined(__cocos2dx__NinePatch__) */

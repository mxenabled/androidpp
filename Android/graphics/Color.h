/*
 * File:   Color.h
 * Author: saulhoward
 *
 * Created on October 22, 2013, 9:37 AM
 */

#ifndef COLOR_H
#define	COLOR_H

#include "AndroidMacros.h"

#include <string>
#include <map>

using namespace std;

ANDROID_BEGIN

class Color {
public:
    Color();
    virtual ~Color();

    static const int BLACK;
    static const int DKGRAY;
    static const int GRAY;
    static const int LTGRAY;
    static const int WHITE;
    static const int RED;
    static const int GREEN;
    static const int BLUE;
    static const int YELLOW;
    static const int CYAN;
    static const int MAGENTA;
    static const int TRANSPARENT;

    /**
     * Return the alpha component of a color int. This is the same as saying
     * color >>> 24
     */
    static int alpha(int color);

    /**
     * Return the red component of a color int. This is the same as saying
     * (color >> 16) & 0xFF
     */
    static int red(int color);

    /**
     * Return the green component of a color int. This is the same as saying
     * (color >> 8) & 0xFF
     */
    static int green(int color);

    /**
     * Return the blue component of a color int. This is the same as saying
     * color & 0xFF
     */
    static int blue(int color);

    /**
     * Return a color-int from red, green, blue components.
     * The alpha component is implicity 255 (fully opaque).
     * These component values should be [0..255], but there is no
     * range check performed, so if they are out of range, the
     * returned color is undefined.
     * @param red  Red component [0..255] of the color
     * @param green Green component [0..255] of the color
     * @param blue  Blue component [0..255] of the color
     */
    static int rgb(int red, int green, int blue);

    /**
     * Return a color-int from alpha, red, green, blue components.
     * These component values should be [0..255], but there is no
     * range check performed, so if they are out of range, the
     * returned color is undefined.
     * @param alpha Alpha component [0..255] of the color
     * @param red   Red component [0..255] of the color
     * @param green Green component [0..255] of the color
     * @param blue  Blue component [0..255] of the color
     */
    static int argb(int alpha, int red, int green, int blue);

    /**
     * Returns the hue component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    static float hue(int color) ;

    /**
     * Returns the saturation component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    static float saturation(int color);

    /**
     * Returns the brightness component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    static float brightness(int color);

    /**
     * Parse the color string, and return the corresponding color-int.
     * If the string cannot be parsed, throws an IllegalArgumentException
     * exception. Supported formats are:
     * #RRGGBB
     * #AARRGGBB
     * 'red', 'blue', 'green', 'black', 'white', 'gray', 'cyan', 'magenta',
     * 'yellow', 'lightgray', 'darkgray', 'grey', 'lightgrey', 'darkgrey',
     * 'aqua', 'fuschia', 'lime', 'maroon', 'navy', 'olive', 'purple',
     * 'silver', 'teal'
     */
    static int parseColor(string colorString);

    /**
     * Convert HSB components to an ARGB color. Alpha set to 0xFF.
     *     hsv[0] is Hue [0 .. 1)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * If hsv values are out of range, they are pinned.
     * @param hsb  3 element array which holds the input HSB components.
     * @return the resulting argb color
     *
     * @hide Pending API council
     */
    static int HSBtoColor(float hsb[]);

    /**
     * Convert HSB components to an ARGB color. Alpha set to 0xFF.
     *     hsv[0] is Hue [0 .. 1)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * If hsv values are out of range, they are pinned.
     * @param h Hue component
     * @param s Saturation component
     * @param b Brightness component
     * @return the resulting argb color
     *
     * @hide Pending API council
     */
    static int HSBtoColor(float h, float s, float b);
    
private:

    static const map<string, int> s_colorNameMap;

};

ANDROID_END

#endif	/* COLOR_H */


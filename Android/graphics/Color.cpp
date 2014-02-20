/*
 * File:   Color.cpp
 * Author: saulhoward
 *
 * Created on October 22, 2013, 9:37 AM
 */

#include "Color.h"

#include <algorithm>
#include <climits>
#include <stdlib.h>

#include "Android/text/Character.h"
#include "Android/utils/Exceptions.h"
#include "Android/utils/MathUtils.h"

ANDROID_BEGIN

Color::Color() {
}

Color::~Color() {
}

const int Color::BLACK       = 0xFF000000;
const int Color::DKGRAY      = 0xFF444444;
const int Color::GRAY        = 0xFF888888;
const int Color::LTGRAY      = 0xFFCCCCCC;
const int Color::WHITE       = 0xFFFFFFFF;
const int Color::RED         = 0xFFFF0000;
const int Color::GREEN       = 0xFF00FF00;
const int Color::BLUE        = 0xFF0000FF;
const int Color::YELLOW      = 0xFFFFFF00;
const int Color::CYAN        = 0xFF00FFFF;
const int Color::MAGENTA     = 0xFFFF00FF;
const int Color::TRANSPARENT = 0;

const map<string, int> Color::s_colorNameMap = {
    {"black", BLACK},
    {"darkgray", DKGRAY},
    {"gray", GRAY},
    {"lightgray", LTGRAY},
    {"white", WHITE},
    {"red", RED},
    {"green", GREEN},
    {"blue", BLUE},
    {"yellow", YELLOW},
    {"cyan", CYAN},
    {"magenta", MAGENTA},
    {"aqua", 0x00FFFF},
    {"fuchsia", 0xFF00FF},
    {"darkgrey", DKGRAY},
    {"grey", GRAY},
    {"lightgrey", LTGRAY},
    {"lime", 0x00FF00},
    {"maroon", 0x800000},
    {"navy", 0x000080},
    {"olive", 0x808000},
    {"purple", 0x800080},
    {"silver", 0xC0C0C0},
    {"teal", 0x008080}
};

int Color::alpha(int color) {
    return color >> 24;
}

/**
 * Return the red component of a color int. This is the same as saying
 * (color >> 16) & 0xFF
 */
int Color::red(int color) {
    return (color >> 16) & 0xFF;
}

/**
 * Return the green component of a color int. This is the same as saying
 * (color >> 8) & 0xFF
 */
int Color::green(int color) {
    return (color >> 8) & 0xFF;
}

/**
 * Return the blue component of a color int. This is the same as saying
 * color & 0xFF
 */
int Color::blue(int color) {
    return color & 0xFF;
}

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
int Color::rgb(int red, int green, int blue) {
    return (0xFF << 24) | (red << 16) | (green << 8) | blue;
}

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
int Color::argb(int alpha, int red, int green, int blue) {
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

/**
 * Returns the hue component of a color int.
 *
 * @return A value between 0.0f and 1.0f
 *
 * @hide Pending API council
 */
float Color::hue(int color) {
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;

    int V = max(b, max(r, g));
    int temp = min(b, min(r, g));

    float H;

    if (V == temp) {
        H = 0;
    } else {
        const float vtemp = (float) (V - temp);
        const float cr = (V - r) / vtemp;
        const float cg = (V - g) / vtemp;
        const float cb = (V - b) / vtemp;

        if (r == V) {
            H = cb - cg;
        } else if (g == V) {
            H = 2 + cr - cb;
        } else {
            H = 4 + cg - cr;
        }

        H /= 6.f;
        if (H < 0) {
            H++;
        }
    }

    return H;
}

/**
 * Returns the saturation component of a color int.
 *
 * @return A value between 0.0f and 1.0f
 *
 * @hide Pending API council
 */
float Color::saturation(int color) {
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;


    int V = max(b, max(r, g));
    int temp = min(b, min(r, g));

    float S;

    if (V == temp) {
        S = 0;
    } else {
        S = (V - temp) / (float) V;
    }

    return S;
}

/**
 * Returns the brightness component of a color int.
 *
 * @return A value between 0.0f and 1.0f
 *
 * @hide Pending API council
 */
float Color::brightness(int color) {
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;

    int V = max(b, max(r, g));

    return (V / 255.f);
}

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
int Color::parseColor(string colorString) {

    if (colorString[0] == '#') {

        // Use a long to avoid rollovers on #ffXXXXXX
        long int color = strtol(colorString.substr(1, colorString.size() - 1).c_str(), NULL, 16);

        if (colorString.size() == 7) {
            // Set the alpha value
            color |= 0x00000000ff000000;
        } else if (colorString.size() != 9) {
            return WHITE;
        }

        return (int) color;

    } else {

        string lower;
        lower.resize(colorString.size());
        transform(colorString.begin(), colorString.end(), lower.begin(), ::tolower);

        int color = s_colorNameMap.find(lower)->second;

        if (color) {
            return color;
        }
    }

    return WHITE;
}

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
int Color::HSBtoColor(float hsb[]) {
    return HSBtoColor(hsb[0], hsb[1], hsb[2]);
}

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
int Color::HSBtoColor(float h, float s, float b) {
    h = MathUtils::constrain(h, 0.0f, 1.0f);
    s = MathUtils::constrain(s, 0.0f, 1.0f);
    b = MathUtils::constrain(b, 0.0f, 1.0f);

    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    const float hf = (h - (int) h) * 6.0f;
    const int ihf = (int) hf;
    const float f = hf - ihf;
    const float pv = b * (1.0f - s);
    const float qv = b * (1.0f - s * f);
    const float tv = b * (1.0f - s * (1.0f - f));

    switch (ihf) {
        case 0:         // Red is the dominant color
            red = b;
            green = tv;
            blue = pv;
            break;
        case 1:         // Green is the dominant color
            red = qv;
            green = b;
            blue = pv;
            break;
        case 2:
            red = pv;
            green = b;
            blue = tv;
            break;
        case 3:         // Blue is the dominant color
            red = pv;
            green = qv;
            blue = b;
            break;
        case 4:
            red = tv;
            green = pv;
            blue = b;
            break;
        case 5:         // Red is the dominant color
            red = b;
            green = pv;
            blue = qv;
            break;
    }

    return 0xFF000000 | (((int) (red * 255.0f)) << 16) |
            (((int) (green * 255.0f)) << 8) | ((int) (blue * 255.0f));
}

ANDROID_END
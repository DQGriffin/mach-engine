//
// Created by Dquavius Griffin on 5/21/25.
//

#ifndef COLOR_H
#define COLOR_H
#include "../math/vec.h"

namespace mach
{
    class Color
    {
    public:
        float r, g, b;

    public:
        Color();

        Color(float r, float g, float b): r(r), g(g), b(b)
        {
            this->setRGB(r, g, b);
        }

        explicit Color(mach::vec3 rgb) : r(rgb.r), g(rgb.g), b(rgb.b)
        {
            this->setRGB(rgb.r, rgb.g, rgb.b);
        }

        void setRGB(float red, float green, float blue)
        {
            if (red > 1.0f)
                red = normalize(red);
            if (green > 1.0f)
                green = normalize(green);
            if (blue > 1.0f)
                blue = normalize(blue);

            this->r = red;
            this->g = green;
            this->b = blue;
        }

    private:
        float normalize(float value)
        {
            return value / 255.0f;
        }
    };
    static Color red = Color(1.0f, 0.0f, 0.0f);
}

#endif //COLOR_H

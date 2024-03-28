#ifndef __COLOR_H__
#define __COLOR_H__
#include "Vec3.h"
#include "Vec4.h"
#include <vector>

class Color
{
public:
    double r, g, b;

    Color();
    Color(double r, double g, double b);
    Color(const Color& other);

    static void line_rasterization(
        Vec3 vertex1, Vec3 vertex2,
        Color color1, Color color2,
        std::vector<std::vector<double>>& depths,
        std::vector<std::vector<Color>>& pixels
    );

    static void triangle_rasterization(
        Vec3 vertex1, Vec3 vertex2, Vec3 vertex3,
        Color color1, Color color2, Color color3,
        std::vector<std::vector<double>>& depths,
        std::vector<std::vector<Color>>& pixels
    );

    static void line_low(
        Vec3 vertex1, Vec3 vertex2,
        Color color1, Color color2,
        std::vector<std::vector<double>>& depths,
        std::vector<std::vector<Color>>& pixels);

    static void line_high(
        Vec3 vertex1, Vec3 vertex2,
        Color color1, Color color2,
        std::vector<std::vector<double>>& depths,
        std::vector<std::vector<Color>>& pixels);

    static double line_function(double x1, double y1, double x2, double y2, double input_x,
                                double input_y);
    friend std::ostream& operator<<(std::ostream& os, const Color& c);
};

#endif

#include <iomanip>
#include <iostream>
#include <algorithm>
#include "Color.h"
#include <vector>
#include <cmath>

Color::Color()
{
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(const Color& other)
{
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

void Color::line_low(
    Vec3 vertex1,
    Vec3 vertex2,
    Color color1,
    Color color2,
    std::vector<std::vector<double>>& depths,
    std::vector<std::vector<Color>>& pixels
    ){

    double dx = vertex2.x - vertex1.x;
    double dy = vertex2.y - vertex1.y;
    int y_step = 1;

    double current_depth = vertex1.z;
    double depth_step = (vertex2.z - vertex1.z)/(vertex2.x-vertex1.x);

    Color current_color = color1;
    Color color_step;

    color_step.r = (color2.r - color1.r) / abs(vertex2.x - vertex1.x);
    color_step.g = (color2.g - color1.g) / abs(vertex2.x - vertex1.x);
    color_step.b = (color2.b - color1.b) / abs(vertex2.x - vertex1.x);

    if(dy < 0.0){
      y_step = -1;
      dy = -dy;
    }

    double d = (2*dy) - dx;
    int y = round(vertex1.y);
    if(vertex1.y == -0.5){
      y = 0;
    }else{
      y = round(vertex1.y);
    }

    int x;
    if(vertex1.x == -0.5){
      x = 0;
    }else{
      x = round(vertex1.x);
    }

    if(x == pixels.size()){
      x = pixels.size()-1;
    }
    if(y == pixels[x].size()){
      y = pixels[x].size()-1;
    }

    for(;x < round(vertex2.x);x+=1){
      pixels[x][y] = current_color;
      depths[x][y] = current_depth;

      if(d > 0){
        y = y + y_step;
        d = d + (2*(dy-dx));
      }else{
        d = d + 2*dy;
      }

      current_depth += depth_step;
      current_color.r += color_step.r;
      current_color.g += color_step.g;
      current_color.b += color_step.b;

    }
}

void Color::line_high(
    Vec3 vertex1,
    Vec3 vertex2,
    Color color1,
    Color color2,
    std::vector<std::vector<double>>& depths,
    std::vector<std::vector<Color>>& pixels
    ){

    double dx = vertex2.x - vertex1.x;
    double dy = vertex2.y - vertex1.y;
    int x_step = 1;

    double current_depth = vertex1.z;
    double depth_step = (vertex2.z - vertex1.z)/(vertex2.y-vertex1.y);

    Color current_color = color1;
    Color color_step;

    color_step.r = (color2.r - color1.r) / abs(vertex2.y - vertex1.y);
    color_step.g = (color2.g - color1.g) / abs(vertex2.y - vertex1.y);
    color_step.b = (color2.b - color1.b) / abs(vertex2.y - vertex1.y);

    if(dx < 0.0){
      x_step = -1;
      dx = -dx;
    }

    double d = (2*dx) - dy;
    int x;
    if(vertex1.x == -0.5){
      x = 0;
    }else{
      x = round(vertex1.x);
    }

    int y;
    if(vertex1.y == -0.5){
      y = 0;
    }else{
      y = round(vertex1.y);
    }

    if(x == pixels.size()){
      x = pixels.size()-1;
    }
    if(y == pixels[x].size()){
      y = pixels[x].size()-1;
    }

    for(;y < round(vertex2.y);y+=1){
      pixels[x][y] = current_color;
      depths[x][y] = current_depth;

      if(d > 0){
        x = x + x_step;
        d = d + (2*(dx-dy));
      }else{
        d = d + 2*dx;
      }

      current_depth += depth_step;
      current_color.r += color_step.r;
      current_color.g += color_step.g;
      current_color.b += color_step.b;

    }
}

void Color::line_rasterization(
    Vec3 vertex1,
    Vec3 vertex2,
    Color color1,
    Color color2,
    std::vector<std::vector<double>>& depths,
    std::vector<std::vector<Color>>& pixels
)
{
    if(abs(vertex2.y - vertex1.y) < abs(vertex2.x-vertex1.x)){
      if(vertex1.x>vertex2.x){
        line_low(vertex2,vertex1,color2,color1,depths,pixels);
      }else{
        line_low(vertex1,vertex2,color1,color2,depths,pixels);
      }
    }else{
      if(vertex1.y>vertex2.y){
        line_high(vertex2,vertex1,color2,color1,depths,pixels);
      }else{
        line_high(vertex1,vertex2,color1,color2,depths,pixels);
      }
    }
}

void Color::triangle_rasterization(
    Vec3 vertex1, Vec3 vertex2, Vec3 vertex3,
    Color color1, Color color2, Color color3,
    std::vector<std::vector<double>>& depths,
    std::vector<std::vector<Color>>& pixels
)
{
    int y_min = round(std::min(std::min(vertex1.y, vertex2.y), vertex3.y));
    int y_max = round(std::max(std::max(vertex1.y, vertex2.y), vertex3.y));

    int x_min = round(std::min(std::min(vertex1.x, vertex2.x), vertex3.x));
    int x_max = round(std::max(std::max(vertex1.x, vertex2.x), vertex3.x));

    for (int y = y_min; y <= y_max; y += 1)
    {
        for (int x = x_min; x <= x_max; x += 1)
        {
            double alpha = line_function(vertex2.x, vertex2.y, vertex3.x, vertex3.y, x, y) /
                line_function(vertex2.x, vertex2.y, vertex3.x, vertex3.y, vertex1.x, vertex1.y);

            double beta = line_function(vertex3.x, vertex3.y, vertex1.x, vertex1.y, x, y) /
                line_function(vertex3.x, vertex3.y, vertex1.x, vertex1.y, vertex2.x, vertex2.y);

            double gamma = line_function(vertex1.x, vertex1.y, vertex2.x, vertex2.y, x, y) /
                line_function(vertex1.x, vertex1.y, vertex2.x, vertex2.y, vertex3.x, vertex3.y);

            if (alpha >= 0 && beta >= 0 && gamma >= 0)
            {
                double red = color1.r * alpha + color2.r * beta + color3.r * gamma;
                double green = color1.g * alpha + color2.g * beta + color3.g * gamma;
                double blue = color1.b * alpha + color2.b * beta + color3.b * gamma;
                double depth = vertex1.z * alpha + vertex2.z * beta + vertex3.z * gamma;

                if (x < 0 || x >= depths.size() || y < 0 || y >= depths[0].size())
                {
                    continue;
                }

                if (depth < depths[x][y])
                {
                    pixels[x][y] = Color(red, green, blue);
                    depths[x][y] = depth;
                }
            }
        }
    }
}

double Color::line_function(double x1, double y1, double x2, double y2, double input_x,
                            double input_y)
{
    return input_x * (y1 - y2) + input_y * (x2 - x1) + x1 * y2 - y1 * x2;
}

std::ostream& operator<<(std::ostream& os, const Color& c)
{
    os << std::fixed << std::setprecision(0) << "rgb(" << c.r << ", " << c.g << ", " << c.b << ")";
    return os;
}

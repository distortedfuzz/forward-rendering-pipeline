#ifndef __ROTATION_H__
#define __ROTATION_H__
#include "Matrix4.h"

class Rotation
{
public:
    int rotationId;
    double angle, ux, uy, uz;

    Rotation();
    Rotation(int rotationId, double angle, double x, double y, double z);
    Matrix4 rotate();
    friend std::ostream &operator<<(std::ostream &os, const Rotation &r);
};

#endif
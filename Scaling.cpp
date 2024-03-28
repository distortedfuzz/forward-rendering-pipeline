#include <iomanip>
#include <iostream>
#include "Scaling.h"

Scaling::Scaling() {
    this->scalingId = -1;
    this->sx = 0;
    this->sy = 0;
    this->sz = 0;
}

Scaling::Scaling(int scalingId, double sx, double sy, double sz)
{
    this->scalingId = scalingId;
    this->sx = sx;
    this->sy = sy;
    this->sz = sz;
}

Matrix4 Scaling::scale(){
    double values[4][4];

    values[0][0] = sx;
    values[0][1] = 0.0;
    values[0][2] = 0.0;
    values[0][3] = 0.0;

    values[1][0] = 0.0;
    values[1][1] = sy;
    values[1][2] = 0.0;
    values[1][3] = 0.0;

    values[2][0] = 0.0;
    values[2][1] = 0.0;
    values[2][2] = sz;
    values[2][3] = 0.0;

    values[3][0] = 0.0;
    values[3][1] = 0.0;
    values[3][2] = 0.0;
    values[3][3] = 1.0;

    Matrix4 scaling_matrix(values);

    return scaling_matrix;

}

std::ostream &operator<<(std::ostream &os, const Scaling &s)
{
    os << std::fixed << std::setprecision(3) << "Scaling " << s.scalingId << " => [" << s.sx << ", " << s.sy << ", " << s.sz << "]";

    return os;
}

#include <iomanip>
#include "Translation.h"

Translation::Translation()
{
    this->translationId = -1;
    this->tx = 0.0;
    this->ty = 0.0;
    this->tz = 0.0;
}

Translation::Translation(int translationId, double tx, double ty, double tz)
{
    this->translationId = translationId;
    this->tx = tx;
    this->ty = ty;
    this->tz = tz;
}

Matrix4 Translation::translate(){

    double values[4][4];

    values[0][0] = 1.0;
    values[0][1] = 0.0;
    values[0][2] = 0.0;
    values[0][3] = tx;

    values[1][0] = 0.0;
    values[1][1] = 1.0;
    values[1][2] = 0.0;
    values[1][3] = ty;

    values[2][0] = 0.0;
    values[2][1] = 0.0;
    values[2][2] = 1.0;
    values[2][3] = tz;

    values[3][0] = 0.0;
    values[3][1] = 0.0;
    values[3][2] = 0.0;
    values[3][3] = 1.0;

    Matrix4 translation_matrix(values);

    return translation_matrix;

}

std::ostream &operator<<(std::ostream &os, const Translation &t)
{
    os << std::fixed << std::setprecision(3) << "Translation " << t.translationId << " => [" << t.tx << ", " << t.ty << ", " << t.tz << "]";
    return os;
}
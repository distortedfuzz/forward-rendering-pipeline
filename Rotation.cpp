#include <iomanip>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
#include "Rotation.h"
#include "Matrix4.h"
#include "Helpers.h"
#include "Vec3.h"

Rotation::Rotation() {
    this->rotationId = -1;
    this->angle = 0;
    this->ux = 0;
    this->uy = 0;
    this->uz = 0;
}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->ux = x;
    this->uy = y;
    this->uz = z;
}

Matrix4 Rotation::rotate() {

    //ensure axis of rotation is normalized and turned to u_axis
    Vec3 axis_of_rotation;
    axis_of_rotation.x = ux;
    axis_of_rotation.y = uy;
    axis_of_rotation.z = uz;

    Vec3 u_axis = normalizeVec3(axis_of_rotation);

    //if axis of rotation already x, y or z we do not need to take the steps of an arbitrary axis
    angle = (angle * 3.14159265) / 180.0;

    if(u_axis.x == 1){
      double main_rotation_values[4][4];

      main_rotation_values[0][0] = 1.0;
      main_rotation_values[0][1] = 0.0;
      main_rotation_values[0][2] = 0.0;
      main_rotation_values[0][3] = 0.0;

      main_rotation_values[1][0] = 0.0;
      main_rotation_values[1][1] = cos(angle);
      main_rotation_values[1][2] = -sin(angle);
      main_rotation_values[1][3] = 0.0;

      main_rotation_values[2][0] = 0.0;
      main_rotation_values[2][1] = sin(angle);
      main_rotation_values[2][2] = cos(angle);
      main_rotation_values[2][3] = 0.0;

      main_rotation_values[3][0] = 0.0;
      main_rotation_values[3][1] = 0.0;
      main_rotation_values[3][2] = 0.0;
      main_rotation_values[3][3] = 1.0;

      Matrix4 main_rotation_matrix(main_rotation_values);
      return main_rotation_matrix;
    }else if(u_axis.y == 1){
      double main_rotation_values[4][4];

      main_rotation_values[0][0] = cos(angle);
      main_rotation_values[0][1] = 0.0;
      main_rotation_values[0][2] = sin(angle);
      main_rotation_values[0][3] = 0.0;

      main_rotation_values[1][0] = 0.0;
      main_rotation_values[1][1] = 1.0;
      main_rotation_values[1][2] = 0.0;
      main_rotation_values[1][3] = 0.0;

      main_rotation_values[2][0] = -sin(angle);
      main_rotation_values[2][1] = 0.0;
      main_rotation_values[2][2] = cos(angle);
      main_rotation_values[2][3] = 0.0;

      main_rotation_values[3][0] = 0.0;
      main_rotation_values[3][1] = 0.0;
      main_rotation_values[3][2] = 0.0;
      main_rotation_values[3][3] = 1.0;

      Matrix4 main_rotation_matrix(main_rotation_values);
      return main_rotation_matrix;
    }else if(u_axis.z == 1){
      double main_rotation_values[4][4];

      main_rotation_values[0][0] = cos(angle);
      main_rotation_values[0][1] = -sin(angle);
      main_rotation_values[0][2] = 0.0;
      main_rotation_values[0][3] = 0.0;

      main_rotation_values[1][0] = sin(angle);;
      main_rotation_values[1][1] = cos(angle);
      main_rotation_values[1][2] = 0.0;
      main_rotation_values[1][3] = 0.0;

      main_rotation_values[2][0] = 0.0;
      main_rotation_values[2][1] = 0.0;
      main_rotation_values[2][2] = 1.0;
      main_rotation_values[2][3] = 0.0;

      main_rotation_values[3][0] = 0.0;
      main_rotation_values[3][1] = 0.0;
      main_rotation_values[3][2] = 0.0;
      main_rotation_values[3][3] = 1.0;

      Matrix4 main_rotation_matrix(main_rotation_values);

      return main_rotation_matrix;
    }

    //continue if it is not x, y or z
    //get the smallest component of u_axis

    std::vector<double> abs_values;

    abs_values.push_back(abs(ux));
    abs_values.push_back(abs(uy));
    abs_values.push_back(abs(uz));

    double smallest_element = 0.0;
    int smallest_index = 0;

    for(int i = 0; i < 3; i++){
      if(i == 0){
        smallest_element = abs_values[0];
      }else{
        if(abs_values[i] < smallest_element){
          smallest_element = abs_values[i];
          smallest_index = i;
        }
      }
    }

    //form v_axis, set smallest of u to 0 and change places of others and negate one
    Vec3 v_axis;

    if(smallest_index == 0){
      v_axis.x = 0.0;

      double hold_y = v_axis.y;
      v_axis.y = -v_axis.z;
      v_axis.z = hold_y;

    }else if(smallest_index == 1){
      v_axis.y = 0.0;

      double hold_x = v_axis.x;
      v_axis.x = -v_axis.z;
      v_axis.z = hold_x;
    }else{
      v_axis.z = 0.0;

      double hold_x = v_axis.x;
      v_axis.x = -v_axis.y;
      v_axis.y = hold_x;
    }

    v_axis = normalizeVec3(v_axis);

    //w_axis is just the cross product of u and v

    Vec3 w_axis = crossProductVec3(u_axis, v_axis);

    //rotation matrix for onb to fit xyz

    double rotation_values[4][4];
    rotation_values[0][0] = u_axis.x;
    rotation_values[0][1] = u_axis.y;
    rotation_values[0][2] = u_axis.z;
    rotation_values[0][3] = 0.0;

    rotation_values[1][0] = v_axis.x;
    rotation_values[1][1] = v_axis.y;
    rotation_values[1][2] = v_axis.z;
    rotation_values[1][3] = 0.0;

    rotation_values[2][0] = w_axis.x;
    rotation_values[2][1] = w_axis.y;
    rotation_values[2][2] = w_axis.z;
    rotation_values[2][3] = 0.0;

    rotation_values[3][0] = 0.0;
    rotation_values[3][1] = 0.0;
    rotation_values[3][2] = 0.0;
    rotation_values[3][3] = 1.0;

    Matrix4 rotation_matrix(rotation_values);

    // matrix for reversing the onb to original location after main rotation

    double reverse_rotation_values[4][4];
    rotation_values[0][0] = u_axis.x;
    rotation_values[0][1] = v_axis.x;
    rotation_values[0][2] = w_axis.x;
    rotation_values[0][3] = 0.0;

    rotation_values[1][0] = u_axis.y;
    rotation_values[1][1] = v_axis.y;
    rotation_values[1][2] = w_axis.y;
    rotation_values[1][3] = 0.0;

    rotation_values[2][0] = u_axis.z;
    rotation_values[2][1] = v_axis.z;
    rotation_values[2][2] = w_axis.z;
    rotation_values[2][3] = 0.0;

    rotation_values[3][0] = 0.0;
    rotation_values[3][1] = 0.0;
    rotation_values[3][2] = 0.0;
    rotation_values[3][3] = 1.0;

    Matrix4 reverse_rotation_matrix(reverse_rotation_values);

    //main rotation matrix around x

    double main_rotation_values[4][4];

    main_rotation_values[0][0] = 1.0;
    main_rotation_values[0][1] = 0.0;
    main_rotation_values[0][2] = 0.0;
    main_rotation_values[0][3] = 0.0;

    main_rotation_values[1][0] = 0.0;
    main_rotation_values[1][1] = cos(angle);
    main_rotation_values[1][2] = -sin(angle);
    main_rotation_values[1][3] = 0.0;

    main_rotation_values[2][0] = 0.0;
    main_rotation_values[2][1] = sin(angle);
    main_rotation_values[2][2] = cos(angle);
    main_rotation_values[2][3] = 0.0;

    main_rotation_values[3][0] = 0.0;
    main_rotation_values[3][1] = 0.0;
    main_rotation_values[3][2] = 0.0;
    main_rotation_values[3][3] = 1.0;

    Matrix4 main_rotation_matrix(main_rotation_values);

    //compute the multiplication and return
    Matrix4 final_matrix = multiplyMatrixWithMatrix(reverse_rotation_matrix, main_rotation_matrix);
    final_matrix = multiplyMatrixWithMatrix(final_matrix, rotation_matrix);


    return final_matrix;
}

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    os << std::fixed << std::setprecision(3) << "Rotation " << r.rotationId << " => [angle=" << r.angle << ", " << r.ux << ", " << r.uy << ", " << r.uz << "]";
    return os;
}
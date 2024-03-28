#include <iomanip>
#include <iostream>
#include "Camera.h"
#include "Helpers.h"

Camera::Camera() {}

Camera::Camera(int cameraId,
               int projectionType,
               Vec3 position, Vec3 gaze,
               Vec3 u, Vec3 v, Vec3 w,
               double left, double right, double bottom, double top,
               double near, double far,
               int horRes, int verRes,
               std::string outputFilename)
{

  this->cameraId = cameraId;
  this->projectionType = projectionType;
  this->position = position;
  this->gaze = gaze;
  this->u = u;
  this->v = v;
  this->w = w;
  this->left = left;
  this->right = right;
  this->bottom = bottom;
  this->top = top;
  this->near = near;
  this->far = far;
  this->horRes = horRes;
  this->verRes = verRes;
  this->outputFilename = outputFilename;
}

Camera::Camera(const Camera &other)
{
  this->cameraId = other.cameraId;
  this->projectionType = other.projectionType;
  this->position = other.position;
  this->gaze = other.gaze;
  this->u = other.u;
  this->v = other.v;
  this->w = other.w;
  this->left = other.left;
  this->right = other.right;
  this->bottom = other.bottom;
  this->top = other.top;
  this->near = other.near;
  this->far = other.far;
  this->horRes = other.horRes;
  this->verRes = other.verRes;
  this->outputFilename = other.outputFilename;
}

Matrix4 Camera::camera_transformation() {

    Matrix4 cam_value_all;

    cam_value_all.values[0][0] = this->u.x;
    cam_value_all.values[0][1] = this->u.y;
    cam_value_all.values[0][2] = this->u.z;
    cam_value_all.values[0][3] = -(this->u.x* this->position.x + this->u.y* this->position.y + this->u.z* this->position.z);

    cam_value_all.values[1][0] = this->v.x;
    cam_value_all.values[1][1] = this->v.y;
    cam_value_all.values[1][2] = this->v.z;
    cam_value_all.values[1][3] = -(this->v.x* this->position.x + this->v.y* this->position.y + this->v.z* this->position.z);

    cam_value_all.values[2][0] = this->w.x;
    cam_value_all.values[2][1] = this->w.y;
    cam_value_all.values[2][2] = this->w.z;
    cam_value_all.values[2][3] = -(this->w.x* this->position.x + this->w.y* this->position.y + this->w.z* this->position.z);

    cam_value_all.values[3][0] = 0.0;
    cam_value_all.values[3][1] = 0.0;
    cam_value_all.values[3][2] = 0.0;
    cam_value_all.values[3][3] = 1.0;

    return cam_value_all;
}

Matrix4 Camera::ortographic_transformation() {

    double ortographic_values[4][4];

    ortographic_values[0][0] = 2/(this->right - this->left);
    ortographic_values[0][1] = 0.0;
    ortographic_values[0][2] = 0.0;
    ortographic_values[0][3] = -(this->right + this->left)
                               /(this->right - this->left);

    ortographic_values[1][0] = 0.0;
    ortographic_values[1][1] = 2/(this->top - this->bottom);
    ortographic_values[1][2] = 0.0;
    ortographic_values[1][3] = -(this->top + this->bottom)
                               /(this->top - this->bottom);

    ortographic_values[2][0] = 0.0;
    ortographic_values[2][1] = 0.0;
    ortographic_values[2][2] = -2/(this->top - this->bottom);
    ortographic_values[2][3] = -(this->top + this->bottom)
                               /(this->top - this->bottom);

    ortographic_values[3][0] = 0.0;
    ortographic_values[3][1] = 0.0;
    ortographic_values[3][2] = 0.0;
    ortographic_values[3][3] = 1.0;

    Matrix4 ortographic_projection(ortographic_values);

    return ortographic_projection;

}

Matrix4 Camera:: perspective_transformation() {

    double perspective_values[4][4];

    perspective_values[0][0] = 2*(this->near)/(this->right - this->left);
    perspective_values[0][1] = 0.0;
    perspective_values[0][2] = (this->right + this->left)
                               /(this->right - this->left);
    perspective_values[0][3] = 0.0;

    perspective_values[1][0] = 0.0;
    perspective_values[1][1] = 2*(this->near)/(this->top - this->bottom);
    perspective_values[1][2] = (this->top + this->bottom)
                               /(this->top - this->bottom);
    perspective_values[1][3] = 0.0;

    perspective_values[2][0] = 0.0;
    perspective_values[2][1] = 0.0;
    perspective_values[2][2] = -(this->far + this->near)
                               /(this->far - this->near);
    perspective_values[2][3] = -(2 * this->far * this->near)
                               /(this->far - this->near);

    perspective_values[3][0] = 0.0;
    perspective_values[3][1] = 0.0;
    perspective_values[3][2] = -1.0;
    perspective_values[3][3] = 0.0;

    Matrix4 perspective_projection(perspective_values);

    return perspective_projection;

}


Matrix4 Camera::viewport_transformation(){

    double viewport_values[4][4];

    viewport_values[0][0] = (this->horRes)/2.0;
    viewport_values[0][1] = 0.0;
    viewport_values[0][2] = 0.0;
    viewport_values[0][3] = (this->horRes-1)/2.0;

    viewport_values[1][0] = 0.0;
    viewport_values[1][1] = this->verRes/2.0;
    viewport_values[1][2] = 0.0;
    viewport_values[1][3] = (this->verRes-1)/2.0;

    viewport_values[2][0] = 0.0;
    viewport_values[2][1] = 0.0;
    viewport_values[2][2] = 0.5;
    viewport_values[2][3] = 0.5;

    viewport_values[3][0] = 0.0;
    viewport_values[3][1] = 0.0;
    viewport_values[3][2] = 0.0;
    viewport_values[3][3] = 1.0;


    Matrix4 viewport_projection(viewport_values);

    return viewport_projection;

}

void Camera::clipping(Vec4 &p0, Vec4 &p1) {

    double te = 0.0;
    double tl = 1.0;
    double dx = p1.x - p0.x;
    double dy = p1.y - p0.y;
    double dz = p1.z - p0.z;

    if(this->projectionType == 0){
      if (Camera::visible(dx, (this->left - p0.x), te, tl)){
        if(Camera::visible(-dx, (p0.x - this->right), te, tl)){
          if(Camera::visible(dy, (this->bottom - p0.y), te, tl)){
            if(Camera::visible(-dy, (p0.y - this->top), te, tl)){
              if(Camera::visible(dz, (-this->near - p0.z), te, tl)){
                if(Camera::visible(-dz, (p0.z - -this->far), te, tl)){

                  if(tl<1){
                    p1.x = p0.x + dx*tl;
                    p1.y = p0.y + dy*tl;
                    p1.z = p0.z + dz*tl;
                  }

                  if(te>0){
                    p0.x = p0.x + dx*te;
                    p0.y = p0.y + dy*te;
                    p0.z = p0.z + dz*te;
                  }
                }
              }
            }
          }
        }
      }
    }else{

      if (Camera::visible(dx, (-1.0 - p0.x), te, tl)){
        if(Camera::visible(-dx, (p0.x - 1.0), te, tl)){
          if(Camera::visible(dy, (-1.0 - p0.y), te, tl)){
            if(Camera::visible(-dy, (p0.y - 1.0), te, tl)){
              if(Camera::visible(dz, (-1.0 - p0.z), te, tl)){
                if(Camera::visible(-dz, (p0.z - 1.0), te, tl)){

                  if(tl<1){
                    p1.x = p0.x + dx*tl;
                    p1.y = p0.y + dy*tl;
                    p1.z = p0.z + dz*tl;
                  }

                  if(te>0){
                    p0.x = p0.x + dx*te;
                    p0.y = p0.y + dy*te;
                    p0.z = p0.z + dz*te;
                  }
                }
              }
            }
          }
        }
      }

    }
}

bool Camera::visible(double den, double diff, double &te, double &tl){

    double t;

    if(den>0.0){
      t = diff/den;
      if(t>tl){
        return false;
      }

      if(t>te){
        te = t;
      }

    }else if(den<0.0){
      t = diff/den;

      if(t<te){
        return false;
      }

      if(t<tl){
        tl = t;
      }
    }else if(diff>0.0){
      return false;
    }

    return true;
}


std::ostream &operator<<(std::ostream &os, const Camera &c)
{
    const char *camType = c.projectionType ? "perspective" : "orthographic";

    os << std::fixed << std::setprecision(6) << "Camera " << c.cameraId << " (" << camType << ") => pos: " << c.position << " gaze: " << c.gaze << std::endl
       << "\tu: " << c.u << " v: " << c.v << " w: " << c.w << std::endl
       << std::fixed << std::setprecision(3) << "\tleft: " << c.left << " right: " << c.right << " bottom: " << c.bottom << " top: " << c.top << std::endl
       << "\tnear: " << c.near << " far: " << c.far << " resolutions: " << c.horRes << "x" << c.verRes << " fileName: " << c.outputFilename;

    return os;
}
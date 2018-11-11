#include <signal.h>
#include <unistd.h>
#include <cmath>
#include "../openglWp.h"
#include "../utils.h"
#include "../vec.h"
#include <stdio.h>

//#include <fenv.h>

#include <eigen3/Eigen/Dense>

void hsvcartesian2cylinder(Eigen::Vector3f& in)
{
  Eigen::Vector3f out = in;

  //in[2] = 0;

  out[0] = in[1] * sin(in[0]*2*M_PI);
  out[1] = in[1] * cos(in[0]*2*M_PI);

  in = out;
}

void rgb2hsv(Eigen::Vector3f& in)
{
  Eigen::Vector3f out;
  double      min, max, delta;

  min = in[0] < in[1] ? in[0] : in[1];
  min = min  < in[2] ? min  : in[2];

  max = in[0] > in[1] ? in[0] : in[1];
  max = max  > in[2] ? max  : in[2];

  out[2] = max;                                // v
  delta = max - min;
  if (delta < 0.00001)
  {
      out[1] = 0;
      out[0] = 0; // undefined, maybe nan?
      in = out;
      return;
  }
  if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
      out[1] = (delta / max);                  // s
  } else {
      // if max is 0, then r = g = b = 0              
      // s = 0, h is undefined
      out[1] = 0.0;
      out[0] = 0.0;                            // its now undefined
      in = out;
      return;
  }
  if( in[0] >= max )                           // > is bogus, just keeps compilor happy
      out[0] = ( in[1] - in[2] ) / delta;        // between yellow & magenta
  else
  if( in[1] >= max )
      out[0] = 2.0 + ( in[2] - in[0] ) / delta;  // between cyan & yellow
  else
      out[0] = 4.0 + ( in[0] - in[1] ) / delta;  // between magenta & cyan

  out[0] *= 60.0;                              // degrees

  if( out[0] < 0.0 )
      out[0] += 360.0;

  out[0] /= 360.;
  in = out;
}

void hsv2rgb(Eigen::Vector3f& in)
{
    //printf("1: %f\t%f\t%f\n", in[0],in[1],in[2]);
    //in[1]=1;
    //in[2]=1;
    double      hh, p, q, t, ff;
    long        i;
    Eigen::Vector3f out;

    if(in[1] <= 0.0) {       // < is bogus, just shuts up warnings
        out[0] = in[2];
        out[1] = in[2];
        out[2] = in[2];
        in = out;
        return;
    }
    hh = in[0] * 360.;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in[2] * (1.0 - in[1]);
    q = in[2] * (1.0 - (in[1] * ff));
    t = in[2] * (1.0 - (in[1] * (1.0 - ff)));

    switch(i) {
    case 0:
        out[0] = in[2];
        out[1] = t;
        out[2] = p;
        break;
    case 1:
        out[0] = q;
        out[1] = in[2];
        out[2] = p;
        break;
    case 2:
        out[0] = p;
        out[1] = in[2];
        out[2] = t;
        break;

    case 3:
        out[0] = p;
        out[1] = q;
        out[2] = in[2];
        break;
    case 4:
        out[0] = t;
        out[1] = p;
        out[2] = in[2];
        break;
    case 5:
    default:
        out[0] = in[2];
        out[1] = p;
        out[2] = q;
        break;
    }
    in = out;
    //printf("2: %f\t%f\t%f\n\n", in[0],in[1],in[2]);
}

namespace plotter
{
  bool down   = false;
  vec down_at;
  float phi             = 0;
  float theta           = 0;
  float xi              = 0;
  float down_at_phi     = 0;
  float down_at_theta   = 0;
  float down_at_xi      = 0;

  std::vector<float> vertices;
  std::vector<float> colors;


  //void (*)

  void cleanup(int signum){

    cleanupWp();
  }


  int (*public_process_sym)(SDL_Keycode sym);

  int process_event(const SDL_Event& e){
  }

  int poll_controls(){
    SDL_Event e;
    while ( SDL_PollEvent(&e) ) {
      switch (e.type) {
        case SDL_QUIT:
          return 1;
        break;
        case SDL_MOUSEBUTTONDOWN:
        {
          int x, y;
          SDL_GetMouseState(&x, &y);
          down = true;
          down_at.x = 2*(float)x/width - 1;
          down_at.y = 1 - 2*(float)y/height;
          down_at_phi = phi;
          down_at_theta = theta;
          down_at_xi = xi;
        }
        break;
        case SDL_MOUSEBUTTONUP:
        {
          int x, y;
          SDL_GetMouseState(&x, &y);
          down = false;
        }
        break;
        case SDL_KEYDOWN:
        {
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              return 1;
            break;
          }
          int error = 0;
          if (public_process_sym)
            error = public_process_sym(e.key.keysym.sym);
          if (error)
            return error;
        break;
        }
      }
    }
    return 0;
  }

  void Rx(Eigen::Matrix3f& mat, float phi)
  {
    Eigen::Matrix3f tmp;
    tmp <<  1,0,0,
            0,cos(phi),-sin(phi),
            0, sin(phi),cos(phi);
    mat *= tmp;
  }

  void Ry(Eigen::Matrix3f& mat, float theta)
  {
    Eigen::Matrix3f tmp;
    tmp <<  cos(theta),0,sin(theta),
            0,1,0,
            -sin(theta),0,cos(theta);
    mat *= tmp;
  }

  void Rz(Eigen::Matrix3f& mat, float xi)
  {
    Eigen::Matrix3f tmp;
    tmp <<  cos(xi),-sin(xi),0,
            sin(xi),cos(xi),0,
            0,0,1;
    mat *= tmp;
  }

  void rotate(Eigen::Matrix3f& mat, float phi, float theta, float xi)
  {
    Rz(mat,xi);
    Ry(mat,theta);
    Rx(mat,phi);
  }

  void draw()
  {
    Eigen::Matrix3f projection;
    projection.setIdentity();

    //xi += .1 * dt;
    if (down)
    {
      int x, y;
      SDL_GetMouseState(&x, &y);
      vec position(2*(float)x/width - 1, 1 - 2*(float)y/height);
      vec displacement;

      displacement = position - down_at;
      displacement *= 1.5;

      phi = down_at_phi + displacement.y;
      theta = down_at_theta + displacement.x;
      //xi = down_at_xi + displacement.z;
    }

    rotate(projection, phi, theta, xi);

    updateWp(vertices, colors, projection);
  }

  int add(const float* start, const uint32_t length)
  {
    int error = 0;
    if (length % 3 != 0)
      error = 1;
    //vertices.insert(vertices.end(), start, start + length - (length % 3));
    //vertices.reserve(length);
    //colors.reserve(length);
    for (Eigen::Vector3f* point = (Eigen::Vector3f*)start; point != (Eigen::Vector3f*)start + length - (length % 3); ++point)
    {
      Eigen::Vector3f vertex = *point;//->cwiseAbs();
      rgb2hsv(vertex);
      //vertex[2] = 1;
      hsvcartesian2cylinder(vertex);
      vertices.insert(vertices.end(), (float*)&vertex, (float*)&vertex + 3);
      

      //colors.insert(colors.end(), {1,1,1});
      Eigen::Vector3f color = *point;//->cwiseAbs();
      //color *= 1.0 / color.maxCoeff();
      colors.insert(colors.end(), (float*)&color, (float*)&color + 3);
    }

    return error;
  }

  //int add(std::initializer_list<float> points)
  //{
  //  int error = 0;
  //  const float* start = points.begin();
  //  const float* end = points.end();
  //  if (std::distance(start, end) % 3 != 0)
  //  {
  //    error = 1;
  //    end -= std::distance(start, end) % 3;
  //  }
  //  add(start, end - start);
  //}

  void init()
  {

    initWp(vertices, colors);

  }
}

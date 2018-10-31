#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <string>
#include "plot/plot.h"


struct pixel
{
  uint8_t b;
  uint8_t g;
  uint8_t r;

  pixel operator=(const size_t& rhs)
  {
    b = rhs;
    g = rhs;
    r = rhs;
    return *this;
  }
};

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

  in = out;
}

int main() {
  char ch = 0;
  int imgNum = 0;
  std::string imgName;
  bool videoMode = true;

  cv::VideoCapture cap("dtu_onboard_video.mp4");  //"dtu_onboard_video.mp4"
  //cap.set(CV_CAP_PROP_POS_MSEC, 1.21e+5);

  if (!cap.isOpened()) {
    printf("Error opening video stream or file\n");
    return -1;
  }

  double frame_rate = cap.get(CV_CAP_PROP_FPS);
  double frame_msec = 1000 / frame_rate;
  double video_time = 0;
  cap.set(CV_CAP_PROP_POS_MSEC, 1.21e+5 - frame_msec * 500);

  cv::Mat frame;
  cv::namedWindow("Window", CV_WINDOW_NORMAL);

  plotter::init();


  //plotter::add({0,0,0,1,0,0});

  double dt;
  uint64_t now, before;
  before = now = SDL_GetPerformanceCounter();

  bool new_frame = false;

  cap >> frame;

  //printf("channels: %d\n", frame.channels());
  new_frame = true;
  bool plot = 1;
  bool done = false;
  while (!done)
  {
    if (plotter::poll_controls())
      break;
    iterate_time(now, before, dt, 60);
    //cap >> frame;

    if (frame.empty()) {
      break;
    }


    uint32_t nrows = frame.rows;
    uint32_t ncols = frame.cols;
    //printf("nrows: %d, ncolsL %d\n", nrows, ncols);

    if (plot){
      plotter::vertices.reserve(nrows*ncols*3);
      plotter::colors.reserve(nrows*ncols*3);

      pixel* start = (pixel*)frame.data + nrows;
      pixel* end = (pixel*)frame.data + (nrows-1)*ncols/3;

      //plotter::add(points, sizeof(points)/sizeof(float));
      //plotter::add({.5,.5,.3, -.9,-.7,-.7});


      //// grey scale
      //for(pixel* p = start;p != end;)
      //{
      //  *p++ = (p->r + p->g + p->b) / 3;
      //}

      //if (cap.get(CV_CAP_PROP_POS_MSEC) != video_time)
      if (new_frame)
      {
        //int error = plotter::add((float*)start, (float*)end - (float*)start);

        for(pixel* p = start; p != end; ++p)
        {
          //printf("%f\t%f\t%f\t\t", *((float*)p), *((float*)p + 1), *((float*)p + 2));
          //printf("%d\t%d\t%d\t\t", p->r, p->g, p->b);
          Eigen::Vector3f fp;
          fp << (float)p->r / 256., (float)p->g / 256., (float)p->b / 256.;
          rgb2hsv(fp);
          *((float*)&fp) /= 360.;
          //printf("%f\t%f\t%f\t\t", *((float*)&fp), *((float*)&fp + 1), *((float*)&fp + 2));
          plotter::add((float*)&fp, 3);
        }

        //printf("error: %d\n", error);
        new_frame = false;
      }

      plotter::draw();
    }
    else {

      pixel* start = (pixel*)frame.data + nrows;
      pixel* end = (pixel*)frame.data + (nrows-1)*ncols;

      for(pixel* p = start; p != end; ++p)
      {
        //p->b = 0;
      }

      cv::imshow("Window", frame);

      if (videoMode) {
        ch = cv::waitKey(0);
      } else {
        ch = cv::waitKey(frame_msec);
      }

      switch (ch){
        case 'i':
          videoMode = !videoMode;
          break;
        case 'j':
          video_time = cap.get(CV_CAP_PROP_POS_MSEC);
          video_time -= frame_msec * 2;
          cap.set(CV_CAP_PROP_POS_MSEC, video_time);
          break;
        case 'u':
          video_time = cap.get(CV_CAP_PROP_POS_MSEC);
          video_time += frame_msec * 2;
          cap.set(CV_CAP_PROP_POS_MSEC, video_time);
          break;
        case 'h':
          video_time = cap.get(CV_CAP_PROP_POS_MSEC);
          video_time -= frame_msec * 200;
          cap.set(CV_CAP_PROP_POS_MSEC, video_time);
          break;
        case 'l':
          video_time = cap.get(CV_CAP_PROP_POS_MSEC);
          video_time += frame_msec * 200;
          cap.set(CV_CAP_PROP_POS_MSEC, video_time);
          break;
        case 'm':
          cap.set(CV_CAP_PROP_POS_MSEC, 1.21e+5);
          break;
        case 27: // escape
          done = true;
          break; 
      }
    }
  }
  cap.release();

  cv::destroyAllWindows();

  return 0;
}

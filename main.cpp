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

cv::VideoCapture cap;
cv::Mat frame;
double frame_rate = cap.get(CV_CAP_PROP_FPS);
double frame_msec = 1000 / frame_rate;
double video_time = 0;
bool plot = 1;
bool new_frame = false;
uint32_t nrows;
uint32_t ncols;

int process_sym(SDL_Keycode sym){
  switch (sym) {
    case SDLK_i:
      plot = !plot;
      break;
    case SDLK_v:
    {
      int x, y;
      SDL_GetMouseState(&x, &y);
      float xf = 2*(float)x/width - 1;
      float yf = 1 - 2*(float)y/height;

      x = (xf + 1) * ncols / 2;
      y = (1 - yf) * nrows / 2;

      printf("x,y: %d,%d\n", x, y);

      Eigen::Vector3f fp;
      pixel* p = (pixel*)frame.data + y * ncols + x;;
      fp << (float)p->r / 255. +.01, (float)p->g / 255. +.01, (float)p->b / 255. + .01;
      plotter::add_highlighted((float*)&fp, 1);
    }
    break;
    //case SDLK_j:
    //  video_time = cap.get(CV_CAP_PROP_POS_MSEC);
    //  video_time -= frame_msec * 2;
    //  cap.set(CV_CAP_PROP_POS_MSEC, video_time);
    //  new_frame = true;
    //  break;
    //case SDLK_u:
    //  video_time = cap.get(CV_CAP_PROP_POS_MSEC);
    //  video_time += frame_msec * 2;
    //  cap.set(CV_CAP_PROP_POS_MSEC, video_time);
    //  new_frame = true;
    //  break;
    //case SDLK_h:
    //  video_time = cap.get(CV_CAP_PROP_POS_MSEC);
    //  video_time -= frame_msec * 200;
    //  cap.set(CV_CAP_PROP_POS_MSEC, video_time);
    //  new_frame = true;
    //  break;
    //case SDLK_l:
    //  video_time = cap.get(CV_CAP_PROP_POS_MSEC);
    //  video_time += frame_msec * 200;
    //  cap.set(CV_CAP_PROP_POS_MSEC, video_time);
    //  new_frame = true;
    //  break;
    case SDLK_m:
      cap.set(CV_CAP_PROP_POS_MSEC, 1.21e+5);
      new_frame = true;
      break;
  }
  return 0;
}

int main() {
  char ch = 0;
  int imgNum = 0;
  std::string imgName;

  cap.open("dtu_onboard_video.mp4");  //"dtu_onboard_video.mp4"
  //cap.set(CV_CAP_PROP_POS_MSEC, 1.21e+5);

  if (!cap.isOpened()) {
    printf("Error opening video stream or file\n");
    return -1;
  }

  cap.set(CV_CAP_PROP_POS_MSEC, 1.21e+5); // - frame_msec * 500);

  cv::namedWindow("Window", CV_WINDOW_NORMAL);

  plotter::init();

  plotter::public_process_sym = process_sym;


  //plotter::add({0,0,0,1,0,0});

  double dt;
  uint64_t now, before;
  before = now = SDL_GetPerformanceCounter();


  cap >> frame;

  //printf("channels: %d\n", frame.channels());
  new_frame = true;
  bool done = false;
  while (!done)
  {
    if (plotter::poll_controls())
      break;
    iterate_time(now, before, dt, 60);
    if (new_frame)
      cap >> frame;

    if (frame.empty()) {
      break;
    }


    nrows = frame.rows;
    ncols = frame.cols;
    //printf("nrows: %d, ncolsL %d\n", nrows, ncols);

    if (plot){
      plotter::vertices.reserve(nrows*ncols*3);
      plotter::colors.reserve(nrows*ncols*3);

      pixel* start = (pixel*)frame.data + nrows;
      pixel* end = (pixel*)frame.data + (nrows-1)*ncols;

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

        printf("start\n");
        for(pixel* p = start; p != end; ++p)
        {
          //printf("%f\t%f\t%f\t\t", *((float*)p), *((float*)p + 1), *((float*)p + 2));
          //printf("%d\t%d\t%d\t\t", p->r, p->g, p->b);
          static Eigen::Vector3f fp;
          fp << (float)p->r / 255., (float)p->g / 255., (float)p->b / 255.;
          //printf("9: %f\t%f\t%f\n", *((float*)&fp), *((float*)&fp + 1), *((float*)&fp + 2));
          //rgb2hsv(fp);
          //*((float*)&fp) /= 360.;
          //printf("0: %f\t%f\t%f\n", *((float*)&fp), *((float*)&fp + 1), *((float*)&fp + 2));
          plotter::add((float*)&fp, 1);
        }
        printf("fin\n");

        //printf("error: %d\n", error);
        new_frame = false;
      }

      plotter::draw(nullptr);
    }
    else {

      pixel* start = (pixel*)frame.data + nrows;
      pixel* end = (pixel*)frame.data + (nrows-1)*ncols;

      for(pixel* p = start; p != end; ++p)
      {
        //p->b = 0;
      }

      plotter::draw(&frame);

      new_frame = false;
    }
  }
  cap.release();

  cv::destroyAllWindows();

  return 0;
}

#include <thread>
#include <signal.h>
#include <unistd.h>
#include <cmath>
#include "../utils.h"
#include "../vec.h"
#include "plotter.h"

#include <eigen3/Eigen/Dense>

int main(int argc, char *argv[])
{
  plotter::init();

  float points[] = {0.,0.,0.,.5,.5,0,-.3,-.7,-.7};
  plotter::add(points, sizeof(points)/sizeof(float));

  plotter::add({.5,.5,.3, -.9,-.7,-.7});

  double dt;
  uint64_t now, before;
  before = now = SDL_GetPerformanceCounter();

  while (true){
    iterate_time(now, before, dt, 60);

    if (plotter::poll_controls())
      break;
    plotter::draw();
  }

  

  return 0;
}

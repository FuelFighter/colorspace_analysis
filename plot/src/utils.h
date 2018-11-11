#pragma once
#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>

#define IS_SET(var,pos) ((var) & (1<<(pos)))

 #define SQ(var) ((var) * (var))

inline uint32_t secs()
{ return (float)SDL_GetTicks() / 1e3; }

inline void iterate_time(uint64_t& now, uint64_t& before, double& dt, float framerate)
{
    now = SDL_GetPerformanceCounter();
    dt = (double)(now - before) / SDL_GetPerformanceFrequency();
    if (dt < 1/framerate){
        SDL_Delay(1000/framerate - 1000*dt);
        now = SDL_GetPerformanceCounter();
        dt = (double)(now - before) / SDL_GetPerformanceFrequency();
    }
    before = now;
}

inline float deg_rad(float deg)
{ return deg * M_PI / 180; }

inline float rad_deg(float rad)
{ return rad * 180 / M_PI; }


inline float limit(float var, float lower, float upper)
{
    if (var < lower)
        return lower;
    else if (var > upper)
        return upper;
    else
        return var;
}

inline float limit_size(float var, float size)
{
    if (var > size)
        return size;
    else if (var < -size)
        return -size;
    else
        return var;
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

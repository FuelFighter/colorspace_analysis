#!/bin/bash
g++ main.cpp -g -std=c++11 `pkg-config --libs opencv` -I/opt/ros/kinetic/include -L/opt/ros/kinetic/lib -lroscpp -lxmlrpcpp -lcpp_common -lrosconsole_log4cxx -lrosconsole_backend_interface -lrosconsole -lrostime -lroscpp_serialization -lpthread -Iplot/src -lSDL2 -lGL -lGLEW -o run

#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/ffmpeg-4.3.1/lib/
g++ test.cpp -lavcodec -lavformat -lswresample -lavutil -I/usr/local/ffmpeg-4.3.1/include/ -L/usr/local/ffmpeg-4.3.1/lib/
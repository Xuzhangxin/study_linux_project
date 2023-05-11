######################################
#
######################################
#source file
#源文件，自动找所有.c和.cpp文件，并将目标定义为同名.o文件
SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
  
#target you can change test to what you want
#目标文件名，输入任意你想要的执行文件名
TARGET  := test
  
#compile and lib parameter
#编译参数
CROSS_COMPILE := 
CC      := $(CROSS_COMPILE)gcc
CXX		:= $(CROSS_COMPILE)g++
LIBS    := -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -lavformat
LDFLAGS := -L/home/xzx/ffmpeg_build/lib/ 
DEFINES :=
INCLUDE := -I. -I/home/xzx/ffmpeg_build/include
CFLAGS  := -g -Wall -O3 $(DEFINES) $(INCLUDE) 
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H
  
  
#i think you should do anything here
#下面的基本上不需要做任何改动了
.PHONY : all clean rebuild
  
all : $(TARGET)
  
objs : $(OBJS)
  
rebuild: clean all
                
clean :
	rm -fr *.so
	rm -fr *.o
	rm -fr $(TARGET)
	
$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

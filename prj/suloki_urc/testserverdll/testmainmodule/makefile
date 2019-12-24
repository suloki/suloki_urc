#target you can change test to what you want
TARGET  := libtestmainmodule.so
#compile and lib parameter
#CROSS_COMPILE := arm-none-linux-gnueabi-
CC      := gcc
#CC    := $(CROSS_COMPILE)$(CC)
CXX    := g++
#CXX  := $(CROSS_COMPILE)$(CXX)
LIBS    := -L../../../../depend/boost/stage_linux/lib -lboost_system -lboost_thread -lboost_log -lpthread -L../../../../lib/ -lsuloki  -L../../../../depend/protobuf/libtmp/ -lprotobuf -L../../../../depend/mysql/lib/ -lmysqlclient -lrt
LDFLAGS:=
DEFINES:= -DSULOKI_64_PLATFORM_SULOKI -DBOOST_LOG_DYN_LINK
INCLUDE:= -I. -I../../../../depend/boost/ -I../../../../depend/protobuf/src/ -I../../../../proto/  -I../../../../depend/loki/ -I../../../../lib/ -I../../../../depend/websocketpp/ -I../../../../depend/mysql/include/ -I/usr/include/ -I/usr/include/mysql/
CFLAGS  := -g -fPIC -Wall -O3 $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H
SHARE   := -fPIC -shared -o
#i think you should do anything here
#source file
SOURCE  := $(wildcard *.c) $(wildcard ../../../../proto/*.cc) $(wildcard *.cpp)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(SOURCE))))

.PHONY : everything objs clean distclean rebuild
everything : $(TARGET)
all : $(TARGET)
objs : $(OBJS)
rebuild: distclean everything
clean :
	rm -fr ../../../../proto/*.o
	rm -fr *.o
	rm -fr $(TARGET)
distclean : clean
	rm -fr $(TARGET)

$(TARGET) : $(OBJS) 
	$(CC) $(CXXFLAGS) $(SHARE) $@ $(OBJS) $(LDFLAGS) $(LIBS)
	cp ./$(TARGET) ../

#source file
SOURCE  := $(wildcard *.c) $(wildcard ../../proto/*.cc) $(wildcard *.cpp)
#OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(SOURCE))))
#target you can change test to what you want
TARGET  := suloki_framework
#compile and lib parameter
#CROSS_COMPILE := arm-none-linux-gnueabi-
CC      := gcc
#CC    := $(CROSS_COMPILE)$(CC)
CXX    := g++
#CXX  := $(CROSS_COMPILE)$(CXX)
#LIBS    := -L../../depend/boost/stage_linux/lib -lboost_system -lboost_thread -L../ -lxhbase -lpthread -L../sotest -ltest -lprotobuf -L../../depend/gtest/build/ -lgtest -lgtest_main
#LIBS    := -L../../depend/boost/stage_linux/lib -lboost_system -lboost_thread -L../ -lxhbase -lpthread -L../sotest -ltest -lprotobuf -L../../proto/ -lxhproto -lcoredumper
#LIBS    := -L../../depend/boost/stage_linux/lib -lboost_system -lboost_thread -lpthread -L../ -lxhbase -L../sotest -ltest -lprotobuf -lrt -ldl
LIBS    := -L../../depend/boost/stage_linux/lib -lboost_system -lboost_thread -lboost_log -lpthread -L../../lib/ -lsuloki  -L../../depend/protobuf/libtmp/ -lprotobuf -lrt -ldl
LDFLAGS:= 
#DEFINES:= -DXH_BOOST_GLOBAL_XHBASE -DXH_64_PLATFORM_GLOBAL_XHBASE -DXH_GTEST_GLOBAL_XHBASE
DEFINES:= -DSULOKI_64_PLATFORM_SULOKI -DBOOST_LOG_DYN_LINK
#INCLUDE:= -I. -I../../depend/boost/ -I../ -I../../depend/gtest/include/
#INCLUDE:= -I. -I../../depend/boost/ -I../ -I../../depend/protobuf/src/ -I../../proto/ -I../../depend/google-coredumper/src/
INCLUDE:= -I. -I../../depend/boost/ -I../../depend/protobuf/src/ -I../../proto/  -I../../depend/loki/ -I../../lib/ -I../../depend/websocketpp/
CFLAGS  := -g -Wall -O3 $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H

#i think you should do anything here
.PHONY : everything objs clean distclean rebuild
everything : $(TARGET)
all : $(TARGET)
objs : $(OBJS)
rebuild: distclean everything

clean :
	rm -fr ../../proto/*.o	
	rm -fr *.o
	rm -fr $(TARGET)
distclean : clean
	rm -fr $(TARGET)
$(TARGET) : $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	cp ./$(TARGET) ../suloki_urc/urcserver/
	cp ./$(TARGET) ../suloki_urc/testserverdll/


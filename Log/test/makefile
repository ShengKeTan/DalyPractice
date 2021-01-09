CC = g++

LIB_DIR = ../util
SD_LIB := -L$(LIB_DIR)

TARFLAGS = -g
CCFLAGS = $(TARFLAGS) -Wall -finline-functions -O2 -D__LINUX__
LFGLAGS = $(SD_LIB)

INCS = -I../util/Include -I../App/Include
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))
LIBS := -llist -ltimer -lpthread -llog

TARGET := sample_test

all: $(TARGET)

$(TARGET): $(OBJS)
		$(CC) $(TARFLAGS) $(OBJS) -o $@ $(SD_LIB) $(LIBS)

%.o: %.cpp
		$(CC) $(CCFLAGS) $(INCS) -c $< -o $@

clean:
		-rm -f $(TARGET) $(OBJS)

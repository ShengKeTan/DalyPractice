CC = g++

TARFLAGS = -g
CCFLAGS = $(TARFLAGS) -Wall -finline-functions -O2 -D__LINUX__

INCS = -I./ -I../Log/App/Include -I../Log/util/Include

LIB_DIR = ./lib
SD_LIB = -L$(LIB_DIR)
LIBS = -llog -llist

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

TARGET := MemManager

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(TARFLAGS) $(OBJS) -o $@ $(SD_LIB) $(LIBS)

%.o: %.cpp
	$(CC) $(CCFLAGS) $(INCS) -c $< -o $@

clean:
	-rm -f $(OBJS) $(TARGET)


CC = g++

DEPFLAGS = -MM -MG -P -w
ARFLAGS = 
CCFLAGS = -Wall -finline-functions -O2 -D__LINUX__ -g
INCFLAGS = -I./Include -I../util/Include

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))
DEPS := $(patsubst %.cpp, %.d, $(SRCS))
LIB := ../util/liblog.a

LIB_DIR = ../util
SD_LIB := -L$(LIB_DIR)
LIBS := -llist -ltimer -lpthread

all: $(LIB)

-include $(DEPS)

$(LIB): $(OBJS)
	ar $(ARFLAGS) rcs $(LIB) $(OBJS)

%.o: %.cpp
	$(CC) $(CCFLAGS) $(INCFLAGS) $(SD_LIB) $(LIBS) -c $< -o $@

%.d: %.cpp
	@$(CC) $(DEPFLAGS) $(INCFLAGS) $< > $@ ;\
	sed 's/\([0-9A-Za-z]*\)\.o/$(subst /,\/,$(dir $<))\1.o $(subst /,\/,$(dir $<))\1.d/g' $@ > $@.$$$$ ; \
	cp $@.$$$$ $@ ; \
	rm -f $@.$$$$

clean:
	rm -rf $(LIB) $(OBJS) $(DEPS)

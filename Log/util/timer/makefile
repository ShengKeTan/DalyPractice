CC = g++

DEPFLAGS = -MM -MG -P -w
ARFLAGS = 
CCFLAGS = -Wall -finline-functions -O2 -D__LINUX__ -g
INCFLAGS = -I../Include

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))
DEPS := $(patsubst %.cpp, %.d, $(SRCS))
LIBS := -lpthread
LIB := ../libtimer.a

all: $(LIB)

-include $(DEPS)

$(LIB): $(OBJS)
	ar $(ARFLAGS) rcs $(LIB) $(OBJS)

%.o: %.cpp
	$(CC) $(CCFLAGS) $(INCFLAGS) $(LIBS) -c $< -o $@

%.d: %.cpp
	@$(CC) $(DEPFLAGS) $(INCFLAGS) $< > $@ ;\
	sed 's/\([0-9A-Za-z]*\)\.o/$(subst /,\/,$(dir $<))\1.o $(subst /,\/,$(dir $<))\1.d/g' $@ > $@.$$$$ ; \
	cp $@.$$$$ $@ ; \
	rm -f $@.$$$$

clean:
	rm -rf $(LIB) $(OBJS) $(DEPS)

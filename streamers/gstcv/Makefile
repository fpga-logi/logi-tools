CC       = g++
PROG     = gst_appsrc
SRCS     = gst_appsrc.cpp options.cpp gstcv.cpp
USE_LIBS = gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 opencv
OBJS     = $(SRCS:.cpp=.o)
CFLAGS = -Wall -Werror `pkg-config --cflags $(USE_LIBS)`
LDLIBS   = -lpthread `pkg-config --libs $(USE_LIBS)`
INCLUDE  =
DEPFILE  = deps.mak

.PHONY: clean

$(PROG): $(OBJS)
		$(CC) $^ $(LDLIBS) -o $@

# How to make the object files:
.cpp.o:
		$(CC) $(CFLAGS) -c $<

deps:
		$(CC) -MM $(CCFLAGS) $(SRCS) > $(DEPFILE)

# Cleaning target (only works with fileutils):
clean:
		$(RM) $(OBJS) $(PROG)

-include $(DEPFILE)


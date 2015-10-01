.PHONY: all clean

ifeq ($(CFLAGS),)
DEPENDENCIES = glib-2.0 gobject-2.0 gupnp-1.0
CC = gcc
CFLAGS = -pipe -O3 -std=c11 -Wall -Wsign-compare -Wuninitialized -Winit-self -Wunused-but-set-parameter -Wunused-function -Wundef -Wshadow -Wpointer-arith -Wfloat-equal -Wwrite-strings -Wstrict-aliasing -Wdouble-promotion -Wunused -Waggregate-return -Wbad-function-cast -Wnested-externs -Wmissing-prototypes -Wstrict-prototypes
CFLAGS += `pkg-config --cflags-only-other $(DEPENDENCIES)`
CPPFLAGS += `pkg-config --cflags-only-I $(DEPENDENCIES)`
LDFLAGS += `pkg-config --libs-only-L $(DEPENDENCIES)`
LDLIBS += `pkg-config --libs-only-l --libs-only-other $(DEPENDENCIES)`
endif

all: flagpole

clean:
	rm -f *.o
	rm -f flagpole

flagpole: flagpole.o

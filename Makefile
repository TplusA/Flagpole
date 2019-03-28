#
# Copyright (C) 2015, 2017, 2019  T+A elektroakustik GmbH & Co. KG
#
# This file is part of Flagpole.
#
# Flagpole is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 3 as
# published by the Free Software Foundation.
#
# Flagpole is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Flagpole.  If not, see <http://www.gnu.org/licenses/>.
#

.PHONY: all clean

ifeq ($(CFLAGS),)
CC = gcc
CFLAGS = -pipe -O3 -std=c11 -Wall -Wsign-compare -Wuninitialized -Winit-self -Wunused-but-set-parameter -Wunused-function -Wundef -Wshadow -Wpointer-arith -Wfloat-equal -Wwrite-strings -Wstrict-aliasing -Wdouble-promotion -Wunused -Waggregate-return -Wbad-function-cast -Wnested-externs -Wmissing-prototypes -Wstrict-prototypes
endif

DEPENDENCIES = glib-2.0 gobject-2.0 gupnp-1.0 libsoup-2.4
CFLAGS += `pkg-config --cflags-only-other $(DEPENDENCIES)`
CPPFLAGS += `pkg-config --cflags-only-I $(DEPENDENCIES)`
LDFLAGS += `pkg-config --libs-only-L $(DEPENDENCIES)`
LDLIBS += `pkg-config --libs-only-l --libs-only-other $(DEPENDENCIES)`

all: flagpole

clean:
	rm -f *.o
	rm -f flagpole

flagpole: flagpole.o

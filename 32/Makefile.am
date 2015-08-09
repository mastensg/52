AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++14

bin_PROGRAMS = ordinal

ordinal_CFLAGS = $(GLFW_CFLAGS)
ordinal_CPPFLAGS = -I/usr/include/freetype2
ordinal_LDADD = $(GLFW_LIBS) -lGL -lfreetype
ordinal_LDFLAGS = -pthread
ordinal_SOURCES = main.cc \
		   gl_core_3_3.c gl_core_3_3.h \
		   res.s res.h \
		   text.v.glsl text.f.glsl

res.o: text.v.glsl text.f.glsl

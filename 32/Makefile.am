AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++14

bin_PROGRAMS = natural

natural_CFLAGS = $(GLFW_CFLAGS)
natural_CPPFLAGS = -I/usr/include/freetype2
natural_LDADD = $(GLFW_LIBS) -lGL -lfreetype
natural_LDFLAGS = -pthread
natural_SOURCES = main.cc \
		   gl_core_3_3.c gl_core_3_3.h \
		   res.s res.h \
		   text.v.glsl text.f.glsl

res.o: text.v.glsl text.f.glsl

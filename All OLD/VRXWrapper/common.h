#ifndef COMMON_H
#define COMMON_H

#include "config.h"

#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include "debug.h"

#include "Vector.h"
#include "Matrix.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <GL/gl.h>
#include <GL/glx.h>

extern const char * g_program_name;

void usage();

//#define USE_PANELS
//#include "panels.h"

#endif//COMMON_H


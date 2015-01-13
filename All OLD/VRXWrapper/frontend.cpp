
#include "common.h"
#include "backend.h"
#include "frontend.h"

#include <X11/extensions/XTest.h>

#include <string.h>
#include <stdlib.h>

Frontend g_frontend;


bool FrontendCreateWindow(bool fullscreen)
{
	Display * dpy = g_frontend._dpy;
	Window root = g_frontend._root;
	XVisualInfo * vis = g_frontend._vis;

	if (g_frontend._w != None)
	{
		FrontendReleaseGrab();
		XDestroyWindow(dpy, g_frontend._w);
	}

	int width;
	int height;
	if (fullscreen)
	{
		width = DisplayWidth(dpy, DefaultScreen(dpy));
		height = DisplayHeight(dpy, DefaultScreen(dpy));
	}
	else
	{
		width = 800;
		height = 600;
	}

	XSetWindowAttributes attr;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap( dpy, root, vis->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask;
	attr.event_mask |= PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
	attr.override_redirect = fullscreen? True : False;
	unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

	Window w = XCreateWindow( dpy, root, 0, 0, width, height,
				0, vis->depth, InputOutput, vis->visual, mask, &attr );

	const char * name = "vrx (ctrl+shift grabs/releases mouse and keyboard)";
	XSizeHints sizehints;
	sizehints.x = 0;
	sizehints.y = 0;
	sizehints.width  = width;
	sizehints.height = height;
	sizehints.flags = USSize | USPosition;
	XSetNormalHints(dpy, w, &sizehints);
	XSetStandardProperties(dpy, w, name, name, None, (char **)NULL, 0, &sizehints);

	//XMapWindow(dpy, w);
	XMapRaised(dpy, w);

	glXMakeCurrent(dpy, w, g_frontend._ctx);

	g_frontend._w = w;
	g_frontend._width = width;
	g_frontend._height = height;

	return true;
}

bool FrontendInitialize()
{
	Display * dpy = XOpenDisplay(NULL);
	if (!dpy)
	{
		fprintf(stderr, "%s: Couldn't open display\n", g_program_name);
		return false;
	}

	Window root = DefaultRootWindow( dpy );

	int scr = DefaultScreen( dpy );
	int attrib[] = { GLX_RGBA,
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			GLX_DOUBLEBUFFER,
			GLX_DEPTH_SIZE, 1,
			None };
	XVisualInfo * vis = glXChooseVisual( dpy, scr, attrib );
	if (!vis)
	{
		fprintf(stderr, "%s: Couldn't get an RGB, Double-buffered visual\n", g_program_name);
		return false;
	}

	// create an invisible cursor
	Colormap cmap = DefaultColormap(dpy, scr);
	XColor black;
	memset(&black, 0, sizeof(XColor));
	char data[] = {0, 0, 0, 0, 0, 0, 0, 0};
	Pixmap pm = XCreateBitmapFromData(dpy, root, data, 8, 8);
	Cursor cursor = XCreatePixmapCursor(dpy, pm, pm, &black, &black, 0, 0);

	GLXContext ctx = glXCreateContext( dpy, vis, NULL, True );
	if (!ctx)
	{
		fprintf(stderr, "%s: glXCreateContext failed\n", g_program_name);
		return false;
	}

	g_frontend._dpy = dpy;
	g_frontend._root = root;
	g_frontend._vis = vis;
	g_frontend._w = None;
	g_frontend._ctx = ctx;
	g_frontend._width = 1;
	g_frontend._height = 1;
	g_frontend._fullscreen = false;
	g_frontend._cursor = cursor;
	g_frontend._grab = false;
	g_frontend._ignore_enter = false;

	FrontendCreateWindow(false);

	// initialize opengl
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearColor(1.f, 1.f, 1.f, 1.f);
	glClearColor(96.f / 255.f, 118.f / 255.f, 98.f / 255.f, 1.f);

	return true;
}

void FrontendToggleGrab()
{
	Display * dpy = g_frontend._dpy;
	Window w = g_frontend._w;
	bool grab = !g_frontend._grab;

	if (grab)
	{
		XGrabKeyboard(dpy, w, True, GrabModeAsync, GrabModeAsync, CurrentTime);
		XDefineCursor(dpy, w, g_frontend._cursor);
		XWarpPointer(dpy, None, w, 0, 0, 0, 0, g_frontend._width/2, g_frontend._height/2);
	}
	else
	{
		XUngrabKeyboard(dpy, CurrentTime);
		XUndefineCursor(dpy, w);
	}

	g_frontend._grab = grab;
}

void FrontendReleaseGrab()
{
	if (!g_frontend._grab)
	{
		return;
	}

	Display * dpy = g_frontend._dpy;
	XUngrabKeyboard(dpy, CurrentTime);
	XUndefineCursor(dpy, g_frontend._w);

	g_frontend._grab = false;
}

void FrontendToggleFullscreen()
{
	Display * dpy = g_frontend._dpy;
	Window w = g_frontend._w;
	bool fullscreen = !g_frontend._fullscreen;

	FrontendCreateWindow(fullscreen);

	if (g_frontend._grab != fullscreen)
		FrontendToggleGrab();

	g_frontend._fullscreen = fullscreen;
}

void FrontendUpdate()
{
	Display * dpy = g_frontend._dpy;
	Window w = g_frontend._w;
	KeySym key;
	XEvent event;
	bool moved = false;
	int moved_x = 0;
	int moved_y = 0;

	while (XPending(dpy) > 0)
	{
		XNextEvent(dpy, &event);
		switch (event.type)
		{
		case Expose:
			break;
		case ConfigureNotify:
			g_frontend._width = event.xconfigure.width;
			g_frontend._height = event.xconfigure.height;
			break;
		case KeyPress:
			key = XKeycodeToKeysym(g_frontend._dpy, event.xkey.keycode, 0);
			//printf("key = %08x  %08x\n", (unsigned int)key, (unsigned int)event.xkey.state);
			if ((key == XK_Shift_L || key == XK_Shift_R) && event.xkey.state & ControlMask)
			{
				FrontendToggleGrab();
				break;
			}
			else if (key == XK_Return && event.xkey.state & Mod1Mask)
			{
				FrontendToggleFullscreen();
				g_frontend._ignore_enter = true;
				break;
			}
			else if (key == XK_Escape && event.xkey.state & Mod4Mask)
			{
				exit(0);
			}

			XTestFakeKeyEvent(g_backend._dpy, XKeysymToKeycode(g_backend._dpy, key), True, 0);
			XFlush(g_backend._dpy);
			break;
		case KeyRelease:
			key = XKeycodeToKeysym(g_frontend._dpy, event.xkey.keycode, 0);
			if (key == XK_Return && g_frontend._ignore_enter)
			{
				g_frontend._ignore_enter = false;
				break;
			}
			XTestFakeKeyEvent(g_backend._dpy, XKeysymToKeycode(g_backend._dpy, key), False, 0);
			XFlush(g_backend._dpy);
			break;
		case MotionNotify:
			if (g_frontend._grab)
			{
				moved_x += event.xmotion.x - g_frontend._width/2;
				moved_y += event.xmotion.y - g_frontend._height/2;
				if (moved_x != 0 || moved_y != 0)
				{
					moved = true;
				}
			}
			break;
		case ButtonPress:
			XTestFakeButtonEvent(g_backend._dpy, event.xbutton.button, True, 0);
			break;
		case ButtonRelease:
			XTestFakeButtonEvent(g_backend._dpy, event.xbutton.button, False, 0);
			break;
		}
	}
	if (moved)
	{
		g_backend._mouse_x += moved_x;
		g_backend._mouse_y += moved_y;
		if (g_backend._mouse_x < 0) g_backend._mouse_x = 0;
		else if (g_backend._mouse_x >= g_backend._width) g_backend._mouse_x = g_backend._width - 1;
		if (g_backend._mouse_y < 0) g_backend._mouse_y = 0;
		else if (g_backend._mouse_y >= g_backend._height) g_backend._mouse_y = g_backend._height - 1;
		XWarpPointer(g_backend._dpy, None, g_backend._root, 0, 0, 0, 0, g_backend._mouse_x, g_backend._mouse_y);
		XFlush(g_backend._dpy);

		XWarpPointer(dpy, None, w, 0, 0, 0, 0, g_frontend._width/2, g_frontend._height/2);
	}
}







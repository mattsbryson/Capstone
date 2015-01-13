
// use with: Xephyr :9 -screen 5760x1080
// then: vrx -display :9

#include "common.h"

/*#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/XTest.h>

#include <GL/gl.h>
#include <GL/glx.h>

#include <unistd.h>
#include <stdio.h>*/
#include <stdlib.h>
#include <string.h>
/*#include <malloc.h>

#include <sys/time.h>

#include <math.h>

#include "Vector.h"
#include "Matrix.h"*/

#include "backend.h"
#include "frontend.h"
#include "bridge.h"

#ifdef ENABLE_OPENNI
#include "openni.h"
#endif

#ifdef ENABLE_HYDRA
#include "Hydra.h"
#endif

const char * g_program_name;


/*struct Frontend
{
	Display * _dpy;
	Window _root;
	XVisualInfo * _vis;
	Window _w;
	GLXContext _ctx;
	int _width;
	int _height;
	bool _fullscreen;
	Cursor _cursor;
	bool _grab;
};

Frontend g_frontend;

bool FrontendInitialize();
void FrontendUpdate();


struct Backend
{
	Display * _dpy;
	Window _root;
	int _damage;
	int _width;
	int _height;
	int _mouse_x;
	int _mouse_y;
	unsigned char * _buffer;
	GLuint _texture;
};

Backend g_backend;

bool BackendInitialize(const char * display_name);
void BackendUpdate();
bool BackendUpdateTexture(int x, int y, int width, int height);



Matrix g_camera(Matrix::identity);
int g_camera_mode;
float g_camera_yaw;
float g_camera_pitch;
Vector3 g_offset;

void CameraUpdate()
{
#ifdef ENABLE_OPENNI
	if (g_user_enabled)
	{
		g_camera.translation()._x = g_user_joints[SK_HEAD][0];
		g_camera.translation()._y = g_user_joints[SK_HEAD][1];
		g_camera.translation()._z = g_user_joints[SK_HEAD][2];
		static bool user_enabled = false;
		if (!user_enabled)
		{
			g_offset = g_camera.translation();
			user_enabled = true;
		}
		return;
	}
#endif

	float pitch, yaw;
	switch (g_camera_mode)
	{
	case 0:
	default:
		yaw = g_camera_yaw * (g_backend._mouse_x / (float)g_backend._width - 0.5f);
		pitch = g_camera_pitch * (g_backend._mouse_y / (float)g_backend._height - 0.5f);
		g_camera.back()._x = -sinf(yaw) * cos(pitch);
		g_camera.back()._y = sinf(pitch);
		g_camera.back()._z = cosf(yaw) * cos(pitch);
		g_camera.up()._x = sin(yaw) * sinf(pitch);
		g_camera.up()._y = cosf(pitch);
		g_camera.up()._z = -cos(yaw) * sinf(pitch);
		g_camera.right()._x = cosf(yaw);
		g_camera.right()._y = 0.f;
		g_camera.right()._z = sinf(yaw);
		break;
	}
}

struct View
{
	// world
	float _pos_x;
	float _pos_y;
	float _pos_z;
	float _yaw;
	float _pitch;
	float _width;
	float _height;
	// window
	float _left;
	float _top;
	float _right;
	float _bottom;
	// draw
	float _vertices[4][4];
};

#define MAX_VIEWS 32
int g_view_count;
View g_view[MAX_VIEWS];

void SetViewMode(int mode)
{
	float total_width = g_backend._width;
	float total_height = g_backend._height;

	switch (mode)
	{
	case 0:
		g_view_count = 1;
		g_view[0]._pos_x = 0;
		g_view[0]._pos_y = 0;
		g_view[0]._pos_z = 57;
		g_view[0]._yaw = 0;
		g_view[0]._pitch = 0;
		g_view[0]._width = 47.22f;
		g_view[0]._height = 26.56f;
		g_view[0]._left = 0;
		g_view[0]._top = 0;
		g_view[0]._right = total_width;
		g_view[0]._bottom = total_height;
		g_camera_yaw = 0.2f;
		g_camera_pitch = 0.1f;
		break;
	default:
		if (mode & 1)
		{
			int screens = mode >> 1;
		}
		else
		{
			int screens = mode >> 1;
			float arc = screens * 45.f;
			float arc_offset = arc * -0.5f + 45.f * 0.5f;
			float yaw = arc_offset * 3.141593f / 180.f;
			float aspect = total_height * screens / total_width;
			float width = 47.f;
			float height = width * aspect;
			g_camera_yaw = 0.8f;
			g_camera_pitch = 0.1f;
			g_view_count = screens;
			for (int v = 0; v < screens; v++)
			{
				struct View & view = g_view[v];
				view._pos_x = 57 * sinf(-yaw);
				view._pos_y = 0;
				view._pos_z = 57 * cosf(-yaw);
				view._yaw = yaw * 180.f / 3.141593f;
				view._pitch = 0;
				view._width = width;//47.22f;
				view._height = height;//26.56f;
				view._left = total_width * v / screens;
				view._top = 0;
				view._right = total_width * (v + 1) / screens;
				view._bottom = total_height;
				yaw += 45 * 3.141593f / 180.f;
			}
		}
	}

	for (int v = 0; v < g_view_count; v++)
	{
		View &view = g_view[v];
		view._vertices[0][0] = -view._width / 2;
		view._vertices[0][1] = view._height / 2;
		view._vertices[0][2] = view._left / total_width;
		view._vertices[0][3] = view._top / total_height;
		view._vertices[1][0] = view._width / 2;
		view._vertices[1][1] = view._height / 2;
		view._vertices[1][2] = view._right / total_width;
		view._vertices[1][3] = view._top / total_height;
		view._vertices[2][0] = view._width / 2;
		view._vertices[2][1] = -view._height / 2;
		view._vertices[2][2] = view._right / total_width;
		view._vertices[2][3] = view._bottom / total_height;
		view._vertices[3][0] = -view._width / 2;
		view._vertices[3][1] = -view._height / 2;
		view._vertices[3][2] = view._left / total_width;
		view._vertices[3][3] = view._bottom / total_height;
	}
}



void SetupProjection2D(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.f, width, height, 0.f, -100.f, 100.f);

	glMatrixMode(GL_MODELVIEW);
}

void SetupProjection3D(int width, int height)
{
	if (height==0)
	{
		height=1;
	}
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float near = 0.1f;
	float yscale = height / (float)width;
	glFrustum(-near, near, -near * yscale, near * yscale, near, 1000.f);

	glMatrixMode(GL_MODELVIEW);
}

void DrawCursor()
{
	float verts[] =
	{
		 0.0f, 0.0f,

		-0.5f,-1.5f,
		-0.2f,-1.2f,
		-0.2f,-2.0f,

		 0.2f,-2.0f,
		 0.2f,-1.2f,
		 0.5f,-1.5f,
	};
	float verts2[] =
	{
		 0.0f, 0.3f,
		-0.2f, 0.2f,

		-0.8f,-1.3f,
		-0.7f,-1.7f,
		-0.5f,-1.7f,

		-0.4f,-1.5f,
		-0.4f,-2.3f,
		 0.4f,-2.3f,
		 0.4f,-1.5f,

		 0.5f,-1.7f,
		 0.7f,-1.7f,
		 0.8f,-1.3f,

		 0.2f, 0.2f,
	};

	glDisable(GL_TEXTURE_2D);
	glVertexPointer(2, GL_FLOAT, 0, verts );

	glColor4f(1.f, 1.f, 1.f, 1.f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 7);

	glTranslatef(0.f, 0.f, -0.01f);
	glVertexPointer(2, GL_FLOAT, 0, verts2 );
	glColor4f(0.f, 0.f, 0.f, 1.f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 13);
}

void DrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetupProjection3D(g_frontend._width, g_frontend._height);

	glLoadIdentity();

	// apply the camera
	Matrix inv = g_camera;
	inv.FastInverse();
	glMultMatrixf(inv._m);

	float mx = g_backend._mouse_x;
	float my = g_backend._mouse_y;

#ifdef ENABLE_OPENNI
	if (g_user_enabled)
#endif
	{
	glPushMatrix();
	glTranslatef(g_offset._x, g_offset._y, g_offset._z);

	for (int v = 0; v < g_view_count; v++)
	{
		View &view = g_view[v];

		glPushMatrix();

		// position virtual view
		glTranslatef( -view._pos_x, -view._pos_y, -view._pos_z);
		glRotatef( -view._yaw, 0, 1, 0);
		glRotatef( -view._pitch, 1, 0, 0);

		glBindTexture(GL_TEXTURE_2D, g_backend._texture);
		glEnable(GL_TEXTURE_2D);
		glClientActiveTexture(GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 4 * 4, view._vertices[0] );
		glTexCoordPointer(2, GL_FLOAT, 4 * 4, view._vertices[0] + 2);
		glColor4f(1.f, 1.f, 1.f, 1.f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// draw virtual cursor
		if (mx >= view._left && mx < view._right && my >= view._top && my < view._bottom)
		{
			float x = view._width * ((mx - view._left) / (float)(view._right - view._left) - 0.5f);
			float y = view._height * ((my - view._top) / (float)(view._bottom - view._top) - 0.5f);
			glTranslatef(x, -y, 0.1f);
			glScalef(0.5f, 0.5f, 1.f);
			DrawCursor();
		}

		glPopMatrix();
	}

	glPopMatrix();
	}

#ifdef ENABLE_OPENNI
	glPushMatrix();

	//glTranslatef(0.f, 0.f,-40.f);
	//glTranslatef(0.f, 0.f,-80.f);
	//glRotatef(27.f, 0.f, 1.f, 0.f);
	//glRotatef(90.f, 0.f, 1.f, 0.f);
	//static float angle = 0.f;
	//angle += 1.f;
	//glRotatef(angle, 0.f, 1.f, 0.f);
	if (!g_user_enabled)
	glRotatef(180.f, 0.f, 1.f, 0.f);
	//glScalef(1.f, 1.f, 2.f);
	//glTranslatef(0.f, 0.f, 80.f);

	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 4 * (3+4), g_depth_verts );
	glColorPointer(4, GL_FLOAT, 4 * (3+4), g_depth_verts + 3);
	glColor4f(1.f, 1.f, 1.f, 1.f);
	glDrawArrays(GL_POINTS, 0, g_depth_verts_count);
	glDisableClientState(GL_COLOR_ARRAY);


	glColor4f(1.f, 0.f, 0.f, 1.f);
	float verts[9][3];

	verts[0][0] = g_user_joints[SK_HEAD][0];
	verts[0][1] = g_user_joints[SK_HEAD][1];
	verts[0][2] = g_user_joints[SK_HEAD][2];
	verts[1][0] = g_user_joints[SK_NECK][0];
	verts[1][1] = g_user_joints[SK_NECK][1];
	verts[1][2] = g_user_joints[SK_NECK][2];
	verts[2][0] = g_user_joints[SK_TORSO][0];
	verts[2][1] = g_user_joints[SK_TORSO][1];
	verts[2][2] = g_user_joints[SK_TORSO][2];
	glVertexPointer(3, GL_FLOAT, 0, verts);
	glDrawArrays(GL_LINE_STRIP, 0, 3);

	verts[0][0] = g_user_joints[SK_LEFT_SHOULDER][0];
	verts[0][1] = g_user_joints[SK_LEFT_SHOULDER][1];
	verts[0][2] = g_user_joints[SK_LEFT_SHOULDER][2];
	verts[1][0] = g_user_joints[SK_LEFT_ELBOW][0];
	verts[1][1] = g_user_joints[SK_LEFT_ELBOW][1];
	verts[1][2] = g_user_joints[SK_LEFT_ELBOW][2];
	verts[2][0] = g_user_joints[SK_LEFT_HAND][0];
	verts[2][1] = g_user_joints[SK_LEFT_HAND][1];
	verts[2][2] = g_user_joints[SK_LEFT_HAND][2];
	verts[3][0] = g_user_joints[SK_LEFT_WRIST][0];
	verts[3][1] = g_user_joints[SK_LEFT_WRIST][1];
	verts[3][2] = g_user_joints[SK_LEFT_WRIST][2];
	glVertexPointer(3, GL_FLOAT, 0, verts);
	glDrawArrays(GL_LINE_STRIP, 0, 4);

	verts[0][0] = g_user_joints[SK_RIGHT_SHOULDER][0];
	verts[0][1] = g_user_joints[SK_RIGHT_SHOULDER][1];
	verts[0][2] = g_user_joints[SK_RIGHT_SHOULDER][2];
	verts[1][0] = g_user_joints[SK_RIGHT_ELBOW][0];
	verts[1][1] = g_user_joints[SK_RIGHT_ELBOW][1];
	verts[1][2] = g_user_joints[SK_RIGHT_ELBOW][2];
	verts[2][0] = g_user_joints[SK_RIGHT_HAND][0];
	verts[2][1] = g_user_joints[SK_RIGHT_HAND][1];
	verts[2][2] = g_user_joints[SK_RIGHT_HAND][2];
	verts[3][0] = g_user_joints[SK_RIGHT_WRIST][0];
	verts[3][1] = g_user_joints[SK_RIGHT_WRIST][1];
	verts[3][2] = g_user_joints[SK_RIGHT_WRIST][2];
	glVertexPointer(3, GL_FLOAT, 0, verts);
	glDrawArrays(GL_LINE_STRIP, 0, 4);

	glVertexPointer(3, GL_FLOAT, 0, verts);
	glDrawArrays(GL_LINES, 0, 6);

	glPopMatrix();
#endif

	glFinish();

	glXSwapBuffers(g_frontend._dpy, g_frontend._w);
}

*/

void usage()
{
	fprintf (stderr, "usage: %s [-display host:dpy]\n", g_program_name);
}

int OnXErrorEvent(Display * dpy, XErrorEvent * error)
{
	printf("had an error\n");
}

/*

bool FrontendInitialize()
{
	const char * name = "vrx (ctrl+shift grabs/releases mouse and keyboard)";

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

	int width = 800;
	int height = 600;
	//int width = DisplayWidth(dpy, DefaultScreen(dpy));
	//int height = DisplayHeight(dpy, DefaultScreen(dpy));

	XSetWindowAttributes attr;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap( dpy, root, vis->visual, AllocNone);
	attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask;
	attr.event_mask |= PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
	attr.override_redirect = False;//True;
	unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

	Window w = XCreateWindow( dpy, root, 0, 0, width, height,
				0, vis->depth, InputOutput, vis->visual, mask, &attr );

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

	glXMakeCurrent(dpy, w, ctx);

	// initialize opengl
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearColor(1.f, 1.f, 1.f, 1.f);
	glClearColor(96.f / 255.f, 118.f / 255.f, 98.f / 255.f, 1.f);

	g_frontend._dpy = dpy;
	g_frontend._root = root;
	g_frontend._vis = vis;
	g_frontend._w = w;
	g_frontend._ctx = ctx;
	g_frontend._width = width;
	g_frontend._height = height;
	g_frontend._fullscreen = false;
	g_frontend._cursor = cursor;
	g_frontend._grab = false;

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

void FrontendToggleFullscreen()
{
	Display * dpy = g_frontend._dpy;
	Window w = g_frontend._w;
	bool fullscreen = !g_frontend._fullscreen;

	int width = fullscreen? DisplayWidth(dpy, DefaultScreen(dpy)) : 800;
	int height = fullscreen? DisplayHeight(dpy, DefaultScreen(dpy)) : 600;

	g_frontend._width = width;
	g_frontend._height = height;
	g_frontend._fullscreen = fullscreen;

	if (g_frontend._grab != fullscreen)
		FrontendToggleGrab();

	XSetWindowAttributes attr;
	attr.override_redirect = fullscreen? True : False;
	XChangeWindowAttributes(dpy, w, CWOverrideRedirect, &attr);

	XMoveResizeWindow(dpy, w, 0, 0, width, height);
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



class GrabServer
{
public:
	Display * _dpy;
	GrabServer(Display * dpy)
	{
		_dpy = dpy;
		XGrabServer(dpy);
	}
	~GrabServer()
	{
		XUngrabServer(_dpy);
	}
};



bool BackendInitialize(const char * display_name)
{
	Display * dpy = XOpenDisplay(display_name);
	if (!dpy)
	{
		fprintf (stderr, "%s:  unable to open display '%s'\n",
					g_program_name, XDisplayName (display_name));
		usage();
		return false;
	}

	Window root = DefaultRootWindow(dpy);

	int damage_error, damage;
	if (!XDamageQueryExtension (dpy, &damage, &damage_error))
	{
		fprintf (stderr, "%s: Backend does not have damage extension\n", g_program_name);
		return false;
	}
	XDamageCreate (dpy, root, XDamageReportRawRectangles);

	XWindowAttributes attrib;
	XGetWindowAttributes(dpy, root, &attrib);
	if (attrib.width ==0 || attrib.height == 0)
	{
		fprintf(stderr, "%s: Bad root with %d x %d\n", 
			g_program_name, attrib.width, attrib.height);
		return false;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, attrib.width, attrib.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	unsigned char * buffer = (unsigned char *)malloc(attrib.width * attrib.height * 4);
	if (!buffer)
	{
		fprintf(stderr, "%s: Failed to allocate buffer %d x %d\n",
			g_program_name, attrib.width, attrib.height);
		return false;
	}

	g_backend._dpy = dpy;
	g_backend._root = root;
	g_backend._damage = damage;
	g_backend._width = attrib.width;
	g_backend._height = attrib.height;
	g_backend._texture = texture;
	g_backend._mouse_x = attrib.width/2;
	g_backend._mouse_y = attrib.height/2;
	g_backend._buffer = buffer;

	BackendUpdateTexture(0, 0, attrib.width, attrib.height);
}

void BackendUpdate()
{
	Display * dpy = g_backend._dpy;
	if (XPending(dpy) > 0)
	{
		Window root = g_backend._root;
		int damage = g_backend._damage;
		XEvent event;
		GrabServer grab(dpy);
		while (XPending(dpy) > 0)
		{
			XNextEvent(dpy, &event);
			switch (event.type)
			{
			default:
				if (event.type == damage + XDamageNotify)
				{
					XDamageNotifyEvent *de = (XDamageNotifyEvent *) &event;
					//printf ("damage %08x %d %d %d %d, %d %d %d %d\n", (int)de->drawable,
					//		de->area.x, de->area.y, de->area.width, de->area.height,
					//		de->geometry.x, de->geometry.y, de->geometry.width, de->geometry.height);
					if (de->drawable == root)
					{
						BackendUpdateTexture(de->area.x, de->area.y, de->area.width, de->area.height);
					}
				}
			}
		}
	}
}

bool BackendUpdateTexture(int x, int y, int width, int height)
{
	Display * dpy = g_backend._dpy;
	Window w = g_backend._root;

	// TODO: use shared memory to reduce copy operations
	XImage *image = XGetImage (dpy, w, x, y, width, height, AllPlanes, ZPixmap);
	if (!image)
	{
		printf(" unabled to get the image\n");
		return false;
	}

	// TODO: use shader to swizzle pixels instead of doing it here
    int bytes_per_pixel = image->bits_per_pixel / 8;
    unsigned char * texture = g_backend._buffer;
    unsigned char * dst = texture;
    for ( int py = 0; py < height; py++)
    {
        unsigned char * src = ((unsigned char*)image->data) + (image->bytes_per_line * py);
        for ( int px = 0; px < width; px++)
        {
            unsigned char t;
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            if (bytes_per_pixel > 3)
                dst[3] = 255;
            src += bytes_per_pixel;
            dst += bytes_per_pixel;
        }
    }

	glBindTexture(GL_TEXTURE_2D, g_backend._texture);
    if (bytes_per_pixel == 4)
    {
        glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture);
    }
    else if (bytes_per_pixel == 3)
    {
        glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, texture);
    }
    else
    {
        printf("depth %d\n", image->depth);
    }

    XDestroyImage(image);

	return true;
}*/



int main(int argc, char **argv)
{
	int i;

	g_program_name = argv[0];

	printf("version %d.%d\n", VRX_VERSION_MAJOR, VRX_VERSION_MINOR);

	Display * dpy;
	const char * display_name = NULL;
	for (i = 1; i < argc; i++)
	{
		char *arg = argv[i];

		if (!strcmp (arg, "-display") || !strcmp (arg, "-d"))
		{
			if (++i >= argc)
			{
				usage();
				exit(0);
			}

			display_name = argv[i];
			continue;
		}
	}

	if (!display_name)
	{
		usage();
		printf(" attempting default of :9\n");
		display_name = ":9";
	}

	XSetErrorHandler(OnXErrorEvent);

	if (!FrontendInitialize())
	{
		exit(0);
	}

	if (!BackendInitialize(display_name))
	{
		exit(0);
	}

#ifdef ENABLE_OPENNI
	if (!OpenNIInit())
	{
		exit(0);
	}
#endif

#ifdef ENABLE_HYDRA
	if (HydraInitialize())
	{
		atexit(HydraExit);
		printf(" Hydra initialized\n");
	}
	else
	{
		fprintf (stderr, "%s: warning: Hydra failed to initialize.\n", g_program_name);
	}
#endif

	//SetViewMode(0);
	SetViewMode(3 * 2);

	while (1)
	{
		BackendUpdate();
		CameraUpdate();
#ifdef ENABLE_OPENNI
		OpenNIUpdate();
#endif
		FrontendUpdate();
		DrawScreen();
	}

	return 1;
}




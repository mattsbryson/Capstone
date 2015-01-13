#ifndef FRONTEND_H
#define FRONTEND_H

struct Frontend
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
	bool _ignore_enter;
};

extern Frontend g_frontend;

bool FrontendInitialize();
void FrontendUpdate();

void FrontendReleaseGrab();

#endif//FRONTEND_H


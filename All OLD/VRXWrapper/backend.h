#ifndef BACKEND_H
#define BACKEND_H

struct Backend
{
	Display * _dpy;
	Window _root;
	int _damage;
	int _width;
	int _height;
	int _mouse_x;
	int _mouse_y;
	int _buffer_size;
	unsigned char * _buffer;
#ifndef USE_PANELS
	GLuint _texture;
#endif
	uint32_t _mask;
	uint32_t _shift;
};

extern Backend g_backend;

bool BackendInitialize(const char * display_name);
void BackendUpdate();
//bool BackendUpdateTexture(int x, int y, int width, int height);
bool BackendUpdateTexture(Window w, GLuint texture, int x, int y, int width, int height);

#endif//BACKEND_H


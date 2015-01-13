#ifndef PANELS_H
#define PANELS_H

struct PanelWindow;

struct Panel
{
	Panel * _next;
	uint32_t _id;

	Matrix _matrix;

	int _width;
	int _height;

	uint32_t _ref;

	uint32_t _count;
	PanelWindow *_windows[1000];
};

struct PanelWindow
{
	PanelWindow * _next;
	Window _w;

	int _x;
	int _y;
	int _width;
	int _height;

	int _order;

	GLuint _texture;
	bool _delete;
	bool _override;

	Panel * _panel;
};

void PanelSetStart(const Matrix &m, int width, int height);

void PanelUpdateBegin();
void PanelUpdate(Window w);
void PanelDamage(Window w, int x, int y, int width, int height);
void PanelUpdateEnd();

void PanelDraw();

void PanelLog();

#endif//PANELS_H


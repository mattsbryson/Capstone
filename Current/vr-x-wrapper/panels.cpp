#include "common.h"

#include "backend.h"
#include "panels.h"

#include <X11/Xatom.h>
#include <X11/extensions/Xdamage.h>

#define TABLE_SIZE 1024
#define TABLE_MASK 1023

Matrix g_panel_start(Matrix::identity);
int g_panel_start_width = 1280;
int g_panel_start_height = 720;
float g_panel_width = 47.22f;
float g_panel_height = -26.56f;

Panel * g_panels[TABLE_SIZE];

PanelWindow * g_windows[TABLE_SIZE];

int g_draw_panel_count;
Panel * g_draw_panel[1024];

int g_window_order;

Panel * g_panel_focus;
Window g_input_focus;

Window PanelGetSubWindow(Window w)
{
	unsigned int nchildren;
	Window *children, dummy;
	if (XQueryTree(g_backend._dpy, w, &dummy, &dummy, &children, &nchildren))
	{
		if (nchildren > 0)
		{
			if ((w & g_backend._mask) != (children[nchildren-1] & g_backend._mask))
			{
				w = children[nchildren-1];
			}
		}
		if (children)
		{
			XFree ((char *)children);
		}
	}
	return w;
}

uint32_t PanelGetId(Window w)
{
	w = PanelGetSubWindow(w);

	return (w & g_backend._mask) >> g_backend._shift;
}

Panel * PanelReference(Window w)
{
	uint32_t id = PanelGetId(w);
	uint32_t index = id & TABLE_MASK;
	Panel * panel = g_panels[index];
	while (panel && panel->_id != id)
	{
		panel = panel->_next;
	}
	if (!panel)
	{
		panel = new Panel;
		panel->_next = g_panels[index];
		panel->_id = id;
		panel->_matrix = g_panel_start;
		panel->_matrix.translation() += Vector3(0,0,-40);
		panel->_width = g_panel_start_width;
		panel->_height = g_panel_start_height;
		panel->_ref = 0;
		g_panels[index] = panel;
	}
	ASSERT(panel);
	panel->_ref++;
	return panel;
}

void PanelRelease(Window w)
{
	uint32_t id = PanelGetId(w);
	uint32_t index = id & TABLE_MASK;
	Panel * prev = NULL;
	Panel * panel = g_panels[index];
	while (panel && panel->_id != id)
	{
		prev = panel;
		panel = panel->_next;
	}
	ASSERT(panel);
	panel->_ref--;
	if (panel->_ref > 0)
	{
		return;
	}
	
	if (prev)
	{
		prev = panel->_next;
	}
	else
	{
		g_panels[index] = panel->_next;
	}
		
	delete panel;
}

PanelWindow * PanelGetWindow(Window w, bool create = false)
{
	uint32_t index = w & TABLE_MASK;
	PanelWindow * window = g_windows[index];
	while (window && window->_w != w)
	{
		window = window->_next;
	}
	if (!window && create)
	{
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		XDamageCreate (g_backend._dpy, w, XDamageReportRawRectangles);

		window = new PanelWindow;
		window->_next = g_windows[index];
		window->_w = w;
		window->_width = 0;
		window->_panel = PanelReference(w);
		window->_texture = texture;

		g_windows[index] = window;
	}
	return window;
}




void PanelSetStart(Matrix &m, int width, int height)
{
	g_panel_start = m;
	g_panel_start_width = width;
	g_panel_start_height = height;
}


void PanelUpdateBegin()
{
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		for (PanelWindow * window = g_windows[i]; window; window = window->_next)
		{
			window->_delete = true;
		}

		for (Panel * panel = g_panels[i]; panel; panel = panel->_next)
		{
			panel->_count = 0;
		}
	}
	g_window_order = 0;
	g_draw_panel_count = 0;

	int revert_to_return;
	XGetInputFocus(g_backend._dpy, &g_input_focus, &revert_to_return);
	g_panel_focus = NULL;

#if 0
	// detect window manager _NET_SUPPORTING_WM_CHECK, _NET_WM_NAME
	static bool inited = false;
	static Atom wmcheck;
	if (!inited)
	{
		wmcheck = XInternAtom(g_backend._dpy, "_NET_SUPPORTING_WM_CHECK", False);
		inited = true;
	}
	Atom type;
	int format;
	unsigned long nitems, after;
	Window *wm;
	if (XGetWindowProperty(g_backend._dpy, g_backend._root, wmcheck, 0L, 100L, False, XA_WINDOW, &type, &format, &nitems, &after, (unsigned char**)&wm) == Success)
	{
		printf("type %08x(%08x) format %d count %d\n", (int)type, (int)XA_WINDOW, format, (int)nitems);
		if (wm)
		{
			printf(" wm %08x\n", (int)*wm);
			XFree(wm);
		}
	}
#endif
}


void PanelUpdate(Window w)
{
	XWindowAttributes attrib;
	XGetWindowAttributes(g_backend._dpy, w, &attrib);
	if (attrib.c_class != InputOutput)
	{
		return;
	}
	if (attrib.map_state != IsViewable)
	{
		return;
	}

	PanelWindow * window = PanelGetWindow(w, true);
	ASSERT(window);
	if (window->_width != attrib.width || window->_height != attrib.height)
	{
		glBindTexture(GL_TEXTURE_2D, window->_texture);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, attrib.width, attrib.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		window->_width = attrib.width;
		window->_height = attrib.height;
		PanelDamage(w, 0, 0, attrib.width, attrib.height);
	}
	window->_x = attrib.x;
	window->_y = attrib.y;
	window->_order = g_window_order;
	window->_delete = false;
	window->_override = attrib.override_redirect;

	Panel * panel = window->_panel;
	if (panel->_count < 1000)
	{
		panel->_windows[panel->_count++] = window;
	}
	
	if (panel->_count == 1)
	{
		g_draw_panel[g_draw_panel_count++] = panel;
	}
	else if (!window->_override)
	{
		int i;
		for (i = 0; i < g_draw_panel_count; i++)
		{
			if (g_draw_panel[i] == panel)
			{
				break;
			}
		}
		for (; i < (g_draw_panel_count - 1); i++)
		{
			g_draw_panel[i] = g_draw_panel[i + 1];
		}
		g_draw_panel[i] = panel;
	}

	g_window_order++;
}


void PanelDamage(Window w, int x, int y, int width, int height)
{
	PanelWindow * window = PanelGetWindow(w);
	if (!window)
	{
		return;
	}
	BackendUpdateTexture(w, window->_texture, x, y, width, height);
}


void PanelUpdateEnd()
{
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		PanelWindow * prev = NULL;
		PanelWindow * window = g_windows[i];
		while (window)
		{
			if (window->_delete)
			{
				PanelRelease(window->_w);
				if (!prev)
				{
					g_windows[i] = window->_next;
				}
				else
				{
					prev->_next = window->_next;
				}
				PanelWindow * del = window;
				window = window->_next;
				glDeleteTextures(1, &del->_texture);
				delete del;
				continue;
			}
			prev = window;
			window = window->_next;
		}
	}

	PanelWindow * window = PanelGetWindow(g_input_focus);
	if (window)
	{
		g_panel_focus = window->_panel;
	}
	else
	{
		g_panel_focus = g_draw_panel[0];
	}

	//restack
#if 0
	if (g_draw_panel_count > 1)
	{
		Panel * panel = g_draw_panel[g_draw_panel_count - 2];
		for (int j = 0; j < panel->_count; j++)
		{
			PanelWindow * window = panel->_windows[j];
			Window w = PanelGetSubWindow(window->_w);
			XRaiseWindow(g_backend._dpy, w);
		}
		XFlush(g_backend._dpy);
	}
#elif 0
	int nwindows = 0;
	Window windows[1024];
	for (int i = g_draw_panel_count - 1; i >=0; i--)
	{
		Panel * panel = g_draw_panel[i];
		for (int j = panel->_count - 1; j >= 0; j--)
		{
			PanelWindow * window = panel->_windows[j];
			windows[nwindows++] = window->_w;
			ASSERT(nwindows < 1024);
		}
	}
	XRestackWindows(g_backend._dpy, windows, nwindows);
#endif
}

void DrawCursor();

void PanelDraw()
{
	glEnable(GL_TEXTURE_2D);
	glClientActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor4f(1.f, 1.f, 1.f, 1.f);

	float offset = 0.f;
	for (int i = 0; i < g_draw_panel_count; i++)
	{
		Panel * panel = g_draw_panel[i];

		glPushMatrix();
		glMultMatrixf(panel->_matrix._m);
		glTranslatef(-g_panel_width / 2, -g_panel_height / 2, 0.f);
		glScalef(g_panel_width / panel->_width, g_panel_height / panel->_height, 1.f);

		for (int j = 0; j < panel->_count; j++)
		{
			PanelWindow * window = panel->_windows[j];
			float x1 = window->_x;
			float y1 = window->_y;
			float x2 = x1 + window->_width;
			float y2 = y1 + window->_height;

			float verts[] =
			{
				x1, y1, offset, 0, 0,
				x2, y1, offset, 1, 0,
				x2, y2, offset, 1, 1,
				x1, y2, offset, 0, 1
			};
			glBindTexture(GL_TEXTURE_2D, window->_texture);
			glVertexPointer(3, GL_FLOAT, 5 * 4, verts );
			glTexCoordPointer(2, GL_FLOAT, 5 * 4, verts + 3);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			offset += 0.1f;
		}

		glPopMatrix();
	}

	if (g_panel_focus)
	{
		Panel * panel = g_panel_focus;

		glPushMatrix();
		glMultMatrixf(panel->_matrix._m);
		glTranslatef(-g_panel_width / 2, -g_panel_height / 2, 0.f);
		glScalef(g_panel_width / panel->_width, g_panel_height / panel->_height, 1.f);

		float mx = g_backend._mouse_x;
		float my = g_backend._mouse_y;
		glTranslatef(mx, my, offset);
		glScalef(18.f, -18.f, 1.f);
		DrawCursor();

		glPopMatrix();
	}

}


void PanelLog()
{
	int window_count = 0;

#if 1
	for (int i = 0; i < g_draw_panel_count; i++)
	{
		Panel * panel = g_draw_panel[i];
		printf("  panel %d\n", panel->_id);
		for (int j = 0; j < panel->_count; j++)
		{
			PanelWindow * window = panel->_windows[j];
			printf("    window %08x, %d, %d %d %d %d\n", (int)window->_w, window->_order, window->_x, window->_y, window->_width, window->_height);
			window_count++;
		}
	}
#else
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		for (PanelWindow * window = g_windows[i]; window; window = window->_next)
		{
			printf("window %08x, %d %d %d %d\n", (int)window->_w, window->_x, window->_y, window->_width, window->_height);
			window_count++;
		}
	}
#endif

	printf("%d windows\n", window_count);
}





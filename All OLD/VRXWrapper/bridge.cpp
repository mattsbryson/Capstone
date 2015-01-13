
#include "common.h"
#include "backend.h"
#include "frontend.h"
#include "bridge.h"

#ifdef ENABLE_OPENNI
#include "openni.h"
#endif

#ifdef ENABLE_HYDRA
#include "Hydra.h"
#endif




#if 0
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

class SemLocker
{
protected:
	int _semid;
	int _cntr;
public:
	SemLocker(int semid, int cntr = 0)
	{
		_semid = semid;
		_cntr = cntr;
		struct sembuf sem_lock={ cntr, -1, SEM_UNDO};
		while (semop(semid, &sem_lock, 1) == -1) {}
	}

	~SemLocker()
	{
		struct sembuf sem_unlock={ _cntr, 1, SEM_UNDO};
		while (semop(_semid, &sem_unlock, 1) == -1) {}
	}
};

class SensorShare
{
protected:
	static const int _size = sizeof(float) * 16;
	int _shmid;
	int _semid;

	float * _data;

public:
	SensorShare()
	{
		// get semaphore
		_semid = semget(0xEA75, 1, IPC_CREAT|IPC_EXCL|0666);
		if (_semid == -1)
		{
			_semid = semget(0xEA75, 1, 0666);
			if (_semid == -1)
			{
				_shmid = -1;
				_data = NULL;
				printf("sensor share failed %d\n", __LINE__);
				return;
			}
		}
		else
		{
			// initialize
			semctl(_semid, 0, SETVAL, 1);
		}
		// get shared memory
		SemLocker locker(_semid);
		bool created;
		_shmid = shmget(0xEA75, _size, IPC_CREAT|IPC_EXCL|0666);
		if (_shmid == -1)
		{
			_shmid = shmget(0xEA75, _size, 0);
			if (_shmid == -1)
			{
				_data = NULL;
				printf("sensor share failed %d\n", __LINE__);
				return;
			}
			created = false;
		}
		else
		{
			created = true;
		}
		// attach to shared memory
		_data = (float *)shmat(_shmid, 0, 0);
		if (_data == (float*)-1)
		{
			_data = NULL;
			printf("sensor share failed %d\n", __LINE__);
			return;
		}
		if (created)
		{
			memset(_data, 0, _size);
		}
		printf("sensor share initialized\n");
	}

	void WriteMatrix(float * matrix)
	{
		if (!_data)
		{
			return;
		}
		SemLocker locker(_semid);
		memcpy(_data, matrix, _size);
	}

	void ReadMatrix(float * matrix)
	{
		if (!_data)
		{
			return;
		}
		SemLocker locker(_semid);
		memcpy(matrix, _data, _size);
	}
};

SensorShare s_share;
#endif




Matrix g_camera(Matrix::identity);
Matrix g_hand(Matrix::identity);
static int s_hands = 0;
int g_camera_mode;
float g_camera_yaw;
float g_camera_pitch;
bool g_hand_draw;
Vector3 g_offset;

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
	if (g_hand_draw)
	{
		glPushMatrix();
		glMultMatrixf(g_hand._m);
		glScalef(4, 4, 8);

		float vertices[] =
		{
			-1.f, -1.f, -1.f,
			 1.f, -1.f, -1.f,
			 1.f,  1.f, -1.f,
			-1.f,  1.f, -1.f,
			-1.f, -1.f,  1.f,
			 1.f, -1.f,  1.f,
			 1.f,  1.f,  1.f,
			-1.f,  1.f,  1.f,
		};
		uint32_t indices[] =
		{
			3, 2, 1, 0,
			2, 3, 7, 6,
			1, 2, 6, 5,
			0, 1, 5, 4,
			3, 0, 4, 7,
			4, 5, 6, 7,
		};
		glDisable(GL_TEXTURE_2D);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glDrawElements(GL_QUADS, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, indices);

		glPopMatrix();
	}

	float mx = g_backend._mouse_x;
	float my = g_backend._mouse_y;

#ifdef ENABLE_OPENNI
	if (g_user_enabled)
#endif
	{
	glPushMatrix();
	glTranslatef(g_offset._x, g_offset._y, g_offset._z);

#ifndef USE_PANELS
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
#else
	PanelDraw();
#endif

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
}

void DrawScreen()
{
	glDisable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if defined(USE_PANELS) && 1
	static int s_count = 0;
	s_count++;
	if (s_count > 90)
	{
		s_count = 0;
		printf("hands: %d\n", s_hands);
		printf("camera: %.3f %.3f %.3f dir:%.3f %.3f %.3f up:%.3f %.3f %.3f right:%.3f %.3f %.3f\n",
				g_camera.translation()._x,
				g_camera.translation()._y,
				g_camera.translation()._z,
				g_camera.back()._x,
				g_camera.back()._y,
				g_camera.back()._z,
				g_camera.up()._x,
				g_camera.up()._y,
				g_camera.up()._z,
				g_camera.right()._x,
				g_camera.right()._y,
				g_camera.right()._z
		);
		printf("%.3f %.3f %.3f %.3f\n",
				g_camera.translation()._w,
				g_camera.back()._w,
				g_camera.up()._w,
				g_camera.right()._w
		);
		PanelLog();
	}
#endif

#if 1
	glViewport(0, 0, g_frontend._width, g_frontend._height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float near = 0.1f;
	float yscale = g_frontend._height / (float)g_frontend._width;
	glFrustum(-near, near, -near * yscale, near * yscale, near, 1000.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// apply the camera
	Matrix inv = g_camera;
	inv.FastInverse();
	glMultMatrixf(inv._m);

	DrawScene();
#else
	float display_dist = 4 * 12 * 2.56f;//12 * 12 * 2.56f;
	float display_diag = 50 * 2.56f;//150 * 2.56f;
	float display_width = display_diag / 1.14734748f;
	float display_height = display_width * 0.5625f;
	float eye_separation = 3.f;
	float left, right, top, bottom;
	float near = 0.1f;
	Matrix inv;

	glEnable(GL_SCISSOR_TEST);

	// left eye
	glViewport(0, 0, g_frontend._width/2, g_frontend._height);
	glScissor(0, 0, g_frontend._width/2, g_frontend._height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	left = near * (-display_width/2 + eye_separation) / display_dist;
	right = near * (display_width/2 + eye_separation) / display_dist;
	top = near * (display_height/2) / display_dist;
	bottom = near * (-display_height/2) / display_dist;
	glFrustum(left, right, bottom, top, near, 1000.f);

	glTranslatef(eye_separation, 0, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// apply the camera
	inv = g_camera;
	inv.FastInverse();
	glMultMatrixf(inv._m);

	DrawScene();

	// right eye
	glViewport(g_frontend._width/2, 0, g_frontend._width/2, g_frontend._height);
	glScissor(g_frontend._width/2, 0, g_frontend._width/2, g_frontend._height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	left = near * (-display_width/2 - eye_separation) / display_dist;
	right = near * (display_width/2 - eye_separation) / display_dist;
	top = near * (display_height/2) / display_dist;
	bottom = near * (-display_height/2) / display_dist;
	glFrustum(left, right, bottom, top, near, 1000.f);

	glTranslatef(-eye_separation, 0, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// apply the camera
	inv = g_camera;
	inv.FastInverse();
	glMultMatrixf(inv._m);

	DrawScene();
#endif

	glFinish();

	glXSwapBuffers(g_frontend._dpy, g_frontend._w);
}

void CameraUpdate()
{
	float pitch, yaw;

#if 0
	{
		float matrix[16];
		s_share.ReadMatrix(matrix);
		g_camera = matrix;
		//g_camera.right() = -g_camera.right();
		g_camera.up() = -g_camera.up();
		//g_camera.back() = -g_camera.back();
		//g_camera.back()._x = -g_camera.back()._x;
		//g_camera.back()._z = -g_camera.back()._z;
		//g_camera.translation() = Vector3(Vector3::zero);
		g_camera.translation()._y = -g_camera.translation()._y;
		g_camera.translation()._z = -g_camera.translation()._z;
		g_camera.translation() *= 100.f;
		g_camera.FastInverse();
		g_camera.right()._w = 0;
		g_camera.up()._w = 0;
		g_camera.back()._w = 0;
		g_camera.translation()._w = 1;
		return;
	}
#endif

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

#ifdef ENABLE_HYDRA
	Hand left, right;
	int hands = HydraGetHands(left, right);
	//static Vector3 s_zone(-60.f, 20.f, 60.f);
	//static Vector3 s_zone(-40.f, 10.f, 40.f);
	static Vector3 s_zone(-10.f, 10.f, 20.f);

	if (hands != s_hands)
	{
		s_hands = hands;
		printf("Hydra Hands: %d\n", hands);
	}

	if (hands & HAND_RIGHT)
	{
		g_hand = right._matrix;
		g_hand.translation() -= s_zone;
		g_hand_draw = true;
	}
	else
	{
		g_hand_draw = false;
	}
	if (hands & HAND_LEFT)
	{
		g_camera = left._matrix;
		g_camera.translation() -= s_zone;
		return;
	}
#endif

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


#ifndef OPENNI_H
#define OPENNI_H

extern GLuint g_depth_texture;

enum SK
{
	SK_HEAD = 0,
	SK_NECK,
	SK_TORSO,
	SK_WAIST,
	SK_RIGHT_COLLAR,
	SK_RIGHT_SHOULDER,
	SK_RIGHT_ELBOW,
	SK_RIGHT_WRIST,
	SK_RIGHT_HAND,
	SK_RIGHT_FINGERTIP,
	SK_LEFT_COLLAR,
	SK_LEFT_SHOULDER,
	SK_LEFT_ELBOW,
	SK_LEFT_WRIST,
	SK_LEFT_HAND,
	SK_LEFT_FINGERTIP,
	SK_COUNT
};

extern bool g_user_enabled;
extern float g_user_joints[SK_COUNT][3];

extern int g_depth_verts_count;
extern float * g_depth_verts;

bool OpenNIInit();
void OpenNIUpdate();

#endif//OPENNI_H


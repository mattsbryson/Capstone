
#include <GL/gl.h>
#include <math.h>

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnHash.h>
#include <XnLog.h>

#include "openni.h"

using namespace xn;

Context g_context;
DepthGenerator g_depth;
ImageGenerator g_image;
UserGenerator g_user;
bool g_image_enabled;
GLuint g_depth_texture;
int g_depth_verts_count = XN_VGA_X_RES * XN_VGA_Y_RES;
float * g_depth_verts = new float[XN_VGA_X_RES * XN_VGA_Y_RES * (3 + 4)];
bool g_user_enabled;
float g_user_joints[SK_COUNT][3];

struct SKToJoint
{
	SK _index;
	XnSkeletonJoint _joint;
};
SKToJoint g_SKToJoint[] =
{
	{SK_HEAD, XN_SKEL_HEAD},
	{SK_NECK, XN_SKEL_NECK},
	{SK_TORSO, XN_SKEL_TORSO},
	{SK_WAIST, XN_SKEL_WAIST},
	//{SK_LEFT_COLLAR, XN_SKEL_LEFT_COLLAR},
	{SK_LEFT_SHOULDER, XN_SKEL_LEFT_SHOULDER},
	{SK_LEFT_ELBOW, XN_SKEL_LEFT_ELBOW},
	{SK_LEFT_WRIST, XN_SKEL_LEFT_WRIST},
	{SK_LEFT_HAND, XN_SKEL_LEFT_HAND},
	//{SK_LEFT_FINGERTIP, XN_SKEL_LEFT_FINGERTIP},
	//{SK_RIGHT_COLLAR, XN_SKEL_RIGHT_COLLAR},
	{SK_RIGHT_SHOULDER, XN_SKEL_RIGHT_SHOULDER},
	{SK_RIGHT_ELBOW, XN_SKEL_RIGHT_ELBOW},
	{SK_RIGHT_WRIST, XN_SKEL_RIGHT_WRIST},
	{SK_RIGHT_HAND, XN_SKEL_RIGHT_HAND},
	//{SK_RIGHT_FINGERTIP, XN_SKEL_RIGHT_FINGERTIP},
	{SK_COUNT, XN_SKEL_HEAD}
};





void XN_CALLBACK_TYPE
User_NewUser(xn::UserGenerator& generator,
             XnUserID nId, void* pCookie)
{
	printf("New User: %d\n", nId);
	g_user.GetPoseDetectionCap().StartPoseDetection("Psi", nId);
}

void XN_CALLBACK_TYPE
User_LostUser(xn::UserGenerator& generator, XnUserID nId,
              void* pCookie)
{
}

void XN_CALLBACK_TYPE
Pose_Detected(xn::PoseDetectionCapability& pose, const XnChar* strPose,
              XnUserID nId, void* pCookie)
{
	printf("Pose %s for user %d\n", strPose, nId);
	g_user.GetPoseDetectionCap().StopPoseDetection(nId);
	g_user.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

void XN_CALLBACK_TYPE
Calibration_Start(xn::SkeletonCapability& capability, XnUserID nId,
                  void* pCookie)
{
	printf("Starting calibration for user %d\n", nId);
}
void XN_CALLBACK_TYPE
Calibration_End(xn::SkeletonCapability& capability, XnUserID nId,
                XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		printf("User calibrated\n");
		g_user.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		printf("Failed to calibrate user %d\n", nId);
		g_user.GetPoseDetectionCap().StartPoseDetection("Psi", nId);
	}
}



XnBool g_bCalibrated;

void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
	if (!g_bCalibrated) // check on player0 is enough
	{
		printf("Look for pose\n");
		g_user.GetPoseDetectionCap().StartPoseDetection("Psi", user);
		return;
	}

	//AssignPlayer(user);
// 	if (g_nPlayer == 0)
// 	{
// 		printf("Assigned user\n");
// 		g_UserGenerator.GetSkeletonCap().LoadCalibrationData(user, 0);
// 		g_UserGenerator.GetSkeletonCap().StartTracking(user);
// 		g_nPlayer = user;
// 	}
}

void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
	printf("Lost user %d\n", user);
	//if (g_nPlayer == user)
	//{
	//	LostPlayer();
	//}
}

void XN_CALLBACK_TYPE PoseDetected(xn::PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, void* cxt)
{
	printf("Found pose \"%s\" for user %d\n", strPose, user);
	g_user.GetSkeletonCap().RequestCalibration(user, TRUE);
	g_user.GetPoseDetectionCap().StopPoseDetection(user);
}

void XN_CALLBACK_TYPE CalibrationStarted(xn::SkeletonCapability& skeleton, XnUserID user, void* cxt)
{
	printf("Calibration started\n");
}


void XN_CALLBACK_TYPE CalibrationCompleted(xn::SkeletonCapability& skeleton, XnUserID user, XnCalibrationStatus eStatus, void* cxt)
{
	printf("Calibration done [%d] %ssuccessfully\n", user, (eStatus == XN_CALIBRATION_STATUS_OK)?"":"un");
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		if (!g_bCalibrated)
		{
			g_user.GetSkeletonCap().SaveCalibrationData(user, 0);
			//g_nPlayer = user;
			g_user.GetSkeletonCap().StartTracking(user);
			g_bCalibrated = TRUE;
		}

		XnUserID aUsers[10];
		XnUInt16 nUsers = 10;
		g_user.GetUsers(aUsers, nUsers);
		for (int i = 0; i < nUsers; ++i)
			g_user.GetPoseDetectionCap().StopPoseDetection(aUsers[i]);
	}
}

bool OpenNIInit()
{
	XnStatus nRetVal;

	nRetVal = g_context.Init();
	if (nRetVal != XN_STATUS_OK)
	{
		printf("kinect failed %d %08x\n", __LINE__, nRetVal);
		return false;
	}

	// Create a depth generator
	nRetVal = g_depth.Create(g_context);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("kinect failed %d %08x\n", __LINE__, nRetVal);
		return false;
	}

	// Create a image generator
	nRetVal = g_image.Create(g_context);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("kinect failed %d %08x\n", __LINE__, nRetVal);
		return false;
	}

	// Create a user generator
	nRetVal = g_user.Create(g_context);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("kinect failed %d %08x\n", __LINE__, nRetVal);
		return false;
	}

	// Set it to VGA maps at 30 FPS
	XnMapOutputMode mapMode;
	mapMode.nXRes = XN_VGA_X_RES;
	mapMode.nYRes = XN_VGA_Y_RES;
	mapMode.nFPS = 30;
	nRetVal = g_depth.SetMapOutputMode(mapMode);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("kinect failed %d %08x\n", __LINE__, nRetVal);
		return false;
	}
	nRetVal = g_image.SetMapOutputMode(mapMode);
	if (nRetVal != XN_STATUS_OK)
	{
		printf("kinect failed %d %08x\n", __LINE__, nRetVal);
		return false;
	}

	//XnCallbackHandle h1, h2;//, h3;
	//g_user.RegisterUserCallbacks(User_NewUser, User_LostUser,
    //                                    NULL, h1);
	//g_user.GetPoseDetectionCap().RegisterToPoseCallbacks(
	//		Pose_Detected, NULL, NULL, h2);
	//g_user.GetSkeletonCap().RegisterCalibrationCallbacks(
	//		Calibration_Start, Calibration_End, NULL, h3);

	XnCallbackHandle hUserCBs, hCalibrationStartCB, hCalibrationCompleteCB, hPoseCBs;
	g_user.RegisterUserCallbacks(NewUser, LostUser, NULL, hUserCBs);
	nRetVal = g_user.GetSkeletonCap().RegisterToCalibrationStart(CalibrationStarted, NULL, hCalibrationStartCB);
	nRetVal = g_user.GetSkeletonCap().RegisterToCalibrationComplete(CalibrationCompleted, NULL, hCalibrationCompleteCB);
	nRetVal = g_user.GetPoseDetectionCap().RegisterToPoseDetected(PoseDetected, NULL, hPoseCBs);

	XnUInt32 nNameLength = 512;
	XnUInt32 nPoses = 40;
	XnChar *poses[40];
	for (int i = 0; i < 40 ; i++)
	{
		poses[i] = new XnChar[512];
		poses[i][0] = 0;
	}
	nRetVal = g_user.GetPoseDetectionCap().GetAllAvailablePoses(poses, nNameLength, nPoses);
	if (nRetVal == XN_STATUS_OK)
	{
		for (int i = 0; i < nPoses; i++)
		{
			printf("   pose %2d \"%s\"\n", i, poses[i]);
		}
	}
	else
	{
		printf("cannot get available poses: %s\n", xnGetStatusString(nRetVal));
	}

	// Set the profile
	//g_user.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_UPPER);
	g_user.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	// Start generating
	nRetVal = g_context.StartGeneratingAll();
	if (nRetVal != XN_STATUS_OK)
	{
		printf("kinect failed %d %08x\n", __LINE__, nRetVal);
		return false;
	}

	XnBool isSupported = g_depth.IsCapabilitySupported("AlternativeViewPoint");
	if(isSupported == TRUE)
	{
		nRetVal = g_depth.GetAlternativeViewPointCap().SetViewPoint(g_image);
		g_image_enabled = (nRetVal == XN_STATUS_OK);
		if(nRetVal != XN_STATUS_OK)
		{
			printf("Getting and setting AlternativeViewPoint failed: %s\n", xnGetStatusString(nRetVal));
		}
	}

	glGenTextures(1, &g_depth_texture);
	glBindTexture(GL_TEXTURE_2D, g_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, XN_VGA_X_RES, XN_VGA_Y_RES, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

	return true;
}


void OpenNIUpdate()
{
	static XnStatus s_nRetVal = ~XN_STATUS_OK;
	XnStatus nRetVal = g_context.WaitOneUpdateAll(g_depth);
	if (nRetVal != s_nRetVal)
	{
		s_nRetVal = nRetVal;
		if (nRetVal == XN_STATUS_OK)
		{
			printf("kinect update working!\n");
		}
		else
		{
			printf("kinect update NOT working!\n");
		}
	}
	if (nRetVal != XN_STATUS_OK)
	{
		return;
	}

	uint8_t d[XN_VGA_X_RES * XN_VGA_Y_RES];
	uint8_t c[XN_VGA_X_RES * XN_VGA_Y_RES * 3];
	const XnDepthPixel* pDepthMap = g_depth.GetDepthMap();
	if (g_image_enabled)
	{
		const XnRGB24Pixel * colors = g_image.GetRGB24ImageMap();
		for (int i = 0; i < XN_VGA_X_RES * XN_VGA_Y_RES; i++)
		{
			d[i] = pDepthMap[i] / 8;
			c[i*3 + 0] = colors[i].nRed;
			c[i*3 + 1] = colors[i].nGreen;
			c[i*3 + 2] = colors[i].nBlue;
		}
	}
	else
	{
		for (int i = 0; i < XN_VGA_X_RES * XN_VGA_Y_RES; i++)
		{
			d[i] = pDepthMap[i] / 8;
			c[i*3 + 0] = d[i];
			c[i*3 + 1] = d[i];
			c[i*3 + 2] = d[i];
		}
	}
	const float aspect = XN_VGA_X_RES / (float)XN_VGA_Y_RES;
	for (int y = 0; y < XN_VGA_Y_RES; y++)
	{
		int index = y * XN_VGA_X_RES;
		float *vert = g_depth_verts + (index * (3+4));
		for (int x = 0; x < XN_VGA_X_RES; x++)
		{
			vert[0] = -((float)pDepthMap[index] / 10.f) * (2.f * x / XN_VGA_X_RES - 1.f) * tan(58/2 * 3.141593f / 180.f);
			vert[1] = -((float)pDepthMap[index] / 10.f) * (2.f * y / XN_VGA_Y_RES - 1.f) * tan(45/2 * 3.141593f / 180.f);
			vert[2] = ((float)pDepthMap[index]) / 10.f;
			vert[3] = c[index *3 + 0] / 255.f;
			vert[4] = c[index *3 + 1] / 255.f;
			vert[5] = c[index *3 + 2] / 255.f;
			vert[6] = 1.f;
			vert += (3+4);
			index++;
		}
	}
	glBindTexture(GL_TEXTURE_2D, g_depth_texture);
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, XN_VGA_X_RES, XN_VGA_Y_RES, GL_LUMINANCE, GL_UNSIGNED_BYTE, d);


	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;
	g_user.GetUsers(aUsers, nUsers);

	if ((nUsers > 0) && g_user.GetSkeletonCap().IsTracking(aUsers[0]))
	{
		for (int i = 0; i < SK_COUNT && g_SKToJoint[i]._index != SK_COUNT; i++)
		{
			int j = g_SKToJoint[i]._index;
			XnSkeletonJoint joint = g_SKToJoint[i]._joint;
	        XnSkeletonJointPosition pos;
	        g_user.GetSkeletonCap().GetSkeletonJointPosition(
		           aUsers[0], joint, pos);
			g_user_joints[j][0] = -0.1f * pos.position.X;
			g_user_joints[j][1] = 0.1f * pos.position.Y;
			g_user_joints[j][2] = 0.1f * pos.position.Z;
		}
		g_user_enabled = true;
	}
	else
	{
		g_user_enabled = false;
	}
#if 0
    for (int i = 0; i < nUsers; ++i)
    {
      if (g_user.GetSkeletonCap().IsTracking(aUsers[i]))
      {
        XnSkeletonJointPosition Head;
        g_user.GetSkeletonCap().GetSkeletonJointPosition(
           aUsers[i], XN_SKEL_HEAD, Head);
        printf("%d: (%f,%f,%f) [%f]\n", aUsers[i],
               Head.position.X, Head.position.Y, Head.position.Z,
               Head.fConfidence);
      }
    }
#endif
}


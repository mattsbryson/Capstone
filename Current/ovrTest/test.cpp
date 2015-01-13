// Include the OculusVR SDK
#include "OVR_CAPI.h"
void main()
{
	// Initializes LibOVR.
	ovr_Initialize();
	ovrHmd hmd = ovrHmd_Create(0);
	ovrHmdDesc hmdDesc;
	if (hmd)
	{
		// Get more details about the HMD
		ovrHmd_GetDesc(hmd, &hmdDesc);
	}
	// do something with the hmd
	ovr_Shutdown();
}


#include <iostream>
#include <array>
#include "OVR.h"
using namespace OVR;

int main()
{
	//Initializing the OVR core
	System::Init();
	System::Init(Log::ConfigureDefaultLog(LogMask_All));
	Ptr<DeviceManager> pManager;
	Ptr<HMDDevice> pHMD;
	pManager = *DeviceManager::Create();
	pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();

	return 0;
}


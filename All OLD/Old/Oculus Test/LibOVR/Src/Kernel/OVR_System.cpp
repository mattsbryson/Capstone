/************************************************************************************

Filename    :   OVR_System.cpp
Content     :   General kernel initialization/cleanup, including that
                of the memory allocator.
Created     :   September 19, 2012
Notes       : 

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR SDK License Version 2.0 (the "License"); 
you may not use the Oculus VR SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "OVR_System.h"
#include "OVR_Threads.h"
#include "OVR_Timer.h"

namespace OVR {

// *****  OVR::System Implementation

// Initializes System core, installing allocator.
void System::Init(Log* log, Allocator *palloc)
{    
    if (!Allocator::GetInstance())
    {
        Log::SetGlobalLog(log);
        Timer::initializeTimerSystem();
        Allocator::setInstance(palloc);
    }
    else
    {
        OVR_DEBUG_LOG(("System::Init failed - duplicate call."));
    }
}

void System::Destroy()
{    
    if (Allocator::GetInstance())
    {
        // Wait for all threads to finish; this must be done so that memory
        // allocator and all destructors finalize correctly.
#ifdef OVR_ENABLE_THREADS
        Thread::FinishAllThreads();
#endif

        // Shutdown heap and destroy SysAlloc singleton, if any.
        Allocator::GetInstance()->onSystemShutdown();
        Allocator::setInstance(0);

        Timer::shutdownTimerSystem();
        Log::SetGlobalLog(Log::GetDefaultLog());
    }
    else
    {
        OVR_DEBUG_LOG(("System::Destroy failed - System not initialized."));
    }
}

// Returns 'true' if system was properly initialized.
bool System::IsInitialized()
{
    return Allocator::GetInstance() != 0;
}

} // OVR


#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>

#include "Hydra.h"

#include <sixense.h>

bool g_hydra_initialized = false;

bool HydraInitialize()
{
	if (g_hydra_initialized)
	{
		return true;
	}

	if (sixenseInit() == SIXENSE_FAILURE)
	{
		return false;
	}

	//sixenseSetFilterEnabled(1);
	//sixenseSetFilterParams(near_range, near_val, far_range, far_val);

	g_hydra_initialized = true;

	return true;
}

void HydraExit()
{
	if (g_hydra_initialized)
	{
		sixenseExit();
		g_hydra_initialized = false;
	}
}

int HydraGetHands(Hand &left, Hand &right)
{
	if (!g_hydra_initialized)
	{
		return 0;
	}

	int hands = 0;
	for( int base = 0; base < sixenseGetMaxBases(); base++ )
	{
		sixenseSetActiveBase(base);

		// Get the latest controller data
		sixenseAllControllerData acd;
		sixenseGetAllNewestData(&acd);

		// For each possible controller
		for( int cont=0; cont < sixenseGetMaxControllers(); cont++ )
		{
			// See if it's enabled
			if( !acd.controllers[cont].enabled )
			{
				continue;
			}
			if (acd.controllers[cont].which_hand == 1)
			{
				hands |= HAND_LEFT;
				left._matrix._m[ 0] = acd.controllers[cont].rot_mat[0][0];
				left._matrix._m[ 1] = acd.controllers[cont].rot_mat[0][1];
				left._matrix._m[ 2] = acd.controllers[cont].rot_mat[0][2];
				left._matrix._m[ 3] = 0;
				left._matrix._m[ 4] = acd.controllers[cont].rot_mat[1][0];
				left._matrix._m[ 5] = acd.controllers[cont].rot_mat[1][1];
				left._matrix._m[ 6] = acd.controllers[cont].rot_mat[1][2];
				left._matrix._m[ 7] = 0;
				left._matrix._m[ 8] = acd.controllers[cont].rot_mat[2][0];
				left._matrix._m[ 9] = acd.controllers[cont].rot_mat[2][1];
				left._matrix._m[10] = acd.controllers[cont].rot_mat[2][2];
				left._matrix._m[11] = 0;
				left._matrix._m[12] = acd.controllers[cont].pos[0] * 0.1f;
				left._matrix._m[13] = acd.controllers[cont].pos[1] * 0.1f;
				left._matrix._m[14] = acd.controllers[cont].pos[2] * 0.1f;
				left._matrix._m[15] = 1;

				left._input[0] = acd.controllers[cont].joystick_x * 1.f / 256.f;
				left._input[1] = acd.controllers[cont].joystick_y * 1.f / 256.f;
				left._input[2] = acd.controllers[cont].trigger * 1.f / 256.f;
				left._input[3] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_BUMPER)? 1.f : 0.f;
				left._input[4] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_START)? 1.f : 0.f;
				left._input[5] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_JOYSTICK)? 1.f : 0.f;
				left._input[6] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_1)? 1.f : 0.f;
				left._input[7] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_2)? 1.f : 0.f;
				left._input[8] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_3)? 1.f : 0.f;
				left._input[9] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_4)? 1.f : 0.f;
			}
			else
			{
				hands |= HAND_RIGHT;
				right._matrix._m[ 0] = acd.controllers[cont].rot_mat[0][0];
				right._matrix._m[ 1] = acd.controllers[cont].rot_mat[0][1];
				right._matrix._m[ 2] = acd.controllers[cont].rot_mat[0][2];
				right._matrix._m[ 3] = 0;
				right._matrix._m[ 4] = acd.controllers[cont].rot_mat[1][0];
				right._matrix._m[ 5] = acd.controllers[cont].rot_mat[1][1];
				right._matrix._m[ 6] = acd.controllers[cont].rot_mat[1][2];
				right._matrix._m[ 7] = 0;
				right._matrix._m[ 8] = acd.controllers[cont].rot_mat[2][0];
				right._matrix._m[ 9] = acd.controllers[cont].rot_mat[2][1];
				right._matrix._m[10] = acd.controllers[cont].rot_mat[2][2];
				right._matrix._m[11] = 0;
				right._matrix._m[12] = acd.controllers[cont].pos[0] * 0.1f;
				right._matrix._m[13] = acd.controllers[cont].pos[1] * 0.1f;
				right._matrix._m[14] = acd.controllers[cont].pos[2] * 0.1f;
				right._matrix._m[15] = 1;

				right._input[0] = acd.controllers[cont].joystick_x * 1.f / 256.f;
				right._input[1] = acd.controllers[cont].joystick_y * 1.f / 256.f;
				right._input[2] = acd.controllers[cont].trigger * 1.f / 256.f;
				right._input[3] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_BUMPER)? 1.f : 0.f;
				right._input[4] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_START)? 1.f : 0.f;
				right._input[5] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_JOYSTICK)? 1.f : 0.f;
				right._input[6] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_1)? 1.f : 0.f;
				right._input[7] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_2)? 1.f : 0.f;
				right._input[8] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_3)? 1.f : 0.f;
				right._input[9] = (acd.controllers[cont].buttons&SIXENSE_BUTTON_4)? 1.f : 0.f;
			}
		}
	}
	return hands;
}


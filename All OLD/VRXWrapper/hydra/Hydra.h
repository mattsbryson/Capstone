#ifndef HYDRA_H
#define HYDRA_H

#include "Vector.h"
#include "Matrix.h"

bool HydraInitialize();
void HydraExit();

#define HAND_LEFT 1
#define HAND_RIGHT 2

enum
{
	INPUT_X,
	INPUT_Y,
	INPUT_TRIGGER,
	INPUT_BUMPER,
	INPUT_START,
	INPUT_STICK,
	INPUT_1,
	INPUT_2,
	INPUT_3,
	INPUT_4,
	INPUT_MAX,
};

struct Hand
{
	Matrix _matrix;
	float _input[INPUT_MAX];
};

// left/right, up/down, trigger, bumper, start, stick, 1, 2, 3, 4
int HydraGetHands(Hand &left, Hand &right);

#endif//HYDRA_H


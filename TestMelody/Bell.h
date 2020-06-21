#pragma once

#include <YG.h>

#ifndef PROJECT_TestMelody
#	error Invalid AppDefines.h !!!
#endif //PROJECT_TestMelody

enum MelodyID
{
	MELODY_DoorBell,
	MELODY_Beethoven5,
};

void PlayMelody(MelodyID melody, uint8_t repeat);

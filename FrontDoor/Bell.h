#ifndef __Bell__
#define __Bell__

#include "FrontDoor.h"

#if COMPILE_BELL
enum MelodyID
{
	MELODY_DoorBell,
	MELODY_Beethoven5,
};

void Play(MelodyID melody, uint8_t repeat);
void StopPlaying();
bool IsPlaying();
void BeQuiet(bool quiet = true);
const ITestTarget& GetBellTest();

void Muuuu();
void Tzif();

#endif //COMPILE_BELL
#endif //__Bell__


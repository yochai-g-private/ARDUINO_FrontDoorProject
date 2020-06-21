#include "Bell.h"

#if COMPILE_BELL
#include <Buzzer.h>
#include <MelodyPlayer.h>

static PasiveBuzzer			buzzer("BELL", PIN_PASSIVE_BUZZER);
static MelodyPlayer			melody_player("MELODY", buzzer);
static bool					quite = false;

//--------------------------------------------------------------------------
// Melodies
//--------------------------------------------------------------------------

static const Sound ring_bell[] = {
	{ G, o2, d4 },
	{ E, o2, d2 },
};

static const Sound beethoven_5[] = {
	{ PAUZA,  d8 },
	{ G,  o1, d8 },
	{ G,  o1, d8 },
	{ G,  o1, d8 },
	//----------------
	{ EF, o1, (Duration)(d1 + d4) },
	{ PAUZA,  d4 },
	//----------------
	{ PAUZA,  d8 },
	{ F,  o1, d8 },
	{ F,  o1, d8 },
	{ F,  o1, d8 },
	//----------------
	{ D,  o1, (Duration)(d1 + d4) },
	{ PAUZA,  (Duration)(d4 + d4)},
};
//--------------------------------------------------------------------------
void Play(MelodyID melody, uint8_t repeat)
{
	if (quite)
		return;

	switch (melody)
	{
		case MELODY_DoorBell :
			LOGGER << "Playing DoorBell" << NL;
			melody_player.Play(ring_bell,	countof(ring_bell),		180, repeat);
			break;

		case MELODY_Beethoven5:
			LOGGER << "Playing Beethoven5" << NL;
			melody_player.Play(beethoven_5, countof(beethoven_5),	100, repeat);
			break;
	}
}
//--------------------------------------------------------------------------
bool IsPlaying()
{
	melody_player.IsPlaying();
}
//--------------------------------------------------------------------------
void StopPlaying()
{
	melody_player.Stop();
}
//--------------------------------------------------------------------------
void BeQuiet(bool _quiet)
{
	quite = _quiet;

	if(quite)
		melody_player.Stop();
}
//--------------------------------------------------------------------------
const ITestTarget& GetBellTest()
{
	struct BellTest : ITestTarget
	{
		BellTest() {	}

		void Reset()
		{
			StopPlaying();
		}

		void Set()
		{
		}

		void OnPush(Tester& tester)
		{
			if (IsPlaying())
				StopPlaying();
			else
				Play(MELODY_DoorBell, 0);
		}

#if YG_USE_LOGGER
		const char* GetName()  const { return "BELL"; }
#endif
	};
	
	static BellTest test;
	return test;
}
//--------------------------------------------------------------------------
void Muuuu()
{
	static const Sound muuuu[] = {
		{ C,  OO, d1 },
	};

	melody_player.Play(muuuu, countof(muuuu), 60, 1);
}
//--------------------------------------------------------------------------
void Tzif()
{
	static const Sound tzif[] = {
		{ C,  o4, d8 },
	};

	melody_player.Play(tzif, countof(tzif), 180, 1);

}
//--------------------------------------------------------------------------
#endif //COMPILE_BELL

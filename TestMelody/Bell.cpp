#include <Buzzer.h>
#include <MelodyPlayer.h>

using namespace YG;

static PasiveBuzzer			buzzer("BELL", 4);
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

enum MelodyID
{
	MELODY_DoorBell,
	MELODY_Beethoven5,
};

//--------------------------------------------------------------------------
void PlayMelody(MelodyID melody, uint8_t repeat)
{
	if (quite)
		return;

	switch (melody)
	{
	case MELODY_DoorBell:
		melody_player.Play(ring_bell, countof(ring_bell), 180, repeat);
		break;

	case MELODY_Beethoven5:
		melody_player.Play(beethoven_5, countof(beethoven_5), 100, repeat);
		break;
	}
}
//--------------------------------------------------------------------------

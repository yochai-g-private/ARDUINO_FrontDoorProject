#include "Clock.h"

#if YG_USE_KODESH_WATCH
#include <PreciseRTC.h>
#include <KodeshWatch.h>
#include <Astro.h>

static uint32_t			sunrise_seconds, sunset_seconds;
static DaTi				current;
//--------------------------------------------------------------------------
void UpdateClockFromSerial()
{
	RTC.UpdateFromSerial();
}
//--------------------------------------------------------------------------
static void set_sunrise(const DaTi& dt)
{
	sunrise_seconds = Astro::GetSunRiseMinutes(dt.Year(), dt.Month(), dt.Day());
	sunrise_seconds *= DaTi::SECONDS_PER_MINUTE;
}
//--------------------------------------------------------------------------
static void set_sunset(const DaTi& dt)
{
	sunset_seconds = Astro::GetSunSetMinutes(dt.Year(), dt.Month(), dt.Day());
	sunset_seconds *= DaTi::SECONDS_PER_MINUTE;
}
//--------------------------------------------------------------------------
static void update_sun_times(const DaTi& dt)
{
	set_sunrise(dt);
	set_sunset(dt);
}
//--------------------------------------------------------------------------
static bool is_night(const DaTi& dt)
{
	uint32_t seconds = dt.GetDaySeconds();
	//_LOG("now=" << seconds << ", sunrise_seconds=" << sunrise_seconds << ", sunset_seconds=" << sunset_seconds << NL);
	return (seconds < sunrise_seconds) || (seconds > sunset_seconds);
}
//--------------------------------------------------------------------------
static void OnKodeshChanged(KodeshWatch& kw)
{
	state.u.s.kodesh = kw.IsKodesh();

	SetSensors();
}
//--------------------------------------------------------------------------
DaTi GetNextHadlaka()
{
	return Kodesh::GetNextHadlaka(current);
}
//--------------------------------------------------------------------------
DaTi GetNextMotzeyKodesh()
{
	return Kodesh::GetNextMotzeyKodesh(current);
}
//--------------------------------------------------------------------------
static void ClockCallback(const DaTi& dt, IRTC::TickType tick_type, void* ctx)
{
	if (current.Day() != dt.Day())
		update_sun_times(dt);

	if (current.Minute() != dt.Minute())
	{
		dt.Show();

		bool night = is_night(dt);

		if (state.u.s.night != night)
		{
			state.u.s.night = night;
		}
	}

	current = dt;
}
//--------------------------------------------------------------------------
bool InitializeClock()
{
	KODESH_WATCH.SetCallback(OnKodeshChanged);
	KODESH_WATCH.SetClockCallback(ClockCallback, NULL);

	current = RTC.Get();

	update_sun_times(current);
	current.Show();

	state.u.s.night  = is_night(current);
	state.u.s.kodesh = KODESH_WATCH.IsKodesh();

	SetSensors();

	return true;
}
//--------------------------------------------------------------------------
#endif //YG_USE_KODESH_WATCH

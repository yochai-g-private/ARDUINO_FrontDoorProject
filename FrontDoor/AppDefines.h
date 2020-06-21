#ifndef __AppDefines__
#define __AppDefines__

#define PROJECT_FrontDoor

//==============================================
// YG defines
//==============================================

#define YG_SIMULATOR				0

#define YG_USE_MICROSD_AUX_LOG		1
#define YG_USE_TRACER				1
//#define YG_USE_LOGGER				1
#define YG_DEBUG_LED_PIN			MicroController::DEBUG_LED_PIN__DEFAULT
//#define YG_USE_RTC				1
#define YG_USE_KODESH_WATCH			1

//==============================================
// Application defines
//==============================================

#define COMPILE_SENSORS				0
#define COMPILE_BELL				1
#define COMPILE_MOTION				0
#define COMPILE_SONAR				0
#define COMPILE_DOOR_OPEN			1
#define COMPILE_SHABAT_SWITCH		1
//#define COMPILE_SELF_TEST			1

#endif //__AppDefines__

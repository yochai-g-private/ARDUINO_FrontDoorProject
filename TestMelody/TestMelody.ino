/*
 Name:		TestMelody.ino
 Created:	12/11/2019 5:56:58 PM
 Author:	MCP
*/

#include <MicroController.h>

using namespace YG;

#include "Bell.h"

//--------------------------------------------------------------------------
// APPLICATION
//--------------------------------------------------------------------------

class Application : public IApplication
{
public:

	const char*   GetApplicationName()  const { return "TestMelody"; }
	Version       GetApplicationVersion() const { return 1; }

	void OnSetup()
	{
		PlayMelody(MELODY_Beethoven5, 5);
	}

	void OnLoop(bool first_loop)
	{
	}

	void OnError()
	{
	}

} application;
//--------------------------------------------------------------------------
IApplication& YG::GetApplication() { return application; }
//--------------------------------------------------------------------------

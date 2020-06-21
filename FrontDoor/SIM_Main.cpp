#include "FrontDoor.h"

#if YG_SIMULATOR
#include <Simulator.h>

#define SIMULATION_SCENARIOS_LIST(DO_WITH)		\
	DO_WITH(DM_StartCholDoorClosed)				\
	DO_WITH(DM_StartBeforeHadlakaDoorClosed)	\
	DO_WITH(DM_StartAfterHadlakaDoorClosed)		\
	DO_WITH(DM_StartCholOpenTheDoor)			\
	DO_WITH(DM_ShabatSwitch)					\
	DO_WITH(DM_BeforeMotzeyShabat)				\
	DO_WITH(DM_BeforeMotzeyShabat_2)			\
	DO_WITH(DM_StartCholSwitchToShabat)			\

	//DO_WITH(OpenAndCloseTheDoor_Manual)	\


SIMULATION_SCENARIOS_LIST(DECLARE_SIMULATOR_SCENARIO)

static const Simulator::Scenario* st_scenarios_array[] = { SIMULATION_SCENARIOS_LIST(SIMULATOR_SCENARIO_POINTER) };
Simulator::Scenarios st_scenarios = { st_scenarios_array, countof(st_scenarios_array) };
const Simulator::Scenarios& SIM_GetScenarios()
{
	return st_scenarios;
}

#endif //YG_SIMULATOR

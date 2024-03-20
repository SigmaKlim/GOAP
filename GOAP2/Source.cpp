

#include "Testing.h"
#pragma optimize( "", off )

int main()
{
	srand(time(0));
	DataBase globalData;
	Planner globalPlanner(globalData);
	Strategist globalStrategist(globalData);
	InitNavigator(globalData);
	Helper helper(globalData);
	InitAttributes(helper);
	InitActions(helper, globalData);
	InitGoals(helper);
	auto initState = helper.MakeValueSet({	{"isCrouching", true},
											{"enemyStatus", EAVEnemyStatus::eNonVisible},
											{"hpLeft", 20},
											{"hKitsLeft", 1},
											{"ammoInMagLeft", 0},
											{"grenadesLeft", 0},
											{"magsLeft", 0},
											{"atNode", *globalData.Navigator.GetNodesByName("AmmoBox").begin()}});
	std::vector<std::shared_ptr<IActionPerformer>> performers(globalData.ActionCatalogue.Size());
	for (size_t i = 0; i < performers.size(); i++)
		performers[i] = std::make_shared<APTest>(*globalData.ActionCatalogue.GetName(i));
	GController controller(initState, performers);
	
	while (true)
	{
		controller.Update();
	}
	return 0;
}



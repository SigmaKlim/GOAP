#pragma once
#include <random>
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>

#include "Planner.h"
#include "Helper.h"
#include "Actions/Basic/AcSimple.h"
#include "Actions/Specific/AcGoTo.h"
#include "Actions/Specific/AcPickupDepletable.h"
#include "Actions/Specific/AcSearchEnemy.h"
#include "Actions/Specific/AcShoot.h"
#include "Actions/Specific/ACUseDepletable.h"
#include "Attributes/Basic/ABool.h"

#include "Attributes/Basic/AEnum.h"
#include "Attributes/Special/AHealth.h"
#include "Attributes/Special/AHKitsLeft.h"
#include "Attributes/Special/AAmmoInMag.h"
#include "Attributes/Special/AAtNode.h"
#include "Attributes/Special/AEnemyStatus.h"
#include "Attributes/Special/AGrenadesLeft.h"
#include "Attributes/Special/AMagsLeft.h"


#include "Conditions/Special/CEqual.h"
#include "Conditions\Special\CGreater.h"
#include "Conditions/Special/CInSet.h"
#include "Navigation/Navigator.h"

#include "Tools/MathHelper.h"

#include "Navigation/NavPathfinder.h"

#include "GController.h"
#include "Actions/Performers/APTest.h"
#include "Goals/Specific/GTest.h"
#pragma optimize( "", off )


inline int TestNumeric()
{
	srand(time(0));
	const std::vector<unsigned> DIMS = { /*5, 15, 30, 100,*/ 500 };
	const int K = 5;
	for (auto& dim : DIMS)
		for (unsigned k = 0; k < K; k++)
		{
			std::string fileName = "test_data/test_matrix_set_" + std::to_string(dim) + "_" + std::to_string(k) + ".txt";
			std::ifstream fin(fileName);
			assert(fin);
			Matrix m;
			MathHelper::MakeEmptyMatrix(m, dim);
			MathHelper::ReadMtrxFromFile(m, fin);
			unsigned start = rand() % dim;
			unsigned finish = rand() % dim;
			while (finish == start)
				finish = rand() % dim;
			NavPathfinder np(m);
			Path<unsigned> p;
			TelemetryData td;
			np.Pathfind(p, start, finish, &td);
			std::cout << "dim = " << dim << "\tk = " << k << "\n";
			std::cout << "start = " << start << "\tfinish = " << finish << "\n";
			std::cout << "path: ";
			for (auto& v : p.Vertices)
				std::cout << v << " ";
			std::cout << "\ncost = " << p.Cost << "\n\n";
		}
	
	return 0;
}

//test function to assign ids and distances to navigator
inline void InitNavigator(DataBase& data)
{
	data.Navigator.AddNode("AmmoBox",	1, {8.0f, 1.0f, 0.0f});
	data.Navigator.AddNode("MedStation", 2, {2.0f, 2.0f, 0.0f});
	data.Navigator.AddNode("MedStation", 3, {8.0f, 4.0f, 0.0f});
	data.Navigator.AddNode("AmmoBox",	4, {5.0f, 5.0f, 0.0f});
	data.Navigator.AddNode("MedStation", 5, {2.0f, 7.0f, 0.0f});
	data.Navigator.AddNode("AmmoBox",	6, {6.0f, 8.0f, 0.0f});
	data.Navigator.AddNode("Cover",		7, {8.0f, 8.0f, 0.0f});
	data.Navigator.AddNode("Cover",		8, {3.0f, 5.0f, 0.0f});
	data.Navigator.AddNode("Cover",		9, {1.0f, 8.0f, 0.0f});

	data.Navigator.AddNode("Arbitrary", 0, {0.0f, 0.0f, 0.0f});
}

inline void InitAttributes(const Helper& helper)
{
	helper.RegisterAttribute("isCrouching",		new ABool);
	helper.RegisterAttribute("enemyStatus",		new AEnemyStatus);
	helper.RegisterAttribute("hpLeft",			new AHealth);
	helper.RegisterAttribute("hKitsLeft",		new AHKitsLeft);
	helper.RegisterAttribute("ammoInMagLeft",	new AAmmoInMag);
	helper.RegisterAttribute("magsLeft",		new AMagsLeft);
	helper.RegisterAttribute("atNode",			new AAtNode);
	helper.RegisterAttribute("grenadesLeft",	new AGrenadesLeft);
}

inline void InitActions(const Helper& helper, const DataBase& data)
{
	ConditionSet	cCrouch = helper.MakeConditionSet	({});
	ValueSet		eCrouch = helper.MakeValueSet		({{"isCrouching", true}});
	helper.RegisterAction("Crouch", new AcSimple(cCrouch, eCrouch, 3));
	
	ConditionSet	cStand = helper.MakeConditionSet	({});
	ValueSet		eStand = helper.MakeValueSet		({{"isCrouching", false}});
	helper.RegisterAction("Stand", new AcSimple(cStand, eStand, 4));
	
	
	ConditionSet	cEngage	= helper.MakeConditionSet	({{"enemyStatus", new CEqual(EAVEnemyStatus::eVisible)}});
	ValueSet		eEngage	= helper.MakeValueSet		({{"enemyStatus", EAVEnemyStatus::eInRangedCombatRadius}});
	helper.RegisterAction("Engage enemy", new AcSimple(cEngage, eEngage, 4));
	
	ConditionSet	cApproach = helper.MakeConditionSet	({{"enemyStatus", new CEqual(EAVEnemyStatus::eInRangedCombatRadius)}});
	ValueSet		eApproach = helper.MakeValueSet		({{"enemyStatus", EAVEnemyStatus::eInCloseCombatRadius}});
	helper.RegisterAction("Approach enemy", new AcSimple(cApproach, eApproach, 3));
	
	helper.RegisterAction("Shoot enemy", new AcShoot(3));
	
	ConditionSet cTGrenade = helper.MakeConditionSet({{"grenadesLeft", new CGreater(0)},
							{"enemyStatus", new CEqual(EAVEnemyStatus::eInRangedCombatRadius)}});
	ValueSet eTGrenade = helper.MakeValueSet({	{"enemyStatus", EAVEnemyStatus::eAttacking}});
	helper.RegisterAction("Throw grenade", new AcSimple(cTGrenade, eTGrenade, 7));
	
	ConditionSet cCut = helper.MakeConditionSet({{"enemyStatus", new CEqual(EAVEnemyStatus::eInCloseCombatRadius)}});
	ValueSet eCut = helper.MakeValueSet({{"enemyStatus", EAVEnemyStatus::eAttacking}});
	helper.RegisterAction("Attack melee", new AcSimple(cCut, eCut, 80));
	
	
	helper.RegisterAction("Go to", new AcGoTo);
	
	helper.RegisterAction("Heal", new ACUseDepletable(	data.GetAttributeId("hKitsLeft"),
	                                                    data.GetAttributeId("hpLeft"), 20, 3));
	
	helper.RegisterAction("Reload", new ACUseDepletable(data.GetAttributeId("magsLeft"),
	                                                    data.GetAttributeId("ammoInMagLeft"), 30, 3,
	                                                    "magazine", "ammo"));
	
	helper.RegisterAction("Pickup health kit",
	                      new AcPickupDepletable(	data.GetAttributeId("hKitsLeft"),
	                                                data.GetAttributeId("atNode"),
	                                                data.Navigator.GetNodesByName("MedStation"), 4,
	                                                "health kit"));
	
	helper.RegisterAction("Pickup magazine pack",
	                      new AcPickupDepletable(	data.GetAttributeId("magsLeft"),
	                                                data.GetAttributeId("atNode"),
	                                                data.Navigator.GetNodesByName("AmmoBox"), 3,
	                                                "magazine pack"));
	
	helper.RegisterAction("Pickup grenade",
	                      new AcPickupDepletable(	data.GetAttributeId("grenadesLeft"),
	                                                data.GetAttributeId("atNode"),
	                                                data.Navigator.GetNodesByName("AmmoBox"), 4,
	                                                "grenade"));
	
	
	helper.RegisterAction("Search enemy", new AcSearchEnemy(data.Navigator.GetMaxDistance() * 5));
}
inline void InitGoals(Helper& helper)
{
	ConditionSet gAttack = helper.MakeConditionSet({{"enemyStatus", new CEqual(EAVEnemyStatus::eAttacking)}});
	helper.RegisterGoal("Attack enemy", new GTest(gAttack, 5.0f));
	
	ConditionSet gHeal = helper.MakeConditionSet({{"hpLeft", new CGreater(59)}});
	helper.RegisterGoal("Heal", new GTest(gHeal, 4.0f));
}
inline int TestGoap()
{
// 	srand(time(0));
// 	GController ai;
// 	//IAction::numAttributes = ai.GetNumAttributes(); //!!!
//
// 	
// 	
// 	
//
// 	ValueSet init = helper.MakeValueSet({	{"isCrouching", true},
// 											{"enemyStatus", EAVEnemyStatus::eNonVisible},
// 											{"hpLeft", 20},
// 											{"hKitsLeft", 1},
// 											{"ammoInMagLeft", 0},
// 											{"grenadesLeft", 0},
// 											{"magsLeft", 0},
// 											{"atNode", *navigator.GetNodesByName("AmmoBox").begin()}});
// 	helper.InitState(init);
// #pragma endregion
// #pragma region Loop
// 	while(true)
// 	{
// 		ai.Update();
// 	}
// #pragma endregion

#pragma region old
	// Planner planner;
	// planner.RegisterAttribute<ABool>("isCrouching");
	// planner.RegisterAttribute<AEnemyStatus>("enemyStatus");
	// planner.RegisterAttribute<AHealth>("hpLeft");
	// planner.RegisterAttribute<AHKitsLeft>("hKitsLeft");
	// planner.RegisterAttribute<AAmmoInMag>("ammoInMagLeft");
	// planner.RegisterAttribute<AMagsLeft>("magsLeft");
	// planner.RegisterAttribute<AAtNode>("atNode");
	// planner.RegisterAttribute<AGrenadesLeft>("grenadesLeft");
	// Navigator navigator = SetUpNavigator(); 
	// AAtNode::navigator = navigator;
	//
	// IAction::numAttributes = planner.GetNumAttributes(); //!!!
	//
	// Helper helper(&planner);
	//
	// ConditionSet	cCrouch = helper.MakeConditionSet	({});
	// ValueSet		eCrouch = helper.MakeValueSet		({{"isCrouching", true}});
	// planner.RegisterActionConstructor("Crouch", AcSimple(cCrouch, eCrouch, 3));
	//
	// ConditionSet	cStand = helper.MakeConditionSet	({});
	// ValueSet		eStand = helper.MakeValueSet		({{"isCrouching", false}});
	// planner.RegisterActionConstructor("Stand", AcSimple(cStand, eStand, 4));
	//
	//
	// ConditionSet	cEngage	= helper.MakeConditionSet	({{"enemyStatus", new CEqual(EAVEnemyStatus::eVisible)}});
	// ValueSet		eEngage	= helper.MakeValueSet		({{"enemyStatus", EAVEnemyStatus::eInRangedCombatRadius}});
	// planner.RegisterActionConstructor("Engage enemy", AcSimple(cEngage, eEngage, 4));
	//
	// ConditionSet	cApproach = helper.MakeConditionSet	({{"enemyStatus", new CEqual(EAVEnemyStatus::eInRangedCombatRadius)}});
	// ValueSet		eApproach = helper.MakeValueSet		({{"enemyStatus", EAVEnemyStatus::eInCloseCombatRadius}});
	// planner.RegisterActionConstructor("Approach enemy", AcSimple(cApproach, eApproach, 3));
	//
	// planner.RegisterActionConstructor("Shoot enemy", AcShoot(planner.GetAttributeId("ammoInMagLeft"),
	// 	planner.GetAttributeId("enemyStatus"), 3));
	//
	// ConditionSet cTGrenade = helper.MakeConditionSet({{"grenadesLeft", new CGreater(0)},
	// 						{"enemyStatus", new CEqual(EAVEnemyStatus::eInRangedCombatRadius)}});
	// ValueSet eTGrenade = helper.MakeValueSet({	{"enemyStatus", EAVEnemyStatus::eAttacking}});
	// planner.RegisterActionConstructor("Throw grenade", AcSimple(cTGrenade, eTGrenade, 7));
	//
	// ConditionSet cCut = helper.MakeConditionSet({{"enemyStatus", new CEqual(EAVEnemyStatus::eInCloseCombatRadius)}});
	// ValueSet eCut = helper.MakeValueSet({{"enemyStatus", EAVEnemyStatus::eAttacking}});
	// planner.RegisterActionConstructor("Attack melee", AcSimple(cCut, eCut, 80));
	//
	//
	// planner.RegisterActionConstructor("Go to", AcGoTo(	planner.GetAttributeId("atNode"),
	// 															planner.GetAttributeId("isCrouching"),
	// 															planner.GetAttributeId("enemyStatus")));
	//
	// planner.RegisterActionConstructor("Heal", ACUseDepletable(	planner.GetAttributeId("hKitsLeft"),
	// 															planner.GetAttributeId("hpLeft"), 20, 3));
	//
	// planner.RegisterActionConstructor("Reload", ACUseDepletable(planner.GetAttributeId("magsLeft"),
	// 															planner.GetAttributeId("ammoInMagLeft"), 30, 3,
	// 															"magazine", "ammo"));
	//
	// planner.RegisterActionConstructor("Pickup health kit",
	// 								AcPickupDepletable(	planner.GetAttributeId("hKitsLeft"),
	// 													planner.GetAttributeId("atNode"),
	// 													navigator.GetNodesByName("MedStation"), 4,
	// 													"health kit"));
	//
	// planner.RegisterActionConstructor("Pickup magazine pack",
	// 							AcPickupDepletable(	planner.GetAttributeId("magsLeft"),
	// 												planner.GetAttributeId("atNode"),
	// 												navigator.GetNodesByName("AmmoBox"), 3,
	// 												"magazine pack"));
	//
	// planner.RegisterActionConstructor("Pickup grenade",
	// 							AcPickupDepletable(	planner.GetAttributeId("grenadesLeft"),
	// 												planner.GetAttributeId("atNode"),
	// 												navigator.GetNodesByName("AmmoBox"), 4,
	// 												"grenade"));
	//
	//
	// planner.RegisterActionConstructor("Search enemy", AcSearchEnemy(planner.GetAttributeId("enemyStatus"),
	// 	planner.GetAttributeId("atNode"),
	// 	planner.GetAttributeId("isCrouching"),
	// 	navigator.GetMaxDistance() * 5));
	//
	// ConditionSet gAttack = helper.MakeConditionSet({{"enemyStatus", new CEqual(EAVEnemyStatus::eAttacking)}});
	// planner.RegisterGoal("Attack enemy", gAttack);
	//
	// ConditionSet gHeal = helper.MakeConditionSet({{"hpLeft", new CGreater(59)}});
	// planner.RegisterGoal("Heal", gHeal);
	//
	// ConditionSet gRefillHKits = helper.MakeConditionSet({{"hKitsLeft", new CGreater(3)}});
	// planner.RegisterGoal("Refill health kits", gRefillHKits);
	//
	// ConditionSet gTakeCover = helper.MakeConditionSet({	{"isCrouching", new CEqual(true)},
	// 													{"atNode", new CInSet(navigator.GetNodesByName("Cover"))}});
	// planner.RegisterGoal("Take cover", gTakeCover);
	//
	// ValueSet init = helper.MakeValueSet({	{"isCrouching", true},
	// 										{"enemyStatus", EAVEnemyStatus::eNonVisible},
	// 										{"hpLeft", 20},
	// 										{"hKitsLeft", 1},
	// 										{"ammoInMagLeft", 0},
	// 										{"grenadesLeft", 0},
	// 										{"magsLeft", 0},
	// 										{"atNode", *navigator.GetNodesByName("AmmoBox").begin()}});
	//
	// SupplementalData initData;
	// initData.initNode = init.GetProperty(planner.GetAttributeId("atNode"));
	// initData.futureGoToDestinationNode = -1;
	// //initData.minimalNumHKits = (currentGoal.IsAffected(planner.GetAttributeId("hKitsLeft")) == true) ?
	// //	Helper::CastAssert<const CGreater>(currentGoal.GetProperty(planner.GetAttributeId("hKitsLeft")).get())->Value + 1 : 0;
	//
	// auto plan = planner.ConstructPlan(TODO, initData);
	// std::cout << "Initial state:\n";
	// planner.PrintValueSet(init);
	// std::cout << "\n";
	// if (plan.Success == true)
	// {
	// 	for (size_t i = 0; i < plan.ActionNames.size(); i++)
	// 		std::cout << i + 1 << ". " << plan.ActionNames[i] << " " << plan.ActionInstances[i].StringData << "\n";
	// 	std::cout << "\n\n";
	// 	std::cout << "Result state:\n";
	// 	planner.PrintValueSet(plan.ResultState);
	// }
	// else
	// {
	// 	std::cout << "Failed to build plan\n";
	// }
#pragma endregion
	return 0;
}


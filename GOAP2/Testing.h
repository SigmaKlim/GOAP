#pragma once
#include <random>
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>

#include "Planner.h"
#include "Helper.h"
#include "Actions/Basic/ACSimple.h"
#include "Actions/Specific/ACGoTo.h"

#include "Attributes/Basic/AEnum.h"
#include "Attributes/Special/AHealth.h"
#include "Attributes/Special/AHKitsLeft.h"
#include "Attributes/Special/AAmmoInMag.h"
#include "Attributes/Special/AAtNode.h"
#include "Attributes/Special/AMagsLeft.h"

#include "Condition/Special/CEqual.h"
#include "Condition/Special/CLarger.h"
#include "Condition/Special/CInSet.h"
#include "Navigation/Navigator.h"

#include "Tools/MathHelper.h"

#include "Navigation/NavPathfinder.h"
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
inline Navigator SetUpNavigator()
{
	Navigator navigator;
	navigator.AddNodes("AmmoBox", {1, 4});
	navigator.AddNodes("MedStation", {2, 3});
	navigator.IsSymmetric = true;
	navigator.SetDistance(1, 2, 3.0f);
	navigator.SetDistance(1, 3, 1.5f);
	navigator.SetDistance(1, 4, 2.5f);
	navigator.SetDistance(2, 3, 3.0f);
	navigator.SetDistance(2, 4, 2.0f);
	navigator.SetDistance(3, 4, 1.5f);
	return navigator;
}

enum EAtValcoverStatus
{
	eInCover,
	eNotInCover
};
enum EAtValBoolean
{
	eTRUE,
	eFALSE
};
enum EAtValenemyStatus
{
	eNonVisible,
	eVisible,
	eInRangedCombatRadius,
	eInCloseCombatRadius,
	eAttacking
};

inline int TestGoap()
{
	Planner planner;
	planner.RegisterAttribute<AEnum>("coverStatus");
	planner.RegisterAttribute<AEnum>("isCrouching");
	planner.RegisterAttribute<AEnum>("enemyStatus");
	planner.RegisterAttribute<AEnum>("isWeaponLoaded");
	planner.RegisterAttribute<AHealth>("hpLeft");
	planner.RegisterAttribute<AHKitsLeft>("hKitsLeft");
	planner.RegisterAttribute<AAmmoInMag>("ammoInMagLeft");
	planner.RegisterAttribute<AMagsLeft>("magsLeft");
	planner.RegisterAttribute<AAtNode>("atNode");

	Navigator navigator = SetUpNavigator(); 
	AAtNode::navigator = navigator;
	
	IActionConstructor::numAttributes = planner.GetNumAttributes(); //!!!
	
	Helper helper(&planner);
	
	ConditionSet	cHide = helper.MakeConditionSet	({{"coverStatus", new CEqual(EAtValcoverStatus::eInCover)}});
	ValueSet		eHide = helper.MakeValueSet		({{"isCrouching", EAtValBoolean::eTRUE}});
	planner.RegisterActionConstructor("Hide", ACSimple(cHide, eHide, 3));

	ConditionSet	cGoToCover = helper.MakeConditionSet	({});
	ValueSet		eGoToCover = helper.MakeValueSet		({{"coverStatus", EAtValcoverStatus::eInCover}});
	planner.RegisterActionConstructor("Go to cover", ACSimple(cGoToCover, eGoToCover, 3));

	ConditionSet	cSearch = helper.MakeConditionSet	({{"enemyStatus", new CEqual(EAtValenemyStatus::eNonVisible)}});
	ValueSet		eSearch = helper.MakeValueSet		({{"enemyStatus", EAtValenemyStatus::eVisible}});
	planner.RegisterActionConstructor("Search enemy", ACSimple(cSearch, eSearch, 10));

	ConditionSet	cEngage	= helper.MakeConditionSet	({{"enemyStatus", new CEqual(EAtValenemyStatus::eVisible)}});
	ValueSet		eEngage	= helper.MakeValueSet		({{"enemyStatus", EAtValenemyStatus::eInRangedCombatRadius}});
	planner.RegisterActionConstructor("Engage enemy", ACSimple(cEngage, eEngage, 4));

	ConditionSet	cApproach = helper.MakeConditionSet	({{"enemyStatus", new CEqual(EAtValenemyStatus::eInRangedCombatRadius)}});
	ValueSet		eApproach = helper.MakeValueSet		({{"enemyStatus", EAtValenemyStatus::eInCloseCombatRadius}});
	planner.RegisterActionConstructor("Approach enemy", ACSimple(cApproach, eApproach, 3));

	ConditionSet	cReload	= helper.MakeConditionSet	({{"ammoInMagLeft", new CLarger(5)}});
	ValueSet		eReload	= helper.MakeValueSet		({{"isWeaponLoaded", EAtValBoolean::eTRUE}});
	planner.RegisterActionConstructor("Reload weapon", ACSimple(cReload, eReload, 2));

	ConditionSet cShoot = helper.MakeConditionSet	({  {"enemyStatus", new CEqual(EAtValenemyStatus::eInRangedCombatRadius)},
														{"isWeaponLoaded", new CEqual(EAtValBoolean::eTRUE)}});
	ValueSet	eShoot	= helper.MakeValueSet		({	{"enemyStatus", EAtValenemyStatus::eAttacking}});
	planner.RegisterActionConstructor("Shoot enemy", ACSimple(cShoot, eShoot, 3));

	planner.RegisterActionConstructor("Go to", ACGoTo(	planner.GetAttributeId("atNode"),
																planner.GetAttributeId("isCrouching")));

	
	ConditionSet gHidden = helper.MakeConditionSet({{"coverStatus", new CEqual(EAtValcoverStatus::eInCover)},
													{"isCrouching", new CEqual(EAtValBoolean::eTRUE)}});
	planner.RegisterGoal("Stay hidden", gHidden);

	ConditionSet gAttack = helper.MakeConditionSet({{"enemyStatus", new CEqual(EAtValenemyStatus::eAttacking)}});
	planner.RegisterGoal("Attack enemy", gAttack);

	ConditionSet gHeal = helper.MakeConditionSet({{"hpLeft", new CLarger(75)}});
	planner.RegisterGoal("Heal", gHeal);

	ConditionSet gGoToAmmoBox = helper.MakeConditionSet({{"atNode", new CInSet(navigator.GetNodesByName("AmmoBox"))}});
	planner.RegisterGoal("Go to ammo box", gGoToAmmoBox);
	
	ValueSet init = helper.MakeValueSet({	{"coverStatus", EAtValcoverStatus::eNotInCover},
											{"isCrouching", EAtValBoolean::eFALSE},
											{"enemyStatus", EAtValenemyStatus::eNonVisible},
											{"isWeaponLoaded", EAtValBoolean::eFALSE},
											{"hpLeft", 100},
											{"hKitsLeft", 3},
											{"ammoInMagLeft", 30},
											{"magsLeft", 3},
											{"atNode", 3}});

	auto& currentGoal = gGoToAmmoBox;
	
	ActionData initData;
	initData.initNode = init.GetProperty(planner.GetAttributeId("atNode"));
	initData.futureGoToDestinationNode = -1;
	initData.minimalNumHKits = (currentGoal.IsAffected(planner.GetAttributeId("hKitsLeft")) == true) ?
		Helper::CastAssert<const CLarger>(currentGoal.GetProperty(planner.GetAttributeId("hKitsLeft")).get())->Value + 1 : 0;
	
	auto plan = planner.ConstructPlan(init, "Go to ammo box", initData);
	if (plan.success == true)
	{
		for (size_t i = 0; i < plan.ActionNames.size(); i++)
			std::cout << i + 1 << ". " << plan.ActionNames[i] << " " << plan.ActionStrings[i] << "\n";
	}
	else
	{
		std::cout << "Failed to build plan\n";
	}
	return 0;
}

/*inline int TestGoap()
{
	//Create navigation pathfinder
	// std::ifstream fin("ata/test_matrix_set_30_0.txt");
	// matrix distanceMatrix(30, std::vector<u_int>(30));
	// MathHelper::ReadMtrxFromFile(distanceMatrix, fin);
	// std::map<std::string, std::vector<unsigned>> pointNameToVertexIds;
	// pointNameToVertexIds.insert({"COVER", {1, 5, 9, 17}});
	// pointNameToVertexIds.insert({"AMMO_BOX", {2, 13, 19}});
	// pointNameToVertexIds.insert({"HEALING_STATION", {6, 14}});
	// NavPathfinder navPathfinder(distanceMatrix, pointNameToVertexIds);
	//Create navigator
	Navigator navigator;
	navigator.AddPoint("COVER");
	navigator.AddLocation("COVER", {1.0f, 2.0f, 0.0f});
	navigator.AddPoint("AMMO_BOX");
	navigator.AddLocation("AMMO_BOX", {4.0f, 2.0f, 0.0f});
	navigator.AddPoint("HEALING_STATION");
	navigator.AddLocation("HEALING_STATION", {5.0f, 2.0f, 0.0f});
	
	//0. Initialize planner
	GPlanner planner;
	//1. Register all attributes and enumerate their values
	LocationAttribute aLocation(navigator, {"ARBITRARY", "COVER", "HEALING_STATION", "AMMO_BOX"});
	planner.RegisterAttribute				("location", aLocation);
	SimpleAttribute aPose				({"CROUCHING", "STANDING"});
	planner.RegisterAttribute				("pose", aPose);
	SimpleAttribute aCoverStatus		({"IN_COVER", "NOT_IN_COVER"});
	planner.RegisterAttribute				("coverStatus", aCoverStatus);
	SimpleAttribute aWeaponDrawn		({"RIFLE", "KNIFE", "GRENADE"});
	planner.RegisterAttribute				("weaponDrawn",	aWeaponDrawn);
	SimpleAttribute aAmmoLeftMagazine	({"NO", "LOW", "AVERAGE", "FULL"});
	planner.RegisterAttribute				("ammoLeftInMagazine", aAmmoLeftMagazine);
	SimpleAttribute aEnemyStatus		({"IN_CLOSE_COMBAT_RANGE", "VISIBLE", "NON_VISIBLE", "DEAD"});
	planner.RegisterAttribute				("enemyStatus", aEnemyStatus);
	SimpleAttribute aHasGrenades		({"FALSE", "TRUE"});
	planner.RegisterAttribute				("hasGrenades", aHasGrenades);
	SimpleAttribute aHpLevel			({"LOW", "AVERAGE", "HIGH"});
	planner.RegisterAttribute				("hpLevel", aHpLevel);
	SimpleAttribute aAmmoLeftBag		({ "NO", "LOW", "AVERAGE", "FULL"});
	planner.RegisterAttribute				("ammoLeftInBag", aAmmoLeftBag);

	//2. Register all goals
	VectorAD gKillAD = {{"enemyStatus", {"DEAD"}}}; 
	WorldState gKill(gKillAD);
	planner.RegisterGoal("KillEnemy", gKill);
	
	//3. Define start state of the world
	VectorAD startAD = {	{"pose",				{"CROUCHING"}},
							{"location",				{"ARBITRARY"}},
							{"coverStatus",			{"IN_COVER"}},
							{"weaponDrawn",			{"KNIFE"}},
							{"ammoLeftInMagazine",	{"NO"}},
							{"enemyStatus",			{"NON_VISIBLE"}},
							{"ammoLeftInBag",		{"NO"}},
							{"hasGrenades",			{"FALSE"}},
							{"hpLevel",				{"AVERAGE"}}};
	WorldState start(startAD);
	
 	//4. Register all available actions by defining their conditions and effects
 	WorldState crouchCnd;
	VectorAD adv = {{"pose", {"CROUCHING"}}}; 
 	WorldState crouchEff(adv);
 	SimpleAction crouch(crouchCnd, crouchEff, 2.0f);
 	planner.RegisterAction("Crouch", crouch);

	VectorAD goToCndAD = {{"pose",	{"STANDING"}}};
  	WorldState goToCnd(goToCndAD);
  	GoToAction goTo(navigator, goToCnd);
  	planner.RegisterAction("GoTo", goTo);
	
	adv = {	{"location",		{"COVER"}},
			{"pose",		{"CROUCHING"}}};
	WorldState takeCoverEff(adv);
	adv = {{"coverStatus", {"IN_COVER"}}};
	WorldState takeCoverCnd(adv);
  	SimpleAction takeCover(takeCoverCnd, takeCoverEff, 2.0f);
  	planner.RegisterAction("TakeCover", takeCover);
  	
  	WorldState standUpCnd;
	adv = {	{"pose",		{"STANDING"}},
	 		{"coverStatus", {"NOT_IN_COVER"}}};
  	WorldState standUpEff(adv);
  	SimpleAction standUp(standUpCnd, standUpEff, 2.0f);
  	planner.RegisterAction("StandUp", standUp);
 	
  	WorldState drawRifleCnd;
	adv = { { "weaponDrawn", {"RIFLE"}} };
  	WorldState drawRifleEff(adv);
  	SimpleAction drawRifle(drawRifleCnd, drawRifleEff, 3.0f);
	planner.RegisterAction("DrawRifle", drawRifle);

  	WorldState drawKnifeCnd;
	adv = { { "weaponDrawn", {"KNIFE"} } };
  	WorldState drawKnifeEff(adv);
  	SimpleAction drawKnife(drawKnifeCnd, drawKnifeEff, 1.0f);
  	planner.RegisterAction("DrawKnife", drawKnife);
  	
	adv = { {"hasGrenades", {"TRUE"}} };
  	WorldState drawGrenadeCnd(adv);
	adv = { { "weaponDrawn", {"GRENADE"}} };
  	WorldState drawGrenadeEff(adv);
  	SimpleAction drawGrenade(drawGrenadeCnd, drawGrenadeEff, 2.0f);
  	planner.RegisterAction("DrawGrenade", drawGrenade);
  	
	adv = {	{"ammoLeftInBag",		{"AVERAGE", "FULL"}},
			{"weaponDrawn",			{"RIFLE"}} };
  	WorldState reloadCnd(adv);
	adv = { { "ammoLeftInMagazine",	{"FULL"} } };
  	WorldState reloadEff(adv);
  	SimpleAction reload(reloadCnd, reloadEff, 3.0f);
  	planner.RegisterAction("Reload", reload);
  	
	adv = { {"pose",		{"STANDING"}},
			{"enemyStatus", {"NON_VISIBLE"}} };
  	WorldState searchCnd(adv);
	adv = { {"enemyStatus", {"VISIBLE"}},
			{"location",	{"ARBITRARY"}} };
  	WorldState searchEff(adv);
  	SimpleAction search(searchCnd, searchEff, 10.0f);
  	planner.RegisterAction("SearchEnemy", search);
  	
	adv = { { "enemyStatus",	{"VISIBLE"} } };
  	WorldState approachCnd(adv);
	adv = { {"enemyStatus",		{"IN_CLOSE_COMBAT_RANGE"}},
			{"location",			{"ARBITRARY"}} };
  	WorldState approachEff(adv);
  	SimpleAction approachEnemy(approachCnd, approachEff, 7.0f);
  	planner.RegisterAction("ApproachEnemy", approachEnemy);
  	
	adv = { { "enemyStatus", {"IN_CLOSE_COMBAT_RANGE"} } };
  	WorldState moveAwayFromEnemyCnd(adv);
	adv = { {"enemyStatus", {"VISIBLE"}},
			{"location",		{"ARBITRARY"}} };
  	WorldState moveAwayFromEnemyEff(adv);
  	SimpleAction moveAwayFromEnemy(moveAwayFromEnemyCnd, moveAwayFromEnemyEff, 7.0f);
  	planner.RegisterAction("MoveAwayFromEnemy", moveAwayFromEnemy);
  	
	adv = { {"enemyStatus", {"VISIBLE"}},
			{"weaponDrawn",	{"GRENADE"}} };
  	WorldState attackGCnd(adv);
	adv = { {"enemyStatus", {"DEAD"}} };
  	WorldState attackGEff(adv);
  	SimpleAction attackGrenade(attackGCnd, attackGEff, 4.0f);
  	planner.RegisterAction("AttackGrenade", attackGrenade);
  	
	adv = { {"enemyStatus",			{"VISIBLE", "IN_CLOSE_COMBAT_RANGE"}},
			{"weaponDrawn",			{"RIFLE"}},
			{"ammoLeftInMagazine",	{"FULL"}} };
  	WorldState attackRCnd(adv);
	adv = { {"enemyStatus",	{"DEAD"}} };
  	WorldState attackREff(adv);
  	SimpleAction attackRifle(attackRCnd, attackREff, 2.0f);
  	planner.RegisterAction("AttackRifle", attackRifle);
  	
	adv = { {"enemyStatus", {"IN_CLOSE_COMBAT_RANGE"}},
			{"weaponDrawn", {"KNIFE"}} };
  	WorldState attackKCnd(adv);
	adv = { { "enemyStatus",	{"DEAD"} } };
  	WorldState attackKEff(adv);
  	SimpleAction attackKnife(attackKCnd, attackKEff, 2.0f);
  	planner.RegisterAction("AttackKnife", attackKnife);
	
	adv = { {"location", {"HEALING_STATION"}} };
  	WorldState healCnd(adv);
	adv = { {"hpLevel", {"HIGH"}} };
  	WorldState healEff(adv);
  	SimpleAction heal(healCnd, healEff, 5.0f);
  	planner.RegisterAction("Heal", heal);
	
	adv = { {"location",			{"AMMO_BOX"}} };
  	WorldState refillAmmoAndGrenadesCnd(adv);
	adv = { {"ammoLeftInBag",	{"FULL"}},
			{"hasGrenades",		{"TRUE"}} };
  	WorldState refillAmmoAndGrenadesEff(adv);
  	SimpleAction refillAmmoAndGrenades(refillAmmoAndGrenadesCnd, refillAmmoAndGrenadesEff, 5.0f);
  	planner.RegisterAction("RefillAmmoAndGrenades", refillAmmoAndGrenades);
	 //5. Pack the in-out structure 
	 Plan plan;
	 plan.StartingWs = start;
	 plan.GoalName = "KillEnemy";

 	//6.
 	
 	//6. Construct plan
 	TelemetryData telemetryData;
 	bool builtPlan = planner.ConstructPlan(plan, &telemetryData);
//
 	//6. Fetch results
 	if (builtPlan == true)
 	{
 		std::cout << "Starting state:\n";
 		for (auto* attributeName : planner.GetAttributeCatalogue().namesLoop)
 		{
 			if (plan.StartingWs.GetAttributeEnumeratorNames(*attributeName).size() != 1)
 				std::cout << "!";
 			std::cout << "\t" + *attributeName + ": " + plan.StartingWs.GetAttributeEnumeratorNames(*attributeName)[0] + "\n";
 		}
 		std::cout << "Goal:\n";
 		std::cout << "\t" + plan.GoalName + "\n";
 		std::cout << "Plan started:\n";
 		for (auto i = 0; i < plan.GetActionSequence().size(); i++)
 			std::cout << "\t" << std::to_string(i + 1) << ". " << plan.GetActionSequence()[i] << "\n";
 		std::cout << "Plan completed\n";
 		std::cout << "Cost: " << std::to_string(plan.GetCost() * planner.GetDistanceDenominator()) << "\n";
 		std::cout << "Memory used on stack: " << std::to_string(telemetryData.totalBytesUsed) << " bytes.\n";
 		std::cout << "Total vertices discovered: " << std::to_string(telemetryData.discoveredNum) << "\n";
 		std::cout << "Total vertices expanded: " << std::to_string(telemetryData.expandedNum) << "\n";
 	}
 	else
 		std::cout << "Could not construct a plan!";
 	return 0;
 }
*/
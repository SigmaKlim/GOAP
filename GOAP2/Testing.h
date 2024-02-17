#pragma once
#include <random>
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>
#include "Tools/MathHelper.h"

#include "GPlanner.h"
#include "BitMask.h"
#include "Actions/GoToAction.h"
#include "Actions/SimpleAction.h"
#include "Navigation/NavPathfinder.h"
#include "Attributes/LocationAttribute.h"
#include "Attributes/SimpleAttribute.h"
#include "Navigation/Navigator.h"
#pragma optimize( "", off )

typedef std::vector<AttributeData> VectorAD;

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
	
#pragma endregion
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
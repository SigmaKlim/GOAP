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
#include "NavPathfinder.h"
#include "Attributes/SimpleAttribute.h"
#pragma optimize( "", off )

typedef AttributeData ad;

inline int TestNumeric()
{
	srand(time(0));
	const std::vector<size_t> DIMS = { /*5, 15, 30, 100,*/ 500 };
	const int K = 5;
	const float DISC_CHANCE = 0.67f;
#pragma region fill_txt
	//for (const auto& dim : DIMS)
	//{
	//	for (auto k = 0; k < K; k++)
	//	{
	//		std::ofstream fout("test_data/test_matrix_set_" + std::to_string(dim) + "_" + std::to_string(k) + ".txt");
	//		if (fout.is_open() == false)
	//			return -1;
	//		std::ofstream fout_("test_data/test_adjacency_list_" + std::to_string(dim) + "_" + std::to_string(k) + ".txt");
	//		if (fout.is_open() == false)
	//			return -1;
	//		mtrx m;
	//		mtrx aList;
	//		MathHelper::MakeRndIncidenceMatrx(m, dim, DISC_CHANCE);
	//		MathHelper::ToAdjacencyList(m, aList);
	//		MathHelper::PrintMtrxToFile(m, fout, ',');
	//		MathHelper::PrintMtrxToFile(aList, fout_, ' ');
	//		fout.close();
	//		fout_.close();
	//	}
	//}
#pragma endregion
#pragma region numerical_tests
	//std::ofstream fout("test_data/results.txt");
	//for (const auto& dim : DIMS)
	//{
	//	if (fout.is_open() == false)
	//		return -1;
	//	for (auto k = 0; k < K; k++)
	//	{
	//		std::ifstream fin("test_data/test_matrix_set_" + std::to_string(dim) + "_" + std::to_string(k) + ".txt");
	//		if (fin.is_open() == false)
	//			return -1;
	//		NumPathfinder tpf(fin, dim);
	//		int start = 0;
	//		int finish = dim - 1;
	//		fin.close();
	//		Path<u_int> path;
	//		tpf.Pathfind(path, start, finish);
	//		std::cout << "dim = " + std::to_string(dim) + "\t k = " + std::to_string(k) + "\n";
	//		std::cout << "start = " + std::to_string(start) + "\t finish = " + std::to_string(finish) + "\n";
	//		std::cout << "path: ";
	//		if (path.vertices.empty())
	//			std::cout << "NONE\ncost = INFTY\n\n";
	//		else
	//		{
	//			for (auto& vertex : path.vertices)
	//				std::cout << vertex << " ";
	//			std::cout << "\n";
	//			std::cout << "cost = " << path.cost << "\n\n";
	//		}

	//		fout << "dim = " + std::to_string(dim) + "\t k = " + std::to_string(k) + "\n";
	//		fout << "start = " + std::to_string(start) + "\t finish = " + std::to_string(finish) + "\n";
	//		fout << "path: ";
	//		if (path.vertices.empty())
	//			fout << "NONE\ncost = INFTY\n\n";
	//		else
	//		{
	//			for (auto& vertex : path.vertices)
	//				fout << vertex << " ";
	//			fout << "\n";
	//			fout << "cost = " << path.cost << "\n\n";
	//		}
	//	}
	//}
#pragma endregion
	return 0;
}
inline int TestGoap()
{
	//Create navigation pathfinder
	std::ifstream fin("ata/test_matrix_set_30_0.txt");
	matrix distanceMatrix(30, std::vector<u_int>(30));
	MathHelper::ReadMtrxFromFile(distanceMatrix, fin);
	std::map<std::string, std::vector<unsigned>> pointNameToVertexIds;
	pointNameToVertexIds.insert({"COVER", {1, 5, 9, 17}});
	pointNameToVertexIds.insert({"AMMO_BOX", {2, 13, 19}});
	pointNameToVertexIds.insert({"HEALING_STATION", {6, 14}});
	NavPathfinder navPathfinder(distanceMatrix, pointNameToVertexIds);
	
	//0. Initialize planner
	GPlanner planner;
	//1. Register all attributes and enumerate their values
	SimpleAttribute aAtPoint			({"ARBITRARY", "COVER", "HEALING_STATION", "AMMO_BOX"});
	planner.RegisterAttribute				("atPoint", aAtPoint);
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
	WorldState gKill({ {"enemyStatus", {"DEAD"}} });
	planner.RegisterGoal("KillEnemy", gKill);
	
	//3. Define start state of the world
	WorldState start({	{"pose",				{"CROUCHING"}},
						{"atPoint",				{"ARBITRARY"}},
						{"coverStatus",			{"IN_COVER"}},
						{"weaponDrawn",			{"KNIFE"}},
						{"ammoLeftInMagazine",	{"NO"}},
						{"enemyStatus",			{"NON_VISIBLE"}},
						{"ammoLeftInBag",		{"NO"}},
						{"hasGrenades",			{"FALSE"}},
						{"hpLevel",				{"AVERAGE"}}});

// 	//4. Register all available actions by defining their conditions and effects
// 	WorldState crouchCnd;
// 	WorldState crouchEff({"pose", {"CROUCHING"}});
// 	SimpleAction crouch(crouchCnd, crouchEff, 2);
// 	planner.RegisterAction("Crouch", crouch);
// 	
// 	WorldState goToCnd({"pose", {"STANDING"}});
// 	GoToAction goTo(navPathfinder, goToCnd);
// 	planner.RegisterAction("GoTo", goTo);
//
// 	WorldState takeCoverCnd({	{"atPoint",		{"COVER"}},
// 								{"pose",		{"CROUCHING"}}});
// 	WorldState takeCoverEff(	{"coverStatus", {"IN_COVER"}});
// 	SimpleAction takeCover(takeCoverCnd, takeCoverEff, 2);
// 	planner.RegisterAction("TakeCover", takeCover);
// 	
// 	WorldState standUpCnd;
// 	WorldState standUpEff({		{"pose",		{"STANDING"}},
// 								{"coverStatus", {"NOT_IN_COVER"}}});
// 	SimpleAction standUp(standUpCnd, standUpEff, 2);
// 	planner.RegisterAction("StandUp", standUp);
// 	
// 	WorldState drawRifleCnd;
// 	WorldState drawRifleEff({"weaponDrawn", {"RIFLE"}});
// 	SimpleAction drawRifle(drawRifleCnd, drawRifleEff, 3);
// 	
// 	WorldState drawKnifeCnd;
// 	WorldState drawKnifeEff({"weaponDrawn", {"KNIFE"}});
// 	SimpleAction drawKnife(drawKnifeCnd, drawKnifeEff, 1);
// 	planner.RegisterAction("DrawKnife", drawKnife);
// 	
// 	WorldState drawGrenadeCnd({"hasGrenades", {"TRUE"}});
// 	WorldState drawGrenadeEff({"weaponDrawn", {"GRENADE"}});
// 	SimpleAction drawGrenade(drawGrenadeCnd, drawGrenadeEff, 2);
// 	planner.RegisterAction("DrawGrenade", drawGrenade);
// 	
// 	WorldState reloadCnd({	{"ammoLeftInBag",	{"AVERAGE", "FULL"}},
// 							{"weaponDrawn",		{"RIFLE"}}});
// 	WorldState reloadEff(	{"ammoLeftInMagazine", "FULL"});
// 	SimpleAction reload(reloadCnd, reloadEff, 3);
// 	planner.RegisterAction("Reload", reload);
// 	
// 	WorldState searchCnd({	{"pose",		{"STANDING"}},
// 											{"enemyStatus", {"NON_VISIBLE"}}});
// 	WorldState searchEff({	{"enemyStatus", {"VISIBLE"}},
// 							{"atPoint",		{"ARBITRARY"}}});
// 	SimpleAction search(searchCnd, searchEff, 10);
// 	planner.RegisterAction("SearchEnemy", search);
// 	
// 	WorldState approachCnd({"enemyStatus",	{"VISIBLE"}});
// 	WorldState approachEff({{"enemyStatus", {"IN_CLOSE_COMBAT_RANGE"}},
// 											{"atPoint", {"ARBITRARY"}}});
// 	SimpleAction approachEnemy(approachCnd, approachEff, 7);
// 	planner.RegisterAction("ApproachEnemy", approachEnemy);
// 	
// 	WorldState moveAwayFromEnemyCnd(	{"enemyStatus", {"IN_CLOSE_COMBAT_RANGE"}});
// 	WorldState moveAwayFromEnemyEff({	{"enemyStatus", {"VISIBLE"}},
// 										{"atPoint",{"ARBITRARY"}}});
// 	SimpleAction moveAwayFromEnemy(moveAwayFromEnemyCnd, moveAwayFromEnemyEff, 7);
// 	planner.RegisterAction("MoveAwayFromEnemy", moveAwayFromEnemy);
// 	
// 	WorldState attackGCnd({{	{"enemyStatus", "VISIBLE"},
// 								{"weaponDrawn","GRENADE"}}});
// 	WorldState attackGEff({"enemyStatus", "DEAD"});
// 	SimpleAction attackGrenade(attackGCnd, attackGEff, 4);
// 	planner.RegisterAction("AttackGrenade", attackGrenade);
// 	
// 	WorldState attackRCnd({	{"enemyStatus", {"VISIBLE", "IN_CLOSE_COMBAT_RANGE"}},
// 												{"weaponDrawn",{"RIFLE"}},
// 												{"ammoLeftInMagazine",{"FULL"}}});
// 	WorldState attackREff({"enemyStatus", "DEAD"});
// 	SimpleAction attackRifle(attackRCnd, attackREff, 2);
// 	planner.RegisterAction("AttackRifle", attackRifle);
// 	
// 	WorldState attackKCnd({ {"enemyStatus", {"IN_CLOSE_COMBAT_RANGE"}},
// 							{"weaponDrawn", {"KNIFE"}}});
// 	WorldState attackKEff({"enemyStatus", "DEAD"});
// 	SimpleAction attackKnife(attackKCnd, attackKEff, 2);
// 	planner.RegisterAction("AttackKnife", attackKnife);
//
// 	WorldState healCnd({"atPoint", "HEALING_STATION"});
// 	WorldState healEff({"hpLevel", "HIGH"});
// 	SimpleAction heal(healCnd, healEff, 5);
// 	planner.RegisterAction("Heal", heal);
//
// 	WorldState refillAmmoAndGrenadesCnd(	{"atPoint",			{"AMMO_BOX"}});
// 	WorldState refillAmmoAndGrenadesEff({	{"ammoLeftInBag",	{"FULL"}},
// 											{"hasGrenades",		{"TRUE"}}});
// 	SimpleAction refillAmmoAndGrenades(refillAmmoAndGrenadesCnd, refillAmmoAndGrenadesEff, 5);
// 	planner.RegisterAction("RefillAmmoAndGrenades", refillAmmoAndGrenades);
// 	//5. Pack the in-out structure 
// 	Plan plan;
// 	plan.StartingWs = start;
// 	plan.GoalName = "KillEnemy";
//
// 	//6.
// 	
// 	//6. Construct plan
// 	TelemetryData telemetryData;
// 	bool builtPlan = planner.ConstructPlan(plan, &telemetryData);
//
// 	//6. Fetch results
// 	if (builtPlan == true)
// 	{
// 		std::cout << "Starting state:\n";
// 		for (auto* attributeName : planner.GetAttributeCatalogue().namesLoop)
// 		{
// 			if (plan.StartingWs.GetAttributeEnumerators(*attributeName).size() != 1)
// 				std::cout << "!";
// 			std::cout << "\t" + *attributeName + ": " + plan.StartingWs.GetAttributeEnumerators(*attributeName)[0] + "\n";
// 		}
// 		std::cout << "Goal:\n";
// 		std::cout << "\t" + plan.GoalName + "\n";
// 		std::cout << "Plan started:\n";
// 		for (auto i = 0; i < plan.GetActionSequence().size(); i++)
// 			std::cout << "\t" << std::to_string(i + 1) << ". " << plan.GetActionSequence()[i] << "\n";
// 		std::cout << "Plan completed\n";
// 		std::cout << "Cost: " << std::to_string(plan.GetCost()) << "\n";
// 		std::cout << "Memory used on stack: " << std::to_string(telemetryData.totalBytesUsed) << " bytes.\n";
// 		std::cout << "Total vertices discovered: " << std::to_string(telemetryData.discoveredNum) << "\n";
// 		std::cout << "Total vertices expanded: " << std::to_string(telemetryData.expandedNum) << "\n";
// 	}
// 	else
// 		std::cout << "Could not construct a plan!";
// 	return 0;
// }
// inline int TestMask()
// {
// 	BitMask m1 = BitMask::MakeOne(100); //00..01
// 	BitMask m2(m1);
// 	bool areSame = (m1 == m2); //true
//
// 	BitMask m3 = m1 << 30;
// 	BitMask m4 = BitMask::MakeAllZeros(100); //00..0
// 	m4.SetBitValue(30, 1);
// 	areSame = (m3 == m4); //true
//
// 	BitMask m5 = BitMask::MakeAllOnes(50); //11..1
// 	m5.SetBitValue(25, 0);
// 	BitMask m6 = ~m5;
// 	areSame = (m6 == (BitMask::MakeOne(50) << 25)); //true
//
// 	BitMask m7 = BitMask::MakeAllZeros(1000);
// 	m7.Invert();
// 	BitMask m8 = m7;
// 	m7.SetBitValue(0, 0);
// 	bool is7LargerThan8 = (m7 > m8); //false

	return 0;
}
#pragma once
#include <random>
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>
#include "MathHelper.h"
#include "NumPathfinder.h"
#include "Plan.h"
#include "BitMask.h"

#pragma optimize( "", off )

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
	//0. Initialize planner
	GPlanner planner;
	//1. Register all attributes and enumerate their values
	planner.RegisterAttribute("location",		{"IN_COVER",
												 "NEAR_COVER",
												 "NOT_IN_COVER"});
	planner.RegisterAttribute("pose",			{"CROUCHING",
												 "STANDING"});
	planner.RegisterAttribute("isWeaponDrawn",	{"FALSE",
												 "TRUE"});
	planner.RegisterAttribute("isWeaponLoaded", {"FALSE",
												 "TRUE"});
	planner.RegisterAttribute("isKnifeDrawn",	{"FALSE",
												 "TRUE"});
	planner.RegisterAttribute("isGrenadeDrawn",	{"FALSE",
												 "TRUE"});
	planner.RegisterAttribute("enemyStatus",	{"IN_CLOSE_COMBAT_RANGE",
												 "VISIBLE",
												 "NON_VISIBLE",
												 "DEAD"});
	planner.RegisterAttribute("hasAmmo",		{"FALSE",
												 "TRUE"});
	planner.RegisterAttribute("hasGrenades",	{"FALSE",
												 "TRUE"});

	//2. Register all goals
	planner.RegisterGoal("GetToCover",t_attr_enum_map({	{"pose", "CROUCHING"},
														{"location", "IN_COVER"}}));
	planner.RegisterGoal("KillEnemy", t_attr_enum_map({	{"enemyStatus", "DEAD"}}));
	
	//3. Define start state of the world
	WorldState start(	t_attr_enum_map({	{"pose", "CROUCHING"},
											{"location", "NOT_IN_COVER"},
											{"isWeaponDrawn", "FALSE"},
											{"isWeaponLoaded", "FALSE"},
											{"isKnifeDrawn", "FALSE"},
											{"isGrenadeDrawn", "FALSE"},
											{"enemyStatus", "NON_VISIBLE"},
											{"hasAmmo", "TRUE"},
											{"hasGrenades", "FALSE"}}));

	//4. Register all available actions by defining their conditions and effects
	WorldState crouchCnd;
	WorldState crouchEff(t_attr_enum_map({{"pose","CROUCHING"}}));
	planner.RegisterAction("Crouch", crouchCnd, crouchEff, 2);
	
	WorldState goToCoverCnd(t_attr_enum_map({{"pose", "STANDING"}}));
	WorldState goToCoverEff(t_attr_enum_map({{"location", "NEAR_COVER"}}));
	planner.RegisterAction("GoToCover", goToCoverCnd, goToCoverEff, 7);

	WorldState takeCoverCnd(t_attr_enum_map({{"location", "NEAR_COVER"}}));
	WorldState takeCoverEff(t_attr_enum_map({{"location", "IN_COVER"}}));
	planner.RegisterAction("TakeCover", takeCoverCnd, takeCoverEff, 2);
	
	WorldState standUpCnd;
	WorldState standUpEff(t_attr_enum_map({{"pose", "STANDING"}}));
	planner.RegisterAction("StandUp", standUpCnd, standUpEff, 2);
	
	WorldState drawWeaponCnd;
	WorldState drawWeaponEff(t_attr_enum_map({{"isWeaponDrawn", "TRUE"}}));
	planner.RegisterAction("DrawWeapon", drawWeaponCnd, drawWeaponEff, 3);
	
	WorldState drawKnifeCnd;
	WorldState drawKnifeEff(t_attr_enum_map({{"isKnifeDrawn", "TRUE"}}));
	planner.RegisterAction("DrawKnife", drawKnifeCnd, drawKnifeEff, 1);
	
	WorldState drawGrenadeCnd(t_attr_enum_map({{"hasGrenades", "TRUE"}}));
	WorldState drawGrenadeEff(t_attr_enum_map({{"isGrenadeDrawn", "TRUE"}}));
	planner.RegisterAction("DrawGrenade", drawGrenadeCnd, drawGrenadeEff, 2);
	
	WorldState reloadCnd(t_attr_enum_map({	{"hasAmmo", "TRUE"}, 
											{"isWeaponDrawn", "TRUE"}}));
	WorldState reloadEff(t_attr_enum_map({	{"isWeaponLoaded", "TRUE"}}));
	planner.RegisterAction("Reload", reloadCnd, reloadEff, 3);
	
	WorldState searchCnd(t_attr_enum_map({	{"pose", "STANDING"},
											{"enemyStatus", "NON_VISIBLE"}}));
	WorldState searchEff(t_attr_enum_map({	{"enemyStatus", "VISIBLE"},
											{"location","NOT_IN_COVER"}}));
	planner.RegisterAction("SearchEnemy", searchCnd, searchEff, 10);
	
	WorldState approachCnd(t_attr_enum_map({{"enemyStatus", "VISIBLE"}}));
	WorldState approachEff(t_attr_enum_map({{"enemyStatus", "IN_CLOSE_COMBAT_RANGE"}}));
	planner.RegisterAction("ApproachEnemy", approachCnd, approachEff, 7);
	
	WorldState moveAwayFromEnemyCnd(t_attr_enum_map({{"enemyStatus", "IN_CLOSE_COMBAT_RANGE"}}));
	WorldState moveAwayFromEnemyEff(t_attr_enum_map({{"enemyStatus", "VISIBLE"}}));
	planner.RegisterAction("MoveAwayFromEnemy", moveAwayFromEnemyCnd, moveAwayFromEnemyEff, 7);
	
	WorldState attackGCnd(t_attr_enum_map({	{"enemyStatus", "VISIBLE"},
											{"isGrenadeDrawn","TRUE"}}));
	WorldState attackGEff(t_attr_enum_map({	{"enemyStatus", "DEAD"}}));
	planner.RegisterAction("AttackGrenade", attackGCnd, attackGEff, 4);
	
	WorldState attackWCnd(t_attr_enum_map({	{"enemyStatus", "VISIBLE"},
											{"isWeaponDrawn","TRUE"},
											{"isWeaponLoaded","TRUE"}}));
	WorldState attackWEff(t_attr_enum_map({	{"enemyStatus", "DEAD"}}));
	planner.RegisterAction("AttackWeapon", attackWCnd, attackWEff, 2);
	
	WorldState attackKCnd(t_attr_enum_map({	{"enemyStatus", "IN_CLOSE_COMBAT_RANGE"},
											{"isKnifeDrawn","TRUE"}}));
	WorldState attackKEff(t_attr_enum_map({	{"enemyStatus", "DEAD"}}));
	planner.RegisterAction("AttackKnife", attackKCnd, attackKEff, 2);
	
	//5. Pack the in-out structure 
	Plan plan;
	plan.StartingWs = start;
	plan.GoalName = "KillEnemy";

	//6. Construct plan
	TelemetryData telemetryData;
	bool builtPlan = planner.ConstructPlan(plan, &telemetryData);

	//6. Fetch results
	if (builtPlan == true)
	{
		std::cout << "Starting state:\n";
		auto attributeNames = WorldState::GetAttributeNamesSet();
		for (auto& attributeName : attributeNames)
		{
			if (plan.StartingWs.GetAttributeEnumerators(attributeName).size() != 1)
				std::cout << "!";
			std::cout << "\t" + attributeName + ": " + plan.StartingWs.GetAttributeEnumerators(attributeName)[0] + "\n";
		}
		std::cout << "Goal:\n";
		std::cout << "\t" + plan.GoalName + "\n";
		std::cout << "Plan started:\n";
		for (auto i = 0; i < plan.GetActionSequence().size(); i++)
			std::cout << "\t" << std::to_string(i) << ". " << plan.GetActionSequence()[i] << "\n";
		std::cout << "Plan completed\n";
		std::cout << "Cost: " << std::to_string(plan.GetCost()) << "\n";
		std::cout << "Memory used on stack: " << std::to_string(telemetryData.totalBytesUsed) << " bytes.\n";
		std::cout << "Total vertices discovered: " << std::to_string(telemetryData.discoveredNum) << "\n";
		std::cout << "Total vertices expanded: " << std::to_string(telemetryData.expandedNum) << "\n";
	}
	else
		std::cout << "Could not construct a plan!";
	return 0;
}

inline int TestMask()
{
	BitMask m1 = BitMask::MakeOne(100); //00..01
	BitMask m2(m1);
	bool areSame = (m1 == m2); //true

	BitMask m3 = m1 << 30;
	BitMask m4 = BitMask::MakeAllZeros(100); //00..0
	m4.SetBitValue(30, 1);
	areSame = (m3 == m4); //true

	BitMask m5 = BitMask::MakeAllOnes(50); //11..1
	m5.SetBitValue(25, 0);
	BitMask m6 = ~m5;
	areSame = (m6 == (BitMask::MakeOne(50) << 25)); //true

	BitMask m7 = BitMask::MakeAllZeros(1000);
	m7.Invert();
	BitMask m8 = m7;
	m7.SetBitValue(0, 0);
	bool is7LargerThan8 = (m7 > m8); //false

	return 0;
}
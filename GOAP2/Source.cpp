#include <random>
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>
#include "MathHelper.h"
#include "Pathfind.hpp"

#include "Testing.h"
#pragma optimize( "", off )

int main()
{
	return test();
}


//Planner planner;
//planner.AddAttribute("armedWithGun");
//planner.AddAttribute("armedWithBomb");
//planner.AddAttribute("enemyAlive");
//planner.AddAttribute("enemyVisible");
//planner.AddAttribute("enemyLinedUp");
//planner.AddAttribute("nearEnemy");
//planner.AddAttribute("weaponLoaded");
//planner.AddAttribute("alive");
//
//planner.AddAction("Scout", { {"armedWithGun", true} }, { {"enemyVisible", true} }, 1);
//planner.AddAction("Approach", { {"enemyVisible", true} }, { {"nearEnemy", true} }, 1);
//planner.AddAction("Aim", { {"enemyVisible", true}, {"weaponLoaded", true} }, { {"enemyLinedUp", true} }, 1);
//planner.AddAction("Shoot", { {"enemyLinedUp", true} }, { {"enemyAlive", false} }, 1);
//planner.AddAction("Load", { {"armedWithGun", true} }, { {"weaponLoaded", true} }, 1);
//planner.AddAction("DetonateBomb", { {"armedWithBomb", true} }, { {"enemyAlive", false}, {"alive", false} }, 1);
//planner.AddAction("Flee", { {"enemyVisible", true} }, { {"nearEnemy", false} }, 1);
//
//planner.AddGoal("KillEnemy", { { "enemyAlive", false } });
//
//WsMask wstate = planner.MakeWs();
//if (planner.FillInWs(wstate, {
//							{"armedWithGun", true},
//							{"armedWithBomb", true},
//							{"enemyAlive", true},
//							{"enemyVisible", false},
//							{"enemyLinedUp", false},
//							{"nearEnemy", false},
//							{"weaponLoaded", false},
//							{"alive", true}
//	}) == false)
//{
//	std::cout << "Could not create wstate. \n";
//	return -1;
//}
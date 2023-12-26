#pragma once
#include "World.h"
class NavPathfinder;
struct ActionInputDataBase
{
    const BitMask* DesiredStateMask; //value mask of the condition the action is trying to satisfy
    const BitMask* AffectedAttributesMask; //mask showing which attributes the action is going to affect
    
    NavPathfinder* NavPathfinder; //navigation pathfinder used to evaluate paths
    std::string DestinationPointName; //used to calculate path cost
};

#include "AStar.h"
#include "Action.h"

struct Vertex
{
    ConditionSet ActiveConditionSet; //A set of conditions required for all previously taken actions
    std::size_t PrevActionId;
    //debug
    std::string PrevActionName; 
};

//class Planner : public AStarSolver<>
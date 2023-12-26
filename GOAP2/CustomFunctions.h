#pragma once
#include "World.h"
#include "NavPathfinder.h"
inline WorldState MakeEffectFromDesiredState(ActionInputDataBase* data)
{
    return WorldState(*data->DesiredStateMask & *data->AffectedAttributesMask, *data->AffectedAttributesMask);
}

inline unsigned CalculateCostGoTo(ActionInputDataBase* data)
{
    auto currentVertex = data->NavPathfinder->EmulateGetCurrentVertex();
    auto solverData = std::pair<unsigned, unsigned>(currentVertex, data->DestinationVertex);
    auto solutionIt = data->NavPathfinder->calculatedPaths.find(solverData);
    if (solutionIt != data->NavPathfinder->calculatedPaths.end())
        return solutionIt->second.Cost;
    Path<unsigned> path;
    data->NavPathfinder->Pathfind(path, currentVertex, data->DestinationVertex);
    data->NavPathfinder->calculatedPaths.insert({solverData, path});
    return path.Cost;
}
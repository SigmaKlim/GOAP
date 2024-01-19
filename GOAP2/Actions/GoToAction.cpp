#include "GoToAction.h"

#include "../NavPathfinder.h"

GoToAction::GoToAction(NavPathfinder& navPathfinder, const WorldState& condition) : _navPathfinder(navPathfinder)
{
    _condition = condition;
}

WorldState GoToAction::GetCondition() const
{
    return WorldState(_condition);
}


WorldState GoToAction::GetEffect(EvaluateActionEffectInputBase* data) const
{
    WorldState effect;
    auto values = data->DesiredStateMask->GetAttributeValues(locationAttributeName);
    if (values.size() != 0)
    {
        effect.AddAttributeValue(locationAttributeName, values[0]);
        effect.AddAttributeValue(enemyStatusAttributeName, "NON_VISIBLE");
    }

    return effect;
}

unsigned GoToAction::GetCost(CalculateActionCostInputBase* data) const
{
    auto enums = data->prevState->GetAttributeEnumerators(locationAttributeName);
    assert(enums.size() == 1);
    auto attributeEffectValueEnumerator = enums[0];
    unsigned destinationVertex = _navPathfinder.EmulateGetDestinationVertexByName(attributeEffectValueEnumerator);
    unsigned currentVertex = _navPathfinder.EmulateGetCurrentVertex();
    auto it = _navPathfinder.calculatedPaths.find({currentVertex, destinationVertex});
    if (it != _navPathfinder.calculatedPaths.end())
        return it->second.Cost;
    Path<unsigned> path;
    _navPathfinder.Pathfind(path, currentVertex, destinationVertex);
    return path.Cost;
}

std::string GoToAction::GetPostfixName(const WorldState& desiredState) const
{
    auto enums = desiredState.GetAttributeEnumerators(locationAttributeName);
    assert(enums.size() == 1);
    return enums[0];
}

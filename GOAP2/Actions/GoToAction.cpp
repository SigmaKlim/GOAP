#include "GoToAction.h"

#include "../NavPathfinder.h"

GoToAction::GoToAction(NavPathfinder& navPathfinder, const WorldState& condition) : _navPathfinder(navPathfinder)
{
    _condition = condition;
}

WorldState GoToAction::GetCondition() const
{
    return _condition;
}


WorldState GoToAction::GetEffect(EvaluateActionEffectInputBase* data) const
{
   /* unsigned attributeIndex = WorldState::FindAttribute(atPointAttributeName);
    BitMask affectedAttributeMask = BitMask::MakeRightOnes(WorldState::GetAttributeNumber() * Attribute::MAX_VALUES, Attribute::MAX_VALUES);
    affectedAttributeMask <<= attributeIndex * Attribute::MAX_VALUES;
    WorldState effect(data->DesiredStateMask->GetValueMask() & affectedAttributeMask, affectedAttributeMask);*/
    //effect.SetAttributeValue(enemyStatusAttributeName, "NON_VISIBLE");
    WorldState effect;
    auto values = data->DesiredStateMask->GetAttributeValues(atPointAttributeName);
    if (values.size() != 0)
    {
        effect.AddAttributeValue(atPointAttributeName, values[0]);
        effect.AddAttributeValue(enemyStatusAttributeName, "NON_VISIBLE");
    }

    return effect;
}

unsigned GoToAction::GetCost(CalculateActionCostInputBase* data) const
{
    auto enums = data->prevState->GetAttributeEnumerators(atPointAttributeName);
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
    auto enums = desiredState.GetAttributeEnumerators(atPointAttributeName);
    assert(enums.size() == 1);
    return enums[0];
}

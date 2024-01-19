#include "GoToAction.h"

#include "../Navigation/Navigator.h"

GoToAction::GoToAction(Navigator& navigator, const WorldState& condition) : _navigator(navigator)
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
    auto enums = data->postState->GetAttributeEnumeratorNames(locationAttributeName);
    assert(enums.size() == 1);
    auto attributeEffectValueEnumerator = enums[0];
    Location currentLocation = {(float)rand()/ RAND_MAX  * 8.0f,
                                (float)rand()/ RAND_MAX  * 8.0f,
                                (float)rand()/ RAND_MAX  * 8.0f};
    return _navigator.GetMinEuclidianDistance(currentLocation, attributeEffectValueEnumerator);
    // unsigned currentVertex = _navPathfinder.EmulateGetCurrentVertex();
    // auto it = _navPathfinder.calculatedPaths.find({currentVertex, destinationVertex});
    // if (it != _navPathfinder.calculatedPaths.end())
    //     return it->second.Cost;
    //Path<unsigned> path;
    //_navPathfinder.Pathfind(path, currentVertex, destinationVertex);
    //return 0;
}

std::string GoToAction::GetPostfixName(const WorldState& desiredState) const
{
    auto enums = desiredState.GetAttributeEnumeratorNames(locationAttributeName);
    assert(enums.size() == 1);
    return enums[0];
}

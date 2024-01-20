#include "GoToAction.h"

#include "../Navigation/Navigator.h"
#include "../World/MyWsData.h"

GoToAction::GoToAction(Navigator& navigator, const WorldState& condition) : _navigator(navigator)
{
    _condition = condition;
}

WorldState GoToAction::GetCondition(const WorldState& preState) const
{
    return WorldState(_condition);
}


WorldState GoToAction::GetEffect(const WorldState& preState) const
{
    WorldState effect;
    auto values = preState.GetAttributeValues(locationAttributeName);
    if (values.size() != 0)
    {
        effect.AddAttributeValue(locationAttributeName, values[0]);
        effect.AddAttributeValue(enemyStatusAttributeName, "NON_VISIBLE");
    }

    return effect;
}

float GoToAction::GetCost(const WorldState& preState) const
{
    auto enums = preState.GetAttributeEnumeratorNames(locationAttributeName);
    assert(enums.size() == 1);
    auto attributeEffectValueEnumerator = enums[0];
    auto ud = std::dynamic_pointer_cast<MyWsData>(preState.UserData);
    if (ud->isLocationValid == false)
        return 0.0f;
    Location currentLocation = std::dynamic_pointer_cast<MyWsData>(preState.UserData)->location;
    return _navigator.GetClosestPoint(currentLocation, attributeEffectValueEnumerator).second;
}

std::string GoToAction::GetEffectPostfix(const WorldState& desiredState) const
{
    auto enums = desiredState.GetAttributeEnumeratorNames(locationAttributeName);
    assert(enums.size() == 1);
    return enums[0];
}

float GoToAction::GetHighestPossibleCost() const
{
    return _navigator.GetMaxEuclideanDistance();
}

WsUserData* GoToAction::ModifyUserData(const WorldState& preState) const
{
    auto enums = preState.GetAttributeEnumeratorNames(locationAttributeName);
    assert(enums.size() == 1);
    auto attributeEffectValueEnumerator = enums[0];
    auto pointName = enums[0];
    MyWsData* newData;
    auto d = dynamic_cast<MyWsData*>(preState.UserData.get());
    if (d == nullptr)
        newData = new MyWsData;
    else
        newData = new MyWsData(*d);
    newData->location =  _navigator.GetClosestPoint(newData->location, pointName).first;
    newData->isLocationValid = true;
    return newData;
}

//ДУРАК
//Нужно location в условия действий переносить, например, в RefillAmmunition, мы ведь при построении плана идем из будущего в прошлое
//
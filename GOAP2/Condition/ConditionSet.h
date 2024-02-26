#pragma once
#include <memory>
#include <vector>

#include "BCondition.h"
#include "PropertyList.h"
class WorldMask;


class ConditionSet : public PropertyList<std::shared_ptr<BCondition>>
{
public:
    ConditionSet(unsigned size) : PropertyList<std::shared_ptr<BCondition>>(size) {}
    template <typename T_Condition>
    void SetCondition(unsigned index, const T_Condition& condition)
    {
        assert(dynamic_cast<const BCondition*>(condition));
        SetProperty(index, std::make_shared<T_Condition>(condition));
    }
    //remove all conditions satisfied in world, if there are no such conditions, return false
    bool Reduce(const WorldMask& world, ConditionSet reducedConditionSet) const;
    

private:

};

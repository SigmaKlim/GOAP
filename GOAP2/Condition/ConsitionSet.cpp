#include "ConditionSet.h"
#include "../WorldMask.h"

bool ConditionSet::Reduce(const WorldMask& world, ConditionSet reducedConditionSet) const
{
    reducedConditionSet = *this;
    bool result = false;
    for (unsigned i = 0; i < Size(); i++)
    {
        if (IsAffected(i) == true && world.IsAffected(i) == true)
        {
            if(_properties[i]->Result(std::dynamic_pointer_cast<Equals>(world._properties[i])->GetAttribute()) == true)
            {
                reducedConditionSet.ClearValue(i);
                result = true;
            }
        }
    }
    return result;
}



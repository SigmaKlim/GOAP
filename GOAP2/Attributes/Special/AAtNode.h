#pragma once
#include "../Basic/IAttribute.h"
#include "../../Navigation/Navigator.h"

class AAtNode : public IAttribute
{
public:
    float GetDifference(t_value value1, t_value value2) const override
    {
        if (value1 == -1 || value2 == -1) // we reserve -1 value for cases when current node is unknown (like after action Patrol)
            return GetMaxDifference() / 2; //in such case we return the same value for any argument
        return (float) navigator.GetDistance(value1, value2);
    }
    float GetMaxDifference() const override
    {
        return (float)navigator.GetMaxDistance();
    }

    static Navigator navigator;
};
#pragma once
#include <assert.h>
#include "../Navigation/Navigator.h"

class LocationAttribute : public Attribute
{
public:
    LocationAttribute(Navigator& navigator, const std::vector<std::string>& enumeratorNames) : Attribute(enumeratorNames), _navigator(navigator) {}
    unsigned GetDifference(unsigned attributeConditionMask, unsigned attributeTargetMask) const override
    {
        if ((attributeConditionMask & attributeTargetMask) == attributeTargetMask)
            return 0;
        auto conditionValues = GetValues(attributeConditionMask);
        auto targetValues = GetValues(attributeTargetMask);
        assert(targetValues.size() == 1);
        auto targetValue = targetValues[0];
        float minDistance = std::numeric_limits<float>::max();
        for (auto& value : conditionValues)
        {
            //random location for debug!!!
            Location currentLocation = {(float)rand()/ RAND_MAX * 8.0f,
                                        (float)rand()/ RAND_MAX * 8.0f,
                                        (float)rand()/ RAND_MAX * 8.0f};
            float distance =_navigator.GetMinEuclideanDistance( currentLocation,
                                                                *GetEnumeratorName(value),
                                                                *GetEnumeratorName(targetValue));
            if (distance < minDistance)
                minDistance = distance;
        }
        return minDistance;
    }
private:
    Navigator& _navigator;
};

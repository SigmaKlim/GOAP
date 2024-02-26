#pragma once
#include <vector>
#include <variant>
#include <cassert>

#include "Condition/ConditionSet.h"
#include "Condition\ConditionImpl\Equals.h"


class WorldMask : public ConditionSet
{
public:
    WorldMask(unsigned size) : ConditionSet(size){}
    void SetAttributeValue(unsigned index, const BAttribute& attribute)
    {
        assert(index < Size());
        SetProperty(index, std::make_shared<Equals>(attribute));
    }

};

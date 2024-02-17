#pragma once
#include <cassert>

#include "../WorldMask.h"

class Condition
{
    virtual ~Condition() = default;
public:
    virtual bool Result(variant value, EValueType valueType) = 0;
};

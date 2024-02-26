#pragma once
#include "../Attributes/BAttribute.h"

class BCondition
{
public:
    virtual ~BCondition() = default;
    virtual bool Result(const BAttribute& attribute) = 0;
    
};

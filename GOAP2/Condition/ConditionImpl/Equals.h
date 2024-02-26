#pragma once

#include <cassert>
#include <memory>

#include "../BCondition.h"

class Equals : public BCondition
{
public:
    Equals(const BAttribute& attribute) : _attributePtr(std::make_shared<BAttribute>(attribute)) {}
    const BAttribute& GetAttribute() const
    {
        return *_attributePtr;
    }
    bool Result(const BAttribute& attribute) override
    {
        switch (_attributePtr->GetValueType())
        {
        case eBool:
            return _attributePtr->GetValue<bool>() == attribute.GetValue<bool>();
        case eInt:
            return _attributePtr->GetValue<int>() == attribute.GetValue<int>();
        case eFloat:
            return _attributePtr->GetValue<float>() == attribute.GetValue<float>();
        default:
            assert(false);
            break;
        }
        return false;
    }
private:
    std::shared_ptr<BAttribute> _attributePtr; 
};


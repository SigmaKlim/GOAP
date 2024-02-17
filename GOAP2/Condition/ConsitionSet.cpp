#include "ConditionSet.h"
std::vector<EValueType>* ConditionSet::_valueTypes = nullptr;


void ConditionSet::ClearCondition(unsigned index)
{
    assert(index < _valueTypes->size());
    _conditions[index].reset();
    _mask[index] = false;
    _affectedAttributesMask--;
}

bool ConditionSet::Satisfies(const WorldMask& world)
{
    for (unsigned i = 0; i < _valueTypes->size(); i++)
    {
        if (_mask[i] == true && world._mask[i] == true)
        {
            if(_conditions[i]->Result(world._values[i], world._valueTypes->at(i)) == false)
                return false;
        }
    }
    return true;
}

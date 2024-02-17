#include "WorldMask.h"

std::vector <EValueType>* WorldMask::_valueTypes = nullptr;

void WorldMask::ClearValue(unsigned index)
{
    assert(index < _valueTypes->size());
    _mask[index] = false;
    _affectedAttributesNum--;
}

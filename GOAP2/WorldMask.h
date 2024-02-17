#pragma once
#include <vector>
#include <variant>
#include <cassert>

typedef std::variant<bool, int, float> variant;

enum EValueType
{
    eNone,
    eBool,
    eInt,
    eFloat
};

class WorldMask
{
public:
    WorldMask() : _values(_valueTypes->size()), _mask(_valueTypes->size(), false), _affectedAttributesNum(0) {} 
    void ClearValue(unsigned index);
private:
    template <typename T>
    void SetValue(unsigned index, T value);
    std::vector<variant> _values;
    std::vector<bool> _mask; //shows which attributes are affected
    unsigned _affectedAttributesNum; //number of 1s in _mask
    
    static std::vector<EValueType>* _valueTypes;

    friend class ConditionSet;
};

template <typename T>
void WorldMask::SetValue(unsigned index, T value)
{
    assert(index < _valueTypes->size());
    switch (_valueTypes[index])
    {
        case eNone:
            assert(false);
            break;
        case eBool:
            _values[index] = static_cast<bool>(value);
            break;
        case eInt:
            _values[index] = static_cast<int>(value);
            break;
        case eFloat:
            _values[index] = static_cast<float>(value);
            break;
    }
    _mask[index] = true;
    _affectedAttributesNum++;
}

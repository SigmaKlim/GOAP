#pragma once
#include <cassert>
#include <variant>
enum EValueType
{
    eBool,
    eInt,
    eFloat,
    eNone
};

typedef std::variant<bool, int, float> variant;

class BAttribute
{
public:
    BAttribute(EValueType valueType) : _valueType(valueType) {}
    virtual ~BAttribute() = default;
    template <typename T>
    void SetValue(const T& value)
    {
        if (std::is_same_v<bool, T>)
            assert(_valueType == eBool);
        else if (std::is_same_v<int, T>)
            assert(_valueType == eInt);
        else if (std::is_same_v<float, T>)
            assert(_valueType == eFloat);
        else
            assert(false);
        _value = value;
    }
    
    template <typename T>
    const T& GetValue() const
    {
        return std::get<T>(_value);
    }
    
    EValueType GetValueType() const
    {
        return _valueType;
    }

    //Returns non-normalized difference between two values of the same attribute. Used to calculate World State heuristics
    virtual float GetDifference(const BAttribute& other) const = 0;
    //Returns maximal possible difference between any two values of this attribute. Used to normalize difference.
    virtual float GetMaxDifference() const = 0;
private:
    EValueType _valueType;
    variant _value;
    
};
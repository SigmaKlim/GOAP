#pragma once
#include "Condition.h"

template <typename T>
class Equals : public Condition
{
public:
    Equals(const T& referenceValue) : _referenceValue(referenceValue) {}
    bool Result(variant value, EValueType valueType) override;
private:
    T _referenceValue;
};

template <typename T>
bool Equals<T>::Result(variant value, EValueType valueType)
{
    return std::get<decltype(_referenceValue)>(value) == _referenceValue;
}

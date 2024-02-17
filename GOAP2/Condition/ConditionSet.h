#pragma once
#include <memory>
#include <vector>

#include "Condition.h"
#include "../WorldMask.h"


class ConditionSet
{
public:
    ConditionSet() : _conditions(_valueTypes->size(), nullptr), _mask(_valueTypes->size(), false), _affectedAttributesMask(0) {}
    template <typename t_cond>
    void SetCondition(unsigned index, const t_cond& condition);
    void ClearCondition(unsigned index);
    bool Satisfies(const WorldMask& world);
private:
    std::vector<std::shared_ptr<Condition>> _conditions;
    std::vector<bool> _mask;
    unsigned _affectedAttributesMask;

    static std::vector<EValueType>* _valueTypes;
};


template <typename t_cond>
void ConditionSet::SetCondition(unsigned index, const t_cond& condition)
{
    assert(index < _valueTypes->size());
    _conditions[index] = dynamic_cast<Condition*>(new t_cond(condition));
    assert(_conditions[index] != nullptr);
    _mask[index] = true;
    _affectedAttributesMask++;
}

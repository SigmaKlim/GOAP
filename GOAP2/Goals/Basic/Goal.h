#pragma once
#include "../../Conditions/Basic/ConditionSet.h"
class Goal
{
public:
    Goal(const ConditionSet& goalConditions, float initPriority = 0.0f) :
    _conditions(goalConditions), _priority(initPriority) {}
    virtual ~Goal() = default;
    virtual float UpdatePriority()
    {
        return _priority;
    }
    virtual ValueSet OverrideAgentState(const ValueSet& resultState)
    {
        return resultState;
    }
    inline const ConditionSet& GetConditions() const;
    inline float GetPriority() const;
protected:
    ConditionSet _conditions;
    float _priority;
};

inline const ConditionSet& Goal::GetConditions() const
{
    return _conditions;
}

inline float Goal::GetPriority() const
{
    return _priority;
}

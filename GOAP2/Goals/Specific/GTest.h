#pragma once
#include "../Basic/Goal.h"

class GTest : public Goal
{
public:
    GTest(const ConditionSet& goalConditions, size_t iAtNode, float initPriority = 0.0f) : Goal(goalConditions, initPriority),  _iAtNode(iAtNode){}
    
    float UpdatePriority() override
    {
        return ++_priority;
    }

    ValueSet OverrideAgentState(const ValueSet& resultState) override
    {
        auto overridenState = resultState;
        if (resultState.GetValue(_iAtNode) == -1)
            overridenState.SetValue(_iAtNode, 0);
        return overridenState;
    }
private:
    size_t _iAtNode;
};
#pragma once
#include "../Basic/IActionConstructor.h"
#include "../../Attributes/Special/AAtNode.h"
#include "../../Condition/Special/CEqual.h"
#include "../../Condition/Special/CInSet.h"

class ACGoTo : public IActionConstructor
{
public:
    ACGoTo(size_t atNodeIndex, size_t isCrouchingIndex) : _atNodeIndex(atNodeIndex), _isCrouchingIndex(isCrouchingIndex){}  
    void ConstructActions(std::vector<Action>& actions, const ConditionSet& requiredConditions, const ActionData& userData) override;

    float GetMaxCost() const override;

private:
    size_t _atNodeIndex;
    size_t _isCrouchingIndex;
};

inline void ACGoTo::ConstructActions(std::vector<Action>& actions, const ConditionSet& requiredConditions,
    const ActionData& userData)
{
    if (requiredConditions.IsAffected(_atNodeIndex) == true)
    {
        std::vector<int> destinations;
        auto* eq = dynamic_cast<const CEqual*>(requiredConditions.GetProperty(_atNodeIndex).get());
        auto inSet = dynamic_cast<const CInSet*>(requiredConditions.GetProperty(_atNodeIndex).get());
        if (eq != nullptr)
            destinations.push_back(eq->Value);
        else if (inSet != nullptr)
            for (auto& destination : inSet->Values)
                destinations.push_back(destination);
        else
            assert(0);
        for (auto& destinationNode : destinations)
        {
            ConditionSet cs(numAttributes);
            cs.SetCondition(_isCrouchingIndex, new CEqual(false)); //set isCrouching condition equal to false
            ValueSet vs(numAttributes);                                      
            vs.SetValue(_atNodeIndex, destinationNode);            //pick atNode attribute value from required conditions and set its effect accordingly
            float newCost = AAtNode::navigator.GetDistance(userData.initNode, destinationNode);
            if (userData.futureGoToDestinationNode != -1)
            {
                newCost +=  AAtNode::navigator.GetDistance(destinationNode, userData.futureGoToDestinationNode) -
                            AAtNode::navigator.GetDistance(userData.futureGoToDestinationNode, userData.initNode); //compensate our previous lack of knowledge of future GoTo departure point
            }
            ActionData newData(userData);
            newData.futureGoToDestinationNode = destinationNode;
            std::string stringData = "-> " + std::to_string(destinationNode);
            Action action(cs, vs, newCost, newData, stringData);//calculate cost of movement via navigator
            actions.push_back(action);
        }
    }
}

inline float ACGoTo::GetMaxCost() const
{
    return (float)AAtNode::navigator.GetMaxDistance();
}

#pragma once
#include "../Basic/IActionConstructor.h"
#include "../../Attributes/Special/AAtNode.h"
#include "../../Condition/Special/CEqual.h"
#include "../../Condition/Special/CInSet.h"
#include "../../Attributes/Special/Enums/EAVEnemyStatus.h"

class ACGoTo : public IActionConstructor
{
public:
    ACGoTo(size_t iAtNode, size_t isCrouchingIndex, size_t iEnemyStatus) : _iAtNode(iAtNode), _iIsCrouching(isCrouchingIndex), _iEnemyStatus(iEnemyStatus){}  
    void ConstructActions(std::vector<Action>& actions, const ConditionSet& requiredConditions, const SupplementalData& userData) override;

    float GetMaxCost() const override;

private:
    size_t _iAtNode;
    size_t _iIsCrouching;
    size_t _iEnemyStatus;
};

inline void ACGoTo::ConstructActions(std::vector<Action>& actions, const ConditionSet& requiredConditions,
    const SupplementalData& userData)
{
    if (requiredConditions.IsAffected(_iAtNode) == true)
    {
        std::vector<int> destinations;
        auto* eq = dynamic_cast<const CEqual*>(requiredConditions.GetProperty(_iAtNode).get());
        auto inSet = dynamic_cast<const CInSet*>(requiredConditions.GetProperty(_iAtNode).get());
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
            cs.SetCondition(_iIsCrouching, new CEqual(false)); //set isCrouching condition equal to false
            ValueSet vs(numAttributes);                                      
            vs.SetValue(_iAtNode, destinationNode);            //pick atNode attribute value from required conditions and set its effect accordingly
            vs.SetValue(_iEnemyStatus, EAVEnemyStatus::eNonVisible);
            float newCost = AAtNode::navigator.GetDistance(userData.initNode, destinationNode);
            if (userData.futureGoToDestinationNode != -1)
            {
                newCost +=  AAtNode::navigator.GetDistance(destinationNode, userData.futureGoToDestinationNode) -
                            AAtNode::navigator.GetDistance(userData.futureGoToDestinationNode, userData.initNode); //compensate our previous lack of knowledge of future GoTo departure point
            }
            SupplementalData newData(userData);
            newData.futureGoToDestinationNode = destinationNode;
            std::string stringData = "-> " + std::to_string(destinationNode) + ";";
            Action action(cs, vs, newCost, newData, stringData);//calculate cost of movement via navigator
            actions.push_back(action);
        }
    }
}

inline float ACGoTo::GetMaxCost() const
{
    return AAtNode::navigator.GetMaxDistance();
}

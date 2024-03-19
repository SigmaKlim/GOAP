#pragma once
#include "../Basic/IAction.h"
#include "../../Attributes/Special/AAtNode.h"
#include "../../Conditions/Special/CEqual.h"
#include "../../Conditions/Special/CInSet.h"
#include "../../Attributes/Special/AEnemyStatus.h"

//In order to use, AAtNode must anly be under InSet condition
class AcGoTo : public IAction
{
public:
    AcGoTo(size_t iAtNode, size_t isCrouchingIndex, size_t iEnemyStatus) : _iAtNode(iAtNode), _iIsCrouching(isCrouchingIndex), _iEnemyStatus(iEnemyStatus){}  
    void ConstructActionInstancesPriori(std::vector<ActionInstanceData>& actions, const ConditionSet& requiredConditions, const SupplementalData& userData) override;
    ActionInstanceData ConstructActionInstancePosteriori(const ValueSet& prevState, const ActionInstanceData& prioriActionInstance) override;
    float GetMaxCost() const override;

private:
    size_t _iAtNode;
    size_t _iIsCrouching;
    size_t _iEnemyStatus;
};

inline void AcGoTo::ConstructActionInstancesPriori(std::vector<ActionInstanceData>& actions, const ConditionSet& requiredConditions,
    const SupplementalData& userData)
{
    if (requiredConditions.IsAffected(_iAtNode) == true)
    {
        std::vector<int> destinations;
        auto* inSet = static_cast<const CInSet*>(requiredConditions.GetProperty(_iAtNode).get());
        for (auto& destination : inSet->Values)
            destinations.push_back(destination);
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
            ActionInstanceData action(cs, vs, newCost, newData, stringData);//calculate cost of movement via navigator
            actions.push_back(action);
        }
    }
}

inline ActionInstanceData AcGoTo::ConstructActionInstancePosteriori(
    const ValueSet& prevState, const ActionInstanceData& prioriActionInstance)
{
    auto posterioriActionInstance = IAction::ConstructActionInstancePosteriori(prevState, prioriActionInstance);
    int destinationNode = prioriActionInstance.Effects.GetProperty(_iAtNode);
    int trueDepartureNode = prevState.GetProperty(_iAtNode); //now we know it
    posterioriActionInstance.Cost = AAtNode::navigator.GetDistance(trueDepartureNode, destinationNode); //true cost of goto action
    return posterioriActionInstance;
}


inline float AcGoTo::GetMaxCost() const
{
    return AAtNode::navigator.GetMaxDistance();
}

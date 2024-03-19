#pragma once


#include "AStar.h"
#include "Tools/Catalogue.h"
#include "Actions/Basic/IAction.h"



struct Plan
{
    Plan(size_t numAttributes);
    ValueSet StartState;
    std::string GoalName;
    
    ConditionSet Goal;

    bool Success = false;
    std::vector<size_t> ActionIds;
    std::vector<std::string> ActionNames;
    std::vector<ActionInstanceData> ActionInstances; //posteriori instances
    
    float TotalCost;
    ValueSet ResultState;
    TelemetryData TelemetryData;
};

struct Vertex
{
    ConditionSet ActiveConditionSet; //A set of conditions required for all previously taken actions
    std::size_t PrevActionId;
    ActionInstanceData PrevActionInstance; //priori previous action instance
    unsigned ActionCtr; //counter of previously included actions
    
    //debug
    std::string PrevActionName;
};

template<>
size_t VertexKey<Vertex>::operator()(const Vertex& k) const;


class Planner : public AStarSolver<Vertex>
{
public:
    Planner(const std::vector<std::shared_ptr<IAttribute>>& attributes, const std::vector<std::shared_ptr<IAction>>& actions, const std::vector<const std::string*>& actionNames);
    void GetNeighbors(std::vector<Vertex>& neighbors, std::vector<float>& distances, const Vertex& vertex, const Vertex& finish) const override;
    bool Satisfies(const Vertex& vertex, const Vertex& finish) const override;
    float GetDistanceDenominator() const override;
    float GetHeuristic(const Vertex& vertex, const Vertex& finish) const override;
    float GetHeuristicsDenominator() const override;

    // template <typename t_attribute>
    // void RegisterAttribute(const std::string& name);
    // template <typename t_action>
    // void RegisterActionConstructor(const std::string& name, const t_action& actionConstructor);
    // void RegisterGoal(const std::string& name, const ConditionSet& goalState);

    //For convenient use
    bool ConstructPlan(Plan& plan, SupplementalData initData) const;
    //size_t GetNumAttributes() const;
    //size_t GetAttributeId(const std::string& name) const;
    //void PrintValueSet(const ValueSet& vs) const;
private:
    const std::vector<std::shared_ptr<IAttribute>>& _attributes;
    const std::vector<std::shared_ptr<IAction>>& _actions;
    //debug
    const std::vector<const std::string*>& _actionNames;
    
    // Catalogue<IAttribute*> _attributeCatalogue;
    // Catalogue<IAction*> _actionCatalogue;
    // Catalogue<ConditionSet> _goalCatalogue;

    const unsigned MAX_NUM_ACTIONS_PER_PLAN = 10;
    friend Plan;

};

// template <typename t_attribute>
// void Planner::RegisterAttribute(const std::string& name)
// {
//     IAttribute* iAttributePtr = static_cast<IAttribute*>(new t_attribute);
//     assert(_attributeCatalogue.AddItem(name, iAttributePtr));
// }
//
// template <typename t_action>
// void Planner::RegisterActionConstructor(const std::string& name, const t_action& actionConstructor)
// {
//     auto* iActionConstructor = static_cast<IAction*>(new t_action(actionConstructor));
//     assert(_actionCatalogue.AddItem(name, iActionConstructor));
// }
//
// inline void Planner::RegisterGoal(const std::string& name, const ConditionSet& goalState)
// {
//     assert(_goalCatalogue.AddItem(name, goalState));    
// }
#pragma once
#include <memory>

#include "Tools/Catalogue.h"
#include "Attributes/Basic/IAttribute.h"
#include "Goals/Basic/Goal.h"
#include "Planner.h"
#include "Actions/Performers/IActionPerformer.h"
#include "Debugger.h"

struct RebuildStatus
{
    bool mustRebuild = false;
    bool mustDropCurrentGoal = false;
};

struct Strategy
{
    std::vector<size_t> GoalIds;
};

class GController
{
public:
    GController();
    void Update();
    void UpdateGoalPriority(const std::string& name, bool mustRebuildStrategy);
    void RequestStrategyRebuild();
    size_t GetNumAttributes() const;
    size_t GetAttributeId(const std::string& name) const;
    SupplementalData GenerateSupData() const;
    //Set value in the agent state. Can be called in BeginPlay or on goal completion
    
private:
    void ConstructStrategy(Strategy& strategy) const;
    Catalogue<std::shared_ptr<IAttribute>> _attributeCatalogue;
    Catalogue<std::shared_ptr<Goal>> _goalCatalogue;
    Catalogue<std::shared_ptr<IAction>> _actionCatalogue;
    std::vector<std::shared_ptr<IActionPerformer>> _actionPerformers;
    Planner _planner;

    ValueSet _agentState;
    int _currentActionIndex = -1;
    Plan _currentPlan;
    int _currentGoalIndex = -1;
    Strategy _currentStrategy;
    friend class Helper;

    bool _mustBuildStrategy = true;
    bool _isGoalFinished = false;
    bool _mustBuildPlan = true;

    Debugger _debugger;
    
    friend Debugger;
    
};

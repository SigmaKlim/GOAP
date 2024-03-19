#include "GController.h"
#include <numeric>

GController::GController() : _planner(  _attributeCatalogue.oRange.Objects,
                                        _actionCatalogue.oRange.Objects,
                                        _actionCatalogue.nRange.Names),
                            _agentState(0), _currentPlan(0), _debugger(*this)
{
}


void GController::Update()
{
    if (_mustBuildStrategy == true)
    {
        ConstructStrategy(_currentStrategy);
        _currentGoalIndex = 0;
        _mustBuildStrategy = false;
        _isGoalFinished = true;
        //debug
        std::cout << "Strategy rebuild called!.\n";
        //
    }
    if(_isGoalFinished == true)
    {
        //debug
        static bool skipNextDebugPrint = true;
        if (skipNextDebugPrint == false)
            std::cout << "Goal " + _currentPlan.GoalName + " completed!\n";
        else
            skipNextDebugPrint = false;
        //
        _currentPlan = Plan(GetNumAttributes());
        _currentPlan.Goal = (*_goalCatalogue.GetItem(_currentStrategy.GoalIds[_currentGoalIndex]))->GetConditions();
        _currentPlan.GoalName = (*_goalCatalogue.GetName(_currentStrategy.GoalIds[_currentGoalIndex]));
        _currentPlan.StartState = _agentState;
        _isGoalFinished = false;
        assert(_planner.ConstructPlan(_currentPlan, GenerateSupData()));
        _currentActionIndex = 0;
        //debug
        std::cout << "Plan for goal " << _currentPlan.GoalName << " constructed!\n";
        _debugger.PrintPlan(_currentPlan);
        //
    }
    if (_currentPlan.ActionIds.size() == 0 || _actionPerformers[_currentPlan.ActionIds[_currentActionIndex]]->PerformAction() == true) //if we complete the action
    {
        _currentActionIndex++; //move on to the next action in the plan
        if (_currentActionIndex >= _currentPlan.ActionIds.size()) //if we complete the plan
        {
            (*_goalCatalogue.GetItem(_currentStrategy.GoalIds[_currentGoalIndex]))->UpdatePriority();
            _currentGoalIndex = (_currentGoalIndex + 1) % _goalCatalogue.Size();
            _agentState = (*_goalCatalogue.GetItem(_currentStrategy.GoalIds[_currentGoalIndex]))->OverrideAgentState(_currentPlan.ResultState);
            _isGoalFinished = true;
            //debug
            std::cout << "\nResult state:\n";
            _debugger.PrintValueSet(_agentState);
            std::cout << "\n";
            //
        }
    }
}

void GController::UpdateGoalPriority(const std::string& name, bool mustRebuildStrategy)
{
    auto goalPtr = *_goalCatalogue.GetItem(name);
    assert(goalPtr != nullptr);
    goalPtr->UpdatePriority();
    _mustBuildStrategy = mustRebuildStrategy;
}

void GController::RequestStrategyRebuild()
{
    _mustBuildStrategy = true;
}

void GController::ConstructStrategy(Strategy& strategy) const
{
    strategy.GoalIds = std::vector<size_t>(_goalCatalogue.Size());
    std::iota(strategy.GoalIds.begin(), strategy.GoalIds.end(), 0);
    auto& g = _goalCatalogue.oRange.Objects;
    std::sort(strategy.GoalIds.begin(), strategy.GoalIds.end(), [&g](size_t i1, size_t i2)
    {
        return g[i1]->GetPriority() > g[i2]->GetPriority();
    });
}

size_t GController::GetNumAttributes() const
{
    return _attributeCatalogue.Size();
}

size_t GController::GetAttributeId(const std::string& name) const
{
    auto* idPtr = _attributeCatalogue.GetId(name);
    assert(idPtr);
    return *idPtr;
}

SupplementalData GController::GenerateSupData() const
{
    SupplementalData data;
    data.initNode = _agentState.GetValue(GetAttributeId("atNode"));
    data.futureGoToDestinationNode = -1;
    return data;
}



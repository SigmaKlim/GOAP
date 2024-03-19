#include "Helper.h"

Helper::Helper(GController& gController) : _gController(gController)
{
    
}

void Helper::RegisterAttribute(const std::string& name, IAttribute* attributePtr)
{
    assert(_gController._attributeCatalogue.AddItem(name, std::shared_ptr<IAttribute>(attributePtr)));
}

void Helper::RegisterGoal(const std::string& name, Goal* goal)
{
    assert(_gController._goalCatalogue.AddItem(name, std::shared_ptr<Goal>(goal)));
}

void Helper::RegisterAction(const std::string& name, IAction* actionPtr, IActionPerformer* actionPerformerPtr)
{
    assert(_gController._actionCatalogue.AddItem(name, std::shared_ptr<IAction>(actionPtr)));
    _gController._actionPerformers.push_back(std::shared_ptr<IActionPerformer>(actionPerformerPtr));
}

void Helper::InitState(const ValueSet& state)
{
    assert(state.NumAffected() == state.Size());
    _gController._agentState = state;
}

void Helper::OverrideState(const std::string& attributeName, t_value value)
{
    size_t aId = _gController.GetAttributeId(attributeName);
    _gController._agentState.SetValue(aId, value);
}

ConditionSet Helper::MakeConditionSet(const std::map<std::string, ICondition*>& input) const
{
    ConditionSet cs(_gController.GetNumAttributes());
    for (auto& pair : input)
    {
        cs.SetCondition(_gController.GetAttributeId(pair.first), pair.second);
    }
    return cs;
}

ValueSet Helper::MakeValueSet(const std::map<std::string, t_value>& input) const
{
    ValueSet vs(_gController.GetNumAttributes());
    for (auto& pair : input)
        vs.SetValue(_gController.GetAttributeId(pair.first), pair.second);
    return vs;
}

#pragma once

#include "GController.h"

class Helper
{
public:
    Helper(GController& gController);

    void RegisterAttribute(const std::string& name, IAttribute* attributePtr);
    void RegisterGoal(const std::string& name, Goal* goalPtr);
    void RegisterAction(const std::string& name, IAction* actionPtr, IActionPerformer* actionPerformerPtr);

    void InitState(const ValueSet& state);
    void OverrideState(const std::string& attributeName, t_value value);
    
    //input: attributeName->ICondition*
    ConditionSet MakeConditionSet(const std::map<std::string, ICondition*>& input) const;
    //input: attributeName->attributeValue
    ValueSet MakeValueSet(const std::map<std::string, t_value>& input) const;
    

private:
    GController& _gController;
};
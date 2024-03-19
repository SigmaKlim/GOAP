#include "Debugger.h"
#include "GController.h"
Debugger::Debugger(const GController& gController) : _gController(gController)
{
}

void Debugger::PrintValueSet(const ValueSet& vs) const
{
    for (size_t i = 0; i < vs.Size(); i++)
        if (vs.IsAffected(i))
            std::cout << *_gController._attributeCatalogue.GetName(i) << ": " <<
                (*_gController._attributeCatalogue.GetItem(i))->GetEnumeratorString(vs.GetProperty(i)) << "\n";
}

void Debugger::PrintPlan(const Plan& plan) const
{
    for (size_t i = 0; i < plan.ActionNames.size(); i++)
        std::cout << i + 1 << ". " << plan.ActionNames[i] << " " << plan.ActionInstances[i].StringData << "\n";
}


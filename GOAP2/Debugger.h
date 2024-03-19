#pragma once
#include "ValueSet.h"

struct Plan;
class GController;

class Debugger
{
public:
    Debugger(const GController& gController);
    void PrintValueSet(const ValueSet& vs) const;
    void PrintPlan(const Plan& plan) const;
private:
    const GController& _gController;
};

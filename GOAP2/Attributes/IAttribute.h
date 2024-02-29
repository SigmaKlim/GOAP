﻿#pragma once

typedef int t_value;

class IAttribute
{
public:
    IAttribute() = default;
    virtual ~IAttribute() = default;

    //Returns non-normalized difference between this attribute value and 'value'. Used to calculate World State heuristics
    virtual float GetDifference(t_value value1, t_value value2) const = 0;
    //Returns maximal possible difference between any two values of this attribute. Used to normalize difference.
    virtual float GetMaxDifference() const = 0;
    
};
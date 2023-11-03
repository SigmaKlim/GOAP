#include "World.h"
#include "Action.h"
#include <bitset>
#include "Plan.h"

std::set<std::string> WorldState::_attributeNames;
unsigned WorldState::_numAttributes = 0;
GPlanner* WorldState::_planner = nullptr;

WorldState::WorldState(const t_attr_enum_map& nameValuePairs)
{
	_valueMask = BitMask::MakeAllZeros(_numAttributes * Attribute::MAX_VALUES);
	_affectedAttributesMask = BitMask::MakeAllZeros(_numAttributes * Attribute::MAX_VALUES);
	for (auto& nameValuePair : nameValuePairs)
	{
		const std::string& attributeName = nameValuePair.first;
		const std::string& enumeratorName = nameValuePair.second;
		const auto& attribute = _planner->GetAttribute(attributeName);
		auto attributeOffset = FindAttribute(attributeName);
		if (attributeOffset == _numAttributes)
		{
			std::cout << "Incorrect attribute name: " + attributeName + "\n";
			exit(-1);
		}
		BitMask affectedAttributeCell = BitMask::MakeRightOnes(_numAttributes * Attribute::MAX_VALUES, Attribute::MAX_VALUES);
		affectedAttributeCell <<= (attributeOffset *  Attribute::MAX_VALUES);
		_affectedAttributesMask |= affectedAttributeCell;
		u_char valueOffset = attribute.GetEnumValue(enumeratorName);
		if (valueOffset == Attribute::MAX_VALUES)
		{
			std::cout << "Incorrect enumerator name: " + enumeratorName + "\n";
			exit(-1);
		}
		_valueMask.SetBitValue(attributeOffset *  Attribute::MAX_VALUES + valueOffset, 1);
	}
}
WorldState::WorldState()
{
	_valueMask = BitMask::MakeAllZeros(_numAttributes * Attribute::MAX_VALUES);
	_affectedAttributesMask = BitMask::MakeAllZeros(_numAttributes * Attribute::MAX_VALUES);
}
WorldState::WorldState(const WorldState& other)
{
	_valueMask = other._valueMask;
	_affectedAttributesMask = other._affectedAttributesMask;
}
WorldState& WorldState::operator=(const WorldState& other)
{
	_valueMask = other._valueMask;
	return *this;
}
WorldState::~WorldState()
{
}
bool WorldState::SetAttributeValue(const std::string& name, u_char value)
{
	auto attributeOffset = FindAttribute(name);
	if (attributeOffset == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	_valueMask.SetBitValue(attributeOffset + value, 1);
	return true;
}
bool WorldState::SetAttributeValue(const unsigned index, u_char value)
{
	if (index > _numAttributes)
	{
		std::cout << "Index(" + std::to_string(index) + ") exceeds numAttributes(" +
			std::to_string(_numAttributes) + ").\n";
		exit(-1);
	}
	_valueMask.SetBitValue(index + value, 1);
	return true;
}
u_char WorldState::GetAttributeValue(const unsigned index) const
{
	if (index > _numAttributes)
	{
		std::cout << "Index(" + std::to_string(index) + ") exceeds numAttributes(" +
			std::to_string(_numAttributes) + ").\n";
		exit(-1);
	}
	BitMask copyMask = _valueMask;
	copyMask >>= index * Attribute::MAX_VALUES;
	const BitMask FILTER_MASK = BitMask::MakeRightOnes(_valueMask.GetNumBits(), Attribute::MAX_VALUES);
	copyMask &= FILTER_MASK;
	return _valueMask.GetBitValue(index);
}
u_char WorldState::GetAttributeValue(const std::string& name) const
{
	auto pos = FindAttribute(name);
	if (pos == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	return _valueMask.GetBitValue(pos);
}

const std::string& WorldState::GetAttributeEnumerator(const std::string& name) const
{
	auto pos = FindAttribute(name);
	if (pos == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	auto value = _valueMask.GetBitValue(pos);
	const auto& attribute = _planner->GetAttribute(name);
	const auto& enumerator = attribute.GetEnumerator(value); 
	return enumerator;
}


// t_mask WorldState::GetMask() const
// {
// 	return _valueMask.GetMask();
// }
unsigned WorldState::FindAttribute(const std::string& name)
{
	auto search =  _attributeNames.find(name);
	return std::distance(_attributeNames.begin(), search);
}

// unsigned WorldState::GetNumAttributes() const
// {
// 	return _numAttributes;
// }

bool WorldState::IsActionUseful(WorldState& modifiedConditionSet, const WorldState& conditionSet, const Action& action)
{
	BitMask significantConditionSet = conditionSet._valueMask & action._effect._affectedAttributesMask; //leave only conditions affected by effects of the action
	BitMask significantActionEffects = conditionSet._affectedAttributesMask & action._effect._valueMask; //leave only effects that influence conditions from the condition set
	if ((significantActionEffects & significantConditionSet) != significantActionEffects 
		|| significantConditionSet == BitMask::MakeAllZeros(significantConditionSet.GetNumBits())) // check if the action effects don't violate conditions from the set and if there are any affected conditions at all 
		return false;
	modifiedConditionSet._valueMask = conditionSet._valueMask & ~action._effect._valueMask; //remove fulfilled conditions form the set
	modifiedConditionSet._affectedAttributesMask = conditionSet._affectedAttributesMask & ~action._effect._affectedAttributesMask; //remove fulfilled conditions form the set
	BitMask significantActionConditions = conditionSet._affectedAttributesMask & action._condition._affectedAttributesMask; //leave only conditions on attributes shared between action conditions and conditions set
	if ((significantActionConditions & significantConditionSet) != significantActionConditions) // check if the action conditions don't violate conditions from the set 
		return false;
	modifiedConditionSet._valueMask |= action._condition._valueMask;
	modifiedConditionSet._affectedAttributesMask |= action._condition._affectedAttributesMask;
	return true;
}

const std::set<std::string>& WorldState::GetAttributeNamesSet()
{
	return _attributeNames;
}
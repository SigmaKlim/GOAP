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

std::vector<u_char> WorldState::GetAttributeValues(const unsigned index) const
{
	if (index > _numAttributes)
	{
		std::cout << "Index(" + std::to_string(index) + ") exceeds numAttributes(" +
			std::to_string(_numAttributes) + ").\n";
		exit(-1);
	}
	BitMask filteredMask = (_valueMask >> (index * Attribute::MAX_VALUES));
	unsigned attributeMask = filteredMask[0];
	std::vector<u_char> values;
	values.reserve(Attribute::MAX_VALUES);
	for (u_char i = 0; i < Attribute::MAX_VALUES; i++)
		if (((attributeMask >> i) & 1) == 1)
			values.push_back(i);
	return values;
}
std::vector<u_char> WorldState::GetAttributeValues(const std::string& name) const
{
	auto index = FindAttribute(name);
	if (index == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	return GetAttributeValues(index);
}

std::vector<std::string> WorldState::GetAttributeEnumerators(const std::string& name) const
{
	auto index = FindAttribute(name);
	if (index == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	auto values = GetAttributeValues(index);
	const auto& attribute = _planner->GetAttribute(name);
	std::vector<std::string> enumerators;
	enumerators.reserve(Attribute::MAX_VALUES);
	for (auto& value : values)
		enumerators.push_back(attribute.GetEnumerator(value)); 
	return enumerators;
}

unsigned WorldState::FindAttribute(const std::string& name)
{
	auto search =  _attributeNames.find(name);
	return std::distance(_attributeNames.begin(), search);
}

bool WorldState::IsActionUseful(WorldState& modifiedConditionSet, const WorldState& conditionSet, const Action& action)
{
	BitMask significantConditionSet = conditionSet._valueMask & action._effect._affectedAttributesMask; //leave only conditions affected by effects of the action
	BitMask significantActionEffects = conditionSet._affectedAttributesMask & action._effect._valueMask; //leave only effects that influence conditions from the condition set
	if ((significantActionEffects & significantConditionSet) != significantActionEffects 
		|| significantConditionSet == BitMask::MakeAllZeros(significantConditionSet.GetNumBits())) // check if the action effects don't violate conditions from the set and if there are any affected conditions at all 
		return false;
	modifiedConditionSet._valueMask = conditionSet._valueMask & ~action._effect._valueMask; //remove fulfilled conditions form the set
	modifiedConditionSet._affectedAttributesMask = conditionSet._affectedAttributesMask & ~action._effect._affectedAttributesMask; //remove fulfilled conditions form the set
	BitMask significantActionConditions = modifiedConditionSet._affectedAttributesMask & action._condition._valueMask; //leave only conditions on attributes shared between action conditions and conditions set
	significantConditionSet = conditionSet._valueMask & action._condition._affectedAttributesMask;
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
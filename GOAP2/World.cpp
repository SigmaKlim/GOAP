#include "World.h"
#include <bitset>
#include "GPlanner.h"

std::set<std::string> WorldState::_attributeNames;
unsigned WorldState::_numAttributes = 0;
GPlanner* WorldState::_planner = nullptr;

WorldState::WorldState(const BitMask& valueMask, const BitMask& affectedAttributesMask)
{
	assert((valueMask & affectedAttributesMask) == valueMask);
	_valueMask = valueMask;
	_affectedAttributesMask = affectedAttributesMask;
	UpdateDebugAttributeValueList();
}

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
	UpdateDebugAttributeValueList();
}

WorldState::WorldState(const t_attr_enums_map& nameValuePairs)
{
	_valueMask = BitMask::MakeAllZeros(_numAttributes * Attribute::MAX_VALUES);
	_affectedAttributesMask = BitMask::MakeAllZeros(_numAttributes * Attribute::MAX_VALUES);
	for (auto& nameValuePair : nameValuePairs)
	{
		const std::string& attributeName = nameValuePair.first;
		const std::vector<std::string>& enumeratorNames = nameValuePair.second;
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
		for (const auto& enumeratorName : enumeratorNames)
		{
			u_char valueOffset = attribute.GetEnumValue(enumeratorName);
			if (valueOffset == Attribute::MAX_VALUES)
			{
				std::cout << "Incorrect enumerator name: " + enumeratorName + "\n";
				exit(-1);
			}
			_valueMask.SetBitValue(attributeOffset *  Attribute::MAX_VALUES + valueOffset, 1);
		}
	}
	UpdateDebugAttributeValueList();
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
	UpdateDebugAttributeValueList();
}
WorldState& WorldState::operator=(const WorldState& other)
{
	_valueMask = other._valueMask;
	_affectedAttributesMask = other._affectedAttributesMask;
	UpdateDebugAttributeValueList();
	return *this;
}
WorldState::~WorldState()
{
}
bool WorldState::SetAttributeValue(const std::string& name, u_char value)
{
	auto attributeIndex = FindAttribute(name);
	if (attributeIndex == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	return SetAttributeValue(attributeIndex, value);
}

bool WorldState::SetAttributeValue(const std::string& name, const std::string& enumerator)
{
	return SetAttributeValue(name, _planner->GetAttribute(name).GetEnumValue(enumerator));
}

bool WorldState::SetAttributeValue(unsigned index, u_char value)
{
	if (index > _numAttributes)
	{
		std::cout << "Index(" + std::to_string(index) + ") exceeds numAttributes(" +
			std::to_string(_numAttributes) + ").\n";
		exit(-1);
	}
	_valueMask.SetBitValue(index * Attribute::MAX_VALUES + value, 1);
	BitMask tmpAffectedAttributesMask = BitMask::MakeRightOnes(_valueMask.GetNumBits(), Attribute::MAX_VALUES);
	tmpAffectedAttributesMask <<= index * Attribute::MAX_VALUES;
	_affectedAttributesMask |= tmpAffectedAttributesMask;
	UpdateDebugAttributeValueList();
	return true;
}

std::vector<u_char> WorldState::GetAttributeValues(unsigned index) const
{
	
	auto attributeMask = GetAttributeMask(index);
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

unsigned WorldState::GetAttributeMask(unsigned index) const
{
	if (index > _numAttributes)
	{
		std::cout << "Index(" + std::to_string(index) + ") exceeds numAttributes(" +
			std::to_string(_numAttributes) + ").\n";
		exit(-1);
	}
	BitMask filteredMask = (_valueMask >> (index * Attribute::MAX_VALUES));
	unsigned attributeMask = filteredMask[0];
	unsigned residueFilter = BitMask::MakeRightOnes(Attribute::MAX_VALUES);
	attributeMask &= residueFilter;
	return attributeMask;
	
}

const BitMask& WorldState::GetValueMask() const
{
	return _valueMask;
}

const BitMask& WorldState::GetAffectedAttributesMask() const
{
	return _affectedAttributesMask;
}

unsigned WorldState::FindAttribute(const std::string& name)
{
	auto search =  _attributeNames.find(name);
	return std::distance(_attributeNames.begin(), search);
}

unsigned WorldState::GetAttributeNumber()
{
	return _numAttributes;
}

void WorldState::UpdateDebugAttributeValueList()
{
	if (_isInDebugMode == false)
		return;
	_debugAttributeValueList.clear();
	for (auto& attributePair : _planner->GetAttributeCatalogue())
	{
		auto& attributeName = attributePair.first;
		auto& attribute = attributePair.second;
		auto values = GetAttributeValues(attributeName);
		std::vector<std::string> enums;
		for (auto& value : values)
			enums.push_back(attribute.GetEnumerator(value));
		if (enums.size() > 0)
			_debugAttributeValueList.insert({attributeName, enums});
	}
}

const std::set<std::string>& WorldState::GetAttributeNamesSet()
{
	return _attributeNames;
}
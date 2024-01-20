#include "World.h"
#include <bitset>
#include "../GPlanner.h"

Catalogue<const Attribute*>* WorldState::_attributeCatalogue = nullptr;
unsigned WorldState::_numAttributes = 0;
WorldState::WorldState(const std::vector<AttributeData>& attributeEnumsPairs)
{
	_valueMask = BitMask::MakeAllZeros(_numAttributes * MAX_NUM_ENUMERATORS);
	_affectedAttributesMask = BitMask::MakeAllZeros(_numAttributes * MAX_NUM_ENUMERATORS);
	for (auto& attributeEnumsPair : attributeEnumsPairs)
	{
		auto& attributeName = attributeEnumsPair.attributeName;
		auto* attributeIdPtr = _attributeCatalogue->GetId(attributeName);
		if (attributeIdPtr == nullptr)
			std::cout << "Wrong attribute name \"" << attributeName << "\".\n";
		else
		{
			for (auto& enumeratorName : attributeEnumsPair.enumeratorNames)
			{
				size_t attributeId = *attributeIdPtr;
				auto* attributePtr = *_attributeCatalogue->GetItem(attributeId);
				const unsigned char* enumeratorIdPtr = attributePtr->GetEnumeratorId(enumeratorName);
				if (enumeratorIdPtr == nullptr)
					std::cout << "Wrong enumerator name \"" << enumeratorName << "\"for attribute \"" << attributeName << "\".\n";
				else
				{
					unsigned char enumeratorId = *enumeratorIdPtr;
					if (enumeratorId >= MAX_NUM_ENUMERATORS)
						std::cout << "Attempted to use enumerator with id greater than WorldState::MAX_NUM_ENUMERATORS (" << MAX_NUM_ENUMERATORS <<
							") for attribute \"" << attributeName << "\". ";
					else
						AddAttributeValue(attributeId, enumeratorId);
				}
			}
		}
	}
	UpdateDebugAttributeValueList();
}

WorldState::WorldState()
{
	_valueMask = BitMask::MakeAllZeros(_numAttributes * MAX_NUM_ENUMERATORS);
	_affectedAttributesMask = BitMask::MakeAllZeros(_numAttributes * MAX_NUM_ENUMERATORS);
}
WorldState::WorldState(const WorldState& other)
{
	_valueMask = other._valueMask;
	_affectedAttributesMask = other._affectedAttributesMask;
	UserData = other.UserData;
	UpdateDebugAttributeValueList();
}
WorldState& WorldState::operator=(const WorldState& other)
{
	_valueMask = other._valueMask;
	_affectedAttributesMask = other._affectedAttributesMask;
	UpdateDebugAttributeValueList();
	UserData = other.UserData;
	return *this;
}
WorldState::~WorldState()
{
}

bool WorldState::ClearAttributeValue(unsigned index)
{
	if (index > _numAttributes)
	{
		std::cout << "Index(" + std::to_string(index) + ") exceeds numAttributes(" +
			std::to_string(_numAttributes) + ").\n";
		return false;
	}
	BitMask tmpMask = BitMask::MakeRightOnes(_valueMask.GetNumBits(), MAX_NUM_ENUMERATORS);
	tmpMask <<= index * MAX_NUM_ENUMERATORS;
	tmpMask.Invert();
	_valueMask &= tmpMask;
	_affectedAttributesMask &= tmpMask;
	return true;
}

bool WorldState::AddAttributeValue(const std::string& attributeName, const std::string& enumeratorName)
{
	auto* attributeIdPtr = _attributeCatalogue->GetId(attributeName);
	if (attributeIdPtr == nullptr)
	{
		std::cout << "Wrong attribute name \"" << attributeName << "\".\n";
		return false;
	}
	auto* attributePtr = *_attributeCatalogue->GetItem(*attributeIdPtr);
	auto* enumeratorIdPtr = attributePtr->GetEnumeratorId(enumeratorName);
	if (enumeratorIdPtr == nullptr)
	{
		std::cout << "Wrong attribute name \"" << attributeName
		<< "\" for attribute \"" << attributeName << "\".\n";
		return false;
	}
	return AddAttributeValue(*attributeIdPtr, *enumeratorIdPtr);
}

bool WorldState::AddAttributeValue(const std::string& attributeName, unsigned char value)
{
	auto* attributeIdPtr = _attributeCatalogue->GetId(attributeName);
	if (attributeIdPtr == nullptr)
	{
		std::cout << "Wrong attribute name \"" << attributeName << "\".\n";
		return false;
	}
	return AddAttributeValue(*attributeIdPtr, value);
}

bool WorldState::AreAllAttributesSet() const
{
	 return _affectedAttributesMask == BitMask::MakeAllOnes(_affectedAttributesMask.GetNumBits());
}

bool WorldState::AddAttributeValue(size_t index, unsigned char value)
{
	if (index >= _numAttributes)
	{
		std::cout << "Index(" + std::to_string(index) + ") exceeds numAttributes(" +
			std::to_string(_numAttributes) + ").\n";
		return false;
	}
	_valueMask.SetBitValue((unsigned)index * (unsigned)MAX_NUM_ENUMERATORS + (unsigned)value, 1);
	BitMask tmpAffectedAttributesMask = BitMask::MakeRightOnes(_valueMask.GetNumBits(), MAX_NUM_ENUMERATORS);
	tmpAffectedAttributesMask <<= (unsigned)index * (unsigned)MAX_NUM_ENUMERATORS;
	_affectedAttributesMask |= tmpAffectedAttributesMask;
	UpdateDebugAttributeValueList();
	return true;
}

std::vector<unsigned char> WorldState::GetAttributeValues(unsigned index) const
{
	
	auto attributeMask = GetAttributeMask(index);
	std::vector<unsigned char> values;
	values.reserve(MAX_NUM_ENUMERATORS);
	for (unsigned char i = 0; i < MAX_NUM_ENUMERATORS; i++)
		if (((attributeMask >> i) & 1) == 1)
			values.push_back(i);
	return values;
}
std::vector<unsigned char> WorldState::GetAttributeValues(const std::string& name) const
{
	auto index = FindAttribute(name);
	if (index == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	return GetAttributeValues(index);
}

std::vector<std::string> WorldState::GetAttributeEnumeratorNames(const std::string& name) const
{
	auto index = FindAttribute(name);
	if (index == _numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name + "\n";
		exit(-1);
	}
	auto values = GetAttributeValues(index);
	const auto* attribute = *_attributeCatalogue->GetItem(name);
	std::vector<std::string> enumerators;
	enumerators.reserve(MAX_NUM_ENUMERATORS);
	for (auto& value : values)
		enumerators.push_back(*attribute->GetEnumeratorName(value)); 
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
	BitMask filteredMask = (_valueMask >> (index * MAX_NUM_ENUMERATORS));
	unsigned attributeMask = filteredMask[0];
	unsigned residueFilter = BitMask::MakeRightOnes(MAX_NUM_ENUMERATORS);
	attributeMask &= residueFilter;
	return attributeMask;
}

unsigned WorldState::FindAttribute(const std::string& name)
{
	auto* attributeId = _attributeCatalogue->GetId(name);
	if (attributeId == nullptr)
		return _attributeCatalogue->Size();
	return *attributeId;
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
	for (auto& attributeNamePtr : _attributeCatalogue->namesLoop)
	{
		auto* attribute = *_attributeCatalogue->GetItem(*attributeNamePtr);
		auto* id = _attributeCatalogue->GetId(*attributeNamePtr);
		auto values = GetAttributeValues(*attributeNamePtr);
		std::vector<std::string> enums;
		for (auto& value : values)
			enums.push_back(*attribute->GetEnumeratorName(value));
		if (enums.size() > 0)
			_debugAttributeValueList.push_back({*attributeNamePtr, enums});
	}
}

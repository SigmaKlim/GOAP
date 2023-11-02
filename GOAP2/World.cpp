#include "World.h"
#include "Action.h"
#include <bitset>
#include "Plan.h"

std::set<std::string> WorldState::_attributeNames;
unsigned WorldState::_numAttributes = 0;
Planner* WorldState::_planner = nullptr;
//
// WsMask::WsMask() 
// { 
// 	//debug
// 	binaryCells.resize(NUM_CELLS); 
// };
// WsMask::WsMask(const t_mask& valueMask_)
// {
// 	//debug
// 	binaryCells.resize(NUM_CELLS);
// 	//
// 	SetMask(valueMask_);
// };
// WsMask::~WsMask() {};
// WsMask& WsMask::operator=(const WsMask& other_)
// {
// 	SetMask(other_.mask);
// 	return *this;
// }
// WsMask& WsMask::operator+=(const WsMask& other_)
// {
// 	SetMask(mask + other_.mask);
// 	return *this;
// }
// WsMask WsMask::operator~()
// {
// 	WsMask inversed(*this);
// 	inversed.SetMask(~mask);
// 	return inversed;
// }
// WsMask& WsMask::operator<<=(unsigned offset_)
// {
// 	SetMask(mask << offset_);
// 	return *this;
// }
// WsMask& WsMask::operator>>=(unsigned offset_)
// {
// 	SetMask(mask >> offset_);
// 	return *this;
// }
// WsMask& WsMask::operator|=(const WsMask& other_)
// {
// 	SetMask(mask | other_.mask);
// 	return *this;
// }
// WsMask& WsMask::operator&=(const WsMask& other_)
// {
// 	SetMask(mask & other_.mask);
// 	return *this;
// }
// WsMask operator&(const WsMask& left_, const WsMask& right_)
// {
// 	auto res(left_);
// 	res &= right_;
// 	return res;
// }
//
// WsMask operator>>(const WsMask& mask_, unsigned offset_)
// {
// 	auto res(mask_);
// 	res >>= offset_;
// 	return res;
// }
//
// WsMask operator<<(const WsMask& mask_, unsigned offset_)
// {
// 	auto res(mask_);
// 	res <<= offset_;
// 	return res;
// }
//
// bool operator==(const WsMask& left_, const WsMask& right_)
// {
// 	return left_.GetMask() == right_.GetMask();
// }
// u_char WsMask::GetValue(u_char index_) const
// {
// 	if (index_ > NUM_CELLS - 1)
// 	{
// 		std::cout << "Index must be lower than NUM_CELLS("
// 			+ std::to_string(NUM_CELLS) + "). \n";
// 		exit(-1);
// 	}
// 	u_char offset = index_ * NUM_BITS_CELL;
// 	t_mask value = 0;
// 	for (unsigned i = 0; i < NUM_BITS_CELL; i++)
// 		value += mask & (1 << (offset + i));
// 	value >>= offset;
// 	return value;
// }
//
// bool WsMask::GetSignificance(u_char index_) const
// {
// 	if (index_ > NUM_CELLS - 1)
// 	{
// 		std::cout << "Index must be lower than NUM_CELLS("
// 			+ std::to_string(NUM_CELLS) + "). \n";
// 		exit(-1);
// 	}
// 	u_char offset = index_ * NUM_BITS_CELL;
// 	t_mask value = mask & (1 << offset);
// 	return value != 0;
// }
//
// void WsMask::SetValue(u_char index_, u_char value_)
// {
// 	if (index_ > NUM_CELLS - 1)
// 	{
// 		std::cout << "Index must be lower than NUM_CELLS("
// 			+ std::to_string(NUM_CELLS) + "). \n";
// 		exit(-1);
// 	}
// 	else if (value_ > Attribute::MAX_VALUES - 1)
// 	{
// 		std::cout << "Value must be lower than NUM_ATTRIBUTES("
// 			+ std::to_string(Attribute::MAX_VALUES) + "). \n";
// 		exit(-1);
// 	}
// 	u_char offset = index_ * NUM_BITS_CELL;
// 	for (unsigned i = 0; i < NUM_BITS_CELL; i++)
// 		mask &= (~(1 << (offset + i)));
// 	t_mask valueWithOffset = value_ << offset;
// 	mask += valueWithOffset;
// 	//debug
// 	binaryCells[index_] = std::bitset<3>(value_).to_string();
// }
//
// t_mask WsMask::GetMask() const
// {
// 	return mask;
// }
// void WsMask::SetMask(const t_mask& mask_)
// {
// 	mask = mask_;
// 	
// 	//debug
// 	for (unsigned cellIndex = 0; cellIndex < NUM_CELLS; cellIndex++)
// 	{
// 		t_mask value = 0;
// 		u_char offset = cellIndex * NUM_BITS_CELL;
// 		for (unsigned i = 0; i < NUM_BITS_CELL; i++)
// 			value += mask & (1 << (offset + i));
// 		value >>= offset;
// 		binaryCells[cellIndex] = std::bitset<4>(value).to_string();
// 	}
// }
//
// bool WsMask::SatisfiesMask(const WsMask& mask_) const
// {
// 	WsMask auxMask = mask_; // as we use NUM_BITS_CELL-numeric system (ternary or quaternary), we need to calculate auxiliary mask which has 11..1 instead eahc non-zero cell in the mask_
// 	static const t_mask NUM_BITS = NUM_CELLS * NUM_BITS_CELL; //we round down number of bits so that it is NUM_BITS_CELL-divisible
// 	//We need to calculate special masks which are similar for all input masks within this simulation (because NUM_CELLS and NUM_BITS_CELL persist). We will use this constant masks in the end of the function
// 	static bool wereConstantsInitialized = false; //the calculation is the same for each mask, so no need to repeat it
// 	static std::vector <WsMask> g(NUM_BITS_CELL); //a series of terms for constant masks
// 	static std::vector <WsMask> constL(NUM_BITS_CELL); //constant masks for left shifts
// 	static std::vector <WsMask> constR(NUM_BITS_CELL); //constant masks for left shifts
// 	if (wereConstantsInitialized == false)
// 	{
// 		for (unsigned s = 1; s < NUM_BITS_CELL; s++)
// 		{
// 			unsigned t1 = 1 << s;
// 			unsigned t2 = ((1 << (NUM_BITS - 1)) - 1) * 2 + 1; //= (1 << NUM_BITS) - 1 ---- to avoid arithmetic overflow  
// 			unsigned t3 = (1 << NUM_BITS_CELL) - 1;
// 			g[s].SetMask(t1 * (t2 / t3)); //g[s] is a number which in binary consists of single cell repeated NUM_CELLS times. The cell has one 1 on s-th position
// 		}
// 		for (unsigned k = 1; k < NUM_BITS_CELL; k++)
// 		{
// 			for (unsigned s = k; s < NUM_BITS_CELL; s++)
// 				constL[k] |= g[s]; //constL[k] is a number which in binary consists of a single cell repeated NUM_CELLS times. The cell is 11..10 and has k 1s
// 			constR[NUM_BITS_CELL - k] = ~constL[k]; //constR[k] = 011..1 cell repeated NUM_CELLS times.
// 		}
// 		wereConstantsInitialized = true;
// 	}
// 	for (unsigned k = 1; k < NUM_BITS_CELL; k++)
// 	{
// 		WsMask left = mask_ << k; //Each bit needs to know values of left bits in the cell. For that we calculate left shifts of the input mask.
// 		auxMask |= (left & constL[k]); //We mask away all the unnecessary left bits from other cells and att the result to the sum
// 		WsMask right = mask_ >> k; //Each bit needs to know values of right bits in the cell. For that we calculate right shifts of the input mask.
// 		auxMask |= (right & constR[k]); //We mask away all the unnecessary right bits from other cells 
// 	}
// 	//auxMask == 11..100..0..... with 00..0 at positions of insignificant cells and 11..1 at position of significant ones.
// 	return (*this & auxMask) == mask_; //If we multiply the value by auxiliary mask, we zero down all unnecessary cells and leave all necessary untouched. If the result is equal to original mask, the value satisfies the mask.
// }

WorldState::WorldState(const t_attr_enum_map& nameValuePairs)
{
	_valueMask = BitMask::MakeAllZeros(_numAttributes * Attribute::MAX_VALUES);
	for (auto& nameValuePair : nameValuePairs)
	{
		const std::string& attributeName = nameValuePair.first;
		const std::string& enumeratorName = nameValuePair.first;
		const auto& attribute = _planner->GetAttribute(attributeName);
		auto attributeOffset = FindAttribute(attributeName);
		if (attributeOffset == _numAttributes)
		{
			std::cout << "Incorrect attribute name: " + attributeName + "\n";
			exit(-1);
		}
		u_char valueOffset = attribute.GetEnumValue(enumeratorName);
		if (attributeOffset == Attribute::MAX_VALUES)
		{
			std::cout << "Incorrect enumerator name: " + enumeratorName + "\n";
			exit(-1);
		}
		_valueMask.SetBitValue(attributeOffset + valueOffset, 1);
	}
}
WorldState::WorldState()
{
}
WorldState::WorldState(const WorldState& other)
{
	_valueMask = other._valueMask;
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
// unsigned WorldState::FindAttribute(const std::string& name)
// {
// 	auto search =  _attributeNames.find(name);
// 	return std::distance(_attributeNames.begin(), search);
// }
//
// unsigned WorldState::GetNumAttributes() const
// {
// 	return _numAttributes;
// }

bool WorldState::IsActionUseful(WorldState& modifiedConditionSet, const WorldState& conditionSet, const Action& action)
{
	BitMask significantConditionSet = conditionSet._valueMask & action._effect._affectedAttributesMask; //leave only conditions affected by effects of the action
	BitMask significantActionEffects = conditionSet._affectedAttributesMask & action._effect._affectedAttributesMask; //leave only effects that influence conditions from the condition set
	if (significantActionEffects != significantConditionSet && significantConditionSet != BitMask::MakeAllZeros(significantConditionSet.GetNumBits())) // check if the action effects don't violate conditions from the set and if there are any affected conditions at all 
		return false;
	modifiedConditionSet._valueMask = conditionSet._valueMask & ~action._effect._valueMask; //remove fulfilled conditions form the set
	modifiedConditionSet._affectedAttributesMask = conditionSet._affectedAttributesMask & ~action._effect._affectedAttributesMask; //remove fulfilled conditions form the set
	BitMask significantActionConditions = conditionSet._affectedAttributesMask & action._condition._affectedAttributesMask; //leave only conditions on attributes shared between action conditions and conditions set
	if (significantActionConditions != significantConditionSet) // check if the action conditions don't violate conditions from the set 
		return false;
	modifiedConditionSet._valueMask |= action._condition._valueMask;
	modifiedConditionSet._affectedAttributesMask |= action._condition._affectedAttributesMask;
	return true;
}

const std::set<std::string>& WorldState::GetAttributeNamesSet()
{
	return _attributeNames;
}
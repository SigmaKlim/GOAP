#include "World.h"
#include "Action.h"
#include <bitset>
#include "Plan.h"

std::set<std::string> WorldState::attributeNames;
unsigned WorldState::numAttributes = 0;
Planner* WorldState::planner = nullptr;

WsMask::WsMask() 
{ 
	//debug
	binaryCells.resize(NUM_CELLS); 
};
WsMask::WsMask(const t_mask& valueMask_)
{
	//debug
	binaryCells.resize(NUM_CELLS);
	//
	SetMask(valueMask_);
};
WsMask::~WsMask() {};
WsMask& WsMask::operator=(const WsMask& other_)
{
	SetMask(other_.mask);
	return *this;
}
WsMask& WsMask::operator+=(const WsMask& other_)
{
	SetMask(mask + other_.mask);
	return *this;
}
WsMask WsMask::operator~()
{
	WsMask inversed(*this);
	inversed.SetMask(~mask);
	return inversed;
}
WsMask& WsMask::operator<<=(unsigned offset_)
{
	SetMask(mask << offset_);
	return *this;
}
WsMask& WsMask::operator>>=(unsigned offset_)
{
	SetMask(mask >> offset_);
	return *this;
}
WsMask& WsMask::operator|=(const WsMask& other_)
{
	SetMask(mask | other_.mask);
	return *this;
}
WsMask& WsMask::operator&=(const WsMask& other_)
{
	SetMask(mask & other_.mask);
	return *this;
}
WsMask operator&(const WsMask& left_, const WsMask& right_)
{
	auto res(left_);
	res &= right_;
	return res;
}

WsMask operator>>(const WsMask& mask_, unsigned offset_)
{
	auto res(mask_);
	res >>= offset_;
	return res;
}

WsMask operator<<(const WsMask& mask_, unsigned offset_)
{
	auto res(mask_);
	res <<= offset_;
	return res;
}

bool operator==(const WsMask& left_, const WsMask& right_)
{
	return left_.GetMask() == right_.GetMask();
}
u_char WsMask::GetValue(u_char index_) const
{
	if (index_ > NUM_CELLS - 1)
	{
		std::cout << "Index must be lower than NUM_CELLS("
			+ std::to_string(NUM_CELLS) + "). \n";
		exit(-1);
	}
	u_char offset = index_ * NUM_BITS_CELL;
	t_mask value = 0;
	for (unsigned i = 0; i < NUM_BITS_CELL; i++)
		value += mask & (1 << (offset + i));
	value >>= offset;
	return value;
}

bool WsMask::GetSignificance(u_char index_) const
{
	if (index_ > NUM_CELLS - 1)
	{
		std::cout << "Index must be lower than NUM_CELLS("
			+ std::to_string(NUM_CELLS) + "). \n";
		exit(-1);
	}
	u_char offset = index_ * NUM_BITS_CELL;
	t_mask value = mask & (1 << offset);
	return value != 0;
}

void WsMask::SetValue(u_char index_, u_char value_)
{
	if (index_ > NUM_CELLS - 1)
	{
		std::cout << "Index must be lower than NUM_CELLS("
			+ std::to_string(NUM_CELLS) + "). \n";
		exit(-1);
	}
	else if (value_ > Attribute::MAX_VALUES - 1)
	{
		std::cout << "Value must be lower than NUM_ATTRIBUTES("
			+ std::to_string(Attribute::MAX_VALUES) + "). \n";
		exit(-1);
	}
	u_char offset = index_ * NUM_BITS_CELL;
	for (unsigned i = 0; i < NUM_BITS_CELL; i++)
		mask &= (~(1 << (offset + i)));
	t_mask valueWithOffset = value_ << offset;
	mask += valueWithOffset;
	//debug
	binaryCells[index_] = std::bitset<3>(value_).to_string();
}

t_mask WsMask::GetMask() const
{
	return mask;
}
void WsMask::SetMask(const t_mask& mask_)
{
	mask = mask_;
	
	//debug
	for (unsigned cellIndex = 0; cellIndex < NUM_CELLS; cellIndex++)
	{
		t_mask value = 0;
		u_char offset = cellIndex * NUM_BITS_CELL;
		for (unsigned i = 0; i < NUM_BITS_CELL; i++)
			value += mask & (1 << (offset + i));
		value >>= offset;
		binaryCells[cellIndex] = std::bitset<4>(value).to_string();
	}
}

bool WsMask::SatisfiesMask(const WsMask& mask_) const
{
	WsMask auxMask = mask_; // as we use NUM_BITS_CELL-numeric system (ternary or quaternary), we need to calculate auxiliary mask which has 11..1 instead eahc non-zero cell in the mask_
	static const t_mask NUM_BITS = NUM_CELLS * NUM_BITS_CELL; //we round down number of bits so that it is NUM_BITS_CELL-divisible
	//We need to calculate special masks which are similar for all input masks within this simulation (because NUM_CELLS and NUM_BITS_CELL persist). We will use this constant masks in the end of the function
	static bool wereConstantsInitialized = false; //the calculation is the same for each mask, so no need to repeat it
	static std::vector <WsMask> g(NUM_BITS_CELL); //a series of terms for constant masks
	static std::vector <WsMask> constL(NUM_BITS_CELL); //constant masks for left shifts
	static std::vector <WsMask> constR(NUM_BITS_CELL); //constant masks for left shifts
	if (wereConstantsInitialized == false)
	{
		for (unsigned s = 1; s < NUM_BITS_CELL; s++)
		{
			unsigned t1 = 1 << s;
			unsigned t2 = ((1 << (NUM_BITS - 1)) - 1) * 2 + 1; //= (1 << NUM_BITS) - 1 ---- to avoid arithmetic overflow  
			unsigned t3 = (1 << NUM_BITS_CELL) - 1;
			g[s].SetMask(t1 * (t2 / t3)); //g[s] is a number which in binary consists of single cell repeated NUM_CELLS times. The cell has one 1 on s-th position
		}
		for (unsigned k = 1; k < NUM_BITS_CELL; k++)
		{
			for (unsigned s = k; s < NUM_BITS_CELL; s++)
				constL[k] |= g[s]; //constL[k] is a number which in binary consists of a single cell repeated NUM_CELLS times. The cell is 11..10 and has k 1s
			constR[NUM_BITS_CELL - k] = ~constL[k]; //constR[k] = 011..1 cell repeated NUM_CELLS times.
		}
		wereConstantsInitialized = true;
	}
	for (unsigned k = 1; k < NUM_BITS_CELL; k++)
	{
		WsMask left = mask_ << k; //Each bit needs to know values of left bits in the cell. For that we calculate left shifts of the input mask.
		auxMask |= (left & constL[k]); //We mask away all the unnecessary left bits from other cells and att the result to the sum
		WsMask right = mask_ >> k; //Each bit needs to know values of right bits in the cell. For that we calculate right shifts of the input mask.
		auxMask |= (right & constR[k]); //We mask away all the unnecessary right bits from other cells 
	}
	//auxMask == 11..100..0..... with 00..0 at positions of insignificant cells and 11..1 at position of significant ones.
	return (*this & auxMask) == mask_; //If we multiply the value by auxiliary mask, we zero down all unnecessary cells and leave all necessary untouched. If the result is equal to original mask, the value satisfies the mask.
}

WorldState::WorldState(const t_attr_enum_map& nameValuePairs_)
{
	for (auto& nameValuePair : nameValuePairs_)
	{
		const std::string& attributeName = nameValuePair.first;
		const auto& attribute = planner->GetAttribute(attributeName);
		u_char value = attribute.GetEnumValue(nameValuePair.second);
		auto pos = FindAttribute(attributeName);
		if (pos == numAttributes)
		{
			std::cout << "Incorrect attribute name: " + attributeName + "\n";
			exit(-1);
		}
		mask.SetValue(pos, value);
	}
}
WorldState::WorldState()
{
}
WorldState::WorldState(const WorldState& other_)
{
	mask.SetMask(other_.mask.mask);
}
WorldState& WorldState::operator=(const WorldState& other_)
{
	mask.SetMask(other_.GetMask());
	return *this;
}
WorldState::~WorldState()
{
}
bool WorldState::SetAttributeValue(const std::string& name_, u_char value_)
{
	auto pos = FindAttribute(name_);
	if (pos == numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name_ + "\n";
		exit(-1);
	}
	mask.SetValue(pos, value_);
	return true;
}
bool WorldState::SetAttributeValue(const unsigned index_, u_char value_)
{
	if (index_ > numAttributes)
	{
		std::cout << "Index(" + std::to_string(index_) + ") exceeds numAttributes(" +
			std::to_string(numAttributes) + ").\n";
		exit(-1);
	}
	mask.SetValue(index_, value_);
	return true;
}
u_char WorldState::GetAttributeValue(const unsigned index_) const
{
	if (index_ > numAttributes)
	{
		std::cout << "Index(" + std::to_string(index_) + ") exceeds numAttributes(" +
			std::to_string(numAttributes) + ").\n";
		exit(-1);
	}
	return mask.GetValue(index_);
}
u_char WorldState::GetAttributeValue(const std::string& name_) const
{
	auto pos = FindAttribute(name_);
	if (pos == numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name_ + "\n";
		exit(-1);
	}
	return mask.GetValue(pos);
}

const std::string& WorldState::GetAttributeEnumerator(const std::string& name_) const
{
	auto pos = FindAttribute(name_);
	if (pos == numAttributes)
	{
		std::cout << "Incorrect attribute name: " + name_ + "\n";
		exit(-1);
	}
	auto value = mask.GetValue(pos);
	const auto& attribute = planner->GetAttribute(name_);
	const auto& enumerator = attribute.GetEnumerator(value); 
	return enumerator;
}


t_mask WorldState::GetMask() const
{
	return mask.GetMask();
}
unsigned WorldState::FindAttribute(const std::string& name_)
{
	auto search =  attributeNames.find(name_);
	return std::distance(attributeNames.begin(), search);
}

unsigned WorldState::GetNumAttributes() const
{
	return numAttributes;
}

bool WorldState::IsActionUseful(WorldState& modifiedState_, const WorldState& thisState_, const Action& action_)
{
	modifiedState_ = thisState_;
	const auto& effects = action_.GetEffect();
	bool wereAnyConditionsSatisfied = false; 
	for (unsigned i = 0; i < numAttributes; i++)
	{
		if (effects.GetAttributeValue(i) == modifiedState_.GetAttributeValue(i)) // check if any effects of the action satisfy any active conditions
		{
			modifiedState_.SetAttributeValue(i, 0);
			wereAnyConditionsSatisfied = true;
		}
	}
	if (wereAnyConditionsSatisfied == false)
		return false; // if no conditions were satisfied within the first loop, the action is not useful
	const auto& conditions = action_.GetCondition();
	for (unsigned i = 0; i < numAttributes; i++)
	{
		if (conditions.GetAttributeValue(i) != 0)
			if (modifiedState_.GetAttributeValue(i) == 0)
				modifiedState_.SetAttributeValue(i, conditions.GetAttributeValue(i)); //add new conditions to the set
			else //but avoid replacing the existing ones; reject action in this case
				return false;
	}
	return true;
}

bool WorldState::SatisfiesMask(const WorldState& mask_) const
{
	return mask.SatisfiesMask(mask_.GetMask());
}

const std::set<std::string>& WorldState::GetAttributeNamesSet()
{
	return attributeNames;
}

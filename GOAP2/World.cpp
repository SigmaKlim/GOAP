#include "World.h"
#include "Action.h"
#include <bitset>
#include "Plan.h"

bool WorldState::wasInitialized = false;
std::set<std::string> WorldState::attributeNames;
unsigned WorldState::numAttributes = 0;
Planner* WorldState::planner = nullptr;

WsMask::WsMask() 
{ 
	//debug
	binaryCells.resize(NUM_CELLS); 
};
WsMask::WsMask(const t_mask& mask_)
{
	//debug
	binaryCells.resize(NUM_CELLS);
	//
	SetMask(mask_);
};
WsMask::~WsMask() {};
u_char WsMask::GetValue(u_char index_) const
{
	if (index_ > NUM_CELLS - 1)
	{
		std::cout << "Index must be lower than NUM_CELLS("
			+ std::to_string(NUM_CELLS) + "). \n";
		exit(-1);
	}
	u_char offset = index_ * CELL_BITS_NUM;
	t_mask value = 0;
	for (auto i = 0; i < CELL_BITS_NUM; i++)
		value += mask & (1 << (offset + i));
	value >>= offset;
	return value;
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
	u_char offset = index_ * CELL_BITS_NUM;
	for (auto i = 0; i < CELL_BITS_NUM; i++)
		mask &= (~(1 << (offset + i)));
	t_mask valueWithOffset = value_ << offset;
	mask += valueWithOffset;
	//debug
	binaryCells[index_] = std::bitset<4>(value_).to_string();
}
t_mask WsMask::GetMask() const
{
	return mask;
}
void WsMask::SetMask(const t_mask& mask_)
{
	mask = mask_;
	//debug
	for (auto cellNum = 0; cellNum < NUM_CELLS; cellNum++)
	{
		t_mask value = 0;
		u_char offset = cellNum * CELL_BITS_NUM;
		for (auto i = 0; i < CELL_BITS_NUM; i++)
			value += mask & (1 << (offset + i));
		value >>= offset;
		binaryCells[cellNum] = std::bitset<4>(value).to_string();
	}
}

void WorldState::InitializeAttributes(const std::set<std::string>& attributeNames_)
{

	for (auto& attributeName : attributeNames_)
	{
		attributeNames.insert(attributeName);
	}
	numAttributes = attributeNames.size();
	wasInitialized = true;
}
WorldState::WorldState(const t_attr_enum_map& nameValuePairs_)
{
	// if (wasInitialized == false)
	// {
	// 	std::cout << "You have to initialize attributes before you create a WorldState object! \n";
	// 	exit(-1);
	// }
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
	// if (wasInitialized == false)
	// {
	// 	std::cout << "You have to initialize attributes before you create a WorldState object! \n";
	// 	exit(-1);
	// }
}
WorldState::WorldState(const WorldState& other_)
{
	// if (wasInitialized == false)
	// {
	// 	std::cout << "You have to initialize attributes before you create a WorldState object! \n";
	// 	exit(-1);
	// };
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
	const auto& effects = action_.GetEff();
	bool wereAnyConditionsSatisfied = false; 
	for (auto i = 0; i < numAttributes; i++)
	{
		if (effects.GetAttributeValue(i) == modifiedState_.GetAttributeValue(i)) // check if any effects of the action satisfy any active conditions
		{
			modifiedState_.SetAttributeValue(i, 0);
			wereAnyConditionsSatisfied = true;
		}
	}
	if (wereAnyConditionsSatisfied == false)
		return false; // if no conditions were satisfied within the first loop, the action is not useful
	const auto& conditions = action_.GetCnd();
	for (auto i = 0; i < numAttributes; i++)
	{
		if (conditions.GetAttributeValue(i) != 0)
			if (modifiedState_.GetAttributeValue(i) == 0)
				modifiedState_.SetAttributeValue(i, conditions.GetAttributeValue(i)); //add new conditions to the set
			else //but avoid replacing the existing ones; reject action in this case
				return false;
	}
	return true;
}
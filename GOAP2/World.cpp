#include "World.h"


Attribute::Attribute(bool value_)
{
	value = value_;
}

Attribute::~Attribute()
{
}

const bool Attribute::GetValue() const
{
	return value;
}

bool WsMask::SetAttributeByIndex(const int i, bool value)
{
	if (i >= attributes.size())
		return false;
	attributes[i].value = value;
	return true;
}

bool WsMask::SetMask(const std::vector<int>& affectedAttributes)
{
	mask = 0;
	for (auto a : affectedAttributes)
		if (a >= attributes.size())
			return false;
		else
			mask += 1 << a;
	affectedAttributesNum = affectedAttributes.size();
	return true;
}

void WsMask::SetMask(int mask_)
{
	mask = mask_;
}

WsMask::WsMask(const int attNum)
{
	attributes = std::vector<Attribute>(attNum, true);
	affectedAttributesNum = attNum;
	mask = (1 << attNum) - 1;
}

WsMask::~WsMask()
{
}

Attribute const& WsMask::GetAttributeByIndex(const int i) const
{
	return attributes[i];
}

int WsMask::GetMask() const
{
	return mask;
}

std::vector<int> WsMask::GetAffectedAttributes() const
{
	std::vector<int> affectedAttributes;
	for (auto i = 0; i < attributes.size(); i++)
		if ((1 << i) & mask)
			affectedAttributes.push_back(i);
	return affectedAttributes;
}

int WsMask::GetAffectedAttributesNum() const
{
	return affectedAttributesNum;
}


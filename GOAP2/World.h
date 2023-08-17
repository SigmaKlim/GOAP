#pragma once
#include <vector>
#include <string>


class Attribute
{
	bool value;
public:
	Attribute(bool value_);
	~Attribute();
	const bool GetValue() const;

	friend class WsMask;
};

class WsMask
{
	std::vector <Attribute> attributes;
	int mask;
	int affectedAttributesNum;

	bool SetAttributeByIndex(const int i, bool value);
	bool SetMask(const std::vector <int>& affectedAttributes);
	void SetMask(int mask_);

public:
	WsMask(const int attNum);
	~WsMask();
	Attribute const& GetAttributeByIndex(const int i) const;
	int GetMask() const;
	std::vector <int> GetAffectedAttributes() const;
	int GetAffectedAttributesNum() const;

	friend class Action;
	friend class Planner;
};
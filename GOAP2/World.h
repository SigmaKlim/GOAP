#pragma once
#include <vector>
#include <set>
#include <string>
#include "Attribute.h"

typedef /*unsigned*/ std::vector <u_char> t_mask;
struct WorldState
{
	void	InitializeAttributes(const std::set<Attribute>& attribytes_);
			WorldState(const std::map<std::string, u_char>& affectedAttributes );
			WorldState(const WorldState& other_);
			~WorldState();
	bool	SetAttributeValue(const std::string& name, u_char value_);
	bool	SetAttributeValue(const unsigned index_, u_char value_);
	//void	SetMask(const t_mask& mask_);

	u_char	GetAttributeValue(const unsigned index_) const;
	u_char	GetAttributeValue(const std::string& name_) const;
	t_mask	GetMask() const;

private:
	
	static unsigned										attributesNum;
	static std::map < const std::string&, Attribute>	attributes;
	static bool											wasInitialized;
	t_mask												mask;

	friend class Action;
	friend class Planner;
};
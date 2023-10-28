#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <limits>
#include "Attribute.h"

typedef std::unordered_map<std::string,std::string> t_attr_enum_map;
typedef unsigned t_mask;
class Action;

struct WsMask
{
				WsMask			();
				WsMask			(const t_mask& valueMask_);
				~WsMask			();
	WsMask&		operator=		(const WsMask& other_);
	WsMask&		operator+=		(const WsMask& other_);
	WsMask		operator~		();
	WsMask&		operator<<=		(unsigned offset_);
	WsMask&		operator>>=		(unsigned offset_);
	WsMask&		operator|=		(const WsMask& other_);
	WsMask& operator&=			(const WsMask& other_);
	u_char		GetValue		(u_char index_) const;
	bool		GetSignificance	(u_char index_) const;
	void		SetValue		(u_char index_, u_char value_);
	//u_char		GetNextValue	(u_char thisIndex_);
	t_mask		GetMask			() const;
	void		SetMask			(const t_mask& mask_);
	bool		SatisfiesMask	(const WsMask& mask_) const;
	
	friend bool   operator==	(const WsMask& left_, const WsMask& right_);
	friend WsMask operator&		(const WsMask& left_, const WsMask& right_);
	friend WsMask operator>>	(const WsMask& mask_, unsigned offset_);
	friend WsMask operator<<	(const WsMask& mask_, unsigned offset_);
	
	t_mask mask = 0;
	const unsigned NUM_BITS_CELL = std::log2(Attribute::MAX_VALUES);
	const unsigned NUM_CELLS = std::numeric_limits<t_mask>::digits / NUM_BITS_CELL;
	//debug
	std::vector <std::string> binaryCells;

	
};

struct WorldState
{
	friend class Action;
	friend class Planner;
						WorldState				(const t_attr_enum_map& nameValuePairs_);
						WorldState				();
						WorldState				(const WorldState& other_);
	WorldState&			operator=				(const WorldState& other_);
						~WorldState				();
	bool				SetAttributeValue		(const std::string& name, u_char value_);
	bool				SetAttributeValue		(const unsigned index_, u_char value_);
	u_char				GetAttributeValue		(const unsigned index_) const;
	u_char				GetAttributeValue		(const std::string& name_) const;
	const std::string&	GetAttributeEnumerator	(const std::string& name_) const;
	t_mask				GetMask					() const;
	unsigned			GetNumAttributes		() const;
	bool				SatisfiesMask			(const WorldState& mask_) const;
	
	static const std::set<std::string>& GetAttributeNamesSet();
	//Returns true of the action fullfilled any of active conditions
	static bool			IsActionUseful	 (WorldState& modifiedState_,  const WorldState& thisState_, const Action& action_);
	//Returns the position of the attribute and numAttributes if the attribute was not found.
	static unsigned		FindAttribute	 (const std::string& name_); 
private:

	WsMask								mask;
	
	static std::set<std::string>	attributeNames;
	static unsigned					numAttributes;
	static Planner* planner;

};
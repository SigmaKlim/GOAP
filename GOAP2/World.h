#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <limits>
#include "Attribute.h"

typedef std::unordered_map<std::string,std::string> t_attr_enum_map;
class Action;
typedef unsigned t_mask;
struct WsMask
{
				WsMask			();
				WsMask			(const t_mask& valueMask_);
				~WsMask			();
	u_char		GetValue		(u_char index_) const;
	bool		GetSignificance	(u_char index_) const;
	void		SetValue		(u_char index_, u_char value_);
	u_char		GetNextValue	(u_char thisIndex_);
	t_mask		GetMask			() const;
	void		SetMask			(const t_mask& valueMask_);

	
	t_mask mask = 0;
	const unsigned CELL_BITS_NUM = std::log2(Attribute::MAX_VALUES);
	const unsigned NUM_CELLS = std::numeric_limits<t_mask>::digits / CELL_BITS_NUM;
	//debug
	std::vector <std::string> binaryCells; 
};

struct WorldState
{
	friend class Action;
	friend class Planner;
					WorldState		 (const t_attr_enum_map& nameValuePairs_);
					WorldState		 ();
					WorldState		 (const WorldState& other_);
	WorldState&		operator=		 (const WorldState& other_);
					~WorldState		 ();
	bool			SetAttributeValue(const std::string& name, u_char value_);
	bool			SetAttributeValue(const unsigned index_, u_char value_);
	u_char			GetAttributeValue(const unsigned index_) const;
	u_char			GetAttributeValue(const std::string& name_) const;
	t_mask			GetMask			 () const;
	//Returns the position of the attribute and numAttributes if the attribute was not found.
	static unsigned	FindAttribute	 (const std::string& name_); 
	//Returns true of the action fullfilled any of active conditions
	static bool IsActionUseful		 (WorldState& modifiedState_,  const WorldState& thisState_, const Action& action_);
	
	unsigned	GetNumAttributes	 () const;
private:

	WsMask								mask;
	
	static std::set<std::string>	attributeNames;
	static unsigned					numAttributes;
	static Planner* planner;

};
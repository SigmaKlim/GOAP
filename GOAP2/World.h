#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <limits>
#include "Attribute.h"
#include "BitMask.h"
#include <boost/functional/hash.hpp>
typedef std::unordered_map<std::string,std::string> t_attr_enum_map;
typedef std::unordered_map<std::string, std::vector<std::string>> t_attr_enums_map;
typedef std::unordered_map<std::string, unsigned> t_attr_mask_map;

struct Vertex;

struct WorldState
{
	friend class Action;
	friend class GPlanner;
								//a constructor for states where each attribute has a concrete single value, used for action effects and actual world states
								WorldState				(const t_attr_enum_map& nameValuePairs);
								//a constructor for states where each attribute has a set of values, used for conditions
								WorldState				(const t_attr_enums_map& nameValuePairs);
								//a constructor for states where each attribute has a set of values, used for conditions
								WorldState				(const t_attr_mask_map& nameValuePairs);
								WorldState				();
								WorldState				(const WorldState& other);
	WorldState&					operator=				(const WorldState& other);
								~WorldState				();
	bool						SetAttributeValue		(const std::string& name, u_char value);
	bool						SetAttributeValue		(const unsigned index, u_char value);
	std::vector<u_char>			GetAttributeValues(const unsigned index) const;
	std::vector<u_char>			GetAttributeValues		(const std::string& name) const;
	std::vector<std::string>	GetAttributeEnumerators	(const std::string& name) const;

	static const std::set<std::string>& GetAttributeNamesSet();
	static bool IsActionUseful	 (WorldState& modifiedConditionSet,  const WorldState& conditionSet, const Action& action);
	//Returns the position of the attribute and numAttributes if the attribute was not found.
	static unsigned		FindAttribute	 (const std::string& name); 
private:

	BitMask							_valueMask; //1s on position of all affected  values within an attribute, 00..0 blocks on positions of all unaffected attributes
	BitMask							_affectedAttributesMask; // auxiliary mask; 11..1 blocks on positions of all affected attributes, 00..0 blocks on other positions
	
	static std::set<std::string>	_attributeNames;
	static unsigned					_numAttributes;
	static GPlanner*				_planner;

};

inline std::size_t hash_value(const BitMask& mask)
{
	std::size_t hash;
	boost::hash<unsigned> hasher;
	for (unsigned i = 0; i < mask.GetNumCells(); i++)
		boost::hash_combine(hash, hasher(mask[i]));
	return hash;
}
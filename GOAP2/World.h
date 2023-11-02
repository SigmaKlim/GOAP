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
//typedef unsigned t_mask;
class Action;

// struct WsMask
// {
// 					WsMask				();
// 					WsMask				(const BitMask& valueMask_);
// 					~WsMask				();
// 	WsMask&			operator=		(const WsMask& other_);
// 	WsMask&			operator+=		(const WsMask& other_);
// 	WsMask			operator~		();
// 	WsMask&			operator<<=		(unsigned offset_);
// 	WsMask&			operator>>=		(unsigned offset_);
// 	WsMask&			operator|=		(const WsMask& other_);
// 	WsMask&			operator&=		(const WsMask& other_);
// 	u_char			GetValue		(u_char index_) const;
// 	bool			GetSignificance	(u_char index_) const;
// 	void			SetValue		(u_char index_, u_char value_);
// 	const BitMask&	GetMask			() const;
// 	void			SetMask			(const BitMask& mask_);
// 	bool			SatisfiesMask	(const WsMask& mask_) const;
// 	
// 	friend bool   operator==	(const WsMask& left_, const WsMask& right_);
// 	friend WsMask operator&		(const WsMask& left_, const WsMask& right_);
// 	friend WsMask operator>>	(const WsMask& mask_, unsigned offset_);
// 	friend WsMask operator<<	(const WsMask& mask_, unsigned offset_);
// 	
// 	BitMask mask = BitMask();
// 	const unsigned NUM_BITS_CELL = std::log2(Attribute::MAX_VALUES);
// 	const unsigned NUM_CELLS = std::numeric_limits<BitMask>::digits / NUM_BITS_CELL;
// 	//debug
// 	std::vector <std::string> binaryCells;
// };

class Vertex;
struct WorldState
{
	friend class Action;
	friend class Planner;
						WorldState				(const t_attr_enum_map& nameValuePairs);
						WorldState				();
						WorldState				(const WorldState& other);
	WorldState&			operator=				(const WorldState& other);
						~WorldState				();
	bool				SetAttributeValue		(const std::string& name, u_char value);
	bool				SetAttributeValue		(const unsigned index, u_char value);
	u_char				GetAttributeValue		(const unsigned index) const;
	u_char				GetAttributeValue		(const std::string& name) const;
	const std::string&	GetAttributeEnumerator	(const std::string& name) const;
	// BitMask				GetMask					() const;
	// unsigned			GetNumAttributes		() const;
	
	
	static const std::set<std::string>& GetAttributeNamesSet();
	static bool IsActionUseful	 (WorldState& modifiedConditionSet,  const WorldState& conditionSet, const Action& action);
	//Returns the position of the attribute and numAttributes if the attribute was not found.
	static unsigned		FindAttribute	 (const std::string& name); 
private:

	BitMask							_valueMask; //1s on position of all affected  values within an attribute, 00..0 blocks on positions of all unaffected attributes
	BitMask							_affectedAttributesMask; // auxiliary mask; 11..1 blocks on positions of all affected attributes, 00..0 blocks on other positions
	
	static std::set<std::string>	_attributeNames;
	static unsigned					_numAttributes;
	static Planner*					_planner;

};

inline std::size_t hash_value(const BitMask& mask)
{
	std::size_t hash;
	boost::hash<unsigned> hasher;
	for (unsigned i = 0; i < mask.GetNumCells(); i++)
		boost::hash_combine(hash, hasher(mask[i]));
	return hash;
}
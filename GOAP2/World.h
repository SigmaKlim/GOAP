#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <limits>
#include "Attribute.h"
#include "BitMask.h"
#include <boost/functional/hash.hpp>

#include "Attributes/Attribute.h"
#include "Tools/Catalogue.h"


struct AttributeData
{
	std::string attributeName;
	std::vector<std::string> enumeratorNames;
};

struct WorldState
{
	friend class Action;
	friend class GPlanner;
								//WorldState(const BitMask& valueMask, const BitMask& affectedAttributesMask);
								//a constructor for states where each attribute has a concrete single value, used for action effects and actual world states
								//WorldState(const std)
								WorldState(const std::initializer_list<AttributeData>& attributeEnumsPairs);
								//WorldState(const std::vector<AttributeTo1Enumerator>& attributeEnumPairs);	
								//WorldState				(const t_attr_enum_map& nameValuePairs);
								//a constructor for states where each attribute has a set of values, used for conditions
								//WorldState				(const t_attr_enums_map& nameValuePairs);
								//a constructor for states where each attribute has a set of values, used for conditions
								//WorldState				(const t_attr_mask_map& nameValuePairs);
								WorldState				();
								WorldState				(const WorldState& other);
	WorldState&					operator=				(const WorldState& other);
								~WorldState				();
	// bool						ClearAttributeValue		(const std::string& name);
	bool						ClearAttributeValue		(unsigned index);
	//Sets attribute value for this state
	bool						AddAttributeValue		(size_t index, unsigned char value);
	//Sets attribute value for this state
	bool						AddAttributeValue		(const std::string& attributeName, const std::string& enumeratorName);
	bool						AddAttributeValue		(const std::string& attributeName, unsigned char value);
	
	std::vector<unsigned char>			GetAttributeValues		(unsigned index) const;
	std::vector<unsigned char>			GetAttributeValues		(const std::string& name) const;
	std::vector<std::string>	GetAttributeEnumerators	(const std::string& name) const;
	unsigned					GetAttributeMask		(unsigned index) const;
	const BitMask& GetValueMask() const;
	const BitMask& GetAffectedAttributesMask() const;
	
	
	static const std::set<std::string>& GetAttributeNamesSet();
	static unsigned		FindAttribute	 (const std::string& name);

	static unsigned		GetAttributeNumber();
private:
	
	BitMask							_valueMask; //1s on position of all affected  values within an attribute, 00..0 blocks on positions of all unaffected attributes
	BitMask							_affectedAttributesMask; // auxiliary mask; 11..1 blocks on positions of all affected attributes, 00..0 blocks on other positions
	
	//debug
	static const bool _isInDebugMode = true;
	std::vector<AttributeData> _debugAttributeValueList;
	void UpdateDebugAttributeValueList();
	//

	static Catalogue<const Attribute*>* _attributeCatalogue;
	static unsigned						_numAttributes;
	static const unsigned char			MAX_NUM_ENUMERATORS = 8;
	
};

inline std::size_t hash_value(const BitMask& mask)
{
	std::size_t hash = 0;
	boost::hash<unsigned> hasher;
	for (unsigned i = 0; i < mask.GetNumCells(); i++)
		boost::hash_combine(hash, hasher(mask[i]));
	return hash;
}

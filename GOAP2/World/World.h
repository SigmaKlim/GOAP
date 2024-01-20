#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <limits>

#include "BitMask.h"
#include <boost/functional/hash.hpp>

#include "../Attributes/Attribute.h"
#include "../Tools/Catalogue.h"


struct AttributeData
{
	std::string attributeName;
	std::vector<std::string> enumeratorNames;
};

struct WsUserData
{
	WsUserData() = default;
	WsUserData(const WsUserData& other) = default;
	virtual ~WsUserData() = default;
	
	//overload in similar fashion if you use custom world state user data struct
	virtual WsUserData* PassUserData() const
	{
		return new WsUserData(*this);
	}
};

struct WorldState
{
	friend class Action;
	friend class GPlanner;
								WorldState				(const std::vector<AttributeData>& attributeEnumsPairs);
								WorldState				();
								WorldState				(const WorldState& other);
	WorldState&					operator=				(const WorldState& other);
								~WorldState				();
	
	bool						ClearAttributeValue		(unsigned index);
	//Sets attribute value for this state
	bool						AddAttributeValue		(size_t index, unsigned char value);
	//Sets attribute value for this state
	bool						AddAttributeValue		(const std::string& attributeName, const std::string& enumeratorName);
	bool						AddAttributeValue		(const std::string& attributeName, unsigned char value);

	bool						AreAllAttributesSet		() const;
	
	std::vector<unsigned char>	GetAttributeValues		(unsigned index) const;
	std::vector<unsigned char>	GetAttributeValues		(const std::string& name) const;
	std::vector<std::string>	GetAttributeEnumeratorNames	(const std::string& name) const;
	unsigned					GetAttributeMask		(unsigned index) const;
	
	static unsigned				FindAttribute	 (const std::string& name);

	static unsigned				GetAttributeNumber();
	std::shared_ptr<WsUserData>	UserData = nullptr;		//used in user-defined GPlanner methods
private:
	
	BitMask							_valueMask;					//1s on position of all affected  values within an attribute, 00..0 blocks on positions of all unaffected attributes
	BitMask							_affectedAttributesMask;	// auxiliary mask; 11..1 blocks on positions of all affected attributes, 00..0 blocks on other positions
	
#pragma region Debug stuff
	static const bool _isInDebugMode = true;
	std::vector<AttributeData> _debugAttributeValueList;
	void UpdateDebugAttributeValueList();
#pragma endregion

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

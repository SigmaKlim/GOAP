#include "World.h"
bool WorldState::wasInitialized = false;
std::map< const std::string&, Attribute> WorldState::attributes;

void WorldState::InitializeAttributes(const std::set<Attribute>& attributes_)
{
	for (auto& attribute : attributes_)
		attributes.insert(std::move(
			std::pair<const std::string&, Attribute>(attribute.GetName(), attribute)));
	attributesNum = attributes_.size();
	wasInitialized = true;
}

WorldState::WorldState(const std::map<std::string, u_char>& affectedAttributes)
{
	if (wasInitialized == false)
	{
		std::cout << "You have to initialize attributes before you create a WorldState object! \n";
		exit(-1);
	}
	for (auto& atValPair : affectedAttributes)
	{
		const std::string& attributeName = atValPair.first;
		auto it = attributes.find(attributeName);
		if (it == attributes.end())
		{
			std::cout << "Incorrect attribute name: " + attributeName + "\n";
			exit(-1);
		}
		size_t pos = std::distance(attributes.begin(), it);
		mask[pos] = atValPair.second;
	}
}

WorldState::WorldState(const WorldState& other_)
{
	mask = other_.mask;
}

WorldState::~WorldState()
{
}

bool WorldState::SetAttributeValue(const std::string& name_, u_char value_)
{
	auto it = attributes.find(name_);
	if (it == attributes.end())
	{
		std::cout << "Incorrect attribute name: " + name_ + "\n";
		exit(-1);
	}
	size_t pos = std::distance(attributes.begin(), it);
	mask[pos] = value_;
	return true;
}

bool WorldState::SetAttributeValue(const unsigned index_, u_char value_)
{
	if (index_ > attributesNum)
	{
		std::cout << "Index(" + std::to_string(index_) + ") exceeds attributeNum(" +
			std::to_string(attributesNum) + ").\n";
		exit(-1);
	}
	mask[index_] = value_;
	return true;
}

u_char	WorldState::GetAttributeValue(const unsigned index_) const
{
	if (index_ > attributesNum)
	{
		std::cout << "Index(" + std::to_string(index_) + ") exceeds attributeNum(" +
			std::to_string(attributesNum) + ").\n";
		exit(-1);
	}
	return mask[index_];
}

u_char	WorldState::GetAttributeValue(const std::string& name_) const
{
	auto it = attributes.find(name_);
	if (it == attributes.end())
	{
		std::cout << "Incorrect attribute name: " + name_ + "\n";
		exit(-1);
	}
	size_t pos = std::distance(attributes.begin(), it);
	return mask[pos];
}
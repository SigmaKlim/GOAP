#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
typedef unsigned char u_char;

struct Attribute
{
	Attribute() {};
	Attribute(const std::vector<std::string>& enumerators_)
	{
		if (enumerators_.size() > MAX_VALUES - 1)
		{
			std::cout << "Number of values cannot exceed MAX_VALUES - 1 + (" + std::to_string(MAX_VALUES - 1) + ".\n";
			return;
		}
		for (auto i = 0; i < enumerators_.size(); i++)
			enumeratorValuePairs.insert({ enumerators_[i], i + 1 });
		enumeratorValuePairs.insert({ "NO_VALUE", 0 });
	}
	static const u_char MAX_VALUES = 8;
	u_char GetEnumValue(const std::string enumeratorName_) const
	{
		auto search = enumeratorValuePairs.find(enumeratorName_);
		if (search == enumeratorValuePairs.end())
		{
			std::cout << enumeratorName_ + "is not an enumerator name of teh attribute.\n";
			return MAX_VALUES;
		}
		return search->second;
	}
private:
	std::unordered_map<std::string, u_char> enumeratorValuePairs;
};


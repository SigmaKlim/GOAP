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
		if (enumerators_.size() > MAX_VALUES)
		{
			std::cout << "Number of values cannot exceed MAX_VALUES.\n";
			return;
		}
		for (auto i = 0; i < enumerators_.size(); i++)
			enumeratorValuePairs.insert({ enumerators_[i], i });
		enumeratorValuePairs.insert({ "NO_VALUE", MAX_VALUES });
	}
	static const u_char MAX_VALUES = 8;
private:
	std::unordered_map<std::string, u_char> enumeratorValuePairs;
};


#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
typedef unsigned char u_char;

struct Attribute
{
	Attribute(	const std::string& name_,
				const std::vector<std::string>& values_)
	{
		if (values_.size() > MAX_VALUES)
		{
			std::cout << "Number of values cannot exceed MAX_VALUES.\n";
			return;
		}
		name = name_;
		hash = std::hash<std::string>{}(name);
		for (auto i = 0; i < values_.size(); i++)
			enumerators.insert({ values_[i], i });
		enumerators.insert({ "NO_VALUE", MAX_VALUES });
	}
	friend bool operator==(const Attribute& left_, const Attribute& right_);
	static const u_char MAX_VALUES = 8;
	const std::string& GetName() const
	{
		return name;
	}
private:
	std::string name;
	size_t hash;
	std::map<std::string, u_char> enumerators;
};

bool operator==(const Attribute& left_, const Attribute& right_)
{
	return left_.hash == right_.hash;
}

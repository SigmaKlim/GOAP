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
		_enumeratorArray.resize(enumerators_.size() + 1);
		for (auto i = 0; i < enumerators_.size(); i++)
		{
			_enumeratorValuePairs.insert({ enumerators_[i], i + 1 });
			_enumeratorArray[i + 1] = enumerators_[i];
		}
		_enumeratorValuePairs.insert({ "NO_VALUE", 0 });
		_enumeratorArray[0] = "NO_VALUE";
	}
	u_char GetEnumValue(const std::string enumeratorName_) const
	{
		auto search = _enumeratorValuePairs.find(enumeratorName_);
		if (search == _enumeratorValuePairs.end())
		{
			std::cout << enumeratorName_ + "is not an enumerator name of teh attribute.\n";
			return MAX_VALUES;
		}
		return search->second;
	}
	const std::string& GetEnumerator(u_char numValue_) const
	{
		if (numValue_ >= _enumeratorArray.size())
		{
			std::cout << std::to_string(numValue_)	+ "is larger then number of enumerators for this attribute("
													+ std::to_string(_enumeratorArray.size()) + ").\n";
			return _enumeratorArray[0];
		}
		return _enumeratorArray[numValue_];
	}
	static const u_char MAX_VALUES = 8;
private:
	std::unordered_map<std::string, u_char> _enumeratorValuePairs;
	std::vector<std::string> _enumeratorArray;
};


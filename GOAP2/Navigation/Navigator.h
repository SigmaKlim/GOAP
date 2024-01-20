#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

struct Location
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
};

struct Navigator
{
    bool    AddPoint(const std::string& pointName)
    {
        if (_pointCatalogue.insert({pointName, std::vector<Location>()}).second == false)
            return false;
        return true;
    }
    bool    AddLocation(const std::string& pointName, const Location& location)
    {
        auto search = _pointCatalogue.find(pointName);
        if (search == _pointCatalogue.end())
        {
            std::cout << "Wrong point name \"" << pointName << "\".\n";
            return false;
        }
        search->second.push_back(location);
        for (auto& pointLocationsPair : _pointCatalogue)
            for (auto& l : pointLocationsPair.second)
            {
                auto newDistance = GetEuclideanDistance(location, l);
                if (newDistance > _maxDistance)
                    _maxDistance = newDistance; 
            }
        return true;
    }
    float   GetEuclideanDistance(const Location& from, const Location& to) const
    {
        return sqrtf(pow(to.X - from.X, 2.0f) + pow(to.Y - from.Y, 2.0f) + pow(to.Z - from.Z, 2.0f)); 
    }
    float   GetMinEuclideanDistance(const Location& currentLocation, const std::string& arrivalPointName)
    {
        float minResultDistance = std::numeric_limits<float>::max();
        auto arrSearch = _pointCatalogue.find(arrivalPointName);
        if (arrSearch == _pointCatalogue.end())
        {
            std::cout << "Wrong point name \"" << arrivalPointName << "\".\n";
            return std::numeric_limits<float>::max();
        }
        for (auto& location : arrSearch->second)
        {
            float distance = GetEuclideanDistance(currentLocation, location); 
            if (distance < minResultDistance)
                minResultDistance = distance;
        }
        return minResultDistance;
    }
    float   GetMinEuclideanDistance(const Location& currentLocation, const std::string& departurePointName, const std::string& arrivalPointName)
    {
        float minDeparturePointDistance = std::numeric_limits<float>::max();
        Location closestDeparturePoint;
        auto depSearch = _pointCatalogue.find(departurePointName);
        if (depSearch == _pointCatalogue.end())
        {
            std::cout << "Wrong point name \"" << departurePointName << "\".\n";
            return std::numeric_limits<float>::max();
        }
        for (auto& location : depSearch->second)
        {
            float distance = GetEuclideanDistance(currentLocation, location); 
            if (distance < minDeparturePointDistance)
            {
                minDeparturePointDistance = distance;
                closestDeparturePoint = location;
            }
        }
        float minResultDistance = std::numeric_limits<float>::max();
        auto arrSearch = _pointCatalogue.find(arrivalPointName);
        if (arrSearch == _pointCatalogue.end())
        {
            std::cout << "Wrong point name \"" << arrivalPointName << "\".\n";
            return std::numeric_limits<float>::max();
        }
        for (auto& location : arrSearch->second)
        {
            float distance = GetEuclideanDistance(closestDeparturePoint, location); 
            if (distance < minResultDistance)
                minResultDistance = distance;
        }
        return minResultDistance;
    }
    //Returns maximal distance between some two locations
    float   GetMaxEuclideanDistance() const
    {
        return _maxDistance;
    }
private:
    std::unordered_map<std::string, std::vector<Location>> _pointCatalogue;
    float _maxDistance = 0.0f;
};
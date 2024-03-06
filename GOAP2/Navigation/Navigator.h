#pragma once
#include <cassert>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <boost/container_hash/hash.hpp>


//A structure that holds ids of nodes, divided into groups by name, as well as distances between nodes (from, to -> dist)
struct Navigator
{
    //Adds nodes, but does not check for copies under other names
    void AddNodes(const std::string& name, const std::unordered_set<int>& ids);
    const std::unordered_set<int>& GetNodesByName(const std::string& name) const;
    //(Re)sets distance for given pair of end nodes
    void SetDistance(int from , int to, float distance);
    float GetDistance(int from, int to) const;
    float GetMaxDistance() const;
    
    bool IsSymmetric = true;
private:
    std::unordered_map<std::string, std::unordered_set<int>> nodeMap; //nodeName -> nodeIds
    std::unordered_map<std::pair<int, int>, float, boost::hash<std::pair<int, int>>> distances;
    float maxDistance = 0;
};






#pragma once
#include <BitMask.h>
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <random>
#include <string>
#include <iostream>

#include "MathHelper.h"
#include "FibonacciHeap.hpp"
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include "Plan.h"


extern const int INFTY;
typedef unsigned long long ull;

template <typename t_vertex>
struct Path
{
	std::vector<t_vertex> Vertices;
	int Cost = 0;
};

template <typename t_id>
struct Node
{
	Node(t_id		 id,
		 t_id		prevNodeId,
		 unsigned	distFromStart,
		 unsigned	heuristic)
		 : _id(id), _prevNodeId(prevNodeId), _distFromStart(distFromStart), _heuristic(heuristic) {};
	bool operator<(const Node& right) const
	{
		return _distFromStart + _heuristic < right._distFromStart + right._heuristic;
	};
	bool operator>(const Node& right) const
	{
		return _distFromStart + _heuristic > right._distFromStart + right._heuristic;
	};
	t_id	  _id = t_id();
	t_id _prevNodeId = 0;
	unsigned _distFromStart = 0;
	unsigned _heuristic = 0;


};
struct TelemetryData
{
	ull totalBytesUsed = 0;		//total amount of memory used
	unsigned expandedNum = 0;	//number of expanded vertices
	unsigned discoveredNum = 0;	//number of discovered vertices
};

template <typename t_vertex, typename t_id>
class BasePathfinder
{
	
protected:
	virtual int			GetHeuristic	(const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const
	{
		return 0;
	};
	virtual void		GetNeighbors	(std::vector<t_vertex>&	neighbors_, const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const = 0; //returns array of newly created node's neighbors
	virtual bool		Satisfies		(const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const = 0; //checks whether this node satisfies conditions of the target node; target is a shell for Node(T* cond), i.e. for any node with set cond field
	virtual t_id		GetId(const t_vertex& vertex_) const = 0;
	virtual unsigned	GetDist			(const t_vertex& from_, const t_vertex& to_) const = 0;
public:
						BasePathfinder	() {};
	virtual				~BasePathfinder	() {};
			bool		Pathfind		(Path<t_vertex>& path_, t_vertex start_, t_vertex finish_ = t_vertex(), TelemetryData* telemetryData = nullptr) const
	{

		FibonacciHeap<Node<t_id>>											discovered; //a queue of discovered but not expanded nodes
		std::unordered_map<t_id, Element<Node<t_id>>*, boost::hash<t_id>>	discMap; //used to access elements of heap and check if a node had been discovered (by id)
		std::unordered_set<t_id, boost::hash<t_id>>							expanded; //a set of expanded nodes, used to check if a node had been expanded
		std::unordered_map<t_id, t_vertex, boost::hash<t_id>>				vertices;
		std::unordered_map <t_id, t_id, boost::hash<t_id>>							cameFrom; // <nodeId, prevNodeId>

		unsigned discoveredHeapSize = 0;
		auto startId = GetId(start_);
		Node<t_id> currentNode(startId, t_id(), 0, GetHeuristic(start_, finish_));
		vertices.insert({ startId, start_ });
		cameFrom.insert({ startId, t_id() });
		Element<Node<t_id>>* currentElement = discovered.insert(currentNode);
		discoveredHeapSize++;
		discMap.insert({ currentNode._id, currentElement });
		if (telemetryData != nullptr)
			++telemetryData->discoveredNum;
				
		std::vector <t_vertex> neighborVertices;
		while (true)
		{
			if (discovered.isEmpty() == true)
			{
				path_.Vertices.clear();
				return false;
			}
			currentNode = discovered.extractMin();
			discoveredHeapSize--;
			discMap.erase(currentNode._id);
			t_vertex currentVertex = vertices.at(currentNode._id);
			if (Satisfies(currentVertex, finish_) == true)
				break;
			expanded.insert({ currentNode._id });
			if (telemetryData != nullptr)
				++telemetryData->expandedNum;
			GetNeighbors(neighborVertices, currentVertex, finish_);
			for (int i = 0; i < neighborVertices.size(); i++)
			{
				auto neighborId = GetId(neighborVertices[i]);
				auto tempIt = discMap.find(neighborId);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<Node<t_id>>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				bool wasExpanded = expanded.find(neighborId) != expanded.end();
				Node<t_id> neighborNode(	neighborId, 
									currentNode._id, 
									currentNode._distFromStart + GetDist(currentVertex, neighborVertices[i]), 
									GetHeuristic(neighborVertices[i], finish_));
				if (wasDiscovered == false && wasExpanded == false)
				{
					neighborElement = discovered.insert({ neighborNode });
					discMap.insert({ neighborId, neighborElement });
					vertices.insert({neighborId, neighborVertices[i]});
					if (cameFrom.insert({neighborId, currentNode._id}).second != true)
						std::cout << "!";
				}
				else if (	wasDiscovered == true && 
							neighborElement->getKey()._distFromStart > neighborNode._distFromStart)
				{
					discovered.decreaseKey(neighborElement, neighborNode);
					if (cameFrom.insert_or_assign(neighborId, currentNode._id).second != false)
						std::cout << "!";
				}
			}
			neighborVertices.clear();
		}
		path_.Cost = currentNode._distFromStart;
		t_id anotherId = currentNode._id;
		while (anotherId != t_id())
		{
			t_vertex anotherVertex = vertices.at(anotherId);
			path_.Vertices.push_back(anotherVertex);
			anotherId = cameFrom.at(anotherId);
		}
		for (u_int i = 0; i < path_.Vertices.size() / 2; i++)
			std::swap(path_.Vertices[i], path_.Vertices[path_.Vertices.size() - 1 - i]);
				const unsigned DISCOVERED_HEAP_ELEMENT_SIZE = sizeof(decltype(discovered.getMin()));
				const unsigned DISCOVERED_MAP_ELEMENT_SIZE	= sizeof(decltype(*discMap.begin()));
				const unsigned EXPANDED_ELEMENT_SIZE		= sizeof(decltype(*expanded.begin()));
				const unsigned VERTICES_ELEMENT_SIZE		= sizeof(decltype(*vertices.begin()));
				const unsigned CAME_FROM_ELEMENT_SIZE		= sizeof(decltype(*cameFrom.begin()));
				if (telemetryData != nullptr)
					telemetryData->totalBytesUsed	+= discoveredHeapSize * DISCOVERED_HEAP_ELEMENT_SIZE
													+ discMap.size() * DISCOVERED_MAP_ELEMENT_SIZE
													+ expanded.size() * EXPANDED_ELEMENT_SIZE
													+ vertices.size() * VERTICES_ELEMENT_SIZE
													+ cameFrom.size() * CAME_FROM_ELEMENT_SIZE;
		return true;
	}

};


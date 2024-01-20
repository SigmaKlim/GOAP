#pragma once
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <cassert>
#include <string>
#include <iostream>

#include "Tools/MathHelper.h"
#include "Tools/FibonacciHeap.hpp"
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>

#define CHECK_DISTANCE_NORMALIZED(value) if (value > 1.0f) std::cout << "WARNING: Normalized distance value (" << std::to_string(value) << ") is greater than 1.0f.\n"; \
	
#define CHECK_HEURISTICS_NORMALIZED(value) if (value > 1.0f) std::cout << "WARNING: Normalized heuristics value (" << std::to_string(value) << ") is greater than 1.0f.\n"; \

typedef unsigned long long ull;

template <typename t_vertex>
struct Path
{
	std::vector<t_vertex> Vertices;
	float Cost = 0;
};

//An internal class for constructing path in graph. The algorithm only uses ids to distinguish between nodes.
//Each id is associated with a concrete user-class vertex. User must override protected methods. User himself is responsible for dealing with vertex-object internals. 
template <typename t_id>
struct Node
{
	Node(t_id		 id,
		 t_id		prevNodeId,
		 float		distFromStart,
		 float		heuristic)
		 : _id(id), _prevNodeId(prevNodeId), _distFromStart(distFromStart), _heuristic(heuristic) {};
	bool operator<(const Node& right) const
	{
		return _distFromStart + _heuristic < right._distFromStart + right._heuristic;
	}
	bool operator>(const Node& right) const
	{
		return _distFromStart + _heuristic > right._distFromStart + right._heuristic;
	}
	t_id	  _id = t_id();
	t_id _prevNodeId = 0;
	float _distFromStart = 0.0f;
	float _heuristic = 0.0f;


};
struct TelemetryData
{
	ull totalBytesUsed = 0;		//total amount of memory used
	unsigned expandedNum = 0;	//number of expanded vertices
	unsigned discoveredNum = 0;	//number of discovered vertices
};


//Utilizes A* pathfinding algorithm. All protected methods must be overriden in class template specialization.
//t_id type must support >,<, == operations. Also std::size_t hash_value(const t_id& id) function must be implemented.
template <typename t_vertex, typename t_id>
class AStarSolver
{
protected:
	virtual float GetHeuristic(const t_vertex& vertex, const t_vertex& finish = t_vertex(),
	                           void* userData = nullptr) const
	{
		return 0;
	}
	//Fills the array with newly created node's neighbors
	virtual		void		GetNeighbors	(std::vector<t_vertex>&	neighbors, const t_vertex& vertex, const t_vertex& finish = t_vertex()) const = 0;
	//Checks whether this node satisfies conditions of the target node.
	virtual		bool		Satisfies		(const t_vertex& vertex, const t_vertex& finish = t_vertex()) const = 0;
	virtual		t_id		GetId			(const t_vertex& vertex) const = 0;
	virtual		float		GetDistance		(const t_vertex& from, const t_vertex& to) const = 0;

	//Returns dDen, so that it is guaranteed that distFromStart / dDen <= 1. Used for normalizing distance.
	virtual float GetDistanceDenominator() const = 0;
	//Returns hDen, so that it is guaranteed that heuristics / hDen <= 1. Used for normalizing heuristics.
	virtual float GetHeuristicsDenominator() const = 0;
public:
							AStarSolver	() {}
	virtual					~AStarSolver	() {}
				bool		Pathfind		(Path<t_vertex>& path, t_vertex start, t_vertex finish = t_vertex(),
												TelemetryData* telemetryData = nullptr, void* userData = nullptr) const
	{

		FibonacciHeap<Node<t_id>>											discovered; //A queue of discovered but not expanded nodes
		std::unordered_map<t_id, Element<Node<t_id>>*, boost::hash<t_id>>	discMap;	//Used to access elements of heap and check if a node had been discovered (by id)
		std::unordered_set<t_id, boost::hash<t_id>>							expanded;	//A set of expanded nodes, used to check if a node had been expanded
		std::unordered_map<t_id, t_vertex, boost::hash<t_id>>				vertices;	//Used to map node id to vertex object
		std::unordered_map <t_id, t_id, boost::hash<t_id>>					cameFrom;	//Used to map node to its preceding node

		unsigned discoveredHeapSize = 0;
		auto startId = GetId(start);
		float normalizedDistance = 0.0f;
		float normalizedHeuristics = GetHeuristic(start, finish, userData) / GetHeuristicsDenominator();
		CHECK_HEURISTICS_NORMALIZED(normalizedHeuristics);
		Node<t_id> currentNode(startId, t_id(), normalizedDistance, normalizedHeuristics);
		vertices.insert({ startId, start });
		cameFrom.insert({ startId, t_id() });
		Element<Node<t_id>>* currentElement = discovered.insert(currentNode);
		discoveredHeapSize++;
		discMap.insert({ currentNode._id, currentElement });
		if (telemetryData != nullptr)
			++telemetryData->discoveredNum;
				
		std::vector <t_vertex> neighborVertices;
		while (true)
		{
			int discoveredHeapSizeDelta = 0;
			if (discovered.isEmpty() == true)
			{
				path.Vertices.clear();
				return false;
			}
			currentNode = discovered.extractMin();
			discoveredHeapSizeDelta--;
			discMap.erase(currentNode._id);
			t_vertex currentVertex = vertices.at(currentNode._id);
			if (Satisfies(currentVertex, finish) == true)
				break;
			expanded.insert({ currentNode._id });
			if (telemetryData != nullptr)
				++telemetryData->expandedNum;
			GetNeighbors(neighborVertices, currentVertex, finish);
			for (int i = 0; i < neighborVertices.size(); i++)
			{
				auto neighborId = GetId(neighborVertices[i]);
				auto tempIt = discMap.find(neighborId);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<Node<t_id>>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				bool wasExpanded = expanded.find(neighborId) != expanded.end();
				normalizedDistance = GetDistance(currentVertex, neighborVertices[i]) / GetDistanceDenominator();
				CHECK_DISTANCE_NORMALIZED(normalizedDistance);
				normalizedHeuristics = GetHeuristic(neighborVertices[i], finish, userData) / GetHeuristicsDenominator();
				CHECK_HEURISTICS_NORMALIZED(normalizedHeuristics);
				Node<t_id> neighborNode(	neighborId, 
											currentNode._id, 
											currentNode._distFromStart + normalizedDistance, 
											normalizedHeuristics);
				if (wasDiscovered == false && wasExpanded == false)
				{
					neighborElement = discovered.insert({ neighborNode });
					discMap.insert({ neighborId, neighborElement });
					if (telemetryData != nullptr)
						++telemetryData->discoveredNum;
					discoveredHeapSizeDelta++;
					vertices.insert({neighborId, neighborVertices[i]});
					assert(cameFrom.insert({neighborId, currentNode._id}).second == true);
				}
				else if (	wasDiscovered == true && 
							neighborElement->getKey()._distFromStart > neighborNode._distFromStart)
				{
					discovered.decreaseKey(neighborElement, neighborNode);
					cameFrom.insert_or_assign(neighborId, currentNode._id).second;
				}
			}
			neighborVertices.clear();
			if (discoveredHeapSizeDelta > 0)
				discoveredHeapSize += discoveredHeapSizeDelta;
		}
		path.Cost = currentNode._distFromStart;
		t_id anotherId = currentNode._id;
		while (anotherId != t_id())
		{
			t_vertex anotherVertex = vertices.at(anotherId);
			path.Vertices.push_back(anotherVertex);
			anotherId = cameFrom.at(anotherId);
		}
		for (u_int i = 0; i < path.Vertices.size() / 2; i++)
			std::swap(path.Vertices[i], path.Vertices[path.Vertices.size() - 1 - i]);
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


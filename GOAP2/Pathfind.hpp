#pragma once
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

extern const int INFTY;
typedef unsigned long long ull;
typedef unsigned t_node_id;
typedef int arc_id;
const t_node_id NO_ID = UINT_MAX;

template <typename t_vertex>
struct Path
{
	std::vector<t_vertex> Vertices;
	int Cost = 0;
};

struct Node
{
	Node(t_node_id id_,
		 t_node_id prevNodeId_,
		 unsigned distFromStart_,
		 unsigned heuristic_)
		 : _id(id_), _prevNodeId(prevNodeId_), _distFromStart(distFromStart_), _heuristic(heuristic_) {};
	bool operator<(const Node& right) const
	{
		return _distFromStart + _heuristic < right._distFromStart + right._heuristic;
	};
	bool operator>(const Node& right) const
	{
		return _distFromStart + _heuristic > right._distFromStart + right._heuristic;
	};
	t_node_id _id = 0;
	t_node_id _prevNodeId = 0;
	//arc_id prevArcId = arc_id();
	unsigned _distFromStart = 0;
	unsigned _heuristic = 0;


};

template <typename t_vertex>
class BasePathfinder
{
	
protected:
	virtual int			GetHeuristic	(const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const
	{
		return 0;
	};
	virtual void		GetNeighbors	(std::vector<t_vertex>&	neighbors_, const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const = 0; //returns array of newly created node's neighbors
	virtual bool		Satisfies		(const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const = 0; //checks whether this node satisfies conditions of the target node; target is a shell for Node(T* cond), i.e. for any node with set cond field
	virtual t_node_id	GetId			(const t_vertex& vertex_) const = 0;
	virtual unsigned	GetDist			(const t_vertex& from_, const t_vertex& to_) const = 0;
public:
						BasePathfinder	() {};
	virtual				~BasePathfinder	() {};
			bool		Pathfind		(Path<t_vertex>& path_, t_vertex start_, t_vertex finish_ = t_vertex()) const
	{

		FibonacciHeap<Node> discovered; //a queue of discovered but not expanded nodes
		std::unordered_map<t_node_id, Element<Node>*> discMap; //used to access elements of heap and check if a node had been discovered (by id)
		std::unordered_set<t_node_id> expanded; //a set of expanded nodes, used to check if a node had been expanded
		std::unordered_map<t_node_id, t_vertex> vertices;
		std::map <t_node_id, t_node_id> cameFrom; // <nodeId, prevNodeId>

		auto startId = GetId(start_);
		Node currentNode(startId, NO_ID, 0, GetHeuristic(start_, finish_));
		vertices.insert({ startId, start_ });
		cameFrom.insert({ startId, NO_ID });
		Element<Node>* currentElement = discovered.insert(currentNode);
		discMap.insert({ currentNode._id, currentElement });
		
		std::vector <t_vertex> neighborVertices;
		while (true)
		{
			if (discovered.isEmpty() == true)
			{
				path_.Vertices.clear();
				return false;
			}
			currentNode = discovered.extractMin();
			discMap.erase(currentNode._id);
			t_vertex currentVertex = vertices.at(currentNode._id);
			if (Satisfies(currentVertex, finish_) == true)
				break;
			expanded.insert({ currentNode._id });
			GetNeighbors(neighborVertices, currentVertex, finish_);
			for (int i = 0; i < neighborVertices.size(); i++)
			{
				auto neighborId = GetId(neighborVertices[i]);
				auto tempIt = discMap.find(neighborId);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<Node>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				bool wasExpanded = expanded.find(neighborId) != expanded.end();
				Node neighborNode(	neighborId, 
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
		t_node_id anotherId = currentNode._id;
		while (anotherId != NO_ID)
		{
			t_vertex anotherVertex = vertices.at(anotherId);
			path_.Vertices.push_back(anotherVertex);
			anotherId = cameFrom.at(anotherId);
		}
		//path_.vertices.push_back(finish_);
		for (u_int i = 0; i < path_.Vertices.size() / 2; i++)
			std::swap(path_.Vertices[i], path_.Vertices[path_.Vertices.size() - 1 - i]);
		return true;
	}
	
};


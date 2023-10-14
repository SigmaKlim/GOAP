#pragma once
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <random>
#include <ctime>
#include <string>
#include <iostream>
//#include <memory>
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
	std::vector<t_vertex> vertices;
	int cost = 0;
};

struct Node
{
	Node(t_node_id id_,
		 t_node_id prevNodeId_,
		 unsigned distFromStart_,
		 unsigned heuristic_)
		 : id(id_), prevNodeId(prevNodeId_), distFromStart(distFromStart_), heuristic(heuristic_) {};
	bool operator<(const Node& right) const
	{
		return distFromStart + heuristic < right.distFromStart + right.heuristic;
	};
	bool operator>(const Node& right) const
	{
		return distFromStart + heuristic > right.distFromStart + right.heuristic;
	};
	t_node_id id = 0;
	t_node_id prevNodeId = 0;
	//arc_id prevArcId = arc_id();
	unsigned distFromStart = 0;
	unsigned heuristic = 0;


};

template <typename t_vertex>
class BasePathfinder
{
	;
protected:
	virtual int			GetHeuristic(const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const
	{
		return 0;
	};
	virtual void		GetNeighbors(std::vector<t_vertex>&	neighbors_, const t_vertex& vertex_) const = 0; //returns array of newly created node's neighbors
	virtual bool		Satisfies(const t_vertex& vertex_, const t_vertex& finish_ = t_vertex()) const { return vertex_ == finish_; }; //checks whether this node satisfies conditions of the target node; target is a shell for Node(T* cond), i.e. for any node with set cond field
	virtual t_node_id	GetId(const t_vertex& vertex_) const = 0;
	virtual unsigned	GetDist(const t_vertex& from_, const t_vertex& to_) const = 0;
public:
	BasePathfinder() {};
	virtual ~BasePathfinder() {};
	void Pathfind(Path<t_vertex>& path_, t_vertex start_, t_vertex finish_ = t_vertex())
	{

		FibonacciHeap<Node> discovered; //a queue of discovered but not expanded nodes
		std::unordered_map<t_node_id, Element<Node>*> discMap; //used to access elements of heap and check if a node had been discovered (by id)
		std::unordered_set<t_node_id> expanded; //a set of expanded nodes, used to check if a node had been expanded
		std::unordered_map<t_node_id, t_vertex> vertices;
		std::map <t_node_id, t_node_id> cameFrom; // <nodeId, prevNodeId>
		
		Node currentNode(0, NO_ID, 0, GetHeuristic(start_, finish_));
		vertices.insert({ 0, start_ });
		cameFrom.insert({ 0, NO_ID });
		Element<Node>* currentElement = discovered.insert(currentNode);
		discMap.insert({ currentNode.id, currentElement });
		
		std::vector <t_vertex> neighborVertices;
		while (true)
		{
			if (discovered.isEmpty() == true)
			{
				path_.vertices.clear();
				return;
			}
			currentNode = discovered.extractMin();
			discMap.erase(currentNode.id);
			t_vertex currentVertex = vertices.at(currentNode.id);
			if (Satisfies(currentVertex, finish_) == true)
				break;
			expanded.insert({ currentNode.id });
			GetNeighbors(neighborVertices, currentVertex);
			for (int i = 0; i < neighborVertices.size(); i++)
			{
				auto neighborId = GetId(neighborVertices[i]);
				auto tempIt = discMap.find(neighborId);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<Node>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				bool wasExpanded = expanded.find(neighborId) != expanded.end();
				Node neighborNode(	neighborId, 
									currentNode.id, 
									currentNode.distFromStart + GetDist(currentVertex, neighborVertices[i]), 
									GetHeuristic(neighborVertices[i], finish_));
				if (wasDiscovered == false && wasExpanded == false)
				{
					neighborElement = discovered.insert({ neighborNode });
					discMap.insert({ neighborId, neighborElement });
					vertices.insert({neighborId, neighborVertices[i]});
					if (cameFrom.insert({neighborId, currentNode.id}).second != true)
						std::cout << "!";
				}
				else if (	wasDiscovered == true && 
							neighborElement->getKey().distFromStart > neighborNode.distFromStart)
				{
					discovered.decreaseKey(neighborElement, neighborNode);
					if (cameFrom.insert_or_assign(neighborId, currentNode.id).second != false)
						std::cout << "!";
				}
			}
			neighborVertices.clear();
		}
		path_.cost = currentNode.distFromStart;
		t_node_id anotherId = currentNode.id;
		while (anotherId != NO_ID)
		{
			t_vertex anotherVertex = vertices.at(anotherId);
			path_.vertices.push_back(anotherVertex);
			anotherId = cameFrom.at(anotherId);
		}
		//path_.vertices.push_back(finish_);
		for (u_int i = 0; i < path_.vertices.size() / 2; i++)
			std::swap(path_.vertices[i], path_.vertices[path_.vertices.size() - 1 - i]);
	}

};


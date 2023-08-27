#pragma once
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <random>
#include <ctime>
#include <string>
#include <iostream>
#include "MathHelper.h"
#include "FibonacciHeap.hpp"

extern const int INFTY;
typedef unsigned long long ull;
typedef ull node_id;
typedef int arc_id;

class Path
{
	friend class BasePathfinder;

	std::vector<arc_id> arcs;
	int cost = 0;

public:
	const std::vector<arc_id>& GetArcList() const
	{
		return arcs;
	}
	int GetCost() const
	{
		return cost;
	}
};


class Node
{
	friend class BasePathfinder;

public:

	const ull& GetId() const
	{
		return id;
	};
	bool operator>(const Node* right) const
	{
		return distFromStart + heuristic > right->distFromStart + right->heuristic;
	};
	bool operator<(const Node* right) const
	{
		return distFromStart + heuristic < right->distFromStart + right->heuristic;
	};
private:
	Node(ull id_, 
		Node* prevNodePtr = nullptr, 
		arc_id prevArc_ = arc_id(), 
		int distFromStart_ = 0,
		int heuristics_ = 0) : 
		id(id_), prevNode (prevNodePtr), prevArcId(prevArc_), distFromStart(distFromStart_), heuristic(heuristics_) {};
	virtual ~Node() {};
	
	node_id id = 0;
	Node* prevNode = nullptr;
	arc_id prevArcId = arc_id();
	int distFromStart = 0;
	int heuristic = 0;
};

class BasePathfinder
{
protected:
	virtual int GetDist(const arc_id& arc) const = 0; //returns the length of arc
	virtual int GetHeuristic(const node_id& node) const
	{
		return 0;
	};
	virtual std::vector <std::pair<node_id, arc_id>> GetTransitions(const node_id id) const = 0; //returns the vector pairs of neighbor id and arc node->neighbor id
	virtual bool Satisfies(const Node* node, const Node* target) const = 0; //checks whether this node satisfies conditions of the target node; target is a shell for Node(T* cond), i.e. for any node with set cond field
public:
	BasePathfinder() {};
	virtual ~BasePathfinder() {};
	Path* Pathfind(node_id start, node_id finish)
	{
		Path* path = new Path();
		struct NodePtr
		{
			Node/*<T_NodeDesc, T_ArcDesc>*/* nodePtr = nullptr;
			bool operator>(const NodePtr& right) const
			{
				return nodePtr->operator>(right.nodePtr);
			}; 
			bool operator<(const NodePtr& right) const
			{
				return nodePtr->operator<(right.nodePtr);
			};
			Node/*<T_NodeDesc, T_ArcDesc>*/* operator-> () const
			{
				return nodePtr;
			}
		};
		FibonacciHeap<NodePtr> discovered; //a queue of discovered but not expanded nodes
		std::map<node_id, Element<NodePtr>*> discMap; //used to access elements of heap and check if a node had been discovered (by id)
		std::map<node_id, NodePtr> expanded; //a set of expanded nodes, used to check if a node had been expanded (by id)
				
		NodePtr currentNode = { new Node(start, nullptr, arc_id(), 0, GetHeuristic(start)) };
		NodePtr finishNode = { new Node(finish) };
		auto currentElement = discovered.insert(currentNode);
		discMap.insert({ currentNode->id, currentElement });
		while (true)
		{
			if (discovered.isEmpty() == true)
				return nullptr;
			currentNode = discovered.extractMin();
			discMap.erase(currentNode->id);
			if (Satisfies(currentNode.nodePtr, finishNode.nodePtr) == true)
				break;
			expanded.insert({ currentNode->id, currentNode });
			std::vector <std::pair<node_id, arc_id>> neighborDescList = GetTransitions(currentNode->id);
			for (int i = 0; i < neighborDescList.size(); i++)
			{
				node_id neighborId = neighborDescList[i].first;
				int dist = GetDist(neighborDescList[i].second);
				int heuristic = GetHeuristic(neighborDescList[i].first);
				auto tempIt = discMap.find(neighborId);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<NodePtr>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				NodePtr neighbor = { new Node(	neighborId, 
												currentNode.nodePtr, 
												neighborDescList[i].second, 
												currentNode->distFromStart + dist, 
												heuristic)};
				bool wasExpanded = expanded.find(neighborId) != expanded.end();
				if (wasDiscovered == false && wasExpanded == false)
				{
					currentElement = discovered.insert(neighbor);
					discMap.insert({ neighbor->id, currentElement });
				}
				else if (wasDiscovered == true && neighborElement->getKey()->distFromStart > neighbor->distFromStart)
				{
					auto toDelete = neighborElement->getKey().nodePtr;
					discovered.decreaseKey(neighborElement, neighbor);
					delete toDelete;
				}
				else
					delete neighbor.nodePtr;
			}
		}
		path->cost = currentNode->distFromStart;
		for (auto prevNode = currentNode.nodePtr; prevNode->prevNode != nullptr; prevNode = prevNode->prevNode)
			path->arcs.push_back(prevNode->prevArcId);
		for (int i = 0; i < path->arcs.size() / 2; i++)
			std::swap(path->arcs[i], path->arcs[path->arcs.size() - 1 - i]);
		for (auto& it : discMap)
			delete it.second->getKey().nodePtr;
		for (auto& it : expanded)
			delete it.second.nodePtr;
		delete finishNode.nodePtr;
		return path;
	}

};

class TestPathfinder : public BasePathfinder
{
	int dim;
	mtrx matrix;

public:
	TestPathfinder(std::ifstream& fin, int dim_)
	{
		dim = dim_;
		MathHelper::MakeEmptyMatrix(matrix, dim);
		MathHelper::ReadMtrxFromFile(matrix, fin, ',');
	}
	int GenerateId(const int& data)
	{
		return data;
	};
	std::vector <std::pair<node_id, arc_id>> GetTransitions(const node_id id) const
	{
		static const unsigned BOUND = pow(10, MathHelper::NumDigits(dim));
		std::vector <std::pair<node_id, arc_id>> neighbors;
		for (int i = 0; i < dim; i++)
			if (matrix[id][i] != INFTY)
				neighbors.push_back({ i, BOUND * id + i });
		return neighbors;
	};
	int GetDist(const arc_id& arcId) const
	{
		static const unsigned BOUND = pow(10, MathHelper::NumDigits(dim));
		unsigned to = arcId % BOUND;
		unsigned from = arcId / BOUND;
		return matrix[from][to];
	}
	bool Satisfies(const Node* node, const Node* target) const
	{
		return node->GetId() == target->GetId();
	};
};
#pragma once
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <random>
#include <ctime>
#include <string>
#include <iostream>
#include <memory>
#include "MathHelper.h"
#include "FibonacciHeap.hpp"

extern const int INFTY;
typedef unsigned long long ull;
typedef ull node_id;
typedef int arc_id;

class Path
{
	template <typename t_node_id>
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

template <typename t_node_id>
class Node
{
	template <typename t_node_id>
	friend class BasePathfinder;

public:

	const t_node_id& GetId() const
	{
		return id;
	};
	const 
	bool operator>(const Node* right) const
	{
		return distFromStart + heuristic > right->distFromStart + right->heuristic;
	};
	bool operator<(const Node* right) const
	{
		return distFromStart + heuristic < right->distFromStart + right->heuristic;
	};

	Node(t_node_id id_, 
		const Node* prevNodePtr_ = nullptr,
		arc_id prevArcId_ = arc_id(),
		unsigned distFromPrev_ = 0,
		unsigned heuristics_ = 0)
	{
		id = id_;
		prevNodePtr = prevNodePtr_;
		prevArcId = prevArcId_;
		distFromStart = (prevNodePtr_ == nullptr) ? 0 : prevNodePtr->distFromStart + distFromPrev_;
		heuristic = heuristics_;
	}
	virtual ~Node() 
	{
	};
private:	
	t_node_id id = 0;
	const Node* prevNodePtr = nullptr;
	arc_id prevArcId = arc_id();
	unsigned distFromStart = 0;
	unsigned heuristic = 0;
};

template <typename t_node_id>
class BasePathfinder
{
protected:
	//virtual int GetDist(const arc_id& arc) const = 0; //returns the length of arc
	virtual int GetHeuristic(const node_id& node) const
	{
		return 0;
	};
	virtual std::vector <Node<t_node_id>*> GetNeighbors(const Node<t_node_id>* node) const = 0; //returns array of newly created node's neighbors
	virtual bool Satisfies(const Node<t_node_id>* node, const Node<t_node_id>* target) const = 0; //checks whether this node satisfies conditions of the target node; target is a shell for Node(T* cond), i.e. for any node with set cond field
public:
	BasePathfinder() {};
	virtual ~BasePathfinder() {};
	Path* Pathfind(t_node_id start, t_node_id finish)
	{
		Path* path = new Path();
		struct NodePtr
		{
			Node<t_node_id>* nodePtr = nullptr;
			bool operator>(const NodePtr& right) const
			{
				return nodePtr->operator>(right.nodePtr);
			}; 
			bool operator<(const NodePtr& right) const
			{
				return nodePtr->operator<(right.nodePtr);
			};
			Node<t_node_id>* operator-> () const
			{
				return nodePtr;
			}
		};
		//struct NodeIdPtr
		//{
		//	t_node_id* nodeIdPtr = nullptr;
		//	//bool operator>(const NodeIdPtr& right) const
		//	//{
		//	//	bool b = *nodeIdPtr > *right.nodeIdPtr;
		//	//	return b;
		//	//};
		//	bool operator <(const NodeIdPtr& right) const
		//	{
		//		bool b = *nodeIdPtr < *right.nodeIdPtr;
		//		return b;
		//	};
		//};
		FibonacciHeap<NodePtr> discovered; //a queue of discovered but not expanded nodes
		std::map<t_node_id*, Element<NodePtr>*, PtrLess<t_node_id>> discMap; //used to access elements of heap and check if a node had been discovered (by id)
		std::map<t_node_id*, NodePtr, PtrLess<t_node_id>> expanded; //a set of expanded nodes, used to check if a node had been expanded (by id)
				
		NodePtr currentNode = { new Node<t_node_id>(start, nullptr, arc_id(), 0, GetHeuristic(start)) };
		NodePtr finishNode = { new Node<t_node_id>(finish) };
		Element<NodePtr>* currentElement = discovered.insert(currentNode);
		discMap.insert({ &currentNode->id, currentElement });
		while (true)
		{
			if (discovered.isEmpty() == true)
				return nullptr;
			currentNode = discovered.extractMin();
			discMap.erase(&currentNode->id);
			if (Satisfies(currentNode.nodePtr, finishNode.nodePtr) == true)
				break;
			expanded.insert({ &currentNode->id, currentNode });
			std::vector <Node<t_node_id>*> neighborList = GetNeighbors(currentNode.nodePtr);
			for (int i = 0; i < neighborList.size(); i++)
			{
				auto tempIt = discMap.find(&neighborList[i]->id);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<NodePtr>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				bool wasExpanded = expanded.find(&neighborList[i]->id) != expanded.end();
				if (wasDiscovered == false && wasExpanded == false)
				{
					neighborElement = discovered.insert({ neighborList[i] });
					discMap.insert({ &neighborList[i]->id, neighborElement });
				}
				else if (wasDiscovered == true && neighborElement->getKey()->distFromStart > neighborList[i]->distFromStart)
				{
					auto toDelete = neighborElement->getKey().nodePtr;
					discovered.decreaseKey(neighborElement, { neighborList[i] });
					//delete toDelete;
				}
				else
					delete neighborList[i];
			}
		}
		path->cost = currentNode->distFromStart;
		for (const Node<t_node_id>* prevNodePtr = currentNode.nodePtr; prevNodePtr->prevNodePtr != nullptr; prevNodePtr = prevNodePtr->prevNodePtr)
			path->arcs.push_back(prevNodePtr->prevArcId);
		for (int i = 0; i < path->arcs.size() / 2; i++)
			std::swap(path->arcs[i], path->arcs[path->arcs.size() - 1 - i]);
	/*	for (auto& it : discMap)
			delete it.second->getKey().nodePtr;*/
		for (auto& it : expanded)
			delete it.second.nodePtr;
		delete finishNode.nodePtr;
		return path;
	}

};

class TestPathfinder : public BasePathfinder<int>
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
	std::vector <Node<int>*> GetNeighbors(const Node<int>* node) const
	{
		static const unsigned BOUND = pow(10, MathHelper::NumDigits(dim));
		std::vector <Node<int>*> neighbors;
		for (int i = 0; i < dim; i++)
			if (matrix[node->GetId()][i] != INFTY)
			{
				neighbors.push_back(new Node<int>(i, node, BOUND * node->GetId() + i, matrix[node->GetId()][i], 0));
			}
			
		return neighbors;
	};
	int GetDist(const arc_id& arcId) const
	{
		static const unsigned BOUND = pow(10, MathHelper::NumDigits(dim));
		unsigned to = arcId % BOUND;
		unsigned from = arcId / BOUND;
		return matrix[from][to];
	}
	bool Satisfies(const Node<int>* node, const Node<int>* target) const
	{
		int id1 = node->GetId();
		int id2 = target->GetId();
		return (node->GetId()) == (target->GetId());
	};
};
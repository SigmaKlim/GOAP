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

template <class T_ArcDesc>
class Path
{
	template <class T_NodeDesc, class T_ArcDesc>
	friend class BasePathfinder;

	std::vector<T_ArcDesc> arcs;
	int cost = 0;

public:
	const std::vector<T_ArcDesc>& GetArcList() const
	{
		return arcs;
	}
	int GetCost() const
	{
		return cost;
	}
};

template <class T_NodeDesc, class T_ArcDesc>
class Node
{
	template <class T_NodeDesc, class T_ArcDesc>
	friend class BasePathfinder;

public:
	const T_NodeDesc& GetData() const
	{
		return data;
	};
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
		T_NodeDesc data_, 
		Node* prevNodePtr = nullptr, 
		T_ArcDesc prevArc_ = T_ArcDesc(), 
		int distFromStart_ = 0,
		int heuristics_ = 0) : 
		id(id_), data(data_), prevNode (prevNodePtr), prevArc(prevArc_), distFromStart(distFromStart_), heuristic(heuristics_) {};
	virtual ~Node() {};
	
	ull id = 0;
	T_NodeDesc data;
	Node* prevNode = nullptr;
	T_ArcDesc prevArc = T_ArcDesc();
	int distFromStart = 0;
	int heuristic = 0;
};

template <class T_NodeDesc, class T_ArcDesc>
class BasePathfinder
{
protected:
	virtual int GetDist(const T_ArcDesc& arc) const = 0; //returns the length of arc
	virtual int GetHeuristic(const T_NodeDesc& node) const
	{
		return 0;
	};
	virtual std::vector <std::pair<T_NodeDesc, T_ArcDesc>> GetNeighborDescList(const T_NodeDesc nodeDesc) const = 0; //returns the vector pairs of neighbor descriptor and arc node->neighbor
	virtual int GenerateId(const T_NodeDesc& data) = 0; //must make unique ID for each Node according to data
	virtual bool Satisfies(const Node<T_NodeDesc, T_ArcDesc>* node, const Node<T_NodeDesc, T_ArcDesc>* target) const = 0; //checks whether this node satisfies conditions of the target node; target is a shell for Node(T* cond), i.e. for any node with set cond field
public:
	BasePathfinder() {};
	virtual ~BasePathfinder() {};
	Path<T_ArcDesc>* Pathfind(T_NodeDesc* start, T_NodeDesc* finish)
	{
		Path<T_ArcDesc>* path = new Path<T_ArcDesc>();
		struct NodePtr
		{
			Node<T_NodeDesc, T_ArcDesc>* nodePtr = nullptr;
			bool operator>(const NodePtr& right) const
			{
				return nodePtr->operator>(right.nodePtr);
			}; 
			bool operator<(const NodePtr& right) const
			{
				return nodePtr->operator<(right.nodePtr);
			};
			Node<T_NodeDesc, T_ArcDesc>* operator-> () const
			{
				return nodePtr;
			}
		};
		FibonacciHeap<NodePtr> discovered; //a queue of discovered but not expanded nodes
		std::map<ull, Element<NodePtr>*> discMap; //used to access elements of heap and check if a node had been discovered (by id)
		std::map<ull, NodePtr> expanded; //a set of expanded nodes, used to check if a node had been expanded (by id)
				
		NodePtr currentNode = { new Node<T_NodeDesc, T_ArcDesc>(GenerateId(*start), 
																*start,
																nullptr,
																T_ArcDesc(),
																0,
																GetHeuristic(*start)) };
		NodePtr finishNode = { new Node<T_NodeDesc, T_ArcDesc>(GenerateId(*finish), *finish) };
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
			std::vector <std::pair<T_NodeDesc, T_ArcDesc>> neighborDescList = GetNeighborDescList(currentNode->data);
			for (int i = 0; i < neighborDescList.size(); i++)
			{
				ull neighborId = GenerateId(neighborDescList[i].first);
				int dist = GetDist(neighborDescList[i].second);
				int heuristic = GetHeuristic(neighborDescList[i].first);
				auto tempIt = discMap.find(neighborId);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<NodePtr>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				NodePtr neighbor = { new Node<T_NodeDesc, T_ArcDesc>(neighborId, 
																	neighborDescList[i].first, 
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
			path->arcs.push_back(prevNode->prevArc);
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

class TestPathfinder : public BasePathfinder<int, std::string>
{
	int dim = 5;
	mtrx matrix;

public:
	TestPathfinder(std::ifstream& fin, int dim_)
	{
		dim = dim_;
		MathHelper::MakeEmptyMatrix(matrix, dim);
		MathHelper::ReadMtrxFromFile(matrix, fin,/* 0,*/ ',');
	}
	int GenerateId(const int& data)
	{
		return data;
	};
	std::vector <std::pair<int, std::string>> GetNeighborDescList(const int nodeDesc) const
	{
		std::vector <std::pair<int, std::string>> neighbors;
		for (int i = 0; i < dim; i++)
			if (matrix[nodeDesc][i] != INFTY)
				neighbors.push_back({ i, std::to_string(nodeDesc) + "-" + std::to_string(i) });
		return neighbors;
	};
	int GetDist(const std::string& arc) const
	{
		auto dashPos = arc.find('-', 0);
		auto from = std::stoi(arc.substr(0, dashPos));
		auto to = std::stoi(arc.substr(dashPos + 1, arc.length() - dashPos - 1));
		if (from < 0 || from > dim || to < 0 || to > dim)
			return -1;
		return matrix[from][to];
	}
	bool Satisfies(const Node<int, std::string>* node, const Node<int, std::string>* target) const
	{
		return node->GetData() == target->GetData();
	};
};
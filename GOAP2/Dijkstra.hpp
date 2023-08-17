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

	void Reverse()
	{
		for (int i = 0; i < arcs.size() / 2; i++)
			std::swap(arcs[i], arcs[arcs.size() - 1 - i]);
	}; //swaps elements of path so that 1st element becomes last etc.
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
		return distFromStart > right->distFromStart;
	};
	bool operator<(const Node* right) const
	{
		return distFromStart < right->distFromStart;
	};
private:
	Node(ull id_, T_NodeDesc data_, Node* prevNode_ = nullptr, T_ArcDesc prevArc_ = T_ArcDesc()) : id(id_), data(data_), prevNode (prevNode_), prevArc(prevArc_) {};
	virtual ~Node() {};
	//bool operator>(const Node& right) const
	//{
	//	return distFromStart > right.distFromStart;
	//}; //for ordering
	//bool operator<(const Node& right) const
	//{
	//	return distFromStart < right.distFromStart;
	//}; //for ordering

	ull id = 0;
	T_NodeDesc data;
	Node* prevNode = nullptr;
	T_ArcDesc prevArc = T_ArcDesc();
	int distFromStart = 0;
		
};

template <class T_NodeDesc, class T_ArcDesc>
class BasePathfinder
{
protected:
	virtual int GetDist(const T_ArcDesc& arc) const = 0; //returns the length of arc
	virtual std::vector <std::pair<T_NodeDesc, T_ArcDesc>> GetNeighborDescList(const T_NodeDesc nodeDesc) const = 0; //returns the vector pairs of neighbor descriptor and arc node->neighbor
	virtual int GenerateId(const T_NodeDesc& data) = 0; //must make unique ID for each Node according to data
	virtual bool Satisfies(const Node<T_NodeDesc, T_ArcDesc>* node, const Node<T_NodeDesc, T_ArcDesc>* target) const = 0; //checks whether this node satisfies conditions of the target node; target is a shell for Node(T* cond), i.e. for any node with set cond field
public:
	BasePathfinder() {};
	virtual ~BasePathfinder() {};
	Path<T_ArcDesc>* Pathfind(T_NodeDesc* start, T_NodeDesc* finish)
	{
		Path<T_ArcDesc>* path = new Path<T_ArcDesc>();
		struct Node_
		{
			Node<T_NodeDesc, T_ArcDesc>* nodePtr;
			bool operator>(const Node_& right) const
			{
				return nodePtr->operator>(right.nodePtr);
			}; 
			bool operator<(const Node_& right) const
			{
				return nodePtr->operator<(right.nodePtr);
			};
			Node<T_NodeDesc, T_ArcDesc>* operator-> () const
			{
				return nodePtr;
			}
		};
		FibonacciHeap<Node<T_NodeDesc, T_ArcDesc>*> discovered; //a queue of discovered but not expanded nodes
		std::map<ull, Element<Node<T_NodeDesc, T_ArcDesc>*>*> discMap; //used to access elements of heap and check if a node had been discovered (by id)
		std::map<ull, Node<T_NodeDesc, T_ArcDesc>*> expanded; //a set of expanded nodes, used to check if a node had been expanded (by id)
				
		Node<T_NodeDesc, T_ArcDesc>* currentNode = new Node<T_NodeDesc, T_ArcDesc>(GenerateId(*start), *start);
		Node<T_NodeDesc, T_ArcDesc>* finishNode = new Node<T_NodeDesc, T_ArcDesc>(666, *finish);//!
		auto currentElement = discovered.insert(currentNode);
		discMap.insert({ currentNode->id, currentElement });
		while (true)
		{
			if (discovered.isEmpty() == true)
				return nullptr;
			currentNode = discovered.extractMin();
			discMap.erase(currentNode->id);
			if (Satisfies(currentNode, finishNode) == true)
				break;
			expanded.insert({ currentNode->id, currentNode });
			std::vector <std::pair<T_NodeDesc, T_ArcDesc>> neighborDescList = GetNeighborDescList(currentNode->data);
			for (int i = 0; i < neighborDescList.size(); i++)
			{
				Node<T_NodeDesc, T_ArcDesc>* neighbor = new Node<T_NodeDesc, T_ArcDesc>(GenerateId(neighborDescList[i].first), neighborDescList[i].first, currentNode, neighborDescList[i].second);
				auto tempIt = discMap.find(neighbor->id);
				bool wasDiscovered = (tempIt != discMap.end());
				Element<Node<T_NodeDesc, T_ArcDesc>*>* neighborElement = (wasDiscovered == true) ? tempIt->second : nullptr;
				bool wasExpanded = expanded.find(neighbor->id) != expanded.end();
				int dist = GetDist(neighborDescList[i].second);
				if (wasDiscovered == false && wasExpanded == false)
				{
					neighbor->distFromStart = currentNode->distFromStart + dist;
					neighbor->prevNode = currentNode;
					currentElement = discovered.insert(neighbor);
					discMap.insert({ neighbor->id, currentElement });
				}
				else if (wasDiscovered == true && neighbor->distFromStart > currentNode->distFromStart + dist)
				{
					neighbor->distFromStart = currentNode->distFromStart + dist;
					neighbor->prevNode = currentNode;
					discovered.decreaseKey(neighborElement, neighbor);
				}
				//else delete neighbor;
			}
		}
		path->cost = currentNode->distFromStart;
		for (auto prevNode = currentNode; prevNode != nullptr; prevNode = prevNode->prevNode)
			path->arcs.push_back(prevNode->prevArc);
		path->Reverse();
		for (auto& it : discMap)
			delete it.second->getKey();
		for (auto& it : expanded)
			delete it.second;
		delete finishNode;
		return path;
	}

};

class TestPathfinder : public BasePathfinder<int, std::string>
{
	const int N = 5;
	const float DISC_CHANCE = 0.67f;
	mtrx matrix;

public:
	TestPathfinder()
	{
		std::ifstream fin("test_data.txt");
		MathHelper::MakeEmptyMatrix(matrix, N);
		MathHelper::ReadMtrxFromFile(matrix, fin, 0, ',');
	}
	int GenerateId(const int& data)
	{
		return data;
	};
	std::vector <std::pair<int, std::string>> GetNeighborDescList(const int nodeDesc) const
	{
		std::vector <std::pair<int, std::string>> neighbors;
		for (int i = 0; i < N; i++)
			if (matrix[nodeDesc][i] != INFTY)
				neighbors.push_back({ i, std::to_string(nodeDesc) + "-" + std::to_string(i) });
		return neighbors;
	};
	int GetDist(const std::string& arc) const
	{
		int dashPos = arc.find('-', 0);
		if (dashPos != 1 && dashPos != 2)
			return -1;
		auto from = std::stoi(arc.substr(0, dashPos));
		auto to = std::stoi(arc.substr(dashPos + 1, arc.length() - dashPos - 1));
		if (from < 0 || from > N || to < 0 || to > N)
			return -1;
		return matrix[from][to];
	}
	bool Satisfies(const Node<int, std::string>* node, const Node<int, std::string>* target) const
	{
		return node->GetData() == target->GetData();
	};
};
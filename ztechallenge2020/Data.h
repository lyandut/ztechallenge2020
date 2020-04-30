#pragma once
#ifndef ZTECHALLEGE2020_DATA_H
#define ZTECHALLEGE2020_DATA_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

#include "utilities.h"

using namespace std;

class Node {
public:
	Node() {}

	Node(int id, string name, int workers, int linkNum, int carNum) :
		id(id), name(name), totalWorkerNum(workers), availWorkerNum(workers),
		relatedLinks(linkNum, vector<bool>(carNum, false)) {}

	int getAvailWorkerNum() const { return availWorkerNum; }

	bool isAllocateWorker(int linkId, int carId) { return relatedLinks[linkId][carId]; }

	void allocateWorker(int linkId = 0, int carId = 0) {
		relatedLinks[linkId][carId] = true;
		--availWorkerNum;
	}

private:
	int id;
	string name;
	int totalWorkerNum;
	int availWorkerNum;
	vector<vector<bool>> relatedLinks; // 相连轨道id
};

class Link {
public:
	struct Car {
		Car(int id, int link, double weight) :
			id(id), linkId(link), maxWeight(weight), availWeight(weight) {};

		int id;
		int linkId;
		double maxWeight;
		double availWeight;
	};

	Link() {}

	Link(int id, string name, int src, int dst, int cost = 1) :
		id(id), name(name), srcNode(src), dstNode(dst), cost(cost) {}

	int getId() const { return id; }

	string getName() const { return name; }

	int getSrcNode()  const { return srcNode; }

	int getDstNode() const { return dstNode; }

	const vector<Car> &getCars() const { return cars; }

	void addCar(Car &&car) { cars.emplace_back(car); }

	void allocateCar(int carId, double weight) { cars[carId].availWeight -= weight; }

	void occupyCar(int carId) { cars[carId].availWeight = 0; }

	int getCost() const { return cost; }

private:
	int id;
	string name;
	int srcNode; // 起点id
	int dstNode; // 终点id
	vector<Car> cars; // 车队
	int cost; // 权重
};

class Item {
public:
	Item() {}

	Item(int id, string name, int src, int dst, double weight) :
		id(id), name(name), srcNode(src), dstNode(dst), weight(weight) {}

	string getName() const { return name; }

	int getSrcNode() const { return srcNode; }

	int getdstNode() const { return dstNode; }

	double getWeight() const { return weight; }

	const set<int> &getIncNodes() const { return incNodes; }

	void addIncNode(int node) { incNodes.insert(node); }

private:
	int id;
	string name;
	int srcNode;
	int dstNode;
	double weight;
	set<int> incNodes; // 必经站点id
};

class Instance {
public:
	Instance(istream &is = cin);

public:
	vector<Node> nodes;
	vector<Link> links;
	vector<Item> items;
};

class Output {
public:
	struct Result {
		using ResultNode = pair<string, int>; // pair<LinkName, CarId+1>

		Result(string item) : itemName(item) {}

		Result(string item, vector<ResultNode> &nodes)
			: itemName(item), resultNodes(nodes) {}

		string itemName;
		vector<ResultNode> resultNodes;
	};

	Output() : totalFailedNum(0), totalFailedWeight(0) {}

	void addSuccessItem(const Item &item, vector<Result::ResultNode> &resultNodes) {
		results.emplace_back(item.getName(), resultNodes);
	}

	void addFailedItem(const Item &item) {
		++totalFailedNum;
		totalFailedWeight += item.getWeight();
		results.emplace_back(item.getName());
	}

	void print(ostream &os = cout);

private:
	int totalFailedNum;
	double totalFailedWeight;
	vector<Result> results;
};

#endif // !ZTECHALLEGE2020_DATA_H

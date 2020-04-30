#pragma once
#ifndef ZTECHALLEGE2020_SOLVER_H
#define ZTECHALLEGE2020_SOLVER_H

#include "Data.h"
#include <random>
#include <ctime>

using AdjList = vector<vector<int>>; // 邻接表，元素为LinkID

using PathNode = pair<int, int>; // pair<NodeID, LinkID>

struct Vertex {
	int id;
	int dist;
	bool operator < (const Vertex &v) const { return dist > v.dist; }
};

struct DijkstraResult {
	int id;
	vector<int> predecessor; // 前驱节点id
	vector<int> preLinks;    // 前驱轨道id
	vector<Vertex> vertexes;

	DijkstraResult() { id = -1; }
};

class Solver {
public:
	Solver(Instance &ins) : ins(ins) { randomEngine.seed(time(nullptr)); }

	void run();

private:
	void buildGraph();

	void dijkstra(int s);

	int getDist(int s, int t) const { return dijkstraResults[s].vertexes[t].dist; }

	void recoverPath(int s, int t, vector<PathNode> &path);

	void solveWithoutWorker(const Item &item, const vector<PathNode> &path);

private:
	default_random_engine randomEngine;

	Instance &ins;
	AdjList adjList;
	vector<DijkstraResult> dijkstraResults;
	Output out;
};

#endif // !ZTECHALLEGE2020_SOLVER_H
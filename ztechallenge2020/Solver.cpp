#include "Solver.h"
#include <queue>
#include <algorithm>
#include <iterator>

template <typename T>
using PriorityQueue = priority_queue<T>;

void Solver::run() {
	buildGraph();

	dijkstraResults.resize(ins.nodes.size());

	sort(ins.items.begin(), ins.items.end(), [](const Item &lhs, const Item &rhs) {
		return lhs.getWeight() > rhs.getWeight();
	});

	for (auto &item : ins.items) {
		if (dijkstraResults[item.getSrcNode()].id == -1) {
			dijkstra(item.getSrcNode());
		}
		vector<PathNode> path;
		recoverPath(item.getSrcNode(), item.getdstNode(), path);
		solveWithoutWorker(item, path);
	}

	//ofstream os("Checker/topoAndRequest1.txt");
	out.print();
}

void Solver::buildGraph() {
	adjList.resize(ins.nodes.size());
	for (auto &link : ins.links) {
		adjList[link.getSrcNode()].push_back(link.getId());
		adjList[link.getDstNode()].push_back(link.getId());
	}
}

void Solver::dijkstra(int s) {
	vector<int> predecessor(ins.nodes.size(), -1);
	vector<int> preLinks(ins.nodes.size(), -1);
	vector<Vertex> vertexes;
	vertexes.reserve(ins.nodes.size());
	for (int i = 0; i < ins.nodes.size(); ++i) {
		vertexes.push_back({ i, INF });
	}

	PriorityQueue<Vertex> queue;
	vertexes[s].dist = 0;
	queue.push(vertexes[s]);
	while (!queue.empty()) {
		Vertex currVertex = queue.top();
		queue.pop();
		for (int currLinkId : adjList[currVertex.id]) {
			auto &l = ins.links[currLinkId];
			int nextVertexId = currVertex.id == l.getSrcNode() ? l.getDstNode() : l.getSrcNode();
			Vertex &nextVertex = vertexes[nextVertexId];
			if (nextVertex.dist > currVertex.dist + l.getCost()) {
				nextVertex.dist = currVertex.dist + l.getCost();
				predecessor[nextVertex.id] = currVertex.id;
				preLinks[nextVertex.id] = currLinkId;
				queue.push(nextVertex);
			}
		}
	}

	// 存储从该起点开始的所有单源最短路径信息
	dijkstraResults[s].id = s;
	dijkstraResults[s].predecessor = predecessor;
	dijkstraResults[s].preLinks = preLinks;
	dijkstraResults[s].vertexes = vertexes;
}

void Solver::recoverPath(int s, int t, vector<PathNode>& path) {
	if (s == t) {
		path.clear();
		return;
	}
	int preNodeId = dijkstraResults[s].predecessor[t];
	int preLinkId = dijkstraResults[s].preLinks[t];
	recoverPath(s, preNodeId, path);
	path.push_back(make_pair(preNodeId, preLinkId));
}

void Solver::solveWithoutWorker(const Item & item, const vector<PathNode>& path) {
	// 检查必经点
	if (!item.getIncNodes().empty()) {
		for (int incNode : item.getIncNodes()) {
			if (find_if(path.begin(), path.end(), [incNode](const PathNode &pNode) {
				return pNode.first == incNode;
			}) == path.end()) {
				out.addFailedItem(item);
				return;
			}
		}
	}

	// 分配起止点拣货员
	auto &srcNode = ins.nodes[item.getSrcNode()];
	auto &dstNode = ins.nodes[item.getdstNode()];
	if (!srcNode.getAvailWorkerNum() || !dstNode.getAvailWorkerNum()) {
		out.addFailedItem(item);
		return;
	}
	srcNode.allocateWorker();
	dstNode.allocateWorker();

	// 计算列车交集 & 随机选择一列车
	set<int> carSet;
	for (auto &car : ins.links[path.front().second].getCars()) {
		carSet.insert(car.id);
	}
	for (auto &pNode : path) {
		set<int> tmpCarSet;
		for (auto &car : ins.links[pNode.second].getCars()) {
			if (dgreater(car.availWeight, item.getWeight())) {
				tmpCarSet.insert(car.id);
			}
		}
		set<int> intersectionSet;
		set_intersection(carSet.begin(), carSet.end(), tmpCarSet.begin(), tmpCarSet.end(),
			inserter(intersectionSet, intersectionSet.end()));
		carSet = intersectionSet;

		if (carSet.empty()) {
			out.addFailedItem(item);
			return;
		}
	}

	vector<int> carCandidates(carSet.begin(), carSet.end());
	uniform_int_distribution<int> u(0, carCandidates.size() - 1);
	int carId = carCandidates[u(randomEngine)];

	// 沿路更新资源 & 生成结果
	vector<Output::Result::ResultNode> resultNodes;
	resultNodes.reserve(path.size());
	for (auto &pNode : path) {
		auto &l = ins.links[pNode.second];
		l.occupyCar(carId); // 完全占有该车
		resultNodes.emplace_back(l.getName(), carId + 1);
	}

	// 添加成功结果
	out.addSuccessItem(item, resultNodes);
}

#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <queue>
#include <algorithm>
#include <iterator>
#include <iomanip>

using namespace std;

#pragma region utilities
#define INF 0x3f3f3f3f

const double EPS = 1e-6;

bool dgreater(double a, double b) { return a > b + EPS; }

bool dequal(double a, double b) { return fabs(a - b) < EPS; }

void split(const string& s, vector<string>& tokens, const string& delimiters = " ") {
	tokens.clear();
	string::size_type lastPos = s.find_first_not_of(delimiters, 0);
	string::size_type pos = s.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos) {
		tokens.push_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(delimiters, pos);
		pos = s.find_first_of(delimiters, lastPos);
	}
}
#pragma endregion

#pragma region Data
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

Instance::Instance(istream &is) {
	string str;
	vector<string> data;
	getline(is, str);	//读取第一行：站点数，轨道数，列车数，列车容量
	split(str, data, ",");
	int nodeNum = stoi(data.at(0), 0, 10);
	int linkNum = stoi(data.at(1), 0, 10);
	int carNum = stoi(data.at(2), 0, 10);
	double carWeight = stod(data.at(3));

	// 每个站点的拣货员数据
	nodes.resize(nodeNum);
	for (int i = 0; i < nodeNum; ++i) {
		getline(is, str);
		split(str, data, ",");
		int nodeId = stoi(data.at(0).substr(1));
		int workerNum = stoi(data.at(1));
		nodes[nodeId] = { nodeId, data.at(0), workerNum, linkNum, carNum };
	}

	// 每条链路的相关信息
	links.resize(linkNum);
	for (int i = 0; i < linkNum; ++i) {
		getline(is, str);
		split(str, data, ",");
		int linkId = stoi(data.at(0).substr(1));
		int srcId = stoi(data.at(1).substr(1));
		int dstId = stoi(data.at(2).substr(1));
		links[linkId] = { linkId, data.at(0), srcId, dstId };
		for (int j = 0; j < carNum; ++j) {
			links[linkId].addCar(Link::Car(j, linkId, carWeight));
		}
	}

	getline(is, str);	// 货物数量
	int requestNum = stoi(str, 0, 10);

	// 每个货物相关信息
	items.resize(requestNum);
	for (int i = 0; i < requestNum; ++i) {
		getline(is, str);
		split(str, data, ",");
		int itemId = stoi(data.at(0).substr(1));
		int srcId = stoi(data.at(1).substr(1));
		int dstId = stoi(data.at(2).substr(1));
		double itemWeight = stod(data.at(3));
		items[itemId] = { itemId, data.at(0), srcId, dstId, itemWeight };
		if (data.at(4) != "null") {
			for (int j = 4; j < data.size(); ++j) {
				int nodeId = stoi(data.at(j).substr(1));
				items[itemId].addIncNode(nodeId);
			}
		}
	}
}

void Output::print(ostream &os) {
	os << totalFailedNum << ",";
	os << setiosflags(ios::fixed) << setprecision(3) << totalFailedWeight << endl;
	for (auto &res : results) {
		os << res.itemName << endl;
		if (res.resultNodes.empty()) {
			os << "null" << endl << "null" << endl;
		}
		else {
			for (int i = 0; i < res.resultNodes.size() - 1; ++i) {
				os << res.resultNodes[i].first << ",";
			}
			os << res.resultNodes.back().first << endl;
			for (int i = 0; i < res.resultNodes.size() - 1; ++i) {
				os << res.resultNodes[i].second << ",";
			}
			os << res.resultNodes.back().second << endl;
		}
	}
}

#pragma endregion

#pragma region Solver
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

template <typename T>
using PriorityQueue = priority_queue<T>;

void Solver::run() {
	buildGraph();

	dijkstraResults.resize(ins.nodes.size());

	//sort(ins.items.begin(), ins.items.end(), [](const Item &lhs, const Item &rhs) {
	//	return lhs.getWeight() > rhs.getWeight();
	//});

	for (auto &item : ins.items) {
		if (dijkstraResults[item.getSrcNode()].id == -1) {
			dijkstra(item.getSrcNode());
		}
		vector<PathNode> path;
		recoverPath(item.getSrcNode(), item.getdstNode(), path);
		solveWithoutWorker(item, path);
	}

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
#pragma endregion

#pragma region Main
int main() {
	Instance ins;

	Solver sol(ins);

	sol.run();

	return 0;
}
#pragma endregion
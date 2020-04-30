#include "Data.h"
#include <iomanip>

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

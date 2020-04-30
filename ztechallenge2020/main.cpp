#include "Solver.h"

int main() {
	ifstream ifs("Instance/topoAndRequest1.txt");

	Instance ins(ifs);

	Solver sol(ins);

	sol.run();

	system("PAUSE");

	return 0;
}

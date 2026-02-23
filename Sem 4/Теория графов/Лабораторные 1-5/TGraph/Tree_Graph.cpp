#include <Tree_graph.h>

Tree_Graph::Tree_Graph(Abstract_Graph* new_graph, QVector<QVector<int>> matrix_cont, QVector<QVector<int>> matrix_weig): Abstract_Graph(*new_graph) {
	matrix_contiguity.fill(QVector<int>(p, 0), p);
	flag_tree = true;
	matrix_contiguity = matrix_cont;
	if (matrix_weig != QVector<QVector<int>>()){
		matrix_weights = matrix_weig;
	}
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			if (matrix_contiguity[i][j] == 0) {
				matrix_weights[i][j] = 0;
				matrix_bandwidths[i][j] = 0;
				matrix_costs[i][j] = 0;
				matrix_flows[i][j] = 0;
			}
		}
	}

}

#include <Unoriented_Graph.h>

Unoriented_Graph::Unoriented_Graph(int count_nodes) {
	p = count_nodes;
	flag_oriented = false;
	flag_tree = false;
	flag_negative_weights = false;

	fill_degrees();
	generate_matrix_contiguity();
	generate_matrix_kirchhoff();
	matrix_weights = matrix_contiguity;
	matrix_bandwidths = matrix_contiguity;
	matrix_costs = matrix_contiguity;
	matrix_flows = QVector<QVector<int>>(p, QVector<int>(p, 0));

	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			q += matrix_contiguity[i][j];
		}
	}
}


Unoriented_Graph::Unoriented_Graph(Abstract_Graph* graph) : Abstract_Graph(*graph) {

}



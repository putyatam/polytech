#include <Flow_Graph.h>





Flow_Graph::Flow_Graph(int count_nodes): Oriented_Graph(count_nodes) {
	matrix_bandwidths = matrix_contiguity;
	matrix_costs = matrix_contiguity;
}
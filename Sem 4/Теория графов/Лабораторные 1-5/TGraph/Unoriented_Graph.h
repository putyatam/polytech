#pragma once
#include <Abstract_Graph.h>

class Unoriented_Graph : public Abstract_Graph {
public:
	Unoriented_Graph(int count_nodes);
	Unoriented_Graph(Abstract_Graph* graph);
};
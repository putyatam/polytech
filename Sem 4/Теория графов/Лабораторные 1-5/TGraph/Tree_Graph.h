#pragma once
#include <Abstract_Graph.h>

class Tree_Graph : public Abstract_Graph {
	
public:
	Tree_Graph(Abstract_Graph* new_graph, QVector<QVector<int>> matrix_cont, QVector<QVector<int>> matrix_weig = QVector<QVector<int>>());
};
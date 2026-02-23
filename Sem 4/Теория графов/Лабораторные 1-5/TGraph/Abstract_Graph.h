#pragma once
#include <QVector>
#include <cmath>
#include <QRandomGenerator>
#include <QDebug>
#include <QPair>

class Abstract_Graph {
protected:
	int p;
	int q;
	QVector<QVector<int>> matrix_contiguity;
	QVector<QVector<int>> matrix_weights;

	QVector<QVector<int>> matrix_bandwidths;
	QVector<QVector<int>> matrix_flows;
	QVector<QVector<int>> matrix_costs;

	QVector<QVector<int>> matrix_kirchhoff;

	QVector<int> degrees;
	bool flag_oriented;
	bool flag_connected;
	bool flag_acyclic;
	bool flag_tree;
	bool flag_negative_weights;

public:
	void set_p(int new_p);
	void set_q(int new_q);
	int get_p();
	int get_q();
	bool get_flag_oriented();
	bool get_flag_connected();
	bool get_flag_acyclic();
	bool get_flag_negative_weights();
	QVector<QPair<int, int>> get_edges(bool duplicates = false);
	QVector<QVector<int>> get_matrix_contiguity();
	QVector<QVector<int>> get_matrix_weights();
	QVector<QVector<int>> get_matrix_bandwidths();
	QVector<QVector<int>> get_matrix_flows();
	QVector<QVector<int>> get_matrix_costs();
	QVector<QVector<int>> get_matrix_kirchhoff();

	void clear_matrix_flows();

	QVector<bool> find_articulation_points();

	void fill_degrees();
	void generate_matrix_contiguity(bool flag_one_drain = true);
	void generate_matrix_weights(bool negative);
	void generate_matrix_bandwidth();
	void generate_matrix_costs();
	void generate_matrix_kirchhoff();
	void find_path(int current, int end, QVector<int>& path, QVector<QVector<int>>& allPaths, bool flag_orient = false);
	QPair<QVector<int>, QVector<QVector<int>>> Dijkstra(int first_node, QVector<QVector<int>> matrix_values, QVector<QPair<int, int>> ignore_edges = QVector<QPair<int, int>>());



	QVector<QVector<QPair<int, int>>> findAllCycles(bool oriented = false);
	int countFaces();

	QPair<int, QVector<QVector<int>>> fordFulkerson(int source, int sink);
	int find_least_cost(int source, int sink, int flow);

	int find_count_spanning_trees();
	QVector<QVector<QVector<int>>> find_spanning_binary_trees();
	QVector<QVector<int>> Boruvka(bool flag_min);
	QVector<QPair<int, int>> Prufer();
	QPair<QVector<QVector<int>>, QVector<QVector<int>>> decodePrufer(const QVector<QPair<int, int>>& pruferCode);

	QVector<QVector<bool>> findAllMaxIndependentSets();

	bool graph_is_euler();
	int node_indeg(int i);
	int node_outdeg(int i);
	QVector<QPair<int, int>> modify_to_euler();
	QVector<int> find_cycle_euler();
	bool graph_is_gam();
	QVector<QPair<int, int>> modify_to_gam();

	QVector<QVector<int>> find_gam_full();
	QVector<int> find_best_cycle_bf();
	QVector<int> find_best_cycle_nb();
	QVector<int> find_best_cycle_pass();
	QVector<int> find_best_cycle_2opt();
};


double rand_Laplace();
void dfs(int node, int parent, const QVector<QVector<int>>& adjacencyMatrix, QVector<bool>& visited, QVector<int>& path, QVector<QVector<QPair<int, int>>>& allCycles);
bool isIndependentCycle(const QVector<QPair<int, int>>& cycle, QSet<QPair<int, int>>& usedEdges);
int dfs_artic_points(QVector<int>* local_numbers, int v, QVector<QSet<int>>* Adj, QVector<int>* visited, QVector<QSet<int>>* childs, QVector<int>* Lows);
int determinant(const QVector<QVector<int>>& matrix);
bool isTree(const QVector<QVector<int>>& adjacencyMatrix);
bool isIndependentSet(const QVector<QVector<int>>& matrix, const QVector<int>& subset);
void findMaxIndependentSets(const QVector<QVector<int>>& matrix, QVector<QVector<int>>& result, QVector<int>& currentSet, int currentVertex);
bool isHamiltonianCycleUtil(const QVector<QVector<int>>& matrix, QVector<int>& path, int pos);
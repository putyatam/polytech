#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTabWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsEllipseItem>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTimer>
#include <QMenuBar>
#include <QWidgetAction>
#include <QGroupBox>
#include <QComboBox>
#include <QGridLayout>
#include <QTextEdit>
#include <QLineEdit>


#include <Node.h>
#include <Abstract_Graph.h>
#include <Oriented_Graph.h>
#include <Unoriented_Graph.h>
#include <Tree_Graph.h>
#include <Graph_visual.h>

class MainWindow : public QWidget {
	Graph_visual* visual;

	QTabWidget* tab_main;
	QTabWidget* tab_tables;
	QTabWidget* tab_algorithms;
	QTableWidget* table_contiguity;
	QTableWidget* table_weights;
	QTableWidget* table_bandwidths;
	QTableWidget* table_costs;
	QTableWidget* table_flows;
	QTableWidget* table_kirchhoff;
	QTableWidget* table_shimbell;

	QWidget* widget_Dijkstra_paths;


	QGridLayout* layout_Dijkstra_paths;

	
	QPushButton* button_update;
	QPushButton* button_shimbell;
	QPushButton* button_find_paths;
	QPushButton* button_show_path;
	QPushButton* button_generate_weights;
	QPushButton* button_autolaying;
	QPushButton* button_show_artic_points;
	QPushButton* button_show_weights;
	QPushButton* button_show_bandwidths;
	QPushButton* button_show_costs;
	QPushButton* button_Dijkstra;
	QPushButton* button_Dijkstra_all;
	QPushButton* button_show_Dijkstra;
	QPushButton* button_generate_bandwidths;
	QPushButton* button_generate_costs;
	QPushButton* button_calculate_ford_fulkerson;
	QPushButton* button_show_saturation;
	QPushButton* button_find_least_cost;
	QPushButton* button_count_spanning_trees;
	QPushButton* button_show_spanning_tree;
	QPushButton* button_min_weight_tree_calculate;
	QPushButton* button_show_boruvka_tree;
	QPushButton* button_Prufer_code;
	QPushButton* button_show_prufer_tree;
	QPushButton* button_show_tree;
	QPushButton* button_find_max_sets;
	QPushButton* button_show_max_set;
	QPushButton* button_check_euler;
	QPushButton* button_modify_euler;
	QPushButton* button_show_modify_edges_euler;
	QPushButton* button_check_gam;
	QPushButton* button_modify_gam;
	QPushButton* button_show_modify_edges_gam;
	QPushButton* button_find_salesman;
	QPushButton* button_show_salesman;
	QPushButton* button_show_euler_cycle;
	QPushButton* button_find_salesman_cycle;
	QPushButton* button_show_salesman_cycle;


	QRadioButton* radiobutton_shimbell_min;
	QRadioButton* radiobutton_shimbell_max;
	QRadioButton* radiobutton_boruvka_min;
	QRadioButton* radiobutton_boruvka_max;


	QSpinBox* spin_start_path;
	QSpinBox* spin_end_path;
	QSpinBox* spin_show_path;
	QDoubleSpinBox* spin_mu;
	QDoubleSpinBox* spin_lambda;
	QSpinBox* spin_nodes;
	QSpinBox* spin_shimbell;
	QSpinBox* spin_Dijkstra_first;
	QSpinBox* spin_Dijkstra_second;
	QSpinBox* spin_ford_fulkerson_first;
	QSpinBox* spin_ford_fulkerson_second;
	QSpinBox* spin_flow_fraction_first;
	QSpinBox* spin_flow_fraction_second;
	QSpinBox* spin_least_first;
	QSpinBox* spin_least_second;
	QSpinBox* spin_show_spanning_tree;
	QSpinBox* spin_show_max_set;
	QSpinBox* spin_salesman;


	QComboBox* combo_Dijkstra_matrix;
	QComboBox* combo_salesman_heuristic;

	
	QLabel* label_count_edges_of_path;
	QLabel* label_count_paths;
	QLabel* label_max_flow;
	QLabel* label_cost_flow;
	QLabel* label_calculated_cost;
	QLabel* label_count_spanning_trees;
	QLabel* label_min_weight_tree;
	QLabel* label_max_set_size;
	QLabel* label_is_euler;
	QLabel* label_is_gam;
	QLabel* label_count_salesman;
	QLabel* label_weight_salesman;
	QLabel* label_salesman_min_weight;

	QLineEdit* line_Prufer;
	QLineEdit* line_Prufer_weights;
	QTextEdit* text_cycles_salesman;
	QLineEdit* text_cycle_euler;
	QLineEdit* text_salesman;
	

	QCheckBox* check_oriented;
	QCheckBox* check_acyclic;
	QCheckBox* check_connected;
	QCheckBox* check_negative_weights;
	
	
	QTimer* timer_autolaying;

	
	QVector<QVector<int>> paths;
	int path_to_show;
	QVector<QVector<int>> all_paths;
	QVector<Node*> nodes;
	QVector<QGraphicsLineItem*> lines;

	QVector<int>* nodes_for_Dijkstra;
	QVector<QVector<int>*> nodes_for_Dijkstra_all;
	QVector<QPushButton*> buttons_show_Dijkstra_all;
	QVector<QTextEdit*>* lines_Dijkstra_all;

	
	Abstract_Graph* graph;
	Abstract_Graph* graph_saved;
	Abstract_Graph* graph_saved_euler;
	Abstract_Graph* graph_saved_gam;
	QVector<QVector<QVector<int>>> trees;
	QVector<QVector<int>> min_tree;
	QVector<QVector<int>> prufer_contiguity;
	QVector<QVector<int>> prufer_weights;

	QVector<QVector<bool>> max_sets_nodes;
	int max_set_to_show;

	QVector<QPair<int, int>> added_lines_euler;
	QVector<QPair<int, int>> added_lines_gam;
	QVector<QVector<int>> cycles_salesman;
	int cycle_to_show;
	QVector<int> cycle_salesman;

public:
	MainWindow(QWidget* parent = nullptr);
	void update_graph();
	void update_tables();
	void update_table_contiguity();
	void update_table_kirchhoff();
	void update_table_weights();
	void update_table_bandwidths();
	void update_table_costs();
	void update_table_flows();
	void allow_find();
	void update_Dijkstra();
	void calculate_Dijkstra();
	void calculate_Dijkstra_all();
	void update_ford_fulkerson();
	void update_spanning_trees();
	void update_cycles();


	void calculate_shimbell();
	QVector<QVector<int>> multiplyMatrices(const QVector<QVector<int>>& A, const QVector<QVector<int>>& B);
};


bool lines_is_cross(QGraphicsLineItem* a, QGraphicsLineItem* b);


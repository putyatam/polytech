#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <Abstract_Graph.h>
#include <Node.h>
#include <Edge.h>
#include <QWheelEvent>
#include <QQueue>


class Graph_visual: public QGraphicsView, public QObject {
	Q_OBJECT
	QGraphicsScene* scene;

	Abstract_Graph* graph;
	QVector<Node*> nodes;
	QVector<Edge*> edges;
	

public:
	Graph_visual(Abstract_Graph* new_graph=nullptr, QWidget* parent=nullptr);
	void update_graph(Abstract_Graph* new_graph);
	void create_edges();
	void update_edges(Node* node);
	void update_all_edges();
	void update_lines_chars(QVector<int> path, QColor clr = Qt::red);
	void update_lines_chars(QVector<QPair<int, int>> lines, QColor clr = Qt::blue);
	void update_lines_chars(QVector<int> path, bool gradient);
	void set_random_poses_nodes();
	void force_move();
	void update_nodes_color(QVector<QColor> vec_colors);
	void update_digits();
	void wheelEvent(QWheelEvent* event) override;
	void positionTreeNodes();
signals:
	void stop_timer();

};

void calculateNodePositions(int node, const QVector<QVector<int>>& children, QVector<int>& xPositions, QVector<int>& yPositions, int depth, int& x);
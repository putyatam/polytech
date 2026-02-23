#pragma once
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsItemGroup>
#include <Node.h>

class Edge: public QGraphicsItemGroup{
	QGraphicsLineItem* line;
	QGraphicsPolygonItem* arrow;
	QGraphicsTextItem* weight;
	QColor color;
	bool oriented;
	int width;
	int digit_weight;
	int digit_bandwidth;
	int digit_cost;
	int digit_flow;
	QPair<Node*, Node*> pair;
public:
	Edge(Node* node1, Node* node2, bool new_oriented, int new_digit_weight, int new_digit_bandwidth, int new_digit_cost, int new_digit_flow = 0, QColor lineColor = Qt::black, int lineWidth = 2);
	QPair<Node*, Node*> get_pair();
	void update_edge(int* new_digit_weight = nullptr, int* new_digit_bandwidth = nullptr, int* new_digit_cost = nullptr, int* new_digit_flow = nullptr);
	void update_color_width(QColor new_color, int new_width);
	QLineF get_linef();
};
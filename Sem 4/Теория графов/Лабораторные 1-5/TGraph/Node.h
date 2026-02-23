#pragma once
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <QGraphicsScene>
#include <QRandomGenerator>

class Node : public QObject, public QGraphicsItemGroup{
	Q_OBJECT
private:
	QGraphicsEllipseItem* ellipse;
	QGraphicsTextItem* number;
	QPointF start_move;
	QColor color;
	bool dragged;

public:
	Node(int digit, int x_coord, int y_coord);
	Node();
	Node(Node& other);

	QGraphicsEllipseItem* get_ellipse();
	QGraphicsTextItem* get_number();
	QPointF get_start();
	bool get_dragged();
	void set_color(QColor new_color);

	void Calculate_force(QVector<Node*>& nodes, QVector<QLineF>& lines);
	void set_random_pos();

	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
signals:
	void Node_moved(Node* node);
};
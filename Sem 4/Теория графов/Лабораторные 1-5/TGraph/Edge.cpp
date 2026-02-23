#define _USE_MATH_DEFINES
#include <cmath>

#include <Edge.h>
#include <globals.h>
#include <QPen>


Edge::Edge(Node* node1, Node* node2, bool new_oriented, int new_digit_weight, int new_digit_bandwidth, int new_digit_cost, int new_digit_flow, QColor LineColor, int lineWidth) {
	oriented = new_oriented;
	digit_weight = new_digit_weight;
	digit_bandwidth = new_digit_bandwidth;
	digit_cost = new_digit_cost;
	digit_flow = new_digit_flow;
	color = LineColor;
	width = lineWidth;
	pair = qMakePair<Node*, Node*>(node1, node2);
	QPointF start(node1->x() + node_radius / 2, node1->y() + node_radius / 2);
	QPointF end(node2->x() + node_radius / 2, node2->y() + node_radius / 2);
	double offset = node_radius / 2;
	int arrowLength = 20;
	int arrowSharpness = 15;
	QPointF direction = end - start;
	double length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());

	direction /= length;

	QPointF adjustedStart = start + direction * offset;
	QPointF adjustedEnd = end - direction * offset;

	QLineF linef(adjustedStart, adjustedEnd);

	line = new QGraphicsLineItem(linef);
	line->setPen(QPen(color, width));
	weight = new QGraphicsTextItem();
	
	QString digit = show_weights ? QString::number(digit_weight) : show_bandwidths ? QString::number(digit_bandwidth) + "(" + QString::number(digit_flow) + ")" : QString::number(digit_cost);

	weight->setHtml("<div style='background-color: white; padding: 10px;'>" + digit + "</div>");
	weight->setPos(linef.center() - weight->boundingRect().center());
	weight->setFont(QFont("Arial", 10));
	weight->setZValue(1000);
	if (show_saturation) {
		double percent = double(digit_flow) / digit_bandwidth;
		color = QColor(percent < 0.5 ? 510 * percent : 255, percent < 0.5 ? 255 : 510 * (1 - percent), 0);
		line->setPen(QPen(color, width));
	}

	if (show_weights or show_bandwidths or show_costs) {
		weight->show();
	}
	else {
		weight->hide();
	}

	if (oriented) {
		double angle = std::atan2(direction.y(), direction.x());

		double halfSharpness = (arrowSharpness / 2) * (M_PI / 180.0);

		QPolygonF arrowHead;
		arrowHead.append(QPointF(0, 0));
		arrowHead.append(QPointF(-arrowLength, -arrowLength * tan(halfSharpness)));
		arrowHead.append(QPointF(-arrowLength, arrowLength * tan(halfSharpness)));
		arrowHead.append(QPointF(0, 0));

		QTransform transform;
		transform.translate(adjustedEnd.x(), adjustedEnd.y());
		transform.rotate(angle * 180 / M_PI);

		arrow = new QGraphicsPolygonItem(arrowHead);
		arrow->setPen(QPen(color, width));
		arrow->setBrush(color);
		arrow->setTransform(transform);
		this->addToGroup(arrow);
	}

	this->addToGroup(line);
	this->addToGroup(weight);
}



QPair<Node*, Node*> Edge::get_pair() {
	return pair;
}

void Edge::update_edge(int* new_digit_weight, int* new_digit_bandwidth, int* new_digit_cost, int* new_digit_flow) {
	delete line;
	if (oriented) {
		delete arrow;
	}
	

	if (new_digit_weight != nullptr) {
		digit_weight = *new_digit_weight;
	}
	if (new_digit_bandwidth != nullptr) {
		digit_bandwidth = *new_digit_bandwidth;
	}
	if (new_digit_cost != nullptr) {
		digit_cost = *new_digit_cost;
	}
	if (new_digit_flow != nullptr) {
		digit_flow = *new_digit_flow;
	}

	QPointF start(pair.first->x() + node_radius / 2, pair.first->y() + node_radius / 2);
	QPointF end(pair.second->x() + node_radius / 2, pair.second->y() + node_radius / 2);
	double offset = node_radius / 2;
	int arrowLength = 20;
	int arrowSharpness = 15;
	QPointF direction = end - start;
	double length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());

	direction /= length;

	QPointF adjustedStart = start + direction * offset;
	QPointF adjustedEnd = end - direction * offset;

	QLineF linef(adjustedStart, adjustedEnd);

	line = new QGraphicsLineItem(linef);
	line->setPen(QPen(color, width));

	QString digit = show_weights ? QString::number(digit_weight) : show_bandwidths ? QString::number(digit_bandwidth) + ("(" + QString::number(digit_flow) + ")") : QString::number(digit_cost);

	weight->setHtml("<div style='background-color: white; padding: 10px;'>" + digit + "</div>");
	weight->setPos(linef.center() - weight->boundingRect().center());
	weight->setFont(QFont("Arial", 10));
	weight->setZValue(1000);

	if (show_saturation) {
		double percent = double(digit_flow) / digit_bandwidth;
		color = QColor(percent < 0.5 ? 510 * percent : 255, percent < 0.5 ? 255 : 510 * (1 - percent), 0);
		line->setPen(QPen(color, width));
	}

	if (show_weights or show_bandwidths or show_costs) {
		weight->show();
	}
	else {
		weight->hide();
	}
	


	line->setPen(QPen(color, width));
	if (oriented) {
		double angle = std::atan2(direction.y(), direction.x());

		double halfSharpness = (arrowSharpness / 2) * (M_PI / 180.0);

		QPolygonF arrowHead;
		arrowHead.append(QPointF(0, 0));
		arrowHead.append(QPointF(-arrowLength, -arrowLength * tan(halfSharpness)));
		arrowHead.append(QPointF(-arrowLength, arrowLength * tan(halfSharpness)));
		arrowHead.append(QPointF(0, 0));

		QTransform transform;
		transform.translate(adjustedEnd.x(), adjustedEnd.y());
		transform.rotate(angle * 180 / M_PI);

		arrow = new QGraphicsPolygonItem(arrowHead);
		arrow->setPen(QPen(color, width));
		arrow->setBrush(color);
		arrow->setTransform(transform);
		this->addToGroup(arrow);
	}
	this->addToGroup(line);
	
	this->addToGroup(weight);
}


void Edge::update_color_width(QColor new_color, int new_width) {
	line->setPen(QPen(new_color, new_width));
	if (oriented) {
		arrow->setPen(QPen(new_color, new_width));
		arrow->setBrush(new_color);
	}
	color = new_color;
	width = new_width;
}


QLineF Edge::get_linef() {
	return line->line();
}

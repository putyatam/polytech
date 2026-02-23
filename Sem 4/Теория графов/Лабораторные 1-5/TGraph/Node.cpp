#include <Node.h>
#include <globals.h>
#include <QBrush>
#include <QFont>


Node::Node(int digit, int x_coord, int y_coord): QGraphicsItemGroup() {

	ellipse = new QGraphicsEllipseItem(0, 0, node_radius, node_radius);
	ellipse->setBrush(QBrush(QColor(173, 216, 230)));

	number = new QGraphicsTextItem(QString::number(digit + 1));
	QRectF numberRect = number->boundingRect();

	number->setPos(digit < 9 ? numberRect.width() / 2 : 0, 0);
	number->setFont(QFont("Arial", 12));

	dragged = false;

	this->addToGroup(ellipse);
	this->addToGroup(number);
	this->setPos(x_coord, y_coord);
	this->setFlag(QGraphicsItem::ItemIsMovable, true);
}

Node::Node() {
	ellipse = new QGraphicsEllipseItem();
	start_move = QPointF();
	number = new QGraphicsTextItem();
}

Node::Node(Node& other) {
	ellipse = other.get_ellipse();
	number = other.get_number();
	start_move = other.get_start();
}

QGraphicsEllipseItem* Node::get_ellipse() {
	return ellipse;
}
QGraphicsTextItem* Node::get_number() {
	return number;
}
QPointF Node::get_start() {
	return start_move;
}
bool Node::get_dragged() {
	return dragged;
}

void Node::Calculate_force(QVector<Node*>& nodes, QVector<QLineF>& lines) {

	QPointF result(0, 0);
	const QPointF currentPos = pos();

	const double pullForce = coef_pull * 10000 * nodes.size();
	for (const Node* node : nodes)
	{
		QPointF direction = node->pos() - currentPos;
		const double lengthSquared = direction.x() * direction.x() + direction.y() * direction.y();
		if (lengthSquared > 0)
		{
			const double invLength = 1.0 / sqrt(lengthSquared);
			direction *= invLength;
			result -= direction * (pullForce / lengthSquared);
		}
	}

	for (const QLineF& line : lines)
	{
		QPointF direction = line.p2() - line.p1();
		const double length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
		if (length > 0)
		{
			direction *= (coef_spring * length / length);
			result += direction;
		}
	}

	QPointF gravityDirection = QPointF(0, 0) - currentPos;
	const double gravityLength = sqrt(gravityDirection.x() * gravityDirection.x() +
		gravityDirection.y() * gravityDirection.y());
	if (gravityLength > 0)
	{
		gravityDirection *= (coef_gravity / gravityLength);
		result += gravityDirection;
	}

	QGraphicsItemAnimation* animation = new QGraphicsItemAnimation;
	QTimeLine* timeLine = new QTimeLine(speed_laying + 10);

	animation->setItem(this);
	timeLine->setFrameRange(0, 5);
	animation->setTimeLine(timeLine);
	animation->setPosAt(0, currentPos);
	animation->setPosAt(1, currentPos + result / 50);

	QObject::connect(timeLine, &QTimeLine::frameChanged,
		[this](int) { emit Node_moved(this); });

	timeLine->start();
}

void Node::set_random_pos() {
	QGraphicsItemAnimation* animation = new QGraphicsItemAnimation;
	animation->setItem(this);

	QTimeLine* timeLine = new QTimeLine(100);
	timeLine->setFrameRange(0, 40);
	animation->setTimeLine(timeLine);

	animation->setPosAt(0, QPointF(this->pos()));
	animation->setPosAt(1, QPointF(QRandomGenerator::global()->bounded(-300, 301), QRandomGenerator::global()->bounded(-300, 301)));

	QObject::connect(timeLine, &QTimeLine::frameChanged, [&]() {
		emit Node_moved(this);
		});
	timeLine->start();
}


void Node::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	QPointF newPos = event->scenePos();
	setPos(newPos - start_move);
	emit Node_moved(this);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	start_move = QPointF(event->scenePos() - pos());
	dragged = true;
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
	dragged = false;
}

void Node::set_color(QColor new_color) {
	ellipse->setBrush(new_color);
}
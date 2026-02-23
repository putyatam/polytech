#define _USE_MATH_DEFINES
#include <cmath>

#include <Graph_visual.h>
#include <globals.h>
#include <qDebug>
#include <QOpenGLWidget>

Graph_visual::Graph_visual(Abstract_Graph* new_graph, QWidget* parent): QGraphicsView(parent) {


	QSurfaceFormat format;
	format.setSamples(4);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	QSurfaceFormat::setDefaultFormat(format);


	scene = new QGraphicsScene();
	this->setScene(scene);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->centerOn(0, 0);
	this->setViewport(new QOpenGLWidget());
	this->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

	this->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	

	this->setBackgroundBrush(Qt::white);

	this->setDragMode(QGraphicsView::ScrollHandDrag);
	this->setRenderHint(QPainter::Antialiasing);
	this->setSceneRect(-4000, -4000, 8000, 8000);
	this->centerOn(0, 0);
	this->setWindowTitle("Graph Visualization");
	/*this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);*/

	if (new_graph != nullptr) {
		update_graph(new_graph);
	}

	
	
	
	
}


void Graph_visual::create_edges() {
	edges.clear();
	QVector<QPair<int, int>> edges_pairs = graph->get_edges(graph->get_flag_oriented());

	for (QPair<int, int> edge : edges_pairs) {
		Edge* new_edge = new Edge(nodes[edge.first], nodes[edge.second], graph->get_flag_oriented(), graph->get_matrix_weights()[edge.first][edge.second], graph->get_matrix_bandwidths()[edge.first][edge.second], graph->get_matrix_costs()[edge.first][edge.second], graph->get_matrix_flows()[edge.first][edge.second]);
		edges.push_back(new_edge);
		scene->addItem(new_edge);
	}
}


void Graph_visual::update_edges(Node* node) {
	for (Edge* edge : edges) {
		if (edge->get_pair().first == node or edge->get_pair().second == node) {
			edge->update_edge();
		}
	}
	
}

void Graph_visual::update_all_edges() {
	for (Edge* edge : edges) {
		edge->update_edge();
	}
	//this->update();
}


void Graph_visual::update_graph(Abstract_Graph* new_graph) {
	scene->clear();
	nodes.clear();
	edges.clear();


	graph = new_graph;
	int randomNumber = QRandomGenerator::global()->bounded(graph->get_p());
	randomNumber = 0;

	double segment = 2 * M_PI / graph->get_p();

	for (int i = 0; i < graph->get_p(); i++) {
		int x_coord = 0 - node_radius / 2 + cos((i + randomNumber) * segment) * 250;
		int y_coord = 0 - node_radius / 2 + sin((i + randomNumber) * segment) * 250;

		Node* node = new Node(i, x_coord, y_coord);
		this->scene->addItem(node);
		nodes.append(node);
		QObject::connect(node, &Node::Node_moved, this, &Graph_visual::update_edges);
	}
	create_edges();
}


void Graph_visual::update_lines_chars(QVector<int> path, QColor clr) {
	QVector<Edge*> path_edges;
	for (int i = 0; i < path.size() - 1; i++) {
		for (Edge* edge : edges) {
			if (edge->get_pair() == qMakePair<Node*, Node*>(nodes[path[i]], nodes[path[i + 1]]) or !graph->get_flag_oriented() and edge->get_pair() == qMakePair<Node*, Node*>(nodes[path[i+1]], nodes[path[i]])) {
				path_edges.push_back(edge);
			}
		}
	}

	for (Edge* edge : edges) {
		if (path_edges.contains(edge)) {
			edge->update_color_width(clr, 4);
		}
		else {
			edge->update_color_width(Qt::black, 2);
		}
	}
}

void Graph_visual::update_lines_chars(QVector<int> path, bool gradient) {
	qDebug() << path;
	QVector<Edge*> path_edges;
	for (int i = 0; i < path.size() - 1; i++) {
		for (Edge* edge : edges) {
			if (edge->get_pair() == qMakePair<Node*, Node*>(nodes[path[i]], nodes[path[i + 1]]) or !graph->get_flag_oriented() and edge->get_pair() == qMakePair<Node*, Node*>(nodes[path[i + 1]], nodes[path[i]])) {
				path_edges.push_back(edge);
				break;
			}
		}
	}
	int c = 0;
	for (Edge* edge : edges) {
		edge->update_color_width(Qt::black, 2);
	}
	/*for (Edge* edge : path_edges) {
		double percent = double(c) / (path.size() - 1);
		QColor color(percent < 0.5 ? 510 * percent : 255, 0, percent < 0.5 ? 255 : 510 * (1 - percent));
		if (path_edges.contains(edge)) {
			c++;
			edge->update_color_width(color, 4);
		}
	}*/
	QTimer* timer = new QTimer(this);
	timer_current = 0;
	QObject::connect(timer, &QTimer::timeout, [=]() mutable {
		if (timer_current > path_edges.size() - 2) {
			timer->stop();
		}
		Edge* edge = path_edges[timer_current];
		double percent = double(timer_current) / (path.size() - 1);
		QColor color(percent < 0.5 ? 510 * percent : 255, 0, percent < 0.5 ? 255 : 510 * (1 - percent));
		if (path_edges.contains(edge)) {
			edge->update_color_width(color, 4);
		}
		timer_current++;
		});
	QObject::connect(this, &Graph_visual::stop_timer, timer, &QTimer::stop);
	timer->start(500);
}


void Graph_visual::update_lines_chars(QVector<QPair<int, int>> lines, QColor clr) {
	QVector<Edge*> path_edges;
	for (QPair<int, int> pair : lines) {
		for (Edge* edge : edges) {
			if (edge->get_pair() == qMakePair<Node*, Node*>(nodes[pair.first], nodes[pair.second]) or !graph->get_flag_oriented() and edge->get_pair() == qMakePair<Node*, Node*>(nodes[pair.second], nodes[pair.first])) {
				path_edges.push_back(edge);
			}
		}
	}

	for (Edge* edge : edges) {
		if (path_edges.contains(edge)) {
			edge->update_color_width(clr, 4);
		}
		else {
			edge->update_color_width(Qt::black, 2);
		}
	}
}



void Graph_visual::force_move() {

	for (Node* node : nodes) {
		if (!node->get_dragged()) {
			QVector<QLineF> local_lines;
			for (Edge* edge : edges) {
				if (edge->get_pair().first == node) {
					local_lines.push_back(edge->get_linef());
				}
				else {
					if (edge->get_pair().second == node) {
						local_lines.push_back(QLineF(edge->get_linef().p2(), edge->get_linef().p1()));
					}
				}
			}
			QVector<Node*> local_nodes;
			for (Node* node_other : nodes) {
				if (node != node_other) {
					local_nodes.push_back(node_other);
				}
			}

			node->Calculate_force(local_nodes, local_lines);
		}
	}
	
}

void Graph_visual::set_random_poses_nodes() {
	for (Node* node : nodes) {
		node->set_random_pos();
	}
}


void Graph_visual::wheelEvent(QWheelEvent* event) {
	// Масштабирование при прокрутке колеса мыши
	if (event->modifiers() & Qt::ControlModifier) {  // Удерживайте Ctrl для масштабирования
		double scaleFactor = 1.15;
		if (event->angleDelta().y() < 0) {
			scaleFactor = 1.0 / scaleFactor;  // Уменьшение масштаба
		}
		scale(scaleFactor, scaleFactor);
	}
	else {
		QGraphicsView::wheelEvent(event);  // Стандартная прокрутка
	}
}

void Graph_visual::update_nodes_color(QVector<QColor> vec_colors) {
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->set_color(vec_colors[i]);
	}
}

void Graph_visual::update_digits() {
	QVector<QVector<int>> weights = graph->get_matrix_weights();
	QVector<QVector<int>> bandwidths = graph->get_matrix_bandwidths();
	QVector<QVector<int>> costs = graph->get_matrix_costs();
	QVector<QVector<int>> flows = graph->get_matrix_flows();
	
	for (Edge* edge : edges) {
		int flow = flows[nodes.indexOf(edge->get_pair().first)][nodes.indexOf(edge->get_pair().second)];
		if (flow == 0) {
			flow = flows[nodes.indexOf(edge->get_pair().second)][nodes.indexOf(edge->get_pair().first)];
		}
		edge->update_edge(&weights[nodes.indexOf(edge->get_pair().first)][nodes.indexOf(edge->get_pair().second)],
			&bandwidths[nodes.indexOf(edge->get_pair().first)][nodes.indexOf(edge->get_pair().second)],
			&costs[nodes.indexOf(edge->get_pair().first)][nodes.indexOf(edge->get_pair().second)],
			&flow);
	}
}


// Рекурсивная функция для расчёта позиций узлов (Post-Order обход)
void calculateNodePositions(int node, const QVector<QVector<int>>& children, QVector<int>& xPositions, QVector<int>& yPositions, int depth, int& x) {
	if (children[node].isEmpty()) {
		xPositions[node] = x++;
		yPositions[node] = depth;
		return;
	}

	// Рекурсивно обрабатываем всех детей
	for (int child : children[node]) {
		calculateNodePositions(child, children, xPositions, yPositions, depth + 1, x);
	}

	// Позиция текущего узла - середина между крайними детьми
	int minX = xPositions[children[node].first()];
	int maxX = xPositions[children[node].last()];
	xPositions[node] = (minX + maxX) / 2;
	yPositions[node] = depth;
}


void Graph_visual::positionTreeNodes() {
	QVector<QVector<int>> children(graph->get_p());

	int root = 0;
	QVector<bool> visited(graph->get_p(), false);
	QQueue<int> queue;
	queue.enqueue(root);
	visited[root] = true;

	while (!queue.isEmpty()) {
		int current = queue.dequeue();
		for (int neighbor = 0; neighbor < graph->get_p(); ++neighbor) {
			if (graph->get_matrix_contiguity()[current][neighbor] == 1 && !visited[neighbor]) {
				children[current].append(neighbor);
				queue.enqueue(neighbor);
				visited[neighbor] = true;
			}
		}
	}

	QVector<int> xPositions(graph->get_p(), 0);
	QVector<int> yPositions(graph->get_p(), 0);
	int startX = 0;

	calculateNodePositions(root, children, xPositions, yPositions, 0, startX);

	// нормализуем координаты
	int minX = *std::min_element(xPositions.begin(), xPositions.end());
	for (int& x : xPositions) {
		x -= minX;
	}
	int minY = *std::min_element(yPositions.begin(), yPositions.end());
	for (int& y : yPositions) {
		y -= minY;
	}

	for (int node = 0; node < graph->get_p(); ++node) {
		nodes[node]->setPos(xPositions[node] * 100, yPositions[node] * 100); // 100 - уровень
	}
	update_all_edges();
}
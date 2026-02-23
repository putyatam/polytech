#include <Abstract_Graph.h>
#include <globals.h>

#include <QQueue>
#include <QStack>

double rand_Laplace() {
	// mu - параметр положения (-inf : +inf)
	// lambda - параметр масштаба (0 : +inf)
	double r = QRandomGenerator::global()->generateDouble();

	if (r <= 0.5) {
		return mu + (log(2 * r) / lambda);
	}
	return mu - (log(2 * (1 - r)) / lambda);
}

void Abstract_Graph::set_p(int new_p) {
	p = new_p;
}
void Abstract_Graph::set_q(int new_q) {
	q = new_q;
}

QVector<QVector<int>> Abstract_Graph::get_matrix_contiguity() {
	return matrix_contiguity;
}

QVector<QVector<int>> Abstract_Graph::get_matrix_kirchhoff() {
	return matrix_kirchhoff;
}

QVector<QVector<int>> Abstract_Graph::get_matrix_weights() {
	return matrix_weights;
}

QVector<QVector<int>> Abstract_Graph::get_matrix_bandwidths() {
	return matrix_bandwidths;
}

QVector<QVector<int>> Abstract_Graph::get_matrix_flows() {
	return matrix_flows;
}

QVector<QVector<int>> Abstract_Graph::get_matrix_costs() {
	return matrix_costs;
}

int Abstract_Graph::get_p() {
	return p;
}

int Abstract_Graph::get_q() {
	return q;
}

bool Abstract_Graph::get_flag_oriented() {
	return flag_oriented;
}
bool Abstract_Graph::get_flag_connected() {
	return flag_connected;
}
bool Abstract_Graph::get_flag_acyclic() {
	return flag_acyclic;
}
bool Abstract_Graph::get_flag_negative_weights() {
	return flag_negative_weights;
}


QVector<QPair<int, int>> Abstract_Graph::get_edges(bool duplicates) {
	QVector<QPair<int, int>> edges;
	for (int i = 0; i < p; i++) {
		for (int j = (duplicates ? 0 : i); j < p; j++) {
			if (matrix_contiguity[i][j] != 0) {
				edges.append(qMakePair<int, int>(i, j));
			}
		}
	}
	return edges;
}


void Abstract_Graph::generate_matrix_weights(bool negative) {
	flag_negative_weights = negative;
	matrix_weights.fill(QVector<int>(p, 0), p);
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			if (matrix_contiguity[i][j] != 0) {
				if (flag_negative_weights) {
					int num = 0;
					while (num == 0) {
						num = rand_Laplace();
					}
					matrix_weights[i][j] = num;
					if (!flag_oriented) {
						matrix_weights[j][i] = matrix_weights[i][j];
					}
				}
				else {
					int num = 0;
					while (num == 0) {
						num = rand_Laplace();
					}
					matrix_weights[i][j] = abs(num);
					if (!flag_oriented) {
						matrix_weights[j][i] = matrix_weights[i][j];
					}
				}
				
			}
		}
	}
}


void Abstract_Graph::generate_matrix_bandwidth() {
	matrix_bandwidths.fill(QVector<int>(p, 0), p);
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			if (matrix_contiguity[i][j] == 1) {
				while (true) {
					int r = rand_Laplace();
					if (r != 0) {
						matrix_bandwidths[i][j] = abs(r);
						if (!flag_oriented) {
							matrix_bandwidths[j][i] = matrix_bandwidths[i][j];
						}
						break;
					}
				}
			}
		}
	}
}


void Abstract_Graph::generate_matrix_costs() {
	matrix_costs.fill(QVector<int>(p, 0), p);
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			if (matrix_contiguity[i][j] == 1) {
				while (true) {
					int r = rand_Laplace();
					if (r != 0) {
						matrix_costs[i][j] = abs(r);
						if (!flag_oriented) {
							matrix_costs[j][i] = matrix_costs[i][j];
						}
						break;
					}
				}
			}
		}
	}
}


void Abstract_Graph::generate_matrix_kirchhoff() {
	matrix_kirchhoff.fill(QVector<int>(p, 0), p);
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			if (i == j) {
				for (int i_ = 0; i_ < p; i_++) {
					matrix_kirchhoff[i][j] += matrix_contiguity[i_][j];
				}
			}
			else {
				if (matrix_contiguity[i][j] == 1) {
					matrix_kirchhoff[i][j] = -1;
				}
				else {
					matrix_kirchhoff[i][j] = 0;
				}
			}
		}
	}
}


void Abstract_Graph::find_path(int current, int end, QVector<int>& path, QVector<QVector<int>>& allPaths, bool orient) {
	path.append(current);

	if (current == end) {
		allPaths.append(path);
	}
	else {
		QVector<QPair<int, int>> edges = get_edges(!flag_oriented);

		if (orient) {
			QVector<QPair<int, int>> copy_edges = edges;
			for (int i = 0; i < copy_edges.size(); i++) {
				edges.push_back(qMakePair(copy_edges[i].second, copy_edges[i].first));
			}
		}
		for (const auto& edge : edges) {
			if (edge.first == current) {
				int next = edge.second;
				if (!path.contains(next)) {
					find_path(next, end, path, allPaths);
				}
			}
		}
	}

	path.removeLast();
}



void dfs(int node, int parent, const QVector<QVector<int>>& adjacencyMatrix, QVector<bool>& visited, QVector<int>& path, QVector<QVector<QPair<int, int>>>& allCycles) {
	visited[node] = true;
	path.push_back(node);

	for (int neighbor = 0; neighbor < adjacencyMatrix[node].size(); ++neighbor) {
		if (adjacencyMatrix[node][neighbor] == 1) {
			if (!visited[neighbor]) {
				dfs(neighbor, node, adjacencyMatrix, visited, path, allCycles);
			}
			else if (neighbor != parent) {
				QVector<QPair<int, int>> cycle;
				for (int i = path.size() - 1; i >= 0; --i) {
					if (path[i] == neighbor) {
						for (int j = i; j < path.size() - 1; ++j) {
							cycle.append(qMakePair(path[j], path[j + 1]));
						}
						cycle.append(qMakePair(path.back(), neighbor));
						break;
					}
				}
				if (!cycle.isEmpty()) {
					allCycles.append(cycle);
				}
			}
		}
	}

	path.pop_back();
}

QVector<QVector<QPair<int, int>>> Abstract_Graph::findAllCycles(bool oriented) {
	QVector<QPair<int, int>> cycles; // Результат: рёбра всех циклов

	QVector<QVector<int>> matrix(p, QVector<int>(p, 0));
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			if (matrix_contiguity[i][j] != 0) {
				matrix[i][j] = 1;
				matrix[j][i] = 1;
			}
		}
	}

	QVector<QVector<QPair<int, int>>> allCycles; // Результат: все циклы
	QVector<bool> visited(p, false); // Вектор посещённых вершин
	QVector<int> path; // Вектор для хранения текущего пути

	// Запускаем DFS для каждой непосещённой вершины
	for (int node = 0; node < p; node++) {
		if (!visited[node]) {
			dfs(node, -1, matrix, visited, path, allCycles);
		}
	}

	return allCycles;
}



// Функция для проверки, является ли цикл независимым
bool isIndependentCycle(const QVector<QPair<int, int>>& cycle, QSet<QPair<int, int>>& usedEdges) {
	for (const auto& edge : cycle) {
		if (usedEdges.contains(edge) || usedEdges.contains(qMakePair(edge.second, edge.first))) {
			return false; // Цикл использует уже использованные рёбра
		}
	}
	return true;
}

// Функция для подсчёта граней
int Abstract_Graph::countFaces() {
	QSet<QPair<int, int>> usedEdges; // Для хранения уже использованных рёбер
	int faceCount = 1; // Начинаем с внешней грани

	for (const auto& cycle : findAllCycles()) {
		if (isIndependentCycle(cycle, usedEdges)) {
			// Добавляем рёбра цикла в использованные
			for (const auto& edge : cycle) {
				usedEdges.insert(edge);
			}
			faceCount++; // Увеличиваем количество граней
		}
	}

	return faceCount;
}


QVector<bool> Abstract_Graph::find_articulation_points() {
	QVector<QSet<int>> Adj(p, QSet<int>());


	for (int i = 0; i < p; i++) {
		for (int j = 0; j < p; j++) {
			if (matrix_contiguity[i][j] == 1) {
				Adj[i].insert(j);
				Adj[j].insert(i);
			}
		}
	}

	QVector<int> local_numbers(p, 0);
	QVector<int> visited;
	QVector<QSet<int>> childs(p, QSet<int>());
	QVector<int> Lows(p, p+1);

	dfs_artic_points(&local_numbers, 0, &Adj, &visited, &childs, &Lows);

	QVector<bool> result(p, false);


	for (int i = 0; i < p; i++) {
		for (int child : childs[i]) {
			count_iter++;
			if (Lows[child] == i) {
				if (i != 0 or i == 0 and childs[i].size() > 1) {
					result[local_numbers.indexOf(i)] = true;
					break;
				}
				
				
			}
		}
	}
	qDebug() << "Artic. points iterations = " << count_iter;
	count_iter = 0;
	return result;
}


int dfs_artic_points(QVector<int>* local_numbers, int v, QVector<QSet<int>>* Adj, QVector<int>* visited, QVector<QSet<int>>* childs, QVector<int>* Lows) {
	QVector<int> low;
	if (!visited->contains(v)) {
		(*local_numbers)[v] = visited->size();
		visited->push_back(v);

		for (int i : (*Adj)[v]) {
			count_iter++;
			if (!visited->contains(i)) {
				(*childs)[(*local_numbers)[v]].insert(visited->size());

				low.push_back(dfs_artic_points(local_numbers, i, Adj, visited, childs, Lows));
			}
			else {
				low.push_back((*local_numbers)[i]);
			}
		}
	}
	(*Lows)[(*local_numbers)[v]] = *std::min_element(low.begin(), low.end());
	return (*Lows)[(*local_numbers)[v]];
}


QPair<QVector<int>, QVector<QVector<int>>> Abstract_Graph::Dijkstra(int current_node, QVector<QVector<int>> matrix_values, QVector<QPair<int, int>> ignore_edges) {
	QVector<bool> isvisited(p, false);

	QVector<bool> infs(p, true);
	infs[current_node] = false;

	QVector<int> ways(p, -1);
	ways[current_node] = 0;

	QVector<QVector<int>> ways_nodes(p, QVector<int>());

	QQueue<int> queue;
	queue.push_back(current_node);

	count_iter = 0;

	while (!queue.isEmpty()) {
		int node = queue.first();
		queue.pop_front();
		for (int i = 0; i < matrix_contiguity[node].size(); i++) {
			count_iter++;
			if (matrix_contiguity[node][i] != 0 and not isvisited[i] and not ignore_edges.contains(qMakePair<int, int>(node, i))) {
				int way = ways[node] + matrix_values[node][i];
				if (way < ways[i] or infs[i]) {
					ways[i] = way;
					infs[i] = false;
					ways_nodes[i] = ways_nodes[node];
					ways_nodes[i].push_back(node);
				}
				queue.push_back(i);
			}
		}
		isvisited[node] = true;
	}
	
	qDebug() << "Dijkstra count iterations = " << count_iter;//
	count_iter = 0;

	return qMakePair(ways, ways_nodes);
}

void Abstract_Graph::clear_matrix_flows() {
	matrix_flows = QVector<QVector<int>>(p, QVector<int>(p, 0));
}


int Abstract_Graph::find_count_spanning_trees() {

	QVector<QVector<int>> submatrix(p - 1, QVector<int>(p - 1));

	for (int i = 1; i < p; ++i) {
		for (int j = 1; j < p; ++j) {
			submatrix[i - 1][j - 1] = matrix_kirchhoff[i][j];
		}
	}

	return ((1 + 1) % 2 == 0 ? 1 : -1) * determinant(submatrix);
}


int determinant(const QVector<QVector<int>>& matrix) {
	const int n = matrix.size();

	if (n == 1) return matrix[0][0];
	if (n == 2) {
		return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
	}

	int det = 0;
	int sign = 1;

	for (int j = 0; j < n; ++j) {
		QVector<QVector<int>> submatrix(n - 1, QVector<int>(n - 1));

		for (int i = 1; i < n; ++i) {
			for (int k = 0, l = 0; k < n; ++k) {
				if (k == j) continue;
				submatrix[i - 1][l++] = matrix[i][k];
			}
		}

		det += sign * matrix[0][j] * determinant(submatrix);
		sign = -sign;
	}

	return det;
}


void Abstract_Graph::fill_degrees() {
	degrees.append(0);
	int done = 0;

	for (int i = 0; i < p - 1; i++) {
		int r = abs(rand_Laplace());

		if (r > 0 and r <= p - done - 1) {
			degrees.append(r);
			done++;
		}
		else {
			i--;
		}
	}
	std::sort(degrees.begin(), degrees.end(), std::greater<int>());
}


void Abstract_Graph::generate_matrix_contiguity(bool flag_one_drain) {
	matrix_contiguity.fill(QVector<int>(p, 0), p);
	QVector<int> degrees_copy = degrees;
	for (int i = 0; i < p; i++) {
		if (i != 0 and flag_one_drain) {
			matrix_contiguity[i - 1][i] = 1;
		}
		for (int k = 0; k < degrees_copy.first() - flag_one_drain ? 1 : 0; k++) {
			int j = qrand() % p;
			if (i < j and matrix_contiguity[i][j] != 1) {
				matrix_contiguity[i][j] = 1;
			}
			else {
				k--;
			}
		}
		degrees_copy.pop_front();
	}
	if (!flag_oriented) {
		for (int i = 0; i < p; i++) {
			for (int j = 0; j < p; j++) {
				if (matrix_contiguity[i][j] == 1) {
					matrix_contiguity[j][i] = 1;
				}
			}
		}
	}
}


QPair<int, QVector<QVector<int>>> Abstract_Graph::fordFulkerson(int source, int drain) {
	QVector<QVector<int>> flows(p, QVector<int>(p, 0));
	int max_flow = 0;
	QVector<int> first_path;
	QVector<QVector<int>> all_paths;
	find_path(source, drain, first_path, all_paths, true);
	for (QVector<int> path : all_paths) {
		QVector<int> flows_edges_path;
		for (int i = 0; i < path.size() - 1; i++) {
			flows_edges_path.push_back(matrix_bandwidths[path[i]][path[i + 1]] - flows[path[i]][path[i + 1]]);
		}

		int min_flow_of_path = *std::min_element(flows_edges_path.begin(), flows_edges_path.end());



		for (int i = 0; i < path.size() - 1; i++) {
			if (matrix_contiguity[path[i]][path[i + 1]] == 1) {
				flows[path[i]][path[i + 1]] += min_flow_of_path;
			}
			else {
				flows[path[i]][path[i + 1]] -= min_flow_of_path;
			}
		}
		max_flow += min_flow_of_path;
	}
	matrix_flows = flows;
	return qMakePair<int, QVector<QVector<int>>>(max_flow, flows);
}


int Abstract_Graph::find_least_cost(int source, int sink, int flow) {
	QVector<QVector<int>> copy_bandwidths = matrix_bandwidths;
	QVector<QPair<int, int>> ignore_edges;

	int main_flow = 0;
	int main_cost = 0;

	while (main_flow != flow) {
		for (int m = 0; m < copy_bandwidths.size(); m++) {
			qDebug() << copy_bandwidths[m];
		}
		QPair<QVector<int>, QVector<QVector<int>>> Dijkstra_result = Dijkstra(source, matrix_costs, ignore_edges);//
		int flow_cost = Dijkstra_result.first[sink];

		int max_flow_of_path = 0;
		QVector<int> flows_of_edges;
		QVector<int> path = Dijkstra_result.second[sink];
		path.push_back(sink);
		for (int i = 0; i < path.size() - 1; i++) {
			flows_of_edges.push_back(copy_bandwidths[path[i]][path[i + 1]]);
		}

		max_flow_of_path = *std::min_element(flows_of_edges.begin(), flows_of_edges.end());
		if (main_flow + max_flow_of_path > flow) {
			max_flow_of_path = flow - main_flow;
		}
		flow_cost *= max_flow_of_path;
		main_flow += max_flow_of_path;

		main_cost += flow_cost;

		for (int i = 0; i < path.size() - 1; i++) {
			copy_bandwidths[path[i]][path[i + 1]] -= max_flow_of_path;
			if (copy_bandwidths[path[i]][path[i + 1]] == 0) {
				ignore_edges.push_back(qMakePair(path[i], path[i + 1]));
			}
		}
		if (main_flow == flow) {
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < p; j++) {
					matrix_flows[i][j] = matrix_bandwidths[i][j] - copy_bandwidths[i][j];
				}
			}
			return main_cost;
		}
	}
}

//QVector<QVector<QVector<int>>> Abstract_Graph::find_spanning_binary_trees() {
//	QVector<QVector<QVector<int>>> result;
//	QVector<QPair<int, int>> edges = get_edges(!flag_oriented);
//	const int n = p;
//
//	if (n <= 1) return { QVector<QVector<int>>(n, QVector<int>(n, 0)) };
//	if (edges.isEmpty()) return result;
//
//	// Структура для хранения состояния
//	struct State {
//		QVector<QVector<int>> tree;
//		QVector<int> parent;
//		QVector<int> out_degree;
//		QVector<int> in_degree;
//		int root;
//	};
//
//	// Очередь для BFS-подхода
//	QQueue<State> queue;
//
//	// Инициализация начальных состояний (каждая 
// а может быть корнем)
//	for (int root = 0; root < n; ++root) {
//		State init;
//		init.tree = QVector<QVector<int>>(n, QVector<int>(n, 0));
//		init.parent = QVector<int>(n, -1);
//		init.out_degree = QVector<int>(n, 0);
//		init.in_degree = QVector<int>(n, 0);
//		init.root = root;
//		queue.enqueue(init);
//	}
//
//	while (!queue.isEmpty()) {
//		State current = queue.dequeue();
//		int edges_used = 0;
//
//		// Подсчет уже использованных ребер
//		for (int i = 0; i < n; ++i) {
//			if (current.parent[i] != -1) edges_used++;
//		}
//
//		// Если добавили n-1 ребро - проверяем на дерево
//		if (edges_used == n - 1) {
//			// Проверяем связность
//			QVector<bool> visited(n, false);
//			QQueue<int> q;
//			q.enqueue(current.root);
//			visited[current.root] = true;
//			int visited_count = 1;
//
//			while (!q.isEmpty()) {
//				int u = q.dequeue();
//				for (int v = 0; v < n; ++v) {
//					if (current.tree[u][v] && !visited[v]) {
//						visited[v] = true;
//						q.enqueue(v);
//						visited_count++;
//					}
//				}
//			}
//
//			if (visited_count == n) {
//				result.append(current.tree);
//			}
//			continue;
//		}
//
//		// Добавляем возможные новые ребра
//		for (int i = 0; i < edges.size(); ++i) {
//			int u = edges[i].first;
//			int v = edges[i].second;
//
//			// Пропускаем ребра, которые уже есть в дереве
//			if (current.tree[u][v]) continue;
//			if (!flag_oriented && current.tree[v][u]) continue;
//
//			// Проверка ограничений:
//			// 1. В ориентированном случае у вершины максимум один вход
//			if (flag_oriented && current.in_degree[v] >= 1) continue;
//
//			// 2. У любой вершины максимум два потомка
//			if (current.out_degree[u] >= 2) continue;
//			if (!flag_oriented) {
//				// В неориентированном случае учитываем обратные ребра
//				if (current.out_degree[v] >= 2) continue;
//			}
//
//			// 3. Не создаем циклов
//			bool creates_cycle = false;
//			int cur = u;
//			while (current.parent[cur] != -1) {
//				if (current.parent[cur] == v) {
//					creates_cycle = true;
//					break;
//				}
//				cur = current.parent[cur];
//			}
//			if (creates_cycle) continue;
//
//			// Создаем новое состояние
//			State new_state = current;
//			new_state.tree[u][v] = 1;
//			if (!flag_oriented) new_state.tree[v][u] = 1;
//
//			new_state.parent[v] = u;
//			new_state.out_degree[u]++;
//			new_state.in_degree[v]++;
//
//			queue.enqueue(new_state);
//		}
//	}
//	
//	result = removeDuplicates(result);
//	QVector<QVector<QVector<int>>> new_result;
//	for (int k = 0; k < result.size(); k++) {
//		int sum = 0;
//		for (int i = 0; i < p; i++) {
//			for (int j = i; j < p; j++) {
//				sum += result[k][i][j];
//			}
//		}
//		if (sum == p - 1) {
//			new_result.append(result[k]);
//		}
//	}
//	
//
//	return new_result;
//}
//
//bool matrixEquals(const QVector<QVector<int>>& a, const QVector<QVector<int>>& b) {
//	if (a.size() != b.size()) return false;
//	for (int i = 0; i < a.size(); ++i) {
//		if (a[i] != b[i]) return false;
//	}
//	return true;
//}
//
//QVector<QVector<QVector<int>>> removeDuplicates(const QVector<QVector<QVector<int>>>& input) {
//	QSet<QVector<QVector<int>>> uniqueSet;
//	QVector<QVector<QVector<int>>> result;
//
//	for (const auto& matrix : input) {
//		if (!uniqueSet.contains(matrix)) {
//			uniqueSet.insert(matrix);
//			result.append(matrix);
//		}
//	}
//
//	return result;
//}


QVector<QVector<QVector<int>>> Abstract_Graph::find_spanning_binary_trees() {
	QVector<QVector<QVector<int>>> result;
	QVector<QPair<int, int>> edges = get_edges();
	QVector<bool> base(edges.size(), false);
	std::fill(base.begin(), base.begin() + p - 1, true);

	QVector<QVector<bool>> permutations;
	do {
		permutations.append(base);
	} while (std::prev_permutation(base.begin(), base.end()));

	for (QVector<bool> k : permutations) {
		QVector<QPair<int, int>> local_edges;
		for (int i = 0; i < edges.size(); i++) {
			if (k[i]) {
				local_edges.append(edges[i]);
			}
		}
		QVector<QVector<int>> matrix(p, QVector<int>(p, 0));
		for (QPair<int, int> pair : local_edges) {
			matrix[pair.first][pair.second] = 1;
			if (!flag_oriented) {
				matrix[pair.second][pair.first] = 1;
			}
		}
		
		bool flag = true;
		if (flag_oriented) {
			for (int j = 0; j < p; j++) {
				int sum = 0;
				for (int i = 0; i < p; i++) {
					sum += matrix[i][j];
				}
				if (sum > 1) {
					flag = false;
					break;
				}
			}
		}
		else {
			
			if (!isTree(matrix)) {
				flag = false;
			}
		}
		

		if (flag) {
			result.append(matrix);
		}
		if (result.size() == 200) {
			return result;
		}

	}
	return result;
	
}

bool isTree(const QVector<QVector<int>>& adjacencyMatrix) {
	const int n = adjacencyMatrix.size();
	if (n == 0) return true;

	int edgeCount = 0;
	QVector<bool> visited(n, false);
	QQueue<int> q;

	q.enqueue(0);
	visited[0] = true;
	int visitedCount = 1;

	while (!q.isEmpty()) {
		int u = q.dequeue();

		for (int v = 0; v < n; ++v) {
			if (adjacencyMatrix[u][v] > 0) {  // ребро между u и v
				edgeCount++;

				if (!visited[v]) {
					visited[v] = true;
					visitedCount++;
					q.enqueue(v);
				}
			}
		}
	}

	edgeCount /= 2;

	
	return (visitedCount == n) and (edgeCount == n - 1);
}


QVector<QVector<int>> Abstract_Graph::Boruvka(bool flag_min) {
	QVector<QVector<int>> mst(p, QVector<int>(p, 0));
	QVector<int> component(p);
	for (int i = 0; i < p; ++i) {
		component[i] = i;
	}

	int componentsRemaining = p;

	while (componentsRemaining > 1) {
		QVector<QPair<int, int>> bestEdges(p, { -1, -1 });
		QVector<int> bestWeights(p, flag_min ? INT_MAX : INT_MIN);

		for (int u = 0; u < p; ++u) {
			for (int v = u + 1; v < p; ++v) {
				if (matrix_contiguity[u][v] && component[u] != component[v]) {
					int currentWeight = matrix_weights[u][v];
					int compU = component[u];
					int compV = component[v];

					if ((flag_min && currentWeight < bestWeights[compU]) ||
						(!flag_min && currentWeight > bestWeights[compU])) {
						bestWeights[compU] = currentWeight;
						bestEdges[compU] = { u, v };
					}

					if ((flag_min && currentWeight < bestWeights[compV]) ||
						(!flag_min && currentWeight > bestWeights[compV])) {
						bestWeights[compV] = currentWeight;
						bestEdges[compV] = { v, u };
					}
				}
			}
		}

		for (int comp = 0; comp < p; ++comp) {
			if (bestEdges[comp].first != -1) {
				int u = bestEdges[comp].first;
				int v = bestEdges[comp].second;

				if (component[u] != component[v]) {
					mst[u][v] = mst[v][u] = 1;

					int oldComp = component[v];
					int newComp = component[u];
					for (int k = 0; k < p; ++k) {
						if (component[k] == oldComp) {
							component[k] = newComp;
						}
					}
					componentsRemaining--;
				}
			}
		}
	}

	return mst;
}


QVector<QPair<int, int>> Abstract_Graph::Prufer(){
	QVector<QPair<int, int>> pruferCode;
	QVector<int> degrees(p, 0);
	QMap<int, int> leafs;


	for (int i = 0; i < p; ++i) {
		degrees[i] = matrix_contiguity[i].count(1);
	}

	for (int i = 0; i < p; ++i) {
		if (degrees[i] == 1) {
			// находим родителя
			for (int j = 0; j < p; ++j) {
				if (matrix_contiguity[i][j] == 1) {
					leafs.insert(i, j);
					break;
				}
			}
		}
	}

	while (leafs.size() > 0) {
		// лист с мин номером
		int minLeaf = leafs.firstKey();
		int parent = leafs.value(minLeaf);

		int edgeWeight = matrix_weights[minLeaf][parent];

		pruferCode.append(qMakePair(parent, edgeWeight));

		// удаляем лист
		degrees[minLeaf] = 0;
		degrees[parent]--;
		leafs.remove(minLeaf);

		if (degrees[parent] == 1 and leafs.size() >= 1) {
			for (int j = 0; j < p; ++j) {
				if (matrix_contiguity[parent][j] == 1 && degrees[j] > 0) {
					leafs.insert(parent, j);
					break;
				}
			}
		}
	}

	if (!pruferCode.isEmpty()) {
		pruferCode.removeLast();
	}

	return pruferCode;
}


QPair<QVector<QVector<int>>, QVector<QVector<int>>> Abstract_Graph::decodePrufer(const QVector<QPair<int, int>>& pruferCode) {
	QVector<QVector<int>> adjacencyMatrix(p, QVector<int>(p, 0));
	QVector<QVector<int>> weightMatrix(p, QVector<int>(p, 0));

	QList<int> vertexList;
	for (int i = 0; i < p; ++i) {
		vertexList.append(i);
	}

	QVector<int> degrees(p, 1);
	for (const auto& pair : pruferCode) {
		degrees[pair.first]++;
	}

	for (int i = 0; i < pruferCode.size(); ++i) {
		int minLeaf = -1;
		for (int v : vertexList) {
			if (degrees[v] == 1) {
				minLeaf = v;
				break;
			}
		}

		const int currentVertex = pruferCode[i].first;
		const int edgeWeight = pruferCode[i].second;

		adjacencyMatrix[minLeaf][currentVertex] = 1;
		adjacencyMatrix[currentVertex][minLeaf] = 1;
		weightMatrix[minLeaf][currentVertex] = edgeWeight;
		weightMatrix[currentVertex][minLeaf] = edgeWeight;


		degrees[minLeaf]--;
		degrees[currentVertex]--;
		vertexList.removeOne(minLeaf);
	}

	return qMakePair<QVector<QVector<int>>, QVector<QVector<int>>>(adjacencyMatrix, weightMatrix);
}


bool isIndependentSet(const QVector<QVector<int>>& matrix, const QVector<int>& subset) {
	for (int i = 0; i < subset.size(); ++i) {
		for (int j = i + 1; j < subset.size(); ++j) {
			int u = subset[i];
			int v = subset[j];
			if (matrix[u][v] == 1) {
				return false;
			}
		}
	}
	return true;
}


void findMaxIndependentSets(const QVector<QVector<int>>& matrix,
	QVector<QVector<int>>& result,
	QVector<int>& currentSet,
	int currentVertex) {
	// является ли независимым
	if (isIndependentSet(matrix, currentSet)) {
		bool isMaximal = true;
		for (int v = 0; v < matrix.size(); v++) {
			if (!currentSet.contains(v)) {
				QVector<int> extendedSet = currentSet;
				extendedSet.append(v);
				if (isIndependentSet(matrix, extendedSet)) {
					isMaximal = false;
					break;
				}
			}
		}
		if (isMaximal) {
			result.append(currentSet);
		}
	}
	else {
		return;
	}


	for (int v = currentVertex + 1; v < matrix.size(); ++v) {
		QVector<int> newSet = currentSet;
		newSet.append(v);
		findMaxIndependentSets(matrix, result, newSet, v);
	}
}

QVector<QVector<bool>> Abstract_Graph::findAllMaxIndependentSets() {
	QVector<QVector<int>> result;
	QVector<int> currentSet;

	QVector<QVector<int>> matrix_contiguity_copy = QVector<QVector<int>>(p, QVector<int>(p, 0));
	for (int i = 0; i < p; i++) {
		for (int j = i; j < p; j++) {
			if (matrix_contiguity[i][j] == 1 or matrix_contiguity[j][i] == 1) {
				matrix_contiguity_copy[i][j] = 1;
				matrix_contiguity_copy[j][i] = 1;
			}
		}
	}

	for (int v = 0; v < matrix_contiguity.size(); ++v) {
		currentSet.clear();
		currentSet.append(v);
		findMaxIndependentSets(matrix_contiguity_copy, result, currentSet, v);
	}

	for (auto& set : result) {
		std::sort(set.begin(), set.end());
	}
	std::sort(result.begin(), result.end());
	result.erase(std::unique(result.begin(), result.end()), result.end());

	std::sort(result.begin(), result.end(),
		[](const QVector<int>& a, const QVector<int>& b) {
			return a.size() < b.size();
		});
	
	QVector<QVector<bool>> result_bool = QVector<QVector<bool>>(result.size(), QVector<bool>(p, false));
	for (int i = 0; i < result.size(); i++) {
		for (int j = 0; j < result[i].size(); j++) {
			result_bool[i][result[i][j]] = true;
		}
	}
	return result_bool;
}


bool Abstract_Graph::graph_is_euler() {
	for (int i = 0; i < p; i++) {
		if (flag_oriented) {
			if (node_indeg(i) != node_outdeg(i)) {
				return false;
			}
		}
		else {
			if (node_indeg(i) % 2 != 0) {
				return false;
			}
		}
	}
	return true;
}

int Abstract_Graph::node_indeg(int v) {
	int count = 0;
	for (int i = 0; i < p; i++) {
		count += matrix_contiguity[i][v];
	}
	return count;
}

int Abstract_Graph::node_outdeg(int v) {
	int count = 0;
	for (int i = 0; i < p; i++) {
		count += matrix_contiguity[v][i];
	}
	return count;
}

QVector<QPair<int, int>> Abstract_Graph::modify_to_euler() {
	QVector<QPair<int, int>> result;
	QVector<int> odd_vertices;
	for (int i = 0; i < p; ++i) {
		if (node_outdeg(i) % 2 != 0) {
			odd_vertices.append(i);
		}
	}

	for (int i = 0; i < odd_vertices.size(); i++) {
		for (int j = 0; j < odd_vertices.size(); j++) {
			if (i != j){
				if (matrix_contiguity[odd_vertices[i]][odd_vertices[j]] == 0) {
					matrix_contiguity[odd_vertices[i]][odd_vertices[j]] = 1;
					matrix_contiguity[odd_vertices[j]][odd_vertices[i]] = 1;
					matrix_weights[odd_vertices[i]][odd_vertices[j]] = 1;
					matrix_weights[odd_vertices[j]][odd_vertices[i]] = 1;
					result.push_back(qMakePair(odd_vertices[i], odd_vertices[j]));
					odd_vertices.remove(i);
					odd_vertices.remove(i < j ? j - 1 : j);
					i--;
					j--;
					break;
				}

			}
		}
	}

	if (!odd_vertices.isEmpty()) {
		for (int i = 0; i < odd_vertices.size(); i++) {
			int v1 = odd_vertices[i];
			for (int j = 0; j < odd_vertices.size(); j++) {
				int v2 = odd_vertices[j];
				if (i != j) {
					for (int k = 0; k < p; k++) {
						if (k != v1 and k != v2 and matrix_contiguity[k][v1] == 0 and matrix_contiguity[k][v2] == 0) {
							matrix_contiguity[k][v1] = 1;
							matrix_contiguity[k][v2] = 1;
							matrix_contiguity[v1][k] = 1;
							matrix_contiguity[v2][k] = 1;
							result.push_back(qMakePair(k, v1));
							result.push_back(qMakePair(k, v2));
							odd_vertices.remove(i);
							odd_vertices.remove(i < j ? j - 1 : j);
							i--;
							j--;
							if (odd_vertices.isEmpty()) {
								return result;
							}
						}
					}

				}
			}
		}
	}
	for (int i = 0; i < odd_vertices.size(); i+=2) {
		matrix_contiguity[odd_vertices[i]][odd_vertices[i + 1]] = 0;
		matrix_contiguity[odd_vertices[i + 1]][odd_vertices[i]] = 0;
	}
	return result;
	
	
}


bool isHamiltonianCycle(const QVector<QVector<int>>& matrix, int pos, QVector<int>& path, QVector<bool>& visited) {
	if (pos == matrix.size()) {
		return matrix[path.back()][path[0]] == 1;
	}

	for (int v = 0; v < matrix.size(); ++v) {
		if (matrix[path[pos - 1]][v] == 1 && !visited[v]) {
			visited[v] = true;
			path[pos] = v;

			if (isHamiltonianCycle(matrix, pos + 1, path, visited)) {
				return true;
			}

			visited[v] = false;
			path[pos] = -1;
		}
	}
	return false;
}

bool Abstract_Graph::graph_is_gam() {
	if (p == 2) {
		return false;
	}

	QVector<int> path(p, -1);
	QVector<bool> visited(p, false);

	for (int start = 0; start < p; ++start) {
		path[0] = start;
		visited[start] = true;

		if (isHamiltonianCycle(matrix_contiguity, 1, path, visited)) {
			return true;
		}

		visited.fill(false);
		path.fill(-1);
	}

	return false;
}

bool hasHamiltonianPath(const QVector<QVector<int>>& matrix, QVector<int>& path, int pos) {
	if (pos == matrix.size()) return true;

	for (int v = 0; v < matrix.size(); ++v) {
		if (matrix[path[pos - 1]][v] && std::find(path.begin(), path.begin() + pos, v) == path.begin() + pos) {
			path[pos] = v;
			if (hasHamiltonianPath(matrix, path, pos + 1)) return true;
		}
	}
	return false;
}

QVector<QPair<int, int>> Abstract_Graph::modify_to_ham() {
	QVector<QPair<int, int>> addedEdges;
	if (p < 2) return addedEdges;
	if (graph_is_gam()) {
		return addedEdges;
	}

	QVector<int> path(p, -1);
	bool pathFound = false;

	for (int start = 0; start < p && !pathFound; ++start) {
		path.fill(-1);
		path[0] = start;
		pathFound = hasHamiltonianPath(matrix_contiguity, path, 1);
	}

	if (!pathFound) {
		for (int i = 0; i < p; ++i) path[i] = i;

		for (int i = 0; i < p - 1; ++i) {
			if (matrix_contiguity[path[i]][path[i + 1]] == 0) {
				matrix_contiguity[path[i]][path[i + 1]] = 1;
				matrix_contiguity[path[i + 1]][path[i]] = 1;
				matrix_weights[path[i]][path[i + 1]] = 1;
				matrix_weights[path[i + 1]][path[i]] = 1;
				addedEdges.append(qMakePair(path[i], path[i + 1]));
			}
		}
	}

	if (p >= 3 && matrix_contiguity[path.back()][path[0]] == 0) {
		matrix_contiguity[path.back()][path[0]] = 1;
		matrix_contiguity[path[0]][path.back()] = 1;
		matrix_weights[path.back()][path[0]] = 1;
		matrix_weights[path[0]][path.back()] = 1;
		addedEdges.append(qMakePair(path.back(), path[0]));
	}

	return addedEdges;
}


QVector<QVector<int>> Abstract_Graph::find_ham_full() {
	QVector<QVector<int>> result;
	if (matrix_contiguity.isEmpty()) return result;

	QVector<int> path;
	path.reserve(p + 1);
	QVector<bool> visited(p, false);
	QSet<QVector<int>> unique_cycles;

	auto normalizeCycle = [](const QVector<int>& cycle) {
		QVector<int> normalized = cycle.mid(0, cycle.size() - 1);

		int min_pos = 0;
		for (int i = 1; i < normalized.size(); ++i) {
			if (normalized[i] < normalized[min_pos]) {
				min_pos = i;
			}
		}

		std::rotate(normalized.begin(), normalized.begin() + min_pos, normalized.end());

		QVector<int> reversed = normalized;
		std::reverse(reversed.begin(), reversed.end());
		std::rotate(reversed.begin(), reversed.end() - 1, reversed.end());

		return (normalized < reversed) ? normalized : reversed;
		};

	auto backtrack = [&](auto&& self, int pos) -> void {
		if (pos == p) {
			if (matrix_contiguity[path.back()][path[0]]) {
				QVector<int> cycle = path;
				cycle.append(path[0]);
				QVector<int> normalized = normalizeCycle(cycle);

				if (!unique_cycles.contains(normalized)) {
					unique_cycles.insert(normalized);
					result.append(cycle);
				}
			}
			return;
		}

		for (int v = 0; v < p; ++v) {
			if (visited[v] || (pos > 0 && !matrix_contiguity[path[pos - 1]][v])) {
				continue;
			}

			visited[v] = true;
			path.append(v);
			self(self, pos + 1);
			path.removeLast();
			visited[v] = false;
		}
		};

	backtrack(backtrack, 0);
	return result;
}


QVector<int> Abstract_Graph::find_cycle_euler() {
	QVector<int> cycle;

	int n = matrix_contiguity.size();
	QVector<QVector<int>> temp_matrix = matrix_contiguity;
	QVector<QStack<int>> stacks(n);
	QStack<int> current_stack;

	current_stack.push(0);

	while (!current_stack.isEmpty()) {
		int v = current_stack.top();
		bool found = false;

		for (int u = 0; u < n; ++u) {
			if (temp_matrix[v][u] > 0) {
				temp_matrix[v][u]--;
				temp_matrix[u][v]--;

				current_stack.push(u);
				found = true;
				break;
			}
		}

		if (!found) {
			cycle.append(current_stack.pop());
		}
	}

	std::reverse(cycle.begin(), cycle.end());
	return cycle;
}


QVector<int> Abstract_Graph::find_best_cycle_bf() {

	QVector<int> vertices;
	for (int i = 0; i < p; ++i) {
		vertices.append(i);
	}

	QVector<int> bestPath;
	int minWeight = INT_MAX;

	do {
		int currentWeight = 0;
		bool valid = true;

		for (int i = 0; i < p; ++i) {
			int j = (i + 1) % p;
			int from = vertices[i];
			int to = vertices[j];

			if (matrix_weights[from][to] <= 0) {
				valid = false;
				break;
			}

			currentWeight += matrix_weights[from][to];
		}

		if (valid && currentWeight < minWeight) {
			minWeight = currentWeight;
			bestPath = vertices;
		}

	} while (std::next_permutation(vertices.begin(), vertices.end()));
	bestPath.push_back(0);
	return bestPath;
}


QVector<int> Abstract_Graph::find_best_cycle_nb() {
	for (int first_node = 0; first_node < p; first_node++) {
		QVector<int> path;
		QVector<bool> visited(p, false);

		path.append(first_node);
		visited[first_node] = true;

		while (path.size() < p) {
			int last = path.back();
			int next = -1;
			int minEdge = std::numeric_limits<int>::max();

			// ищем ближайшую
			for (int i = 0; i < p; ++i) {
				if (!visited[i] && matrix_weights[last][i] > 0 && matrix_weights[last][i] < minEdge) {
					minEdge = matrix_weights[last][i];
					next = i;
				}
			}

			if (next == -1) {
				break;
			}

			path.append(next);
			visited[next] = true;
		}

		if (matrix_weights[path.back()][0] == 0) {
			continue;
		}
		if (path.size() != p) {
			continue;
		}
		path.push_back(first_node);
		return path;
	}
	return QVector<int>();
}

QVector<int> Abstract_Graph::find_best_cycle_pass() {
	QVector<int> best_path;
	int min_weight = INT_MAX;
	QVector<int> vertices;

	for (int i = 1; i < p; ++i) {
		vertices.push_back(i);
	}

	do {
		int current_weight = 0;
		bool valid = true;

		if (matrix_weights[0][vertices[0]] == 0) {
			valid = false;
		}
		else {
			current_weight += matrix_weights[0][vertices[0]];
		}

		for (int i = 0; i < vertices.size() - 1 && valid; ++i) {
			if (matrix_weights[vertices[i]][vertices[i + 1]] == 0) {
				valid = false;
			}
			else {
				current_weight += matrix_weights[vertices[i]][vertices[i + 1]];

				// прерываем если текущий путь хуже
				if (current_weight >= min_weight) {
					valid = false;
					break;
				}
			}
		}

		if (valid && matrix_weights[vertices.back()][0] != 0) {
			current_weight += matrix_weights[vertices.back()][0];

			if (current_weight < min_weight) {
				min_weight = current_weight;
				best_path = vertices;
				best_path.push_front(0);
				best_path.push_back(0);
			}
		}

	} while (std::next_permutation(vertices.begin(), vertices.end()));

	return best_path;
}


int calculateCycleWeight(const QVector<QVector<int>>& matrix, const QVector<int>& path) {
	int weight = 0;
	int n = path.size();

	for (int i = 0; i < n; ++i) {
		int u = path[i];
		int v = path[(i + 1) % n];
		if (matrix[u][v] == 0) {
			return -1;
		}
		weight += matrix[u][v];
	}
	return weight;
}


QVector<int> Abstract_Graph::find_best_cycle_2opt() {
	for (int first_node = 0; first_node < p; first_node++) {

		QVector<int> path;
		QVector<bool> visited(p, false);
		int current = first_node;
		path.push_back(current);
		visited[current] = true;

		for (int step = 1; step < p; ++step) {
			int next = -1;
			int min_dist = std::numeric_limits<int>::max();

			for (int i = 0; i < p; ++i) {
				if (!visited[i] && matrix_weights[current][i] > 0 && matrix_weights[current][i] < min_dist) {
					min_dist = matrix_weights[current][i];
					next = i;
				}
			}

			if (next == -1) {
				break;
			};

			path.push_back(next);
			visited[next] = true;
			current = next;
		}

		if (matrix_weights[path.back()][path[0]] == 0) {
			continue;
		}

		auto twoOptOptimize = [&](QVector<int>& path) {
			bool improved = true;
			int n = path.size();

			while (improved) {
				improved = false;
				for (int i = 0; i < n - 1; ++i) {
					for (int j = i + 1; j < n; ++j) {

						int a = path[i], b = path[(i + 1) % n];
						int x = path[j], y = path[(j + 1) % n];

						if (matrix_weights[a][x] == 0 || matrix_weights[b][y] == 0)
							continue;

						int delta = matrix_weights[a][x] + matrix_weights[b][y] - matrix_weights[a][b] - matrix_weights[x][y];

						if (delta < 0) {
							std::reverse(path.begin() + i + 1, path.begin() + j + 1);
							improved = true;
						}
					}
				}
			}
			};

		twoOptOptimize(path);

		if (calculateCycleWeight(matrix_weights, path) == -1)
			continue;
		if (path.size() != p) {
			continue;
		}

		path.push_back(path[0]);
		return path;
	}
}
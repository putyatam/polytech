#include "node.h"
#include <math.h>

Node* Node::start_node = nullptr;
Node* Node::error_node = nullptr;
Effect* Node::error_effect = nullptr;
Node* Node::current_node = nullptr;

deque<char> Node::data;
char Node::prev_char;
string Node::buffer;
vector<pair<string, string>> Node::errors;
vector<Lexem*> Node::table;


void Node::set_params(function<bool(char)> match, vector<Node*> next_nodes, vector<Effect*> current_effects, string name) {
	this->match = match;
	this->next_nodes = next_nodes;
	this->current_effects = current_effects;
	this->name = name;
}

void Node::apply_effects() {
	for (Effect* eff : this->current_effects) {
		(*eff)();
	}
}

bool Node::go_to_next(bool debug) {
	if (!Node::data.empty()) {
		char c = Node::data.front();
		if (debug) {
			cout << "Предыдущий символ - '" << Node::prev_char << "'" << endl;
			cout << "Текущий символ - '" << c << "'" << endl;
		}
		for (Node* next_node : current_node->next_nodes) {
			if (next_node->match(c)) {
				current_node = next_node;
				current_node->apply_effects();
				if (debug) {
					cout << "Переходим в узел " << Node::current_node->name << endl;
				}
				return true;
			}
		}
		current_node = error_node;
		current_node->apply_effects();
		if (debug) {
			cout << "Переходим в узел " << Node::current_node->name << endl;
		}
		return true;
	}
	return false;
}

pair<vector<Lexem*>, vector<pair<string, string>>> Node::analyze(string str, bool debug, bool with_context) {

	Node::data = deque<char>(str.begin(), str.end());
	Node::current_node = Node::start_node;

	if (!with_context) {
		table.clear();
		errors.clear();
	}
	int i = 1;
	while (Node::go_to_next(debug)) {
		i;
	};
	if (!Node::buffer.empty()) {
		Node::buffer.pop_back();
		Node::errors.push_back({ Node::buffer, "Многострочный комменатрий не завершён"});
		Node::buffer.clear();
	}
	return { table, errors };
}

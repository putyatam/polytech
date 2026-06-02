#pragma once
#include "effect.h"
#include "lexem.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Node {
private:
	function<bool(char)> match;
	vector<Node*> next_nodes;
	vector<Effect*> current_effects;
	string name;

public:
	static Node* start_node;
	static Node* error_node;
	static Effect* error_effect;
	static Node* current_node;

	static deque<char> data;
	static char prev_char;
	static string buffer;
	static vector<pair<string, string>> errors;
	static vector<Lexem*> table;
	
	void set_params(function<bool(char)>, vector<Node*>, vector<Effect*>, string);
	void apply_effects();
	static bool go_to_next(bool);
	static pair<vector<Lexem*>, vector<pair<string, string>>> analyze(string, bool = false , bool = false);
};

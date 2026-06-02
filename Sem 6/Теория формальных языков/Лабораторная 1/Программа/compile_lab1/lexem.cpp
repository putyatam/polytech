#include "lexem.h"

#include <stdexcept>

using namespace std;


map<string, int> Lexem::type_to_int;
map<int, string> Lexem::int_to_type;
map<string, string> Lexem::type_to_rus;
map<string, int> Lexem::ids;
int Lexem::count_ids = 0;

Lexem::Lexem(string type_str, string token) {
	this->token = token;
	this->type = Lexem::type_to_int[type_str];

	if (this->type == 0) {
		auto result = Lexem::ids.try_emplace(this->token, Lexem::count_ids);
		if (result.second) {
			Lexem::count_ids += 1;
		}
	}
}


void Lexem::set_types(initializer_list<pair<string, string>> new_types) {
	int count_types = 0;
	for (pair<string, string> t : new_types) {
		auto result = Lexem::type_to_int.try_emplace(t.first, count_types);
		if (result.second) {
			Lexem::int_to_type.emplace(count_types, t.first);
			Lexem::type_to_rus.emplace(t.first, t.second);
			count_types++;
		}
	}
}


string Lexem::get_token() {
	return this->token;
}

string Lexem::get_type(bool rus) {
	if (rus) {
		return Lexem::type_to_rus[Lexem::int_to_type[this->type]];
	}
	return Lexem::int_to_type[this->type];
}

string Lexem::get_value() {

	if (this->get_type() == "id") {
		auto result = Lexem::ids.find(this->token);
		if (result != Lexem::ids.end()) {
			return this->token + " : " + to_string(result->second);
		}
		return "не доступно";
	}

	if (this->get_type() == "const") {
		return this->token;
	}

	else {
		return "";
	}
}

void Lexem::clr_ids() {
	Lexem::ids.clear();
}
#pragma once
#include <string>
#include <vector>
#include <map>
#include <string>
using namespace std;

class Lexem {
private:
	static map<string, int> type_to_int;
	static map<int, string> int_to_type;
	static map<string, string> type_to_rus;
	static map<string, int> ids;
	static int count_ids;
	int type;
	string token;
	
public:
	static void set_types(initializer_list<pair<string, string>>);
	static void clr_ids();
	Lexem(string, string);
	string get_token();
	string get_type(bool = false);
	string get_value();
};
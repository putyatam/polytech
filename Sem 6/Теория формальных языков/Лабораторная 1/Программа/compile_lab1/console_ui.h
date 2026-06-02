#pragma once
#include <string>
#include <vector>
#include "lexem.h"

#include <iostream>

using namespace std;


string lexems_table_to_str(vector<Lexem*>, string, string, string, string);
string pairs_table_to_str(const vector<pair<string, string>>& data, const string, const string, const string, int, int);
void print_analysis_result(pair<vector<Lexem*>, vector<pair<string, string>>>);
void input_loop();
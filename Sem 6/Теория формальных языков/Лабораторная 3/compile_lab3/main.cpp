#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <limits>

#define NOMINMAX
#include <windows.h>

using namespace std;

const string COLOR_RED = "\033[31m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_YELLOW = "\033[33m";
const string COLOR_CYAN = "\033[36m";
const string COLOR_RESET = "\033[0m";


typedef map<string, vector<vector<string>>> Grammar;

struct TreeNode {
	string symbol;
	vector<TreeNode*> children;

	TreeNode(string s) : symbol(s) {}

	~TreeNode() {
		for (auto c : children) {
			delete c;
		}
	}
};

class LanguageModel {
private:
	Grammar rules;
	vector<string> vocabulary;
	size_t max_parse_pos = 0;

	void addRule(const string& l, const vector<vector<string>>& r) {
		rules[l] = r;
	}

	void buildGrammar() {
		addRule("subj_1sg", { {"yo"} });
		addRule("subj_2sg", { {"tú"} });
		addRule("subj_3sg", { {"él"}, {"ella"}, {"usted"}, {"noun_phrase_ms"}, {"noun_phrase_fs"} });
		addRule("subj_1pl", { {"nosotros"}, {"nosotras"} });
		addRule("subj_2pl", { {"vosotros"}, {"vosotras"} });
		addRule("subj_3pl", { {"ellos"}, {"ellas"}, {"ustedes"}, {"noun_phrase_mp"}, {"noun_phrase_fp"} });

		addRule("opt_subj_1sg", { {"subj_1sg"}, {} });
		addRule("opt_subj_2sg", { {"subj_2sg"}, {} });
		addRule("opt_subj_3sg", { {"subj_3sg"}, {} });
		addRule("opt_subj_1pl", { {"subj_1pl"}, {} });
		addRule("opt_subj_2pl", { {"subj_2pl"}, {} });
		addRule("opt_subj_3pl", { {"subj_3pl"}, {} });

		addRule("sentence", { {"affirmative"}, {"negative"}, {"interrogative"} });

		addRule("affirmative", {
			{"opt_subj_1sg", "verb_1sg", "complements", "."},
			{"opt_subj_2sg", "verb_2sg", "complements", "."},
			{"opt_subj_3sg", "verb_3sg", "complements", "."},
			{"opt_subj_1pl", "verb_1pl", "complements", "."},
			{"opt_subj_2pl", "verb_2pl", "complements", "."},
			{"opt_subj_3pl", "verb_3pl", "complements", "."}
			});

		addRule("negative", {
			{"opt_subj_1sg", "no", "verb_1sg", "complements", "."},
			{"opt_subj_2sg", "no", "verb_2sg", "complements", "."},
			{"opt_subj_3sg", "no", "verb_3sg", "complements", "."},
			{"opt_subj_1pl", "no", "verb_1pl", "complements", "."},
			{"opt_subj_2pl", "no", "verb_2pl", "complements", "."},
			{"opt_subj_3pl", "no", "verb_3pl", "complements", "."}
			});

		addRule("opt_qword", { {"question_word"}, {} });
		addRule("interrogative", {
			{"¿", "opt_qword", "verb_1sg", "opt_subj_1sg", "complements", "?"},
			{"¿", "opt_qword", "verb_2sg", "opt_subj_2sg", "complements", "?"},
			{"¿", "opt_qword", "verb_3sg", "opt_subj_3sg", "complements", "?"},
			{"¿", "opt_qword", "verb_1pl", "opt_subj_1pl", "complements", "?"},
			{"¿", "opt_qword", "verb_2pl", "opt_subj_2pl", "complements", "?"},
			{"¿", "opt_qword", "verb_3pl", "opt_subj_3pl", "complements", "?"}
			});

		addRule("noun_phrase_ms", { {"opt_det_ms", "opt_list_adj_ms", "noun_ms", "list_postmod_ms"} });
		addRule("noun_phrase_fs", { {"opt_det_fs", "opt_list_adj_fs", "noun_fs", "list_postmod_fs"} });
		addRule("noun_phrase_mp", { {"opt_det_mp", "opt_list_adj_mp", "noun_mp", "list_postmod_mp"} });
		addRule("noun_phrase_fp", { {"opt_det_fp", "opt_list_adj_fp", "noun_fp", "list_postmod_fp"} });

		addRule("opt_det_ms", { {"det_ms"}, {} }); addRule("opt_det_fs", { {"det_fs"}, {} });
		addRule("opt_det_mp", { {"det_mp"}, {} }); addRule("opt_det_fp", { {"det_fp"}, {} });

		addRule("opt_list_adj_ms", { {"list_adj_ms"}, {} });
		addRule("opt_list_adj_fs", { {"list_adj_fs"}, {} });
		addRule("opt_list_adj_mp", { {"list_adj_mp"}, {} });
		addRule("opt_list_adj_fp", { {"list_adj_fp"}, {} });

		addRule("list_adj_ms", { {"adj_ms", "list_adj_ms"}, {"adj_ms"} });
		addRule("list_adj_fs", { {"adj_fs", "list_adj_fs"}, {"adj_fs"} });
		addRule("list_adj_mp", { {"adj_mp", "list_adj_mp"}, {"adj_mp"} });
		addRule("list_adj_fp", { {"adj_fp", "list_adj_fp"}, {"adj_fp"} });

		addRule("list_postmod_ms", { {"postmod_ms", "list_postmod_ms"}, {} });
		addRule("list_postmod_fs", { {"postmod_fs", "list_postmod_fs"}, {} });
		addRule("list_postmod_mp", { {"postmod_mp", "list_postmod_mp"}, {} });
		addRule("list_postmod_fp", { {"postmod_fp", "list_postmod_fp"}, {} });

		addRule("det_ms", { {"el"}, {"un"}, {"este"}, {"mi"}, {"tu"}, {"su"}, {"nuestro"}, {"vuestro"}, {"todo"}, {"poco"}, {"mucho"} });
		addRule("det_fs", { {"la"}, {"una"}, {"esta"}, {"mi"}, {"tu"}, {"su"}, {"nuestra"}, {"vuestra"}, {"toda"}, {"poca"}, {"mucha"} });
		addRule("det_mp", { {"los"}, {"unos"}, {"estos"}, {"mis"}, {"tus"}, {"sus"}, {"nuestros"}, {"vuestros"}, {"todos"}, {"pocos"}, {"muchos"} });
		addRule("det_fp", { {"las"}, {"unas"}, {"estas"}, {"mis"}, {"tus"}, {"sus"}, {"nuestras"}, {"vuestras"}, {"todas"}, {"pocas"}, {"muchas"} });

		addRule("noun_ms", { {"libro"}, {"coche"}, {"tiempo"}, {"año"}, {"trabajo"}, {"amigo"}, {"profesor"}, {"perро"}, {"gato"}, {"árbol"}, {"problema"} });
		addRule("noun_fs", { {"casa"}, {"ciudad"}, {"vida"}, {"escuela"}, {"amiga"}, {"profesora"}, {"mesa"}, {"puerta"}, {"flor"}, {"noche"}, {"mano"} });
		addRule("noun_mp", { {"libros"}, {"coches"}, {"tiempos"}, {"años"}, {"trabajos"}, {"amigos"}, {"profesores"}, {"perros"}, {"gatos"}, {"árboles"}, {"problemas"} });
		addRule("noun_fp", { {"casas"}, {"ciudades"}, {"vidas"}, {"escuelas"}, {"amigas"}, {"profesoras"}, {"mesas"}, {"puertas"}, {"flores"}, {"noches"}, {"manos"} });

		addRule("adj_ms", { {"bueno"}, {"nuevo"}, {"viejo"}, {"pequeño"}, {"hermoso"}, {"grande"}, {"interesante"}, {"importante"}, {"rojo"}, {"rápido"}, {"inteligente"} });
		addRule("adj_fs", { {"buena"}, {"nueva"}, {"vieja"}, {"pequeña"}, {"hermosa"}, {"grande"}, {"interesante"}, {"importante"}, {"roja"}, {"rápida"}, {"inteligente"} });
		addRule("adj_mp", { {"buenos"}, {"nuevos"}, {"viejos"}, {"pequeños"}, {"hermosos"}, {"grandes"}, {"interesantes"}, {"importantes"}, {"rojos"}, {"rápidos"}, {"inteligentes"} });
		addRule("adj_fp", { {"buenas"}, {"nuevas"}, {"viejas"}, {"pequeñas"}, {"hermosas"}, {"grandes"}, {"interesantes"}, {"importantes"}, {"rojas"}, {"rápidas"}, {"inteligentes"} });

		addRule("postmod_ms", { {"prep_phrase_ms"}, {"relative_clause"} });
		addRule("postmod_fs", { {"prep_phrase_fs"}, {"relative_clause"} });
		addRule("postmod_mp", { {"prep_phrase_mp"}, {"relative_clause"} });
		addRule("postmod_fp", { {"prep_phrase_fp"}, {"relative_clause"} });

		addRule("prep_phrase_ms", { {"preposition", "noun_phrase_ms"} });
		addRule("prep_phrase_fs", { {"preposition", "noun_phrase_fs"} });
		addRule("prep_phrase_mp", { {"preposition", "noun_phrase_mp"} });
		addRule("prep_phrase_fp", { {"preposition", "noun_phrase_fp"} });

		addRule("relative_clause", { {"que", "verb_any", "complements"} });
		addRule("verb_any", { {"verb_1sg"}, {"verb_2sg"}, {"verb_3sg"}, {"verb_1pl"}, {"verb_2pl"}, {"verb_3pl"} });

		addRule("preposition", { {"en"}, {"a"}, {"de"}, {"con"}, {"por"}, {"para"}, {"sin"}, {"sobre"}, {"desde"}, {"hasta"}, {"entre"}, {"bajo"} });

		vector<string> inf = { "hablar", "comer", "vivir", "estudiar", "viajar", "escribir", "leer", "correr", "abrir", "recibir", "cantar", "bailar", "trabajar" };
		vector<string> irr = { "tendr", "vendr", "saldr", "podr", "querr", "habr", "dir", "har", "sabr", "pondr", "valdr", "cabr" };
		vector<string> bases = inf;
		bases.insert(bases.end(), irr.begin(), irr.end());

		vector<vector<string>> v_1sg, v_2sg, v_3sg, v_1pl, v_2pl, v_3pl;
		for (const string& b : bases) {
			v_1sg.push_back({ b + "é" }); v_2sg.push_back({ b + "ás" });
			v_3sg.push_back({ b + "á" }); v_1pl.push_back({ b + "emos" });
			v_2pl.push_back({ b + "éis" }); v_3pl.push_back({ b + "án" });
		}
		addRule("verb_1sg", v_1sg); addRule("verb_2sg", v_2sg);
		addRule("verb_3sg", v_3sg); addRule("verb_1pl", v_1pl);
		addRule("verb_2pl", v_2pl); addRule("verb_3pl", v_3pl);

		addRule("complements", { {"complement", "complements"}, {} });
		addRule("complement", { {"direct_object"}, {"indirect_object"}, {"adverbial"} });

		addRule("direct_object", { {"noun_phrase_ms"}, {"noun_phrase_fs"}, {"noun_phrase_mp"}, {"noun_phrase_fp"},
								  {"lo"}, {"la"}, {"los"}, {"las"} });

		addRule("indirect_object", { {"a", "noun_phrase_ms"}, {"a", "noun_phrase_fs"}, {"a", "noun_phrase_mp"}, {"a", "noun_phrase_fp"},
									{"le"}, {"les"} });

		addRule("adverbial", { {"mañana"}, {"pronto"}, {"siempre"}, {"nunca"}, {"rápidamente"}, {"bien"},
							  {"mal"}, {"aquí"}, {"allí"}, {"también"}, {"probablemente"}, {"seguramente"}, {"quizás"} });

		addRule("question_word", { {"qué"}, {"quién"}, {"quiénes"}, {"cuándo"}, {"dónde"}, {"adónde"},
								  {"por qué"}, {"cómo"}, {"cuál"}, {"cuáles"}, {"cuánto"}, {"cuánta"}, {"cuántos"}, {"cuántas"} });

		set<string> terms;
		for (const auto& pair : rules) {
			for (const auto& prod : pair.second) {
				for (const auto& sym : prod) {
					if (rules.find(sym) == rules.end() && sym != "." && sym != "?" && sym != "¿") {
						terms.insert(sym);
					}
				}
			}
		}
		vocabulary.assign(terms.begin(), terms.end());
	}

	void generateNode(const string& symbol, vector<string>& output, int depth = 0) {
		if (rules.find(symbol) == rules.end()) {
			output.push_back(symbol);
			return;
		}
		const auto& productions = rules[symbol];
		if (productions.empty()) return;

		int idx = 0;
		//if (depth < 16) {
		//	if (productions.size() > 1) {
		//		idx = (rand() % 100 < (1 - depth / 8.0) * 100) && depth > 8 ? 0 : (rand() % productions.size());
		//	}
		//	else {
		//		idx = productions.size() - 1;
		//	}

		//	
		//}
		//
		//else {
		//	for (int i = 0; i < (int)productions.size(); ++i) {
		//		if (productions[i].empty()) { idx = i; break; }
		//		if (productions[i].size() == 1 && rules.find(productions[i][0]) == rules.end()) { idx = i; }
		//	}
		//}
		if (symbol.contains("list_postmod") || symbol == "complements") {
			if (rand() % 100 < (depth > 16 ? 16 : depth) / 16.0 * 100) {
				idx = productions.size() - 1;
			}
			else {
				idx = rand() % (productions.size() - 1);
			}
		}
		else {
			idx = rand() % productions.size();
		}

		for (const string& sym : productions[idx]) {
			generateNode(sym, output, depth + 1);
		}
	}

	bool parseNode(const vector<string>& tokens, size_t& pos, const string& symbol, TreeNode*& out_node) {
		if (pos > max_parse_pos) max_parse_pos = pos;

		if (rules.find(symbol) == rules.end()) {
			if (pos < tokens.size() && tokens[pos] == symbol) {
				out_node = new TreeNode(symbol);
				pos++;
				if (pos > max_parse_pos) max_parse_pos = pos;
				return true;
			}
			return false;
		}

		size_t original_pos = pos;
		for (const auto& prod : rules[symbol]) {
			TreeNode* current = new TreeNode(symbol);
			bool match = true;

			if (prod.empty()) {
				current->children.push_back(new TreeNode("ε"));
			}
			else {
				for (const string& sym : prod) {
					TreeNode* child = nullptr;
					if (!parseNode(tokens, pos, sym, child)) {
						match = false;
						break;
					}
					if (child) current->children.push_back(child);
				}
			}

			if (match) {
				out_node = current;
				return true;
			}
			delete current;
			pos = original_pos;
		}
		return false;
	}

	int parseErrorTree(const vector<string>& tokens, size_t& pos, const string& symbol, TreeNode*& out_node) {
		if (rules.find(symbol) == rules.end()) {
			if (pos < tokens.size() && tokens[pos] == symbol) {
				out_node = new TreeNode(symbol);
				pos++;
				return 1;
			}
			if (pos == max_parse_pos) {
				string bad_word = (pos < tokens.size()) ? tokens[pos] : "КОНЕЦ";
				out_node = new TreeNode(COLOR_RED + "[ОЖИДАЛОСЬ: " + symbol + ", ВСТРЕЧЕНО: " + bad_word + "]" + COLOR_RESET);
				return 2;
			}
			return 0;
		}

		size_t original_pos = pos;
		for (const auto& prod : rules[symbol]) {
			TreeNode* current = new TreeNode(symbol);
			int status = 1;

			if (prod.empty()) {
				current->children.push_back(new TreeNode("ε"));
			}
			else {
				for (const string& sym : prod) {
					TreeNode* child = nullptr;
					int res = parseErrorTree(tokens, pos, sym, child);
					if (res == 1) {
						if (child) current->children.push_back(child);
					}
					else if (res == 2) {
						if (child) current->children.push_back(child);
						status = 2;
						break;
					}
					else {
						status = 0;
						break;
					}
				}
			}

			if (status == 1 || status == 2) {
				out_node = current;
				return status;
			}
			delete current;
			pos = original_pos;
		}
		return 0;
	}

	void replaceAll(string& str, const string& from, const string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}

	void printTree(TreeNode* node, string prefix = "", bool isLast = true) {
		if (!node) return;
		cout << prefix << (isLast ? "└── " : "├── ");

		if (node->children.empty()) {
			if (node->symbol == "ε") cout << COLOR_CYAN << node->symbol << COLOR_RESET << "\n";
			else if (node->symbol.find("[ОЖИДАЛОСЬ") != string::npos) cout << node->symbol << "\n";
			else cout << COLOR_GREEN << node->symbol << COLOR_RESET << "\n";
		}
		else {
			cout << COLOR_YELLOW << node->symbol << COLOR_RESET << "\n";
		}

		for (size_t i = 0; i < node->children.size(); ++i) {
			printTree(node->children[i], prefix + (isLast ? "    " : "│    "), i == node->children.size() - 1);
		}
	}

	bool isCapitalized(const string& str) {
		if (str.empty()) return false;
		unsigned char c1 = str[0];
		unsigned char c2 = str.size() > 1 ? str[1] : 0;
		if (c1 == 0xC3 && c2 == 0x89) return true;
		if (c1 >= 'A' && c1 <= 'Z') return true;
		return false;
	}

	string lowercaseUtf8(const string& str) {
		if (str.empty()) return str;
		unsigned char c1 = str[0];
		unsigned char c2 = str.size() > 1 ? str[1] : 0;
		if (c1 == 0xC3 && c2 == 0x89) return "\xC3\xA9" + str.substr(2);
		string res = str;
		if (c1 >= 'A' && c1 <= 'Z') res[0] = c1 + 32;
		return res;
	}

	string capitalizeUtf8(const string& str) {
		if (str.empty()) return str;
		unsigned char c1 = str[0];
		unsigned char c2 = str.size() > 1 ? str[1] : 0;
		if (c1 == 0xC3 && c2 == 0xA9) return "\xC3\x89" + str.substr(2);
		string res = str;
		if (c1 >= 'a' && c1 <= 'z') res[0] = c1 - 32;
		return res;
	}

public:
	LanguageModel() {
		srand((unsigned)time(0));
		buildGrammar();
	}

	string tokensToString(const vector<string>& tokens) {
		string res = "";
		bool capitalize_next = true;
		for (size_t i = 0; i < tokens.size(); i++) {
			if (i > 0 && tokens[i] != "." && tokens[i] != "?" && tokens[i - 1] != "¿") {
				res += " ";
			}
			string current_token = tokens[i];
			if (capitalize_next && current_token != "¿") {
				current_token = capitalizeUtf8(current_token);
				capitalize_next = false;
			}
			res += current_token;
		}
		return res;
	}

	vector<string> generateTokenList() {
		vector<string> tokens;
		generateNode("sentence", tokens);
		return tokens;
	}

	string getRandomWord() {
		if (vocabulary.empty()) return "error";
		return vocabulary[rand() % vocabulary.size()];
	}

	void checkAndPrintSentence(string text, int tree_flag = 0) {
		replaceAll(text, ".", " . ");
		replaceAll(text, "?", " ? ");
		replaceAll(text, "¿", " ¿ ");

		vector<string> tokens;
		stringstream ss(text);
		string item;
		while (ss >> item) tokens.push_back(item);

		if (tokens.empty()) return;

		int target_idx = (tokens[0] == "¿") ? 1 : 0;
		bool has_capital = false;
		if (target_idx < tokens.size()) {
			has_capital = isCapitalized(tokens[target_idx]);
			if (has_capital) {
				tokens[target_idx] = lowercaseUtf8(tokens[target_idx]);
			}
		}

		if (!has_capital) {
			cout << COLOR_RED << "[ОШИБКА] " << COLOR_RESET << "Синтаксическая ошибка. Предложение должно начинаться с заглавной буквы.\n";
			return;
		}

		max_parse_pos = 0;
		size_t pos = 0;
		TreeNode* root = nullptr;
		bool is_valid = parseNode(tokens, pos, "sentence", root);
		is_valid = is_valid && (pos == tokens.size());

		if (is_valid) {
			cout << COLOR_GREEN << "[ОК] " << COLOR_RESET << "Предложение корректно.\n";
			if (tree_flag != 0) {
				cout << "Дерево разбора:\n";
				printTree(root);
			}
		}
		else {
			cout << COLOR_RED << "[ОШИБКА] " << COLOR_RESET << "Синтаксическая ошибка.\n";
			cout << "Анализатор остановился на позиции " << max_parse_pos << ": ";

			for (size_t i = 0; i < tokens.size(); ++i) {
				string word_to_print = tokens[i];
				if (i == target_idx) word_to_print = capitalizeUtf8(word_to_print);

				if (i > 0 && tokens[i] != "." && tokens[i] != "?" && tokens[i - 1] != "¿") cout << " ";

				if (i == max_parse_pos) cout << COLOR_RED << word_to_print << COLOR_RESET;
				else cout << word_to_print;
			}
			if (max_parse_pos == tokens.size()) cout << COLOR_RED << " [КОНЕЦ]" << COLOR_RESET;
			cout << "\n";

			if (tree_flag == 2) {
				cout << "Частичное дерево разбора:\n";
				TreeNode* err_root = nullptr;
				size_t err_pos = 0;
				parseErrorTree(tokens, err_pos, "sentence", err_root);
				printTree(err_root);
				delete err_root;
			}
		}
		delete root;
	}
};

int treeNeeded = 0;

bool readInt(int& out_value, const string& prompt, int min_val = INT_MIN, int max_val = INT_MAX) {
	string line;
	cout << prompt;
	if (!getline(cin, line)) return false;

	size_t start = line.find_first_not_of(" \t\r\n");
	if (start == string::npos) return false;

	size_t end = line.find_last_not_of(" \t\r\n");
	string trimmed = line.substr(start, end - start + 1);

	try {
		size_t pos;
		long val = stol(trimmed, &pos);

		if (pos < trimmed.length()) return false;

		if (val < min_val || val > max_val) return false;

		out_value = static_cast<int>(val);
		return true;
	}
	catch (...) {
		return false;
	}
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	LanguageModel model;
	int treeNeeded = 0;
	int choice;
	bool running = true;

	while (running) {
		cout << "\n============================================\n";
		cout << "            Главное меню            \n";
		cout << "1. Ввести предложение\n";
		cout << "2. Сгенерировать предложения\n";
		cout << "3. Выводить дерево: " << ((treeNeeded == 0) ? "Нет" : ((treeNeeded == 1) ? "Только для корректных" : "Для всех")) << endl;
		cout << "4. Выход\n";
		cout << "============================================\n";

		while (!readInt(choice, "Выберите действие: ", 1, 4)) {
			cout << "Введите число от 1 до 4.\n";
		}

		if (choice == 1) {
			cout << "\nВведите предложение на испанском:\n> ";
			string input;
			getline(cin, input);
			model.checkAndPrintSentence(input, treeNeeded);
		}
		else if (choice == 2) {
			int n, m;
			while (!readInt(n, "Количество строк: ", 1, 100)) {
				cout << "Введите число от 1 до 100.\n";
			}
			while (!readInt(m, "Вероятность ошибки (0-100): ", 0, 100)) {
				cout << "Введите число от 0 до 100.\n";
			}

			for (int i = 0; i < n; ++i) {
				vector<string> tokens = model.generateTokenList();
				if ((rand() % 100) < m && !tokens.empty()) {
					int attempts = 0;
					while (attempts++ < 10) {
						int idx = rand() % tokens.size();
						if (tokens[idx] != "." && tokens[idx] != "?" && tokens[idx] != "¿") {
							tokens[idx] = model.getRandomWord();
							break;
						}
					}
				}
				string sentence = model.tokensToString(tokens);
				cout << "\nСтрока " << i + 1 << ": " << sentence << "\n";
				model.checkAndPrintSentence(sentence, treeNeeded);
			}
		}
		else if (choice == 3) {
			treeNeeded = (treeNeeded + 1) % 3;
		}
		else if (choice == 4) {
			running = false;
		}
	}
	return 0;
}
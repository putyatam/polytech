#include "node.h"
#include "lexem.h"
#include "console_ui.h"

#include <string>
#include <array>
#include <Windows.h>


string alph = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+-*/:=|().\n\t ";

bool is_slash(char c) {
	return c == '/';
}

bool in_alph(char c) {
	return alph.find(c) != string::npos;
}

bool in_num_chars(char c) {
	return ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9' || c == '.');
}

bool is_ws_char(char c) {
	return c == ' ' || c == '\n' || c == '\t';
}

bool is_id_char(char c) {
	return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9';
}

enum class Group {
	all,
	comment,
	mr_comment,
	id,
	id_over,
	assign,
	number,
	number_over
};

string get_err_comment(Group cur_group) {
	switch (cur_group)
	{
	case Group::all: return "Неизвестная лексема";
	case Group::comment: return "Неизвестные символы в комментарии";
	case Group::mr_comment: return "Неизвестные символы в многострочном комментарии";
	case Group::id: return "Неправильное название идентификатора";
	case Group::id_over: return "Идентификатор содержит больше 15 символов";
	case Group::assign: return "После символа ':' ожидался символ '='";
	case Group::number: return "Неверный формат вещественной константы";
	case Group::number_over: return "Вещественная константа содержит больше 15 символов";
	default:
		break;
	}
}



int main() {

	int count_chars_identifier = 0;
	int count_chars_number = 0;

	Group cur_group = Group::all;

	// Задание типов лексем
	Lexem::set_types({ 
		{"id", "Идентификатор"}, 
		{"arithmetic", "Арифметический оператор"}, 
		{"assign", "Знак присваивания"}, 
		{"left_bracket", "Левая скобка"}, 
		{"right_bracket", "Правая скобка"}, 
		{"const", "Константа"}, 
		{"separator", "Разделитель"}
		});

	// Создание базовых лексем
	Lexem* lexem_plus = new Lexem("arithmetic", "+");
	Lexem* lexem_minus = new Lexem("arithmetic", "-");
	Lexem* lexem_mult = new Lexem("arithmetic", "*");
	Lexem* lexem_div = new Lexem("arithmetic", "/");
	Lexem* lexem_assign = new Lexem("assign", ":=");
	Lexem* lexem_lt_bracket = new Lexem("left_bracket", "(");
	Lexem* lexem_rt_bracket = new Lexem("right_bracket", ")");
	Lexem* lexem_sep = new Lexem("separator", "|");

	// удаление первого символа
	Effect shift([]() { 
		Node::prev_char = Node::data.front(); 
		Node::data.pop_front();
		});
	// очистка буфера
	Effect clr_buf([]() { Node::buffer.clear(); });
	// удаление последнего символа буфера
	Effect pop_back_buf([]() { Node::buffer.pop_back(); });
	// добавление ошибки
	Effect add_error([&cur_group]() {
		string err_com = get_err_comment(cur_group);
		
		Node::errors.push_back({ Node::buffer, err_com });
		});
	// инкрементирование счётчика символов идентфиикатора
	Effect add_char_in_id([&cur_group, &count_chars_identifier]() {
		if (++count_chars_identifier > 15) {
			cur_group = Group::id_over;
		}
		});
	// инкрементирование счётчика символов константы
	Effect add_char_in_number([&cur_group, &count_chars_number]() {
		if (++count_chars_number > 15) {
			cur_group = Group::number_over;
		}
		});

	// очистка счётчика символов идентификатора
	Effect clear_count_id([&count_chars_identifier]() {
		count_chars_identifier = 0;
		});

	// очистка счётчика символов константы
	Effect clear_count_number([&count_chars_number]() {
		count_chars_number = 0;
		});

	// Основные эффекты
	Effect y1([]() { Node::buffer.push_back(Node::data.front());});
	Effect y3([]() { Node::table.push_back(new Lexem("id", Node::buffer)); });
	Effect y4([&lexem_plus]() { Node::table.push_back(lexem_plus); });
	Effect y5([&lexem_assign]() { Node::table.push_back(lexem_assign); });
	Effect y6([&lexem_minus]() { Node::table.push_back(lexem_minus); });
	Effect y7([&lexem_mult]() { Node::table.push_back(lexem_mult); });
	Effect y8([&lexem_div]() { Node::table.push_back(lexem_div); });
	Effect y9([&lexem_lt_bracket]() { Node::table.push_back(lexem_lt_bracket); });
	Effect y10([&lexem_rt_bracket]() { Node::table.push_back(lexem_rt_bracket); });
	Effect y11([]() { Node::table.push_back(new Lexem("const", Node::buffer)); });
	Effect y12([&lexem_sep]() { Node::table.push_back(lexem_sep); });

	// Эффекты для отслеживания группы узлов
	Effect in_ws([&cur_group, &count_chars_identifier, &count_chars_number]() {
		cur_group = Group::all; 
		count_chars_identifier = 0; 
		count_chars_number = 0;
		});
	Effect in_comment([&cur_group]() { cur_group = Group::comment; });
	Effect in_mr_comment([&cur_group]() { cur_group = Group::mr_comment; });
	Effect in_id([&cur_group]() { cur_group = Group::id; });
	Effect in_assign([&cur_group]() { cur_group = Group::assign; });
	Effect in_number([&cur_group]() { cur_group = Group::number; });

	// Создание узлов диаграммы
	vector<Node> nodes(28);
	vector<Node*> nodes_first({ &nodes[0], &nodes[1], &nodes[11], &nodes[14], &nodes[15], &nodes[17], &nodes[18], &nodes[19], &nodes[20], &nodes[21], &nodes[27] });
	nodes[0].set_params([](char c) {return is_ws_char(c); }, nodes_first, { &clr_buf, &in_ws, &shift }, "0");
	nodes[1].set_params(is_slash, { &nodes[2], &nodes[5], &nodes[6] }, { &clr_buf, &shift }, "1");
	nodes[2].set_params(is_slash, { &nodes[3], &nodes[4] }, { &in_comment, &shift }, "2");
	nodes[3].set_params([](char c) {return c != '\n'; }, { &nodes[3], &nodes[4] }, { &y1, &shift }, "3");
	nodes[4].set_params([](char c) {return c == '\n'; }, nodes_first, { &shift, &clr_buf }, "4");
	nodes[5].set_params([](char c) {return c != '/' && c != '*' && in_alph(c); }, nodes_first, { &y8 }, "5");
	nodes[6].set_params([](char c) {return c == '*'; }, { &nodes[7], &nodes[8] }, { &in_mr_comment, &shift }, "6");
	nodes[7].set_params([](char c) {return c != '*'; }, { &nodes[7], &nodes[8] }, { &y1, &shift }, "7");
	nodes[8].set_params([](char c) {return c == '*'; }, { &nodes[9], &nodes[10] }, { &y1, &shift }, "8");
	nodes[9].set_params(is_slash, nodes_first, { &pop_back_buf, &shift, &clr_buf}, "9");
	nodes[10].set_params([](char c) {return c != '/'; }, { &nodes[7], &nodes[8] }, { &in_mr_comment, &y1 }, "10");
	nodes[11].set_params([](char c) {return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z'; }, { &nodes[12], &nodes[13] }, { &clr_buf, &in_id, &y1, &shift, &clear_count_id, &add_char_in_id }, "11");
	nodes[12].set_params([&count_chars_identifier](char c) {return not ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9') && in_alph(c) && count_chars_identifier <= 15;; }, nodes_first, {&y3 }, "12");
	nodes[13].set_params([&count_chars_identifier](char c) {return ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '0' <= c && c <= '9') && count_chars_identifier <= 15; }, { &nodes[12], &nodes[13] }, { &y1, &shift, &add_char_in_id }, "13");
	nodes[14].set_params([](char c) {return c == '+'; }, nodes_first, { &clr_buf, &in_ws, &y4, &shift }, "14");
	nodes[15].set_params([](char c) {return c == ':'; }, { &nodes[16] }, { &clr_buf, &in_assign, &shift }, "15");
	nodes[16].set_params([](char c) {return c == '='; }, nodes_first, { &y5, &in_ws, &shift }, "16");
	nodes[17].set_params([](char c) {return c == '-'; }, nodes_first, { &clr_buf, &in_ws, &y6, &shift }, "17");
	nodes[18].set_params([](char c) {return c == '*'; }, nodes_first, { &clr_buf, &in_ws, &y7, &shift }, "18");
	nodes[19].set_params([](char c) {return c == '('; }, nodes_first, { &clr_buf, &in_ws, &y9, &shift }, "19");
	nodes[20].set_params([](char c) {return c == ')'; }, nodes_first, { &clr_buf, &in_ws, &y10, &shift }, "20");
	nodes[21].set_params([](char c) {return c == '0'; }, { &nodes[22] }, { &clr_buf, &in_number, &y1, &shift, &clear_count_number, &add_char_in_number }, "21");
	nodes[22].set_params([](char c) {return c == 'x'; }, { &nodes[23] }, { &y1, &shift, &add_char_in_number }, "22");
	nodes[23].set_params([&count_chars_number](char c) {return ('a' <= c && c <= 'f' || '0' <= c && c <= '9') && count_chars_number <= 15; }, { &nodes[23], &nodes[24], &nodes[26] }, { &y1, &shift, &add_char_in_number }, "23");
	nodes[24].set_params([&count_chars_number](char c) {return c == '.' && count_chars_number <= 15; }, { &nodes[25] }, { &y1, &shift, &add_char_in_number }, "24");
	nodes[25].set_params([&count_chars_number](char c) {return ('a' <= c && c <= 'f' || '0' <= c && c <= '9') && count_chars_number <= 15; }, { &nodes[25], &nodes[26] }, { &y1, &shift, &add_char_in_number }, "25");
	nodes[26].set_params([](char c) {return not in_num_chars(c); }, nodes_first, {&y11, &clr_buf, &in_ws }, "26");
	nodes[27].set_params([](char c) {return c == '|'; }, nodes_first, { &clr_buf, &in_ws, &y12, &shift }, "27");

	// Узел сборщика ошибки
	Node error_builder_node;
	error_builder_node.set_params(
		[&cur_group](char c) { 
			switch (cur_group)
			{
			case Group::id: return not is_id_char(c) && in_alph(c);
			case Group::id_over: return not is_id_char(c) && in_alph(c);
			case Group::comment: return c == '\n';
			case Group::mr_comment: 
				if (c == '/' && Node::prev_char == '*') {
					Node::buffer.pop_back();
					return true;
				}
				return false;
				
			case Group::number: return !in_num_chars(c);
			case Group::number_over: return !in_num_chars(c);
			default: return in_alph(c);
			}
		}, nodes_first, { &add_error, &clr_buf, &in_ws }, "error_builder");

	// Узел обработчика ошибки
	Node::error_node = new Node();
	vector<Node*> nodes_from_error = nodes_first;
	nodes_from_error.push_back(Node::error_node);
	Node::error_node->set_params([](char c) {return false; }, { &error_builder_node}, { &y1, &shift }, "error");

	Node::start_node = &nodes[0];
	// Запуск обработчика пользовательского ввода
	input_loop();
}

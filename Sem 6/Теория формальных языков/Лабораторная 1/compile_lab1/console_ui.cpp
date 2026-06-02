#include "console_ui.h"
#include "node.h"

#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>


int utf8_length(const string& str) {
    int length = 0;
    for (int i = 0; i < str.length(); ++i) {
        if ((str[i] & 0xC0) != 0x80) {
            length++;
        }
    }
    return length;
}

vector<string> split_by_newline(const string& str) {
    vector<string> lines;
    stringstream ss(str);
    string line;
    while (getline(ss, line, '\n')) {
        lines.push_back(line);
    }
    if (lines.empty() && !str.empty()) lines.push_back("");
    return lines;
}

string lexems_table_to_str(const vector<Lexem*> table, const string title,
    const string col1, const string col2, const string col3) {

    string result;

    auto get_max_visible_width = [](const string& str) {
        vector<string> lines = split_by_newline(str);
        int max_w = 0;
        for (const auto& l : lines) max_w = max(max_w, utf8_length(l));
        return max_w;
        };

    int max_l1 = get_max_visible_width(col1) + 2;
    int max_l2 = get_max_visible_width(col2) + 2;
    int max_l3 = get_max_visible_width(col3) + 2;

    for (Lexem* lexem : table) {
        max_l1 = max(max_l1, get_max_visible_width(lexem->get_token()) + 2);
        max_l2 = max(max_l2, get_max_visible_width(lexem->get_type(true)) + 2);
        max_l3 = max(max_l3, get_max_visible_width(lexem->get_value()) + 2);
    }

    auto repeat_str = [](const string& s, int n) {
        string r;
        while (n-- > 0) r += s;
        return r;
        };

    auto left_align = [&](const string& text, int width) {
        int len = utf8_length(text);
        return " " + text + string(max(0, width - len - 1), ' ');
        };

    auto center = [&](const string& text, int width) {
        int len = utf8_length(text);
        int total_pad = width - len;
        int left = total_pad / 2;
        return string(left, ' ') + text + string(total_pad - left, ' ');
        };

    int total_table_width = max_l1 + max_l2 + max_l3 + 2;
    int title_pad = (total_table_width + 2 - utf8_length(title)) / 2;
    if (title_pad > 0) result += string(title_pad, ' ');
    result += title + "\n";

    result += "╔" + repeat_str("═", max_l1) + "╤" + repeat_str("═", max_l2) + "╤" + repeat_str("═", max_l3) + "╗\n";
    result += "║" + center(col1, max_l1) + "│" + center(col2, max_l2) + "│" + center(col3, max_l3) + "║\n";
    result += "╠" + repeat_str("═", max_l1) + "╪" + repeat_str("═", max_l2) + "╪" + repeat_str("═", max_l3) + "╣\n";

    for (size_t i = 0; i < table.size(); ++i) {
        vector<string> lines1 = split_by_newline(table[i]->get_token());
        vector<string> lines2 = split_by_newline(table[i]->get_type(true));
        vector<string> lines3 = split_by_newline(table[i]->get_value());

        size_t row_height = max({ lines1.size(), lines2.size(), lines3.size() });

        for (size_t h = 0; h < row_height; ++h) {
            string s1 = (h < lines1.size()) ? lines1[h] : "";
            string s2 = (h < lines2.size()) ? lines2[h] : "";
            string s3 = (h < lines3.size()) ? lines3[h] : "";

            result += "║" + left_align(s1, max_l1) + "│"
                + left_align(s2, max_l2) + "│"
                + left_align(s3, max_l3) + "║\n";
        }

        if (i < table.size() - 1) {
            result += "╟" + repeat_str("─", max_l1) + "┼" + repeat_str("─", max_l2) + "┼" + repeat_str("─", max_l3) + "╢\n";
        }
    }

    result += "╚" + repeat_str("═", max_l1) + "╧" + repeat_str("═", max_l2) + "╧" + repeat_str("═", max_l3) + "╝\n";

    return result;
}

vector<string> get_utf8_chars(const string& str) {
    vector<string> chars;
    for (int i = 0; i < str.length(); ++i) {
        string ch;
        ch += str[i];
        if ((str[i] & 0x80) != 0) {
            if ((str[i] & 0xE0) == 0xC0) { ch += str[++i]; }
            else if ((str[i] & 0xF0) == 0xE0) { ch += str[++i]; ch += str[++i]; }
            else if ((str[i] & 0xF8) == 0xF0) { ch += str[++i]; ch += str[++i]; ch += str[++i]; }
        }
        chars.push_back(ch);
    }
    return chars;
}

vector<string> wrap_text(const string& text, int max_width) {
    if (max_width <= 0) return { "" };
    vector<string> chars = get_utf8_chars(text);
    vector<string> lines;
    string current_line = "";
    int current_len = 0;

    for (const string& ch : chars) {
        if (current_len >= max_width) {
            lines.push_back(current_line);
            current_line = "";
            current_len = 0;
        }
        current_line += ch;
        current_len++;
    }
    if (!current_line.empty() || lines.empty()) lines.push_back(current_line);
    return lines;
}

#include <sstream>
#include <algorithm>

vector<string> smart_wrap(const string& text, int max_width) {
    vector<string> final_lines;
    stringstream ss(text);
    string segment;

    while (getline(ss, segment, '\n')) {
        vector<string> wrapped_segment = wrap_text(segment, max_width);
        final_lines.insert(final_lines.end(), wrapped_segment.begin(), wrapped_segment.end());
    }

    if (final_lines.empty() && !text.empty()) final_lines.push_back("");
    return final_lines;
}

string pairs_table_to_str(const vector<pair<string, string>>& data,
    const string title,
    const string col1_name, const string col2_name,
    int limit_w1, int limit_w2) {

    auto get_max_visible_width = [](const string& str) {
        stringstream ss(str);
        string line;
        int max_w = 0;
        while (getline(ss, line, '\n')) max_w = max(max_w, utf8_length(line));
        return max_w;
        };

    int content_w1 = get_max_visible_width(col1_name);
    int content_w2 = get_max_visible_width(col2_name);

    for (const auto& p : data) {
        content_w1 = max(content_w1, get_max_visible_width(p.first));
        content_w2 = max(content_w2, get_max_visible_width(p.second));
    }

    int w1 = min(content_w1, limit_w1) + 2;
    int w2 = min(content_w2, limit_w2) + 2;

    auto repeat_str = [](const string& s, int n) {
        string r; while (n-- > 0) r += s; return r;
        };

    auto center = [&](const string& text, int width) {
        int len = utf8_length(text);
        int pad = max(0, width - len);
        int left = pad / 2;
        return string(left, ' ') + text + string(pad - left, ' ');
        };

    auto left_align = [&](const string& text, int width) {
        int len = utf8_length(text);
        return " " + text + string(max(0, width - len - 1), ' ');
        };

    string result;
    int total_w = w1 + w2 + 1;

    int title_len = utf8_length(title);
    int title_pad = (total_w + 2 - title_len) / 2;
    if (title_pad > 0) result += string(title_pad, ' ');
    result += title + "\n";

    result += "╔" + repeat_str("═", w1) + "╤" + repeat_str("═", w2) + "╗\n";
    result += "║" + center(col1_name, w1) + "│" + center(col2_name, w2) + "║\n";
    result += "╠" + repeat_str("═", w1) + "╪" + repeat_str("═", w2) + "╣\n";

    for (size_t i = 0; i < data.size(); ++i) {
        vector<string> lines1 = smart_wrap(data[i].first, w1 - 2);
        vector<string> lines2 = smart_wrap(data[i].second, w2 - 2);

        size_t row_height = max(lines1.size(), lines2.size());

        for (size_t h = 0; h < row_height; ++h) {
            string l_text = (h < lines1.size()) ? lines1[h] : "";
            string r_text = (h < lines2.size()) ? lines2[h] : "";
            result += "║" + left_align(l_text, w1) + "│" + left_align(r_text, w2) + "║\n";
        }

        if (i < data.size() - 1) {
            result += "╟" + repeat_str("─", w1) + "┼" + repeat_str("─", w2) + "╢\n";
        }
    }

    result += "╚" + repeat_str("═", w1) + "╧" + repeat_str("═", w2) + "╝\n";

    return result;
}

void print_analysis_result(pair<vector<Lexem*>, vector<pair<string, string>>> output) {
    cout << lexems_table_to_str(output.first, "Таблица лексем", "Лексема", "Тип лексемы", "Значение") << endl;
    cout << pairs_table_to_str(output.second, "Таблица ошибок", "Значение", "Тип ошибки", 10, 20) << endl;
    //cout << "Буфер: '" << Node::buffer << "'" << endl;
}


void input_loop() {
    cout << "===== Лексический анализатор кода =====" << endl;
    string user_input;
    string data;
    while (true) {
        cout << "\nВыберите действие:\n\t[1] Ввести код программы\n\t[2] Прочитать код программы из файла\n\t[exit] Выход" << endl;
        getline(cin, user_input);

        if (user_input == "1") {
            cout << "\nВведите код программы:" << endl;
            getline(cin, data);
        }
        else if (user_input == "2") {
            bool in_reading = true;
            bool exit = false;
            while (in_reading) {
                string path;
                cout << "\nВведите путь к файлу ('exit' чтобы отменить):\n";
                getline(cin, path);
                if (path == "exit") {
                    exit = true;
                    in_reading = false;
                }
                else {
                    ifstream file(path);
                    if (file.is_open()) {
                        data = string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
                        in_reading = false;

                        cout << "\033[92mФайл прочитан.\033[0m\n\tКоличество строк - " << count(data.begin(), data.end(), '\n') + 1 << "\n\tКоличество символов - " << utf8_length(data) << endl;
                        bool in_print_file = true;
                        while (in_print_file) {
                            cout << "\nВывести содержимое файла? [Да / Нет]" << endl;
                            getline(cin, user_input);
                            if (user_input == "Да" || user_input == "да" || user_input == "Д" || user_input == "д") {
                                cout << "\n\033[94m(Начало программы)\033[0m\n" << data << "\n\033[94m(Конец программы)\033[0m" << endl;
                                in_print_file = false;
                            }
                            else if (user_input == "Нет" || user_input == "нет" || user_input == "Н" || user_input == "н") {
                                in_print_file = false;
                            }
                            else {
                                cout << "\033[91mВыбор не распознан. Повторите попытку.\033[0m" << endl;
                                continue;
                            }
                        }
                    }
                    else {
                        cout << "\033[91mВведённый путь не найден. Повторите попытку.\033[0m" << endl;
                    }
                }
            }
            if (exit) { continue; };
        }
        else if (user_input == "exit"){
            break;
        }
        else{
            cout << "\033[91mВыбор не распознан. Повторите попытку.\033[0m" << endl;
            continue;
        }

        bool in_output = true;
        bool exit = false;
        while (in_output) {
            cout << "\nВыберите действие:\n\t[1] Провести лексический анализ кода программы\n\t[2] Очистить контекст\n\t[3] Вывести код программы\n\t[4] Ввести новый код программы\n\t[exit] Выход" << endl;
            getline(cin, user_input);
            if (user_input == "1") {
                print_analysis_result(Node::analyze(data + "\n", false, true));
            }
            else if (user_input == "2") {
                Node::errors.clear();
                Node::table.clear();
                Lexem::clr_ids();
            }
            else if (user_input == "3") {
                cout << "\n\033[94m(Начало программы)\033[0m\n" << data << "\n\033[94m(Конец программы)\033[0m" << endl;
            }
            else if (user_input == "4") {
                in_output = false;
            }
            else if (user_input == "exit") {
                exit = true;
                in_output = false;
            }
            else {
                cout << "\033[91mВыбор не распознан. Повторите попытку.\033[0m" << endl;
                continue;
            }
        }
        if (exit) { break; };
        
    }
    cout << "Выход из программы." << endl;
    
    
}
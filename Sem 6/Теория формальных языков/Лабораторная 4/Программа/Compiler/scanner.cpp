#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "scanner.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <codecvt>

using namespace std;


std::wstring readFile(const std::string& path) {
    std::wifstream f(path);

    if (!f.is_open()) return L"";

    f.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    std::wstringstream wss;
    wss << f.rdbuf();
    std::wstring content = wss.str();

    return content;
}



ScanResult tokenize(FSM<std::wstring>* fsm, const std::string& filePath) {
    
    ScanResult result;
    // Читаем файл, путь к которому передали в аргументах
    std::wstring data = readFile(filePath);

    if (data.empty() && !filePath.empty()) {
        // Можно добавить ошибку "файл не найден" в result.errors
    }
    data = data + L'\n';
    fsm->start(data, 0);

    return result;
}


ScanResult tokenize_str(FSM<std::wstring>* fsm, std::wstring data) {
    ScanResult result;
	data = data + L'\n';
    fsm->start(data, 0);

    return result;
}


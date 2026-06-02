#include "header.h"
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <random>
#include <regex>
#include <charconv>
#include <chrono>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;


random_device rd;
mt19937 gen(rd());
uniform_real_distribution<double> dist(0.0, 1.0);
wstring alphabetReplace = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя0123456789-";
uniform_int_distribution<> replaceSymbolId(0, alphabetReplace.size() - 1);
wstring alphabet = L" -0123456789ACDEGHIKLMNPRSVXYabcdefgiklmnoprstuvАВГДЗИКМПСУЮавгдзийклмнорстучьэя";
wstring successStr;
wstring failureStr;
bool useRegex = false;
std::wregex re(L"ГАЗ (1[2348]|2([1245]|(3(08|30)))|31(0(2(21?|9)?|[35])|1(05?|1))|46|6[1479]|А|М-(1|20|72)|Siber)|УАЗ (2(206|3(15|6(08|[25]|32))|7(46|60|722)|9232)|3(1(5(0|1[249]?|3|9)|6[025])|303|741|9(09[45]?|625?))|469|Cargo|Combi|Hunter|Patriot|Pickup|Profi)|Москвич (21(3[678]|4[012])|3|4(0[0-37-8]|1[012]|2[3467]|3(0|4П))|[568]|Дуэт|Иван Калита|Князь Владимир|Святогор|Юрий Долгорукий)|(ВАЗ|LADA) (1111|2(1(0[1-9]|1[0-5]|2[0139]|31)|32[89])|Aura|EL Lada|Granta|Iskra|Kalina|Largus|Niva|Priora|Revolution|Vesta|XRAY)");


wstring genString(float prop) {
    vector<wstring>allV={L"1111",L"2101",L"2102",L"2103",L"2104",L"2105",L"2106",L"2107",L"2108",L"2109",L"2110",L"2111",L"2112",L"2113",L"2114",L"2115",L"2120",L"2121",L"2123",L"2129",L"2131",L"2328",L"2329",L"Aura",L"EL Lada",L"Granta",L"Iskra",L"Kalina",L"Largus",L"Niva",L"Priora",L"Revolution",L"Vesta",L"XRAY"};
    vector<wstring>allU={L"2206",L"2315",L"23608",L"2362",L"23632",L"2365",L"2746",L"2760",L"27722",L"29232",L"3150",L"3151",L"31512",L"31514",L"31519",L"3153",L"3159",L"3160",L"3162",L"3165",L"3303",L"3741",L"3909",L"39094",L"39095",L"3962",L"39625",L"469",L"Cargo",L"Combi",L"Hunter",L"Patriot",L"Pickup",L"Profi"};
    vector<wstring>allG={L"12",L"13",L"14",L"18",L"21",L"22",L"2308",L"2330",L"24",L"25",L"3102",L"31022",L"310221",L"31029",L"3103",L"3105",L"3110",L"31105",L"3111",L"46",L"61",L"64",L"67",L"69",L"А",L"М-1",L"М-20",L"М-72",L"Siber"};
    vector<wstring>allM={L"2136",L"2137",L"2138",L"2140",L"2141",L"2142",L"3",L"400",L"401",L"402",L"403",L"407",L"408",L"410",L"411",L"412",L"423",L"424",L"426",L"427",L"430",L"434П",L"5",L"6",L"8",L"Дуэт",L"Иван Калита",L"Князь Владимир",L"Святогор",L"Юрий Долгорукий"};

    vector<vector<wstring>> names = { allV, allU, allG, allM };

    vector<vector<wstring>> brandGroup = { {L"ВАЗ", L"LADA"}, {L"УАЗ"}, {L"ГАЗ"}, {L"Москвич"} };

    uniform_int_distribution<> dBrandGroup(0, brandGroup.size() - 1);
    int brandGroupId = dBrandGroup(gen);

    uniform_int_distribution<> dBrand(0, brandGroup[brandGroupId].size() - 1);
    int brandId = dBrand(gen);
    
    uniform_int_distribution<> dName(0, names[brandGroupId].size() - 1);
    int NameId = dName(gen);

    wstring result = brandGroup[brandGroupId][brandId] + L" " + names[brandGroupId][NameId];

    if (dist(gen) <= prop) {
        uniform_int_distribution<> dReplaceId(0, result.size() - 1);
        result[dReplaceId(gen)] = alphabetReplace[replaceSymbolId(gen)];
    }

    return result;
}


std::wostream& operator<<(std::wostream& os, FSMResult result) {
    switch (result) {
    case FSMResult::Match: return os << L"Строка соответствует";
    case FSMResult::Mismatch: return os << L"Строка не соответствует";
    case FSMResult::OutOfAlphabet:  return os << L"Используется символ не из алфавита";
    }
}

wstring resToStr(FSMResult result) {
    switch (result) {
    case FSMResult::Match: return L"Строка соответствует";
    case FSMResult::Mismatch: return L"Строка не соответствует";
    case FSMResult::OutOfAlphabet:  return L"Используется символ не из алфавита";
    }
}

wstring realization(bool a) {
    if (a) { return L"regex"; }
    else { return L"КА"; }
}

bool areAllCharsIn(const std::wstring& str, const std::wstring& allowed) {
    if (allowed.empty()) return str.empty();

    return std::all_of(str.begin(), str.end(), [&allowed](wchar_t ch) {
        return allowed.find(ch) != std::wstring::npos;
        });
}


void userIO(FSM<wstring, wchar_t> fsm) {
    while (true) {
        wcout << L"\nВыберите дейстие:\n\t[1] Ввести строку\n\t[2] Сгенерировать строки\n\t[3] Реализация (" << realization(useRegex) << L")\n\t[e] Выход" << endl;
        wstring input;
        getline(wcin, input);

        vector<wstring> vecStr;


        if (input == L"1") {
            wcout << L"Введите строку: ";
            wstring data;
            getline(wcin, data);
            vecStr.push_back(data);
        }
        else if (input == L"2") {
            int count = 0;
            float prop = 0;
            while (true) {
                wcout << L"Введите кол-во строк: ";
                string data;

                getline(cin, data);
                auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), count);

                if (ec == std::errc{}) {
                    if (count < 1 || count > 5000) {
                        wcout << L"\nВведённое число выходит за дапазон [1, 5000]. Повторите попытку." << endl;
                        continue;
                    }
                    break;
                }
                else if (ec == std::errc::invalid_argument) {
                    wcout << L"Неверный ввод. Повторите попытку." << endl;
                    continue;
                }
                else {
                    wcout << L"Введённое число выходит за дапазон [1, 5000]. Повторите попытку." << endl;
                    continue;
                }
            }

            while (true) {
                wcout << L"Введите вероятность случайной замены символа в строке: ";
                string data;

                getline(cin, data);
                auto [ptr, ec] = std::from_chars(data.data(), data.data() + data.size(), prop);

                if (ec == std::errc{} and ptr == data.data() + data.size()) {
                    if (prop < 0 || prop > 1) {
                        wcout << L"Введённое число выходит за дапазон [0, 1]. Повторите попытку." << endl;
                        continue;
                    }
                    break;
                }
                else if (ec == std::errc::invalid_argument) {
                    wcout << L"Неверный ввод. Повторите попытку." << endl;
                    continue;
                }
                else {
                    wcout << L"Введённое число выходит за дапазон [0, 1]. Повторите попытку." << endl;
                    continue;
                }
            }
            for (int i = 0; i < count; i++) {
                vecStr.push_back(fsm.genElem(gen, &alphabet, prop, 33));
            }
            wcout << L"Сгенерировано " << count << L" строк." << endl;
        }
        else if (input == L"3") {
            useRegex = !useRegex;
            continue;
        }
        else if (input == L"e") {
            wcout << L"Выход из программы" << endl;
            break;
        }
        else {
            wcout << L"Неверный ввод. Повторите попытку." << endl;
            continue;
        }

        FSMResult res;

        wstring result;

        int countSucc = 0;
        int countFail = 0;
        
        auto start = std::chrono::steady_clock::now();
        for (wstring s : vecStr) {
            if (useRegex) {
                if (areAllCharsIn(s, alphabet)) {
                    if (regex_match(s, re)) {
                        res = FSMResult::Match;
                        countSucc++;
                        result += L"\"\033[48;2;0;70;0m" + s + L"\033[40m\": " + resToStr(res) + L"\n";
                    }
                    else {
                        res = FSMResult::Mismatch;
                        countFail++;
                        result += L"\033[48;2;200;0;0m" + s + L"\033[40m\": " + resToStr(res) + L"\n";
                    }
                }
                else {
                    res = FSMResult::OutOfAlphabet;
                    result += L"\033[48;2;200;0;0m" + s + L"\033[40m\": " + resToStr(res) + L"\n";
                }
            }
            else {
                failureStr = s;
                successStr.clear();
                res = fsm.start(s, 33, &alphabet);

                if (res == FSMResult::Match) {
                    countSucc++;
                }
                else if (res == FSMResult::Mismatch) {
                    countFail++;
                }
                result += L"\"\033[48;2;0;70;0m" + successStr + L"\033[48;2;200;0;0m" + failureStr + L"\033[40m\": " + resToStr(res) + L"\n";
            }
        }
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> time = end - start;

        wcout << L"Строки обработаны за " << time.count() << L" мс.\n";
        wcout << countSucc << L" строк соответствует" << endl;
        wcout << countFail << L" строк не соответствует" << endl;
        wcout << vecStr.size() - countSucc - countFail << L" строк имеют символы не из алфавита" << endl;
        while (true) {
            wcout << L"\nВывести результаты обработки?\n\t[1] Да\n\t[2] Нет" << endl;
            getline(wcin, input);
            if (input == L"1") {
                wcout << L"Обработанные строки:" << endl;
                wcout << result << endl;
                break;
            }
            else if (input == L"2") {
                break;
            }
            else {
                wcout << L"Неверный ввод. Повторите попытку." << endl;
                continue;
            }
        }
        vecStr.clear();
        
    }
}


int main() {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    wstring alphabet = L" -0123456789ACDEGHIKLMNPRSVXYabcdefgiklmnoprstuvАВГДЗИКМПСУЮавгдзийклмнорстучьэя";

    Action<wchar_t(wchar_t)> baseAction([](wchar_t c) { return c; });

    
    bool (*is0)(wchar_t) = [](wchar_t c) { return c == L'0'; };
    bool (*is1)(wchar_t) = [](wchar_t c) { return c == L'1'; };
    bool (*is2)(wchar_t) = [](wchar_t c) { return c == L'2'; };
    bool (*is3)(wchar_t) = [](wchar_t c) { return c == L'3'; };
    bool (*is4)(wchar_t) = [](wchar_t c) { return c == L'4'; };
    bool (*is5)(wchar_t) = [](wchar_t c) { return c == L'5'; };
    bool (*is6)(wchar_t) = [](wchar_t c) { return c == L'6'; };
    bool (*is7)(wchar_t) = [](wchar_t c) { return c == L'7'; };
    bool (*is8)(wchar_t) = [](wchar_t c) { return c == L'8'; };
    bool (*is9)(wchar_t) = [](wchar_t c) { return c == L'9'; };


    FSM<wstring, wchar_t>::TransitionMapNull tG;
    tG[0] = {
        {1, is1},
        {3, is2},
        {10, is3},
        {21, is4},
        {23, is6},
        {25, [](wchar_t c) { return c == L'А'; }},
        {26, [](wchar_t c) { return c == L'М'; }}
    };
    tG[1] = {
        {2, [](wchar_t c) { return c == L'2' || c == L'3' || c == L'4' || c == L'8'; }},
    };
    tG[2] = {};
    tG[3] = {
        {4, [](wchar_t c) { return c == L'1' || c == L'2' || c == L'4' || c == L'5'; }},
        {5, is3}
    };
    tG[4] = {};
    tG[5] = {
        {6, is0},
        {8, is3}
    };
    tG[6] = {
        {7, is8}
    };
    tG[7] = {};
    tG[8] = {
        {9, is0}
    };
    tG[9] = {};
    tG[10] = {
        {11, is1}
    };
    tG[11] = {
        {12, is0},
        {17, is1}
    };
    tG[12] = {
        {13, is2},
        {16, [](wchar_t c) { return c == L'3' || c == L'5'; }}
    };
    tG[13] = {
        {14, is2},
        {15, is9}
    };
    tG[14] = {
        {15, is1}
    };
    tG[15] = {};
    tG[16] = {};
    tG[17] = {
        {18, is0},
        {20, is1}
    };
    tG[18] = {
        {19, is5}
    };
    tG[19] = {};
    tG[20] = {};
    tG[21] = {
        {22, is6}
    };
    tG[22] = {};
    tG[23] = {
        {24, [](wchar_t c) { return c == L'1' || c == L'4' || c == L'7' || c == L'9'; }}
    };
    tG[24] = {};
    tG[25] = {};
    tG[26] = {
        {27, [](wchar_t c) { return c == L'-'; }}
    };
    tG[27] = {
        {28, is1},
        {29, is2},
        {31, is7}
    };
    tG[28] = {};
    tG[29] = {
        {30, is0}
    };
    tG[30] = {};
    tG[31] = {
        {32, is2}
    };
    tG[32] = {};
    tG[33] = { // start
        {34, [](wchar_t c) { return c == L'Г'; }}
    };
    tG[34] = {
        {35, [](wchar_t c) { return c == L'А'; }}
    };
    tG[35] = {
        {36, [](wchar_t c) { return c == L'З'; }}
    };
    tG[36] = {
        {0, [](wchar_t c) { return c == L' '; }}
    };
    FSM<wstring, wchar_t> fsmG(
        37,
        tG,
        { 2, 4, 7, 9, 13, 14, 15, 16, 18, 19, 20, 22, 24, 25, 28, 30, 32 },
        &baseAction,
        nullptr
    );
    vector<wstring> strsG = { L"Siber" };
    for (wstring s : strsG) {
        fsmG = mergeFSM(fsmG, 0, fsmFromWstr(s, &baseAction), 0);
    }


    FSM<wstring, wchar_t>::TransitionMapNull tU;
    tU[0] = {
        {1, is2},
        {26, is3},
        {49, is4},
        {56, [](wchar_t c) {return c == L'C'; }},
        {57, [](wchar_t c) {return c == L'P'; }}
    };
    tU[1] = {
        {2, is2},
        {5, is3},
        {14, is7},
        {22, is9}
    };
    tU[2] = {
        {3, is0}
    };
    tU[3] = {
        {4, is6}
    };
    tU[4] = {};
    tU[5] = {
        {6, is1},
        {8, is6}
    };
    tU[6] = {
        {7, is5}
    };
    tU[7] = {};
    tU[8] = {
        {9, is0},
        {11, [](wchar_t c) {return c == L'2' || c == L'5'; }},
        {12, is3}
    };
    tU[9] = {
        {10, is8}
    };
    tU[10] = {};
    tU[11] = {};
    tU[12] = {
        {13, is2}
    };
    tU[13] = {};
    tU[14] = {
        {15, is4},
        {17, is6},
        {19, is7}
    };
    tU[15] = {
        {16, is6}
    };
    tU[16] = {};
    tU[17] = {
        {18, is0}
    };
    tU[18] = {};
    tU[19] = {
        {20, is2}
    };
    tU[20] = {
        {21, is2}
    };
    tU[21] = {};
    tU[22] = {
        {23, is2}
    };
    tU[23] = {
        {24, is3}
    };
    tU[24] = {
        {25, is2}
    };
    tU[25] = {};
    tU[26] = {
        {27, is1},
        {36, is3},
        {39, is7},
        {42, is9}
    };
    tU[27] = {
        {28, is5},
        {34, is6}
    };
    tU[28] = {
        {29, is0},
        {30, is1},
        {32, is3},
        {33, is9}
    };
    tU[29] = {};
    tU[30] = {
        {31, [](wchar_t c) {return c == L'2' || c == L'4' || c == L'9'; }}
    };
    tU[31] = {};
    tU[32] = {};
    tU[33] = {};
    tU[34] = {
        {35, [](wchar_t c) {return c == L'0' || c == L'2' || c == L'5'; }}
    };
    tU[35] = {};
    tU[36] = {
        {37, is0}
    };
    tU[37] = {
        {38, is3}
    };
    tU[38] = {};
    tU[39] = {
        {40, is4}
    };
    tU[40] = {
        {41, is1}
    };
    tU[41] = {};
    tU[42] = {
        {43, is0},
        {46, is6}
    };
    tU[43] = {
        {44, is9}
    };
    tU[44] = {
        {45, [](wchar_t c) {return c == L'4' || c == L'5'; }}
    };
    tU[45] = {};
    tU[46] = {
        {47, is2}
    };
    tU[47] = {
        {48, is5}
    };
    tU[48] = {};
    tU[49] = {
        {50, is6}
    };
    tU[50] = {
        {51, is9}
    };
    tU[51] = {};
    tU[52] = { // start
        {53, [](wchar_t c) {return c == L'У'; }}
    };
    tU[53] = {
        {54, [](wchar_t c) {return c == L'А'; }}
    };
    tU[54] = {
        {55, [](wchar_t c) {return c == L'З'; }}
    };
    tU[55] = {
        {0, [](wchar_t c) {return c == L' '; }}
    };
    tU[56] = {};
    tU[57] = {};
    FSM<wstring, wchar_t> fsmU(
        58,
        tU,
        { 4, 7, 10, 11, 13, 16, 18, 21, 25, 29, 30, 31, 32, 33, 35, 38, 41, 44, 45, 47, 48, 51 },
        & baseAction,
        nullptr
    );
    fsmU = mergeFSM(fsmU, 0, fsmFromWstr(L"Hunter", &baseAction), 0);
    vector<wstring> strsU1 = { L"argo", L"ombi" };
    for (wstring s : strsU1) {
        fsmU = mergeFSM(fsmU, 56, fsmFromWstr(s, &baseAction), 0);
    }
    vector<wstring> strsU2 = { L"atriot", L"ickup", L"rofi" };
    for (wstring s : strsU2) {
        fsmU = mergeFSM(fsmU, 57, fsmFromWstr(s, &baseAction), 0);
    }


    FSM<wstring, wchar_t>::TransitionMapNull tM;
    tM[0] = {
        {1, is2},
        {7, is3},
        {8, is4},
        {19, [](wchar_t c) {return c == L'5' || c == L'6' || c == L'8'; }}
    };
    tM[1] = {
        {2, is1}
    };
    tM[2] = {
        {3, is3},
        {5, is4}
    };
    tM[3] = {
        {4, [](wchar_t c) {return c == L'6' || c == L'7' || c == L'8'; }}
    };
    tM[4] = {};
    tM[5] = {
        {6, [](wchar_t c) {return c == L'0' || c == L'1' || c == L'2'; }}
    };
    tM[6] = {};
    tM[7] = {};
    tM[8] = {
        {9, is0},
        {11, is1},
        {13, is2},
        {15, is3}
    };
    tM[9] = {
        {10, [](wchar_t c) {return L'0' <= c && c <= L'3' || c == L'7' || c == L'8'; }}
    };
    tM[10] = {};
    tM[11] = {
        {12, [](wchar_t c) {return L'0' <= c && c <= L'2'; }}
    };
    tM[12] = {};
    tM[13] = {
        {14, [](wchar_t c) {return c == L'3' || c == L'4' || c == L'6' || c == L'7'; }}
    };
    tM[14] = {};
    tM[15] = {
        {16, is0},
        {17, is4}
    };
    tM[16] = {};
    tM[17] = {
        {18, [](wchar_t c) {return c == L'П'; }}
    };
    tM[18] = {};
    tM[19] = {};
    tM[20] = { // start
        {21, [](wchar_t c) {return c == L'М'; }}
    };
    tM[21] = {
        {22, [](wchar_t c) {return c == L'о'; }}
    };
    tM[22] = {
        {23, [](wchar_t c) {return c == L'с'; }}
    };
    tM[23] = {
        {24, [](wchar_t c) {return c == L'к'; }}
    };
    tM[24] = {
        {25, [](wchar_t c) {return c == L'в'; }}
    };
    tM[25] = {
        {26, [](wchar_t c) {return c == L'и'; }}
    };
    tM[26] = {
        {27, [](wchar_t c) {return c == L'ч'; }}
    };
    tM[27] = {
        {0, [](wchar_t c) {return c == L' '; }}
    };
    FSM<wstring, wchar_t> fsmM(
        28,
        tM,
        { 4, 6, 7, 10, 12, 14, 16, 18, 19 },
        & baseAction,
        nullptr
    );
    vector<wstring> strsM = { L"Дуэт", L"Иван Калита", L"Князь Владимир", L"Святогор", L"Юрий Долгорукий" };
    for (wstring s : strsM) {
        fsmM = mergeFSM(fsmM, 0, fsmFromWstr(s, &baseAction), 0);
    }


    FSM<wstring, wchar_t>::TransitionMapNull tV;
    tV[0] = {
        {1, is1},
        {5, is2}
    };
    tV[1] = {
        {2, is1}
    };
    tV[2] = {
        {3, is1}
    };
    tV[3] = {
        {4, is1}
    };
    tV[4] = {};
    tV[5] = {
        {6, is1},
        {15, is3}
    };
    tV[6] = {
        {7, is0},
        {9, is1},
        {11, is2},
        {13, is3}
    };
    tV[7] = {
        {8, [](wchar_t c) {return L'0' <= c && c <= L'9'; }}
    };
    tV[8] = {};
    tV[9] = {
        {10, [](wchar_t c) { return L'0' <= c && c <= L'5'; }}
    };
    tV[10] = {};
    tV[11] = {
        {12, [](wchar_t c) {return c == L'0' || c == L'1' || c == L'3' || c == L'9'; }}
    };
    tV[12] = {};
    tV[13] = {
        {14, is1}
    };
    tV[14] = {};
    tV[15] = {
        {16, is2}
    };
    tV[16] = {
        {17, [](wchar_t c) {return c == L'8' || c == L'9'; }}
    };
    tV[17] = {};
    tV[18] = { // start
        {19, [](wchar_t c) {return c == L'В'; }},
        {22, [](wchar_t c) {return c == L'L'; }}
    };
    tV[19] = {
        {20, [](wchar_t c) {return c == L'А'; }}
    };
    tV[20] = {
        {21, [](wchar_t c) {return c == L'З'; }}
    };
    tV[21] = {
        {0, [](wchar_t c) {return c == L' '; }}
    };
    tV[22] = {
        {23, [](wchar_t c) {return c == L'A'; }}
    };
    tV[23] = {
        {24, [](wchar_t c) {return c == L'D'; }}
    };
    tV[24] = {
        {25, [](wchar_t c) {return c == L'A'; }}
    };
    tV[25] = {
        {0, [](wchar_t c) {return c == L' '; }}
    };
    FSM<wstring, wchar_t> fsmV(
        26,
        tV,
        { 4, 8, 10, 12, 14, 17 },
        & baseAction,
        nullptr
    );
    vector<wstring> strsV = { L"Aura", L"EL Lada", L"Granta", L"Iskra", L"Kalina", L"Largus", L"Niva", L"Priora", L"Revolution", L"Vesta", L"XRAY", };
    for (wstring s : strsV) {
        fsmV = mergeFSM(fsmV, 0, fsmFromWstr(s, &baseAction), 0);
    }


    
    function<void(wchar_t)> harv_log([](wchar_t c) {
        successStr.push_back(c);
        failureStr.erase(0, 1);
        });

    FSM<wstring, wchar_t> fsmAll = mergeFSM(
        mergeFSM(fsmG, 33, fsmU, 52), 
        33, 
        mergeFSM(fsmM, 20, fsmV, 18), 
        20,
        harv_log
    );

    fsmAll.genElemsOfConditions(&alphabet);

    userIO(fsmAll);

    return 0;
}



#include "Header.h"
#include "GrayCode.h"
#include "Mult.h"
#include "Operations.h"

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isN(const string& str) {
    if (str.empty()) { return false; }

    size_t strt = 0;
    if (str[0] == '-' || str[0] == '+') {
        strt = 1;
    }

    for (size_t i = strt; i < str.length(); ++i) {
        if (not isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

Mult creating_universum() {
    string Answer;
    cout << "Введите разрядность universum (не больше " << max_rank << "): ";
    while (true) {
        cin >> Answer;
        if (isN(Answer)) {
            if (stoi(Answer) < 0) {
                cout << "Разрядность не может быть отрицательной, введите разрядность заново: ";
                clearInputBuffer();
            }
            else {
                if (stoi(Answer) > max_rank) {
                    cout << "Разрядность не может быть больше " << max_rank <<", введите разрядность заново : ";
                    clearInputBuffer();
                }
                else {
                    Mult un(stoi(Answer));
                    cout << "Universum с разрядностью " << Answer << ": \n";
                    un.Pr_mult("Universum");
                    return un;
                }
                
            }
        }
        else {
            cout << "Недопустимый ввод, введите число заново: ";
            clearInputBuffer();
        }
    }
}

Mult creating_mult(string S, Mult* parent) {
    if (parent->Get_Power() == 0) {
        cout << "Так как Universum имеет разрядность 0, то его подмножество автоматически создано пустым." << endl;
        Mult M(parent, true, 0);
        M.Pr_mult(S);
        return M;
    }
    cout << "Выберите каким способом будет сформировано множество " << S << ":\n";
    cout << "\t(1) - Заполнение вручную\n\t(2) - Автоматическое заполнение\n\t(e) - Выход\n";
    string Answer;
    while (true) {
        cin >> Answer;
        if (Answer == "e" or Answer == "1" or Answer == "2") {
            if (Answer == "e") { exit(0); }
            if (Answer == "1") {
                Mult M(parent, false);
                M.Pr_mult(S);
                return M;
            }
            if (Answer == "2") {
                cout << "Для автоматического заполнения множества введите его мощность (она не может быть больше " << parent->Get_Power() << "): ";
                while (true) {
                    cin >> Answer;
                    if (isN(Answer)) {
                        if (stoi(Answer) < 0) {
                            cout << "Мощность не может быть отрицательной, введите мощность заново : ";
                            clearInputBuffer();
                        }
                        else {
                            if (stoi(Answer) > parent->Get_Power()) {
                                cout << "Мощность не может быть больше мощности Universum, введите мощность заново: ";
                                clearInputBuffer();
                            }
                            else {
                                Mult M(parent, true, stoi(Answer));
                                M.Pr_mult(S);
                                return M;
                            }
                        }
                    }
                    else {
                        cout << "Недопустимый ввод, введите число заново: ";
                        clearInputBuffer();
                    }
                }
            }
        }
        else {
            cout << "Недопустимый ввод, введите ответ заново: ";
            clearInputBuffer();
        }
    }
}

Mult start() {
    while (true) {
        cout << "Выберите действие:" << "\n\t(1) - Ввести разрядность Universum\n\t(e) - Выход из программы" << endl;
        string Answer;
        while (true) {
            cin >> Answer;
            if (Answer == "e" or Answer == "1") {
                if (Answer == "e") { exit(0); }
                if (Answer == "1") {
                    return creating_universum();
                }
            }
            else {
                cout << "Недопустимый ввод, введите ответ заново: ";
                clearInputBuffer();
            }
        }
    }
}

void Oper(Mult& Universum, Mult& A, Mult& B) {
    cout << "Создан Universum и его подмножества:\n";
    Universum.Pr_mult("Universum");
    A.Pr_mult("A");
    B.Pr_mult("B");

    cout << "Выберите операцию, которую хотите провести:";
    cout << "\n\t(1) - Объединение A и B\n\t(2) - Пересечение A и B\n\t(3) - Разность A и B";
    cout << "\n\t(4) - Разность B и A\n\t(5) - Симметрическая разность A и B";
    cout << "\n\t(6) - Дополнение A\n\t(7) - Дополнение B";
    cout << "\n\t(8) - Арифметическая сумма A и B\n\t(9) - Арифметическая разность A и B\n\t(10) - Арифметическая разность B и A";
    cout << "\n\t(11) - Арифметическое произведение A и B\n\t(12) - Арифметическое деление A и B\n\t(13) - Арифметическое деление B и A";
    cout << "\n\t(14) - Все операции между A и B\n\t(e) - Выход из программы\n";
    string Answer;
    while (true) {
        cin >> Answer;
        if (Answer == "e") {
            exit(0);
        }
        else {
            if (isN(Answer) and stoi(Answer) <= 14 and stoi(Answer) >= 1) {
                int int_Answer = stoi(Answer);
                switch (int_Answer)
                {
                case 1:
                    Union(A, B).Pr_mult("Объединение A и B");
                    break;
                case 2:
                    Intersect(A, B).Pr_mult("Пересечение A и B");
                    break;
                case 3:
                    Difference(A, B).Pr_mult("Разность A и B");
                    break;
                case 4:
                    Difference(B, A).Pr_mult("Разность B и A");
                    break;
                case 5:
                    Simmetric_Difference(A, B).Pr_mult("Симметрическая разность A и B");
                    break;
                case 6:
                    Addition(A).Pr_mult("Дополнение A");
                    break;
                case 7:
                    Addition(B).Pr_mult("Дополнение B");
                    break;
                case 8:
                    Arithmetic_Sum(A, B).Pr_mult("Арифметическая сумма A и B");
                    break;
                case 9:
                    Arithmetic_Difference(A, B).Pr_mult("Арифметическая разность A и B");
                    break;
                case 10:
                    Arithmetic_Difference(B, A).Pr_mult("Арифметическая разность B и A");
                    break;
                case 11:
                    Arithmetic_Product(A, B).Pr_mult("Арифметическое произведение A и B");
                    break;
                case 12:
                    Arithmetic_Division(A, B).Pr_mult("Арифметическое деление A и B");
                    break;
                case 13:
                    Arithmetic_Division(B, A).Pr_mult("Арифметическое деление B и A");
                    break;
                case 14:
                    All_operations(A, B);
                    break;
                }
                break;
            }
            else {
                cout << "Недопустимый ввод, введите ответ заново: ";
                clearInputBuffer();
            }
        }
    }
    cout << "Выберите действие:\n\t(1) - Выбрать операцию\n\t(2) - Начать заново\n\t(e) - Выход из программы\n";
    while (true) {
        cin >> Answer;
        if (Answer == "1" or Answer == "2" or Answer == "e") {
            if (Answer == "1") {
                Oper(Universum, A, B);
            }
            if (Answer == "2") {
                break;
            }
            if (Answer == "e") {
                exit(0);
            }
        }
        else {
            cout << "Недопустимый ввод, введите ответ заново: ";
            clearInputBuffer();
        }
    }
    
}



int main() {
    srand(time(0));
    setlocale(LC_ALL, "Russian");

    while (true) {
        Mult Universum = start();
        Mult A = creating_mult("A", &Universum);
        Mult B = creating_mult("B", &Universum);
        Oper(Universum, A, B);
    }
    


}
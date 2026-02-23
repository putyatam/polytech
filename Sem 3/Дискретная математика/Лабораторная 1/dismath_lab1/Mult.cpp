#include "Header.h"
#include "Mult.h"
#include "GrayCode.h"

Mult::Mult(int n, bool rndm) {
	Parent_mult = nullptr;
	Amount = n;
	Codes = GrayCode(Amount);
	if (n == 0) {
		Power = 0;
		for (int i = 0; i < (1 << Amount); i++) {
			Values.push_back(0);
		}
		return;
	}
	int r;
	if (rndm) {
		for (int i = 0; i < (1 << Amount); i++) {
			r = rand() % max_value + 1;
			Power += r;
			Values.push_back(r);
		}
	}
}

Mult::Mult(const Mult* parent, bool rndm, unsigned int pwr) {
	Parent_mult = parent;
	Amount = Parent_mult->Get_Amount();
	Codes = Parent_mult->Get_Codes();
	Power = 0;
	if (pwr == 0 and rndm) {
		Power = 0;
		for (int i = 0; i < (1 << Amount); i++) {
			Values.push_back(0);
		}
		return;
	}
	vector<int> Parent_Values = Parent_mult->Get_Values();
	
	if (rndm) {
		int r;
		for (int i = 0; i < (1 << Amount); i++) {
			r = rand() % parent->Get_Values()[i];
			Values.push_back(r);
			Power += r;
		}

		if (Power == pwr) {
			return;
		}

		if (Power > pwr) {
			while (true) {
				for (int i = 0; i < (1 << Amount); i++) {
					if (Values[i] > 0) {
						Values[i]--;
						Power--;
					}
					if (Power == pwr) {
						return;
					}

				}
				if (Power == pwr) {
					return;
				}
			}
		}
		
		if (Power < pwr) {
			while (true) {
				for (int i = 0; i < (1 << Amount); i++) {
					if (Values[i] < parent->Get_Values()[i]) {
						Values[i]++;
						Power++;
					}
					if (Power == pwr) {
						return;
					}

				}
				if (Power == pwr) {
					return;
				}
			}
		}
	}
	else {
		string Answer;
		cout << "Выбран ввод вручную. Далее в скобках после каждого эалемента будет указываться его кратность в Universum." << endl;
		for (int i = 0; i < (1 << Amount); i++) {
			cout << "Введите кратность элемента " << Parent_mult->Get_Codes()[i] << "[" << Parent_mult->Get_Values()[i] << "]: ";
			while (true) {
				cin >> Answer;
				if (isN(Answer)) {
					if (stoi(Answer) < 0) {
						cout << "Кратность не может быть отрицательной, введите число заново: ";
						clearInputBuffer();
					}
					else {
						if (stoi(Answer) > Parent_mult->Get_Values()[i]) {
							cout << "Кратность не может быть больше " << Parent_mult->Get_Values()[i] << ", введите число заново: ";
							clearInputBuffer();
						}
						else {
							Values.push_back(stoi(Answer));
							Power += stoi(Answer);
							break;
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
}

Mult::Mult(const Mult* parent, vector<int>& v) {
	Parent_mult = parent;
	Codes = Parent_mult->Get_Codes();
	Amount = Parent_mult->Get_Amount();
	Values = v;
	Power = 0;
	for (auto iter : Values) {
		Power += iter;
	}
}



void Mult::Pr_elem(int i) const {
	cout << Codes[i] << " - " << Values[i];
}

void Mult::Pr_mult(string str) const {
	cout << "\tМножество " << str << endl;
	if (Power == 0) {
		cout << "Пустое множество" << endl << endl;
		return;
	}
	for (int i = 0; i < (1 << Amount); i++) {
		Pr_elem(i);
		cout << endl;
	}
	cout << "Мощность множества: " << Power << endl << endl;
}

const vector<string>& Mult::Get_Codes() const {
	return Codes;
}
const vector<int>& Mult::Get_Values() const {
	return Values;
}
unsigned int Mult::Get_Amount() const {
	return Amount;
}

const Mult* Mult::Get_Parent() const {
	return Parent_mult;
}

unsigned int Mult::Get_Power() const {
	return Power;
}

int Mult::operator[](int i) const {
	return Values[i];
}
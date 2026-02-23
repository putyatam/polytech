#include "Operations.h"

Mult Union(const Mult& m1, const Mult& m2) {
	vector<int> result;
	for (int i = 0; i < (1 << m1.Get_Amount()); i++) {
		result.push_back(max(m1[i], m2[i]));
	}
	return Mult(m1.Get_Parent(), result);
}

Mult Intersect(const Mult& m1, const Mult& m2) {
	vector<int> result;
	for (int i = 0; i < (1 << m1.Get_Amount()); i++) {
		result.push_back(min(m1[i], m2[i]));
	}
	return Mult(m1.Get_Parent(), result);
}

Mult Difference(const Mult& m1, const Mult& m2) {
	return Intersect(m1, Addition(m2));
}

Mult Simmetric_Difference(const Mult& m1, const Mult& m2) {
	return Difference(Union(m1, m2), Intersect(m1, m2));
}

Mult Addition(const Mult& m) {
	vector<int> result;
	for (int i = 0; i < (1 << m.Get_Amount()); i++) {
		result.push_back(m.Get_Parent()->Get_Values()[i] - m[i]);
	}
	return Mult(m.Get_Parent(), result);
}



Mult Arithmetic_Sum(const Mult& m1, const Mult& m2) {
	vector<int> result;
	int r;
	for (int i = 0; i < (1 << m1.Get_Amount()); i++) {
		r = m1[i] + m2[i];
		if (r > (m1.Get_Parent()->Get_Values()[i])) {
			result.push_back(m1.Get_Parent()->Get_Values()[i]);
		}
		else {
			result.push_back(r);
		}
	}
	return Mult(m1.Get_Parent(), result);
}

Mult Arithmetic_Difference(const Mult& m1, const Mult& m2) {
	vector<int> result;
	for (int i = 0; i < (1 << m1.Get_Amount()); i++) {
		if (m1[i] > m2[i]) {
			result.push_back(m1[i] - m2[i]);
		}
		else {
			result.push_back(0);
		}
	}
	return Mult(m1.Get_Parent(), result);
}

Mult Arithmetic_Product(const Mult& m1, const Mult& m2) {
	vector<int> result;
	int r;
	for (int i = 0; i < (1 << m1.Get_Amount()); i++) {
		r = m1[i] * m2[i];
		if (r > (m1.Get_Parent()->Get_Values()[i])) {
			result.push_back(m1.Get_Parent()->Get_Values()[i]);
		}
		else {
			result.push_back(r);
		}
	}
	return Mult(m1.Get_Parent(), result);
}

Mult Arithmetic_Division(const Mult& m1, const Mult& m2) {
	vector<int> result;
	int r;
	for (int i = 0; i < (1 << m1.Get_Amount()); i++) {
		if (m2[i] == 0) {
			result.push_back(0);
		}
		else {
			r = m1[i] / m2[i];
			result.push_back(r);
		}
	}
	return Mult(m1.Get_Parent(), result);
}


void All_operations(const Mult& A, const Mult& B) {
	Union(A, B).Pr_mult("Объединение A и B");
	Intersect(A, B).Pr_mult("Пересечение A и B");
	Difference(A, B).Pr_mult("Разность A и B");
	Difference(B, A).Pr_mult("Разность B и A");
	Simmetric_Difference(A, B).Pr_mult("Симметрическая разность A и B");
	Addition(A).Pr_mult("Дополнение A");
	Addition(B).Pr_mult("Дополнение B");
	Arithmetic_Sum(A, B).Pr_mult("Арифметиечская сумма A и B");
	Arithmetic_Difference(A, B).Pr_mult("Арифметическая разность A и B");
	Arithmetic_Difference(B, A).Pr_mult("Арифметическая разность B и A");
	Arithmetic_Product(A, B).Pr_mult("Арифметическое произведение A и B");
	Arithmetic_Division(A, B).Pr_mult("Арифметическое деление A и B");
	Arithmetic_Division(B, A).Pr_mult("Арифметическое деление B и A");
}

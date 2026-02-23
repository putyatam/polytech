#include "number.h"
#include <QVector>

QVector<QVector<QChar>> createSum(const QVector<QChar>& symbols) {
	int size = symbols.size();
	QVector<QVector<QChar>> table(size, QVector<QChar>(size));

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			int sumIndex = (i + j) % size;
			table[i][j] = symbols[sumIndex];
		}
	}
	return table;
}

QVector<QVector<QChar>> createSumTr(const QVector<QChar>& symbols) {
	int size = symbols.size();
	QVector<QVector<QChar>> table(size, QVector<QChar>(size));

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			int carryIndex = (i + j) / size;
			table[i][j] = vec_alphabet[carryIndex];
		}
	}
	return table;
}

QVector<QVector<QChar>> createMul(const QVector<QChar>& symbols) {
	int size = symbols.size();
	QVector<QVector<QChar>> table(size, QVector<QChar>(size));

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			int productIndex = (i * j) % size;
			table[i][j] = symbols[productIndex];
		}
	}
	return table;
}

QVector<QVector<QChar>> createMulTr(const QVector<QChar>& symbols) {
	int size = symbols.size();
	QVector<QVector<QChar>> table(size, QVector<QChar>(size));
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			int carryIndex = (i * j) / size;
			table[i][j] = vec_alphabet[carryIndex];
		}
	}
	return table;
}

void update_tables() {
	sum_value = createSum(vec_alphabet);
	sum_tr = createSumTr(vec_alphabet);
	mul_value = createMul(vec_alphabet);
	mul_tr = createMulTr(vec_alphabet);
}

QVector<QChar> vec_alphabet = {'0','1','2','3','4','5','6','7','8','9'};
int size_of_number = 8;
QVector<QVector<QChar>> sum_value = createSum(vec_alphabet);
QVector<QVector<QChar>> sum_tr = createSumTr(vec_alphabet);
QVector<QVector<QChar>> mul_value = createMul(vec_alphabet);
QVector<QVector<QChar>> mul_tr = createMulTr(vec_alphabet);


Number border_abs(QString(size_of_number, vec_alphabet.last()));
bool border_flag = false;

Number::Number(QString new_n) {
	if (new_n == NULL) {
		new_n = QString(size_of_number + 1, vec_alphabet.first());
	}
	isminus = new_n.contains("-");
	n = new_n.remove("-");
	n = new_n.remove("+");
	if (new_n.size() > size_of_number + 1) {
		new_n.remove(0, new_n.size() - size_of_number - 1);
	}
	else {
		for (int i = 0; size_of_number + 1 - n.size(); i++) {
			n.push_front(vec_alphabet[0]);
		}
	}
}

bool Number::get_isminus() const {
	return isminus;
}
QString Number::get_n() const {
	return n;
}

QDebug operator<<(QDebug debug, const Number& number) {
	debug << (number.isminus ? "-" + number.n : number.n);
	return debug;
}

bool Number::operator<(Number other) {
	for (int i = 0; i < size_of_number + 1; i++) {
		if (vec_alphabet.indexOf(n[i]) < vec_alphabet.indexOf(other.n[i])) {
			return true;
		}
		if (vec_alphabet.indexOf(n[i]) > vec_alphabet.indexOf(other.n[i])) {
			return false;
		}
	}
	return false;
}

Number Number::operator+(Number other) {
	if (not isminus and other.isminus) {
		Number number_left(this->n);
		Number number_right(other.n);
		return number_left - number_right;
	}
	if (isminus and not other.isminus) {
		Number number_left(other.n);
		Number number_right(this->n);
		return number_left - number_right;
	}
	QString right = other.n;
	QString result;
	QChar transfer = vec_alphabet.first();
	for (int i = size_of_number; i >= 0; i--) {
		result.push_front(get_sum_value(get_sum_value(n[i], right[i]), transfer));
		transfer = get_sum_value(get_sum_tr(n[i], right[i]), get_sum_tr(get_sum_value(n[i], right[i]), transfer));
	}
	Number n_result(isminus and other.isminus ? "-" + result : result);
	if (border_abs < n_result) {
		border_flag = true;
		return Number();
	}
	return n_result;
}

Number Number::operator-(Number other) {
	if (not isminus and other.isminus) {
		Number number_left(this->n);
		Number number_right(other.n);
		return number_left + number_right;
	}
	if (isminus and not other.isminus) {
		Number number_left(this->n);
		Number number_right("-" + other.n);
		return number_left + number_right;
	}
	QString left, right;
	if (*this < other) {
		left = other.n;
		right = n;
	}
	else {
		left = n;
		right = other.n;
	}
	
	QString result;
	QString borrow = vec_alphabet.first();
	for (int i = size_of_number; i >= 0; i--) {
		result.push_front(get_dif_value(left[i], right[i]));
		if (i != 0) {
			borrow.push_front(compare_less(left[i], right[i]) ? vec_alphabet[1] : vec_alphabet.first());
		}
	}
	Number n_result(result);
	Number n_borrow(borrow);
	if (n_borrow.n != QString(size_of_number + 1, vec_alphabet.first())) {
		n_result = n_result - n_borrow;
	}
	if (border_abs < n_result) {
		border_flag = true;
		return Number();
	}
	return (*this < other) != (isminus and other.isminus) ? Number("-" + n_result.n) : n_result;
}

Number Number::operator*(Number other) {
	Number n_result;
	Number temp;
	for (int i = size_of_number; i >= 0; i--) {
		for (int j = size_of_number; j >= 0; j--) {
			temp = Number(QString(get_mul_tr(this->n[i], other.n[j])) + QString(get_mul_value(this->n[i], other.n[j])) + QString(2 * size_of_number - i - j, vec_alphabet.first()));
			n_result = n_result + temp;
		}
	}
	n_result = isminus != other.isminus ? Number("-" + n_result.n) : Number(n_result.n);
	return n_result;
}

QPair<Number, Number> Number::operator/(Number other) {
	if (other.n == QString(size_of_number + 1, vec_alphabet.first())) {
		if (this->n == QString(size_of_number + 1, vec_alphabet.first())) {
			return qMakePair(Number(QStringLiteral("range")), Number());
		}
		return qMakePair(Number(QStringLiteral("empty")), Number());
	}
	Number res, mod;
	Number number_right(other.n);
	Number other_abs(other.n);
	while (number_right < *this or number_right.n == this->n) {
		number_right = number_right + other_abs;
		++res;
	}
	if (this->isminus) {
		++res;
	}
	res = other.isminus != this->isminus ? Number("-" + res.n) : res;

	mod = *this - (res * other);
	if (other.n == mod.n) {
		res = res - Number(vec_alphabet[1]);
		mod = Number();
	}
	return qMakePair(res, mod);
}

void Number::operator++() {
	Number delta(vec_alphabet[1]);
	*this = *this + delta;
}


QChar get_sum_value(QChar digit1, QChar digit2) {
	return sum_value[vec_alphabet.indexOf(digit1)][vec_alphabet.indexOf(digit2)];
}
QChar get_sum_tr(QChar digit1, QChar digit2) {
	return sum_tr[vec_alphabet.indexOf(digit1)][vec_alphabet.indexOf(digit2)];
}
QChar get_mul_value(QChar digit1, QChar digit2) {
	return mul_value[vec_alphabet.indexOf(digit1)][vec_alphabet.indexOf(digit2)];
}
QChar get_mul_tr(QChar digit1, QChar digit2) {
	return mul_tr[vec_alphabet.indexOf(digit1)][vec_alphabet.indexOf(digit2)];
}
QChar get_dif_value(QChar digit1, QChar digit2) {
	return vec_alphabet[sum_value[vec_alphabet.indexOf(digit2)].indexOf(digit1)];
}
QChar get_dif_tr(QChar digit1, QChar digit2) {
	return sum_tr[vec_alphabet.indexOf(digit1)][vec_alphabet.indexOf(digit2)];
}

bool compare_less(QChar digit1, QChar digit2) {
	return vec_alphabet.indexOf(digit1) < vec_alphabet.indexOf(digit2);
}

QString Number::harvest() {
	int index = 0;
	if (n == QString(size_of_number+1, vec_alphabet.first())) {
		return vec_alphabet.first();
	}
	while (index < n.length() && n[index] == vec_alphabet.first()) {
		index++;
	}
	return isminus ? "-" + n.mid(index) : n.mid(index);
}
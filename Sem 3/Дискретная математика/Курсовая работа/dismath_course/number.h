#pragma once
#include <QString>
#include <QDebug>
#include <QPair>
#include <QMessageBox>
class Number {
private:
	bool isminus;
	QString n;
public:
	Number(QString new_n = NULL);
	bool get_isminus() const;
	QString get_n() const;
	friend QDebug operator<<(QDebug debug, const Number& number);
	bool operator<(Number);
	bool operator>=(Number);
	Number operator+(Number);
	Number operator-(Number);
	Number operator*(Number);
	QPair<Number, Number> operator/(Number);
	void operator++();
	QString harvest();
};

QChar get_sum_value(QChar digit1, QChar digit2);
QChar get_sum_tr(QChar digit1, QChar digit2);

QChar get_mul_value(QChar digit1, QChar digit2);
QChar get_mul_tr(QChar digit1, QChar digit2);

QChar get_dif_value(QChar digit1, QChar digit2);
QChar get_dif_tr(QChar digit1, QChar digit2);

bool compare_less(QChar digit1, QChar digit2);

void update_tables();
QVector<QVector<QChar>> createSum(const QVector<QChar>&);
QVector<QVector<QChar>> createSumTr(const QVector<QChar>&);
QVector<QVector<QChar>> createMul(const QVector<QChar>&);
QVector<QVector<QChar>> createMulTr(const QVector<QChar>&);

extern QVector<QChar> vec_alphabet;
extern QVector<QVector<QChar>> sum_value;
extern QVector<QVector<QChar>> sum_tr;
extern QVector<QVector<QChar>> mul_value;
extern QVector<QVector<QChar>> mul_tr;
extern int size_of_number;
extern bool border_flag;
extern Number border_abs;
#pragma once
#include <QWidget>
#include <QObject>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "number.h"

class Calculator : public QWidget {
	QLineEdit* line1;
	QLineEdit* line2;
	QComboBox* operations;
	QLabel* result;
	QLabel* modulo;
	QLabel* text_mod;
	QPushButton* calculate_button;
	QLabel* text_digits;
	QLabel* text_alphabet;
	QPushButton* apply_button;
	QSpinBox* spin_digits;
	QLineEdit* line_alphabet;
public:
	Calculator(QWidget* parent = nullptr);
	void check_input();
	void calculate();
	void apply();
};

void debug_number_to_normal(Number n1, Number n2, Number res, QString sign);
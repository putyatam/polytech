#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include <limits>
#include <regex>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>
#include <QRegularExpression>
#include <QDebug>

using namespace std;

//bool sint_tree(bool x, bool y, bool z, bool k);

QVector<int> vector_f = { 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1 };
QVector<int> vector_x = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 };
QVector<int> vector_y = { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1 };
QVector<int> vector_z = { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
QVector<int> vector_k = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };


struct node {
	node* f;
	node* t;
	bool value;
	int type;

	node(int new_type = 4, bool new_value = false);
	node* get_next(bool flag);
};

node* createBDR();

int value_by_bdr(node* core, QVector<int> values);
void table_view_with_all(node* core);

void table_view();

QString getSDNF();

QString getSKNF();

QVector<QVector<int>> getTriangle();

QVector<int> getUserInput();
QString getSingleInput();
void all_values(node* core, QVector<int> values);
int value_by_sdnf(QVector<int> vec);
QString getPJ();
int value_by_pj(QVector<int> values);
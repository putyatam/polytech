#pragma once
#include "Page.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QRegExp>
#include <QLineEdit>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>
#include <QApplication>

const QStringList Headers = { QStringLiteral("Фамилия"), QStringLiteral("Имя"), QStringLiteral("Отчество"), QStringLiteral("Адрес"),
	QStringLiteral("Дата рождения"), QStringLiteral("E-mail"), QStringLiteral("Номера телефонов") };

class Book : public QObject{
	Q_OBJECT;
private:
	QVector<Page*> Pages;
	QTableWidget* Table;
	int width;
public:
	Book(QWidget* parent_);
	void add_Page(Page* new_page, bool Flag = true);

	QTableWidget* get_Table();
	int get_Width();
	QVector<Page*> getPages();
	void clear();

public slots:
	void add_Row();
	void check_item(QTableWidgetItem* item);
	void update();
	void delete_row();
	void confirmExit(QApplication* app, QString Name);
};

void writeBookToJsonFile(Book* book, const QString& fileName);
bool readBookFromJsonFile(Book* book, const QString& fileName);

QString getSaveFilePath(QWidget* parent = nullptr);
QString getOpenFilePath(QWidget* parent = nullptr);

void setWidgetsEnabled(QGridLayout* layout);
void set_numbers_const(QTableWidget* Table);
void disableWidgetsInLastColumn(QTableWidget* tableWidget);
QTableWidget* search(Book* book, int index_of_combo, const QString& request);
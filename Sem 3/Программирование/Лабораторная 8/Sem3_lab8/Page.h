#pragma once

#include <QString>
#include <QDate>
#include <QVector>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QlineEdit>
#include <QComboBox>
#include <QPushButton>

class Book;

class Page : public QWidget {
	Q_OBJECT;
private:
	QString Surname;
	QString Name;
	QString Patronymic;
	QString Address;
	QDate DOB;
	QString Email;
	QVector<QString> Phone_Numbers;
	QVector<int> Phone_types;

public:
	Page();
	Page(QString surname_, QString name_, QString patronymic_, QString address_, QDate dob_, QString email_, QVector<QString> phone_numbers_, QVector<int> phone_types_);
	QString getSurname() const;
	QString getName() const;
	QString getPatronymic() const;
	QString getAddress() const;
	QDate getDOB() const;
	QString getEmail() const;
	QVector<QString> getPhone_Numbers() const;
	QVector<int> getPhone_types();
	QVector<QString> getPhone_types_str();
	QStringList getAll_Data_Strings() const;
	QGridLayout* getPhones_Layout();

	void setSurname(QString surname_);
	void setName(QString name_);
	void setPatronymic(QString patronymic_);
	void setAddress(QString address_);
	void setDOB(QDate dob_);
	void setEmail(QString email_);
	void setPhone_Numbers(QVector<QString> phone_numbers_);
	void setPhone_types(QVector<int> new_phone_types);

	void check_line(QLineEdit* line, int n);
	void check_combo(int n, int index);

signals:
	void numbers_updated();
public slots:
	void add_row_to_numbers();
	
};
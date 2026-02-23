#include "Page.h"
#include "Book.h"

Page::Page() {
	
}

Page::Page(QString surname_, QString name_, QString patronymic_, QString address_, QDate dob_, QString email_, QVector<QString> phone_numbers_, QVector<int> phone_types_) {
	Surname = surname_;
	Name = name_;
	Patronymic = patronymic_;
	Address = address_;
	DOB = dob_;
	Email = email_;
	Phone_Numbers = phone_numbers_;
	Phone_types = phone_types_;
}

QString Page::getSurname() const { return Surname; }
QString Page::getName() const { return Name; }
QString Page::getPatronymic() const { return Patronymic; }
QString Page::getAddress() const { return Address; }
QDate Page::getDOB() const { return DOB; }
QString Page::getEmail() const { return Email; }
QVector<QString> Page::getPhone_Numbers() const { return Phone_Numbers; }
QVector<int> Page::getPhone_types() { return Phone_types; }

QStringList Page::getAll_Data_Strings() const{
	QStringList Out;
	Out << (Surname.isEmpty() ? QString(QStringLiteral("Не задано")) : Surname);
	Out << (Name.isEmpty() ? QString(QStringLiteral("Не задано")) : Name);
	Out << (Patronymic.isEmpty() ? QString(QStringLiteral("Не задано")) : Patronymic);
	Out << (Address.isEmpty() ? QString(QStringLiteral("Не задано")) : Address);
	Out << (DOB.isNull() ? QString(QStringLiteral("Не задано")) : DOB.toString("dd.MM.yyyy"));
	Out << (Email.isEmpty() ? QString(QStringLiteral("Не задано")) : Email);
	return Out;
}

QGridLayout* Page::getPhones_Layout() {
	QGridLayout* layout_phones = new QGridLayout();
	QStringList str_types = { QStringLiteral("Домашний"), QStringLiteral("Рабочий") , QStringLiteral("Личный") };
	for (int i = 0; i < Phone_Numbers.size(); i++) {

		QLineEdit* line = new QLineEdit();
		QComboBox* types = new QComboBox();

		line->setText(Phone_Numbers[i]);
		types->addItems(str_types);
		types->setCurrentIndex(Phone_types[i]);

		layout_phones->addWidget(line, i, 0);
		QObject::connect(line, &QLineEdit::editingFinished, [line, i, this]() {
			check_line(line, i);
			});
		layout_phones->addWidget(types, i, 1);
		QObject::connect(types, QOverload<int>::of(&QComboBox::currentIndexChanged), [i, this](int index) {
			check_combo(i, index);
			});
	}
	QPushButton* add_number_button = new QPushButton(QStringLiteral("Добавить номер"));
	layout_phones->addWidget(add_number_button);
	QObject::connect(add_number_button, &QPushButton::clicked, this, &Page::add_row_to_numbers);
	return layout_phones;
}


void Page::setSurname(QString surname_) { Surname = surname_; }
void Page::setName(QString name_) { Name = name_; }
void Page::setPatronymic(QString patronymic_) { Patronymic = patronymic_; }
void Page::setAddress(QString address_) { Address = address_; }
void Page::setDOB(QDate dob_) { DOB = dob_; }
void Page::setEmail(QString email_) { Email = email_; }
void Page::setPhone_Numbers(QVector<QString> phone_numbers_) { Phone_Numbers = phone_numbers_; }


void Page::add_row_to_numbers() {
	Phone_Numbers.push_back(QStringLiteral("Не задано")); 
	Phone_types.push_back(0);
	emit numbers_updated();
}

void Page::check_line(QLineEdit* line, int n) {
	QString input = line->text().trimmed();
	if (line->text() == QString(QStringLiteral("Не задано"))) {
		return;
	}
	QRegExp regex("\\+?[ ]?[0-9][0-9]?[0-9]?[ ]?(\\(([0-9]{3}\\))|([0-9]{3}))[ ]?[0-9]{3}(([0-9]{4})|([-][0-9]{2}[ -]?[0-9]{2})|( [0-9]{2} [0-9]{2}))");
	bool isValid = regex.exactMatch(input);
	if (not isValid and line->text() != QString(QStringLiteral("Не задано"))) {
		line->setText(QStringLiteral("Не задано"));
		Phone_Numbers[n] = QStringLiteral("Не задано");
		QMessageBox::warning(this, QStringLiteral("Некорректные данные"),QStringLiteral("Не верный номер телефона. Номер телефона должен быть написан по одному из следующих образцов: +7 812 1234567, 8(800)123-1212, + 38 (032) 123 11 11. Строка автоматически заменена на \"Не задано\""));


	}
	else {
		QString number_removed = line->text().remove("+").remove(" ").remove("-").remove("(").remove(")");

		QString lasts = number_removed.right(10);
		QString firsts = number_removed.left(number_removed.size() - 10);
		QString out = firsts + " (" + lasts.left(3) + ") " + lasts.mid(3, 3) + "-" + lasts.mid(6, 2) + "-" + lasts.mid(8, 2);
		if (line->text().startsWith("+")) {
			out = "+" + out;
		}
		line->setText(out);
		Phone_Numbers[n] = out;
	}
}

void Page::check_combo(int n, int index) {
	Phone_types[n] = index;
}

QVector<QString> Page::getPhone_types_str() {
	QVector<QString> out;
	for (auto i : Phone_types) {
		out.push_back(QString::number(i));
	}
	return out;
}

void Page::setPhone_types(QVector<int> new_phone_types) { Phone_types = new_phone_types; }

//bool Page::check_const(QTableWidget* table) {
//	if (table->editTriggers() == QAbstractItemView::NoEditTriggers) {
//		return 
//	}
//}
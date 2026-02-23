#include "Book.h"

int width_column = 148;

Book::Book(QWidget* parent_) {
	Table = new QTableWidget(0, Headers.size(), parent_);
	Table->setHorizontalHeaderLabels(Headers);
	Table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	Table->resizeColumnsToContents();
	width = 0;
	for (int i = 0; i < Headers.size(); i++) {
		if (Table->columnWidth(i) < width_column) {
			Table->setColumnWidth(i, width_column);
		}
		width += Table->columnWidth(i);
	}

	QObject::connect(Table, &QTableWidget::itemChanged, this, &Book::check_item);
}

void Book::add_Page(Page* new_page, bool Flag) {
	if (Flag) {
		Pages.push_back(new_page);
	}
	int new_Row = Table->rowCount();
	Table->insertRow(new_Row);
	QStringList Data_List = new_page->getAll_Data_Strings();
	for (int i = 0; i < Data_List.size(); i++) {
		Table->setItem(new_Row, i, new QTableWidgetItem(Data_List[i]));
		Table->resizeRowToContents(new_Row);
	}
	QGridLayout* layout_of_phones_item = new_page->getPhones_Layout();
	QWidget* phones_item = new QWidget();
	phones_item->setMinimumWidth(200);
	phones_item->setLayout(layout_of_phones_item);
	Table->setCellWidget(new_Row, Headers.size() - 1, phones_item);
	Table->resizeColumnToContents(Headers.size() - 1);
	Table->resizeRowToContents(new_Row);
	QObject::connect(new_page, &Page::numbers_updated, this, &Book::update);
}

QTableWidget* Book::get_Table() { return Table; }
int Book::get_Width() { return width; }
QVector<Page*> Book::getPages() { return Pages; }
void Book::add_Row() { this->add_Page(new Page()); }


void Book::check_item(QTableWidgetItem* item) {
	switch (item->column())
	{
	case 0:
	case 1:
	case 2:
	{
		QString input = item->text().trimmed();
		QRegExp regex(QStringLiteral("^[A-ZА-ЯЁ][a-zа-яёA-ZА-ЯЁ]*[ -]?[0-9]*([ -]?[a-zа-яёA-ZА-ЯЁ]*[ -]?[0-9]*)*"));
		bool isValid = regex.exactMatch(input);
		if (input.startsWith('-') || input.endsWith('-')) {
			isValid = false;
		}
		if (isValid) {
			item->setText(input);
			if (item->column() == 0) {
				Pages[item->row()]->setSurname(item->text());
			}
			if (item->column() == 1) {
				Pages[item->row()]->setName(item->text());
			}
			if (item->column() == 2) {
				Pages[item->row()]->setPatronymic(item->text());
			}
		}
		else {
			QMessageBox::warning(Table->parentWidget(), QStringLiteral("Некорректные данные"),
				QStringLiteral("Фамилия, имя и отчество должны начинаться с заглавной буквы, могут содержать только буквы, цифры, дефис и пробел, а также не должны оканчиваться на дефис. Строка автоматически заменена на \"Не задано\""));
			item->setText(QStringLiteral("Не задано"));
			if (item->column() == 0) {
				Pages[item->row()]->setName(item->text());
			}
			if (item->column() == 1) {
				Pages[item->row()]->setSurname(item->text());
			}
			if (item->column() == 2) {
				Pages[item->row()]->setPatronymic(item->text());
			}
		}
		break;
	}
	case 3:
	{
		Pages[item->row()]->setAddress(item->text());
		break;
	}
	case 4:
	{
		QString input = item->text().trimmed();
		if (item->text() == QString(QStringLiteral("Не задано"))) {
			return;
		}
		QRegExp regex(QStringLiteral("[0-9]+\\.[0-9]+\\.[0-9]+"));
		bool isValid = regex.exactMatch(input);
		if (isValid) {
			QStringList digits = input.split(".");
			QDate date = QDate(digits[2].toInt(), digits[1].toInt(), digits[0].toInt());
			QDate today = QDate::currentDate();
			if (date.isValid() and (date <= today)) {
				item->setText(date.toString("dd.MM.yyyy"));
				Pages[item->row()]->setDOB(QDate::fromString(item->text(), "dd.MM.yyyy"));
				return;
			}
		}
		else {
			item->setText(QStringLiteral("Не задано"));
			Pages[item->row()]->setDOB(QDate());
			QMessageBox::warning(Table->parentWidget(), QStringLiteral("Некорректные данные"),
				QStringLiteral("Не верная дата. Строка автоматически заменена на \"Не задано\""));
		}
		break;
	}
	case 5:
	{
		QString input = item->text().trimmed();
		if (item->text() == QString(QStringLiteral("Не задано"))) {
			return;
		}
		QRegExp regex(QStringLiteral("[a-zA-Z0-9\.]+[ ]?@[ ]?[a-zA-Z0-9\.]+"));
		bool isValid = regex.exactMatch(input);
		if (isValid) {
			item->setText(input);
			Pages[item->row()]->setEmail(input);
		}
		else {
			item->setText(QStringLiteral("Не задано"));
			Pages[item->row()]->setEmail(QStringLiteral("Не задано"));
			QMessageBox::warning(Table->parentWidget(), QStringLiteral("Некорректные данные"),
				QStringLiteral("Не верный E-mail. Строка автоматически заменена на \"Не задано\""));
		}
		break;
	}
	default:
		break;
	}
}

void Book::update() {
	Table->clearContents();
	Table->setRowCount(0);
	for (auto i : Pages) {
		this->add_Page(i, false);
	}
}

void Book::clear() {
	Pages.clear();
	Table->clearContents();
	Table->setRowCount(0); 
}



void Book::delete_row() {
	QList<QTableWidgetSelectionRange> selectedRanges = Table->selectedRanges();
	QSet<int> rowsToDelete;

	for (const QTableWidgetSelectionRange& range : selectedRanges) {
		for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
			rowsToDelete.insert(row);
		}
	}
	QList<int> sortedRows = rowsToDelete.toList();
	std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
	for (int row : sortedRows) {
		Table->removeRow(row);
		Pages.remove(row);
	}
}


void writeBookToJsonFile(Book* book, const QString& fileName) {
	QFile file;
	if (not fileName.endsWith(".json")) {
		file.setFileName(fileName + ".json");
	}
	else {
		file.setFileName(fileName);
	}
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(book->get_Table()->parentWidget(), QStringLiteral("Ошибка сохранения"),
			QStringLiteral("Таблица не была сохранена"));
		return;
	}
	QJsonArray jsonArray;
	for (auto page : book->getPages()) {
		QJsonObject jsonObject;
		jsonObject["Surname"] = page->getSurname();
		jsonObject["Name"] = page->getName();
		jsonObject["Patronymic"] = page->getPatronymic();
		jsonObject["Address"] = page->getAddress();
		jsonObject["DOB"] = page->getDOB().toString("dd.MM.yyyy");
		jsonObject["Email"] = page->getEmail();

		QJsonArray phoneNumbersArray = QJsonArray::fromStringList(QStringList(page->getPhone_Numbers().toList()));
		QJsonArray phoneTypesArray = QJsonArray::fromStringList(page->getPhone_types_str().toList());

		jsonObject["Phone_Numbers"] = phoneNumbersArray;
		jsonObject["Phone_types"] = phoneTypesArray;

		jsonArray.append(jsonObject);
	}
	QJsonDocument jsonDoc(jsonArray);
	file.write(jsonDoc.toJson());
	file.close();
}

bool readBookFromJsonFile(Book* book, const QString& fileName) {
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(book->get_Table()->parentWidget(), QStringLiteral("Ошибка открытия файла"),
			QStringLiteral("Не удалось открыть файл"));
		return false;
	}
	QByteArray fileData = file.readAll();
	file.close();
	QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
	if (!jsonDoc.isArray()) {
		QMessageBox::warning(book->get_Table()->parentWidget(), QStringLiteral("Ошибка открытия файла"),
			QStringLiteral("Неверный формат JSON"));
		return false;
	}
	QJsonArray jsonArray = jsonDoc.array();
	for (const QJsonValue& value : jsonArray) {
		if (!value.isObject()) {
			continue;
		}
		QJsonObject obj = value.toObject();
		Page* page = new Page();
		page->setSurname(obj["Surname"].toString());
		page->setName(obj["Name"].toString());
		page->setPatronymic(obj["Patronymic"].toString());
		page->setAddress(obj["Address"].toString());
		page->setDOB(QDate::fromString(obj["DOB"].toString(), "dd.MM.yyyy"));
		page->setEmail(obj["Email"].toString());
		QJsonArray phoneNumbersArray = obj["Phone_Numbers"].toArray();
		QVector<QString> new_phone_numbers;
		for (const QJsonValue& phoneNumber : phoneNumbersArray) {
			new_phone_numbers.append(phoneNumber.toString());
		}
		page->setPhone_Numbers(new_phone_numbers);
		QJsonArray phoneTypesArray = obj["Phone_types"].toArray();
		QVector<int> new_phone_types;
		for (const QJsonValue& phoneType : phoneTypesArray) {
			new_phone_types.append(phoneType.toInt());
		}
		page->setPhone_types(new_phone_types);
		book->add_Page(page);
	}
	book->update();
	return true;
}

QString getSaveFilePath(QWidget* parent) {
	// Открываем диалог выбора файла для сохранения
	QString fileName = QFileDialog::getSaveFileName(
		parent,
		QStringLiteral("Сохранить файл"),               // Заголовок диалога
		"",                             // Начальный путь (можно указать по умолчанию)
		"JSON Files (*.json);;All Files (*.*)" // Фильтры для файлов
	);

	return fileName; // Возвращаем выбранный путь (или пустую строку, если отменено)
}

QString getOpenFilePath(QWidget* parent) {
	// Открываем диалог выбора файла для сохранения
	QString fileName = QFileDialog::getOpenFileName(
		parent,
		QStringLiteral("Открыть файл"),               // Заголовок диалога
		"",                             // Начальный путь (можно указать по умолчанию)
		"JSON Files (*.json);;All Files (*.*)" // Фильтры для файлов
	);

	return fileName; // Возвращаем выбранный путь (или пустую строку, если отменено)
}

void Book::confirmExit(QApplication* app, QString Name) {
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this->Table->parentWidget(), QStringLiteral("Подтвердите выход"), QStringLiteral("Сохранить файл перед выходом?"),
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);

	if (reply == QMessageBox::Yes) {
		writeBookToJsonFile(this, Name);
		app->quit();
	}
	if (reply == QMessageBox::No) {
		app->quit();
	}
	if (reply == QMessageBox::Abort) {
		return;
	}
}

void setWidgetsEnabled(QGridLayout* layout) {
	for (int i = 0; i < layout->rowCount(); ++i) {
		for (int j = 0; j < layout->columnCount(); ++j) {
			QWidget* widget = layout->itemAt(i * layout->columnCount() + j)->widget();
			if (widget) {
				widget->setEnabled(widget->isEnabled() ? false : true);
			}
		}
	}
}

void set_numbers_const(QTableWidget* Table) {
	for (int i = 0; i < Table->rowCount(); i++) {
		setWidgetsEnabled(qobject_cast<QGridLayout*>(Table->itemAt(i, Headers.size() - 1)->tableWidget()->layout()));
	}
}

void disableWidgetsInLastColumn(QTableWidget* table) {
	QWidget* layout_widget1;
	QWidget* layout_widget2;
	QWidget* layout_widget3;
	QGridLayout* grid_layout;
	for (int i = 0; i < table->rowCount(); i++) {
		grid_layout = qobject_cast<QGridLayout*>(table->cellWidget(i, table->columnCount() - 1)->layout());
		for (int j = 0; j < grid_layout->rowCount() - 1; j++) {
			layout_widget1 = grid_layout->itemAtPosition(j, 0)->widget();
			layout_widget2 = grid_layout->itemAtPosition(j, 1)->widget();

			layout_widget1->setEnabled(not layout_widget1->isEnabled());
			layout_widget2->setEnabled(not layout_widget2->isEnabled());
		}
		layout_widget3 = grid_layout->itemAtPosition(grid_layout->rowCount() - 1, 0)->widget();
		layout_widget3->setEnabled(not layout_widget3->isEnabled());
	}
}

QTableWidget* search(Book* book, int index_of_combo, const QString& request) {
	if (book->get_Table() == nullptr || index_of_combo < 0 || index_of_combo >= book->get_Table()->columnCount()) {
		return nullptr;
	}
	Book* result_book = new Book(book->get_Table()->parentWidget());
	book->get_Table()->setGeometry(QRect(0, 31, 1200, 770));
	book->get_Table()->setHorizontalHeaderLabels(Headers);
	for (Page* p : book->getPages()) {
		bool flag = false;
		switch (index_of_combo)
		{
		case 0:
			flag = p->getSurname().contains(request);
			break;
		case 1:
			flag = p->getName().contains(request);
			break;
		case 2:
			flag = p->getPatronymic().contains(request);
			break;
		case 3:
			flag = p->getAddress().contains(request);
			break;
		case 4:
			flag = p->getDOB().toString("dd.MM.yyyy").contains(request);
			break;
		case 5:
			flag = p->getEmail().contains(request);
			break;
		case 6:
			for (QString number : p->getPhone_Numbers()) {
				flag = number.contains(request);
				if (flag) {
					break;
				}
			}
			break;
		default:
			break;
		}
		if (flag) {
			result_book->add_Page(p);
		}
	}
	return result_book->get_Table();
}
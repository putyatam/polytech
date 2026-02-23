#include <calculator.h>
#include <QLayout>
#include <QRegularExpression>
#include <QLine>

Calculator::Calculator(QWidget* parent) {

	line1 = new QLineEdit(this);
	line2 = new QLineEdit(this);
	operations = new QComboBox(this);
	result = new QLabel(this);
	modulo = new QLabel(this);
	QLabel* text_res = new QLabel(QStringLiteral("Результат: "), this);
	text_mod = new QLabel(QStringLiteral("Остаток: "), this);
	calculate_button = new QPushButton(QStringLiteral("Посчитать", this));

	line1->setPlaceholderText(QStringLiteral("Число 1"));
	line2->setPlaceholderText(QStringLiteral("Число 2"));
	operations->addItems({ "+", "-", "*", "/" });
	text_mod->hide();
	calculate_button->setDisabled(true);

	text_digits = new QLabel(QStringLiteral("Количество разрядов:"));
	text_alphabet = new QLabel(QStringLiteral("Алфавит:"));
	apply_button = new QPushButton(QStringLiteral("Применить"));
	spin_digits = new QSpinBox();
	QString alphabet_str;
	for (int i = 0; i < vec_alphabet.size(); i++) {
		alphabet_str.append(i == vec_alphabet.size() - 1 ? vec_alphabet[i] : vec_alphabet[i] + ',');
	}
	line_alphabet = new QLineEdit(alphabet_str);
	apply_button->setDisabled(true);
	spin_digits->setMinimum(1);
	spin_digits->setMaximum(32);
	spin_digits->setValue(8);
	QRegularExpression regex2("[a-zA-Z0-9,]*");
	QRegularExpressionValidator* validator2 = new QRegularExpressionValidator(regex2, line_alphabet);
	line_alphabet->setValidator(validator2);

	QGridLayout* layout_main = new QGridLayout(this);
	QHBoxLayout* layout_lines = new QHBoxLayout();
	QGridLayout* layout_result = new QGridLayout();
	QVBoxLayout* layout_calc_menu = new QVBoxLayout();
	QHBoxLayout* layout_consts = new QHBoxLayout();

	layout_calc_menu->addLayout(layout_lines);
	layout_calc_menu->addWidget(calculate_button);
	//layout_calc_menu->addLayout(layout_result);

	layout_lines->addWidget(line1);
	layout_lines->addWidget(operations);
	layout_lines->addWidget(line2);

	layout_result->addWidget(text_res, 0, 0);
	layout_result->addWidget(text_mod, 1, 0);
	layout_result->addWidget(result, 0, 1);
	layout_result->addWidget(modulo, 1, 1);

	layout_consts->addWidget(text_digits);
	layout_consts->addWidget(spin_digits);
	layout_consts->addWidget(text_alphabet);
	layout_consts->addWidget(line_alphabet);
	layout_consts->addWidget(apply_button);

	QFrame* HorLine1 = new QFrame;
	HorLine1->setFrameShape(QFrame::HLine);
	HorLine1->setFrameShadow(QFrame::Sunken);
	QFrame* HorLine2 = new QFrame;
	HorLine2->setFrameShape(QFrame::HLine);
	HorLine2->setFrameShadow(QFrame::Sunken);

	layout_main->addLayout(layout_consts, 0, 0);
	layout_main->addWidget(HorLine1, 1, 0);
	layout_main->addLayout(layout_calc_menu, 2, 0);
	layout_main->addWidget(HorLine2, 3, 0);
	layout_main->addLayout(layout_result, 4, 0);

	QRegularExpression regex("^[+-]?[" + QString(vec_alphabet.data(), vec_alphabet.size()) + "]{0," + QString::number(size_of_number) + "}$");
	QRegularExpressionValidator* validator = new QRegularExpressionValidator(regex, line1);

	line1->setValidator(validator);
	line2->setValidator(validator);

	QObject::connect(line1, &QLineEdit::textChanged, this, &Calculator::check_input);
	QObject::connect(line2, &QLineEdit::textChanged, this, &Calculator::check_input);
	QObject::connect(calculate_button, &QPushButton::clicked, this, &Calculator::calculate);
	QObject::connect(line_alphabet, &QLineEdit::textChanged, [&]() {
		QString alphabet_str;
		for (int i = 0; i < vec_alphabet.size(); i++) {
			alphabet_str.append(i == vec_alphabet.size() - 1 ? vec_alphabet[i] : vec_alphabet[i] + ',');
		}
		apply_button->setEnabled(line_alphabet->text() != alphabet_str ? true : false) ;
		});
	QObject::connect(spin_digits, QOverload<int>::of(&QSpinBox::valueChanged), [&]() {
		apply_button->setEnabled(size_of_number != spin_digits->value() ? true : false);
		});
	QObject::connect(apply_button, &QPushButton::clicked, this, &Calculator::apply);
	update_tables();
}

void Calculator::apply(){
	QStringList alphabet_list = line_alphabet->text().remove(',').split("");
	alphabet_list.removeAll("");
	alphabet_list.removeDuplicates();
	line_alphabet->setText(alphabet_list.join(','));
	vec_alphabet.clear();
	for (int i = 0; i < alphabet_list.size(); i++) {
		vec_alphabet.append(alphabet_list[i][0]);
	}
	qDebug() << vec_alphabet;
	size_of_number = spin_digits->value();
	update_tables();
	border_abs = QString(size_of_number, vec_alphabet.last());

	QRegularExpression regex("^[+-]?[" + QString(vec_alphabet.data(), vec_alphabet.size()) + "]{0," + QString::number(size_of_number) + "}$");
	QRegularExpressionValidator* validator = new QRegularExpressionValidator(regex, line1);

	line1->setValidator(validator);
	line2->setValidator(validator);
	line1->clear();
	line2->clear();
	result->clear();
	modulo->clear();
	text_mod->hide();
}

void Calculator::check_input() {
	if (!line1->text().isEmpty() and !line2->text().isEmpty()) {
		this->calculate_button->setDisabled(false);
	}
	else {
		this->calculate_button->setDisabled(true);
	}
}

void Calculator::calculate() {
	Number n_left(line1->text());
	Number n_right(line2->text());
	Number n_result;
	QPair<Number, Number> nums;
	switch (operations->currentIndex())
	{
	case 0:
		n_result = n_left + n_right;
		debug_number_to_normal(n_left, n_right, n_result, "+");
		break;
	case 1:
		n_result = n_left - n_right;
		debug_number_to_normal(n_left, n_right, n_result, "-");
		break;
	case 2:
		n_result = n_left * n_right;
		debug_number_to_normal(n_left, n_right, n_result, "*");
		break;
	case 3:
		nums = n_left / n_right;
		debug_number_to_normal(n_left, n_right, nums.first, "//");
		debug_number_to_normal(n_left, n_right, nums.second, "%");
		n_result = nums.first;
		break;
	default:
		qDebug() << "operation index error";
		break;
	}

	if (operations->currentIndex() != 3) {
		text_mod->hide();
		modulo->hide();
	}
	else {
		if (n_result.get_n().endsWith("empty")) {
			result->setText(QStringLiteral("Пустое множество"));
			return;
		}
		else if (n_result.get_n().endsWith("range")) {
			result->setText("[-" + QString(size_of_number, vec_alphabet.last()) + "; " + QString(size_of_number, vec_alphabet.last()) + "]");
			return;
		}
		modulo->show();
		modulo->setText(nums.second.harvest());
		text_mod->show();
	}
	result->setText(n_result.harvest());
	if (border_flag) {
		border_flag = false;
		QMessageBox m_box;
		m_box.setWindowTitle(QStringLiteral("Выход за диапазон допустимых значений"));
		m_box.setText(QStringLiteral("Результат операции вышел за диапазон допустимых значений [-") + QString(size_of_number, vec_alphabet.last()) + "; " + QString(size_of_number, vec_alphabet.last()) + QStringLiteral("]. Результат не является действительным."));
		m_box.setIcon(QMessageBox::Warning);
		m_box.exec();
	}
	
}

void debug_number_to_normal(const Number n1, const Number n2, const Number res, QString sign) {
	qDebug() << "----------";
	QString str_n1, str_n2, str_res;
	for (int i = 0; i < size_of_number + 1; i++) {
		str_n1.push_back(QString::number(vec_alphabet.indexOf(n1.get_n()[i])));
		str_n2.push_back(QString::number(vec_alphabet.indexOf(n2.get_n()[i])));
		str_res.push_back(QString::number(vec_alphabet.indexOf(res.get_n()[i])));
	}
	qDebug() << n1 << sign << n2 << "=" << res;
	qDebug() << (n1.get_isminus() ? "-" + str_n1 : str_n1) << sign << (n2.get_isminus() ? "-" + str_n2 : str_n2) << "=" << (res.get_isminus() ? "-" + str_res : str_res);
	qDebug() << (n1.get_isminus() ? "-" : "") << str_n1.toInt(nullptr, vec_alphabet.size()) << sign << (n2.get_isminus() ? "-" : "") << str_n2.toInt(nullptr, vec_alphabet.size()) << "=" << (res.get_isminus() ? "-" : "") << str_res.toInt(nullptr, vec_alphabet.size());
	

}
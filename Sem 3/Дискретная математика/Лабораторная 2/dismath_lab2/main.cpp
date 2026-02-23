#include "Header.h"

int main() {
    setlocale(LC_ALL, "Russian");
    node* bdr = createBDR();

    while (true) {
        qDebug() << QStringLiteral("Выберете действие:").toUtf8().constData() << "\n\t" << QStringLiteral("[1] - Вывод таблицы истинности").toUtf8().constData() << "\n\t" << QStringLiteral("[2] - Вывод значения по четырём переменным").toUtf8().constData() << "\n\t" <<
            QStringLiteral("[3] - вывод СДНФ").toUtf8().constData() << "\n\t" << QStringLiteral("[4] - Вывод СКНФ").toUtf8().constData() << "\n\t" << QStringLiteral("[5] - Вывод полинома Жегалкина").toUtf8().constData() << "\n\t" << QStringLiteral("[e] - Выход").toUtf8().constData() << endl;
        QString answer = getSingleInput();
        QByteArray byteArray = answer.toUtf8(); // Преобразование QString в QByteArray
        const char* cstr = byteArray.constData(); // Получение const char*
        QVector<QVector<int>> vec;
        QString triangle;
        switch (cstr[0])
        {
        case '1':
            table_view_with_all(bdr);
            //table_view_with_bdr(bdr);
            break;
        case '2':
            all_values(bdr, getUserInput());
            break;
        case '3':
            qDebug() << "\t" << QStringLiteral("Формула СДНФ:").toUtf8().constData() << endl;
            qDebug() << getSDNF().toUtf8().constData() << endl;
            break;
        case '4':
            qDebug() << "\t" << QStringLiteral("Формула СКНФ:").toUtf8().constData() << endl;
            qDebug() << getSKNF().toUtf8().constData() << endl;
            break;
        case '5':
            vec = getTriangle();
            for (int i = 0; i < vec.size(); i++) {
                for (int n = 0; n < i; n++) {
                    triangle.push_back(" ");
                }
                for (int j = 0; j < vec[i].size(); j++) {
                    if (j != 0) {
                        triangle.push_back(" ");
                    }
                    triangle.push_back(QString::number(vec[i][j]));
                }
                triangle.push_back("\n");
            }
            qDebug().nospace() << triangle.toUtf8().constData() << endl;
            qDebug() << "\t" << QStringLiteral("Полином Жегалкина").toUtf8().constData() << endl;
            qDebug() << getPJ().toUtf8().constData() << endl;
            break;

        case 'e':
            exit(0);
        default:
            break;
        }

    }

}


node* createBDR() {
    node* root_true = new node(4, true);
    node* root_false = new node(4, false);
    node* out = new node(0);

    out->f = new node(1);
    out->f->f = new node(2);
    out->f->f->t = root_false;
    out->f->f->f = new node(3);
    out->f->f->f->f = root_false;
    out->t = new node(2);
    out->t->f = new node(3);
    out->t->f->t = root_false;
    out->f->f->f->t = root_true;
    out->f->t = new node(2);
    out->f->t->f = root_true;
    out->t->f->f = root_true;
    out->t->t = root_true;
    out->f->t->t = new node(3);
    out->f->t->t->t = root_true;
    out->f->t->t->f = root_false;

    return out;
}

int value_by_bdr(node* core, QVector<int> values) {
    node* final = core;
    int n;
    while (true) {
        if (final->type != 4) {
            final = final->get_next(values[final->type]);
        }
        else {
            break;
        }
    }
    return final->value;
}

void table_view() {
    qDebug() << "\t" << QStringLiteral("Таблица истинности:").toUtf8().constData() << "\n" << QStringLiteral("x y z k  f").toUtf8().constData() << endl;
    for (int i = 0; i < vector_f.size(); i++) {
        qDebug() << vector_x[i] << " " << vector_y[i] << " " << vector_z[i] << " " << vector_k[i] << "  " << vector_f[i] << endl;
    }
}

QString getSDNF() {
    QString out;
    for (int i = 0; i < vector_f.size(); i++) {
        if (vector_f[i]) {
            if (!out.isEmpty()) {
                out.push_back(' ');
                out.push_back('v');
                out.push_back(' ');
            }
            if (vector_x[i] == 0) {
                out.push_back('~');
            }
            out.push_back('x');
            out.push_back('^');
            if (vector_y[i] == 0) {
                out.push_back('~');
            }
            out.push_back('y');
            out.push_back('^');
            if (vector_z[i] == 0) {
                out.push_back('~');
            }
            out.push_back('z');
            out.push_back('^');
            if (vector_k[i] == 0) {
                out.push_back('~');
            }
            out.push_back('k');
        }
    }
    return out;
}
QString getSKNF() {
    QString out;

    for (int i = 0; i < vector_f.size(); i++) {
        if (!vector_f[i]) {
            if (!out.isEmpty()) {
                out.push_back(' ');
                out.push_back('^');
                out.push_back(' ');
            }

            out.push_back('(');
            if (vector_x[i] == 1) {
                out.push_back('~');
            }
            out.push_back('x');
            out.push_back('v');
            if (vector_y[i] == 1) {
                out.push_back('~');
            }
            out.push_back('y');
            out.push_back('v');
            if (vector_z[i] == 1) {
                out.push_back('~');
            }
            out.push_back('z');
            out.push_back('v');
            if (vector_k[i] == 1) {
                out.push_back('~');
            }
            out.push_back('k');
            out.push_back(')');
        }
    }
    return out;
}

QVector<QVector<int>> getTriangle() {
    QVector<QVector<int>> out;
    out.push_back(vector_f);
    for (int i = 0; i < vector_f.size() - 1; i++) {
        QVector<int> vec;
        for (int j = 0; j < out[i].size() - 1; j++) {
            vec.push_back((out[i][j] + out[i][j + 1]) % 2);
        }
        out.push_back(vec);
    }
    return out;
}

QVector<int> getUserInput() {
    QTextStream qin(stdin);
    QVector<int> numbers;
    bool validInput = false;

    while (!validInput) {
        // Запрашиваем ввод
        qDebug() << QStringLiteral("Введите 4 цифры (0 или 1) через пробел:").toUtf8().constData();
        QString line = qin.readLine().trimmed();

        // Разбиваем строку на части
        QStringList splitLine = line.split(' ');

        // Проверяем, что введено 4 элемента
        if (splitLine.size() != 4) {
            qDebug() << QStringLiteral("Неправильный ввод. Пожалуйста, введите ровно 4 цифры.").toUtf8().constData();
            continue;
        }

        // Проверяем, что все элементы - это 0 или 1
        validInput = true;
        numbers.clear();
        for (const QString& str : splitLine) {
            if (str != "0" && str != "1") {
                validInput = false;
                break;
            }
            numbers.append(str.toInt());
        }

        if (!validInput) {
            qDebug() << QStringLiteral("Неправильный ввод. Пожалуйста, вводите только 0 или 1.").toUtf8().constData();
        }
    }

    return numbers;
}

QString getSingleInput() {
    QTextStream qin(stdin);
    QString input;
    bool validInput = false;

    while (!validInput) {
        // Запрашиваем ввод
        qDebug() << QStringLiteral("Введите число от 1 до 5 или букву 'e' для выхода:").toUtf8().constData();
        input = qin.readLine().trimmed(); // Читаем строку и удаляем пробелы по краям

        // Проверяем ввод
        if ((input >= "1" && input <= "5") || input == "e") {
            validInput = true; // Ввод корректный
        }
        else {
            qDebug() << QStringLiteral("Неправильный ввод. Попробуйте снова.").toUtf8().constData();
        }
    }

    return input;
}

void all_values(node* core, QVector<int> values) {
    qDebug() << "\t" << QStringLiteral("Значения").toUtf8().constData() << "\n" << QStringLiteral("Значение по таблице истинности - ").toUtf8().constData() << vector_f[values[0] * 8 + values[1] * 4 + values[2] * 2 + values[3]] << "\n" <<
        QStringLiteral("Значение по БДР - ").toUtf8().constData() << value_by_bdr(core, values) << "\n" <<
        QStringLiteral("Значение по СДНФ - ").toUtf8().constData() << value_by_sdnf(values) << "\n" <<
        QStringLiteral("Значение по ПЖ - ").toUtf8().constData() << value_by_pj(values) << endl;
}

node::node(int new_type, bool new_value) {
    f = nullptr;
    t = nullptr;
    type = new_type;
    if (type == 4) {
        value = new_value;
    }
    else {
        value = NULL;
    }
}

node* node::get_next(bool flag) {
    return flag ? t : f;
}

void table_view_with_all(node* core) {
    qDebug().nospace() << "\t" << QStringLiteral("Таблица истинности : ").toUtf8().constData() << "\n" << QStringLiteral("x y z k  f БДР СДНФ ПЖ").toUtf8().constData() << endl;
    QVector<int> vec;
    for (int i = 0; i < vector_f.size(); i++) {
        vec = { vector_x[i], vector_y[i], vector_z[i], vector_k[i] };
        qDebug().nospace() << vector_x[i] << " " << vector_y[i] << " " << vector_z[i] << " " << vector_k[i] << "  " << vector_f[i] << "  " << value_by_bdr(core, vec) <<
            "   " << value_by_sdnf(vec) << "   " << value_by_pj(vec);
    }
}

int value_by_sdnf(QVector<int> vec) {
    QStringList sdnf = getSDNF().split('v');
    QString str
    if (vec[0] == 1) {
        str.push_back("x^");
    }
    else {
        str.push_back("~x^");
    }
    if (vec[1] == 1) {
        str.push_back("y^");
    }
    else {
        str.push_back("~y^");
    }
    if (vec[2] == 1) {
        str.push_back("z^");
    }
    else {
        str.push_back("~z^");
    }
    if (vec[3] == 1) {
        str.push_back("k");
    }
    else {
        str.push_back("~k");
    }
    for (QString i : sdnf) {
        i = i.trimmed();
        if (str == i) {
            return 1;
        }
    }
    return 0;
}

QString getPJ() {
    QVector<int> coef;
    for (auto vec : getTriangle()) {
        coef.push_back(vec.first());
    }
    QString out;
    for (int i = 0; i < vector_f.size(); i++) {
        if (coef[i]) {
            if (i == 0) {
                out.push_back("1");
            }
            if (!out.isEmpty()) {
                out.push_back(" + ");
            }
            if (vector_x[i]) {
                out.push_back("x");
            }
            if (vector_y[i]) {
                out.push_back("y");
            }
            if (vector_z[i]) {
                out.push_back("z");
            }
            if (vector_k[i]) {
                out.push_back("k");
            }
        }
    }
    return out;
}

int value_by_pj(QVector<int> values) {
    QStringList PJ = getPJ().remove(" ").trimmed().split("+");
    int sum = 0;
    for (QString i : PJ) {
        if (i == "1") {
            sum++;
        }
        if (i.contains("x") and values[0] == 1 or !i.contains("x") and values[0] == 1 or !i.contains("x") and values[0] == 0) {
            if (i.contains("y") and values[1] == 1 or !i.contains("y") and values[1] == 1 or !i.contains("y") and values[1] == 0) {
                if (i.contains("z") and values[2] == 1 or !i.contains("z") and values[2] == 1 or !i.contains("z") and values[2] == 0) {
                    if (i.contains("k") and values[3] == 1 or !i.contains("k") and values[3] == 1 or !i.contains("k") and values[3] == 0) {
                        sum++;
                    }
                }
            }
        }
    }
    return sum % 2;

}
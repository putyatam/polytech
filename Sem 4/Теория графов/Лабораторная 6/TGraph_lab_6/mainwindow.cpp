#include "mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QRandomGenerator>
#include <qDebug>
#include <QOpenGLWidget>
#include <QGestureEvent>
#include <QGraphicsScene>
#include <QMessageBox>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include <map>
#include <random>
#include <clocale>
#include <codecvt>
#include <locale>




QString symbols = QStringLiteral("абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ 0123456789!@#$%^&*()_-+=[]{}|;:'\",.<>/?`~");
bool double_compressed = false;
bool compressed = false;


MainWindow::MainWindow(QWidget* parent) {
    dict_tree = new dict_rb_tree();
    dict_hash = new dict_hash_table<QString, int>();
    tab_main = new QTabWidget(this);
    tab_main->setFixedSize(1000, 600);

    button_hash_add = new QPushButton(QStringLiteral("Добавить"), this);
    button_hash_remove = new QPushButton(QStringLiteral("Удалить"), this);
    button_hash_find = new QPushButton(QStringLiteral("Найти"), this);
    button_hash_clear = new QPushButton(QStringLiteral("Очистить таблицу"), this);
    button_hash_file = new QPushButton(QStringLiteral("Загрузить из файла"), this);
    button_hash_generate_file = new QPushButton(QStringLiteral("Сгенерировать файл"), this);
    button_hash_update = new QPushButton(QStringLiteral("Обновить"), this);

    check_hash_clear = new QCheckBox(QStringLiteral("Дополнить текущий"), this);

    line_hash_add_key = new QLineEdit(this);
    line_hash_add_value = new QLineEdit(this);
    QIntValidator* val1 = new QIntValidator(0, 9999999, line_hash_add_value);
    line_hash_add_value->setValidator(val1);
    line_hash_remove_key = new QLineEdit(this);
    line_hash_remove_result = new QLineEdit(this);
    line_hash_remove_result->setReadOnly(true);
    line_hash_find_key = new QLineEdit(this);
    line_hash_find_result = new QLineEdit(this);
    line_hash_find_result->setReadOnly(true);
    line_size_hash = new QLineEdit(this);
    QIntValidator* val2 = new QIntValidator(1, 9999999, line_size_hash);
    line_size_hash->setValidator(val2);
    line_size_hash->setText(QString::number(dict_hash->get_size_table()));
    

    label_hash = new QLabel(QStringLiteral("Хэш-таблица"), this);
    label_hash->setFont(QFont("Arial", 12, QFont::Bold));
    label_hash_add_key = new QLabel(QStringLiteral("Ключ"), this);
    label_hash_add_value = new QLabel(QStringLiteral("Значение"), this);
    label_hash_remove_key = new QLabel(QStringLiteral("Ключ"), this);
    label_hash_remove_result = new QLabel(QStringLiteral("Результат"), this);
    label_hash_find_key = new QLabel(QStringLiteral("Ключ"), this);
    label_hash_find_result = new QLabel(QStringLiteral("Результат"), this);
    label_hash_size = new QLabel(QStringLiteral("Размер таблицы"), this);


    table_hash = new QTableWidget(this);
    table_hash->setFixedSize(500, 500);
    //table_hash->setSortingEnabled(true);
    table_hash->setHorizontalHeaderLabels(QStringList(QList<QString>{QStringLiteral("Хэш"), QStringLiteral("Ключ"), QStringLiteral("Значение")}));
    table_hash->verticalHeader()->hide();
    QHBoxLayout* layout_hash = new QHBoxLayout(this);
    QGridLayout* layout_hash_functions = new QGridLayout(this);
    layout_hash_functions->addWidget(label_hash, 0, 0, 1, 3, Qt::AlignTop | Qt::AlignCenter);

    
    layout_hash_functions->addWidget(label_hash_size, 1, 0);
    layout_hash_functions->addWidget(line_size_hash, 1, 1);
    layout_hash_functions->addWidget(button_hash_update, 1, 2);

    QFrame* horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_hash_functions->addWidget(horLine, 2, 0, 1, 3);

    layout_hash_functions->addWidget(button_hash_generate_file, 3, 1);
    layout_hash_functions->addWidget(button_hash_clear, 3, 0, 2, 1, Qt::AlignCenter);
    layout_hash_functions->addWidget(button_hash_file, 4, 1);
    layout_hash_functions->addWidget(check_hash_clear, 4, 2);
    

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_hash_functions->addWidget(horLine, 5, 0, 1, 3);

    layout_hash_functions->addWidget(label_hash_add_key, 6, 0, Qt::AlignBottom | Qt::AlignCenter);
    layout_hash_functions->addWidget(label_hash_add_value, 6, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_hash_functions->addWidget(line_hash_add_key, 7, 0, Qt::AlignTop | Qt::AlignCenter);
    layout_hash_functions->addWidget(line_hash_add_value, 7, 1, Qt::AlignTop | Qt::AlignCenter);

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_hash_functions->addWidget(horLine, 8, 0, 1, 3);

    layout_hash_functions->addWidget(label_hash_remove_key, 9, 0, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_hash_functions->addWidget(line_hash_remove_key, 10, 0, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_hash_functions->addWidget(label_hash_remove_result, 9, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_hash_functions->addWidget(line_hash_remove_result, 10, 1, 1, 1, Qt::AlignTop | Qt::AlignCenter);

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_hash_functions->addWidget(horLine, 11, 0, 1, 3);

    layout_hash_functions->addWidget(label_hash_find_key, 12, 0, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_hash_functions->addWidget(line_hash_find_key, 13, 0, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_hash_functions->addWidget(label_hash_find_result, 12, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_hash_functions->addWidget(line_hash_find_result, 13, 1, 1, 1, Qt::AlignTop | Qt::AlignCenter);

    layout_hash_functions->addWidget(button_hash_add, 7, 2, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_hash_functions->addWidget(button_hash_remove, 10, 2, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_hash_functions->addWidget(button_hash_find, 13, 2, 1, 1, Qt::AlignTop | Qt::AlignCenter);


    QObject::connect(button_hash_add, &QPushButton::clicked, [&]() {
        if (line_hash_add_key->text() == "" or line_hash_add_value->text() == "") {
            return;
        }
        dict_hash->add(line_hash_add_key->text(), line_hash_add_value->text().toInt());
        update_table();
        });
    QObject::connect(button_hash_remove, &QPushButton::clicked, [&]() {
        line_hash_remove_result->setText(dict_hash->remove(line_hash_remove_key->text()) ? QStringLiteral("Удалён") : QStringLiteral("Не удалён"));
        update_table();
        });
    QObject::connect(button_hash_find, &QPushButton::clicked, [&]() {
        int find_index = dict_hash->find(line_hash_find_key->text());
        line_hash_find_result->setText(find_index == -1 ? QStringLiteral("Не найдено") : QString::number(find_index));
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(line_hash_find_result->text() != QStringLiteral("Не найдено") ? QStringLiteral("Результат: значение найдено") : QStringLiteral("Результат: значение не найдено"));
        msgBox.exec();
        });
    QObject::connect(button_hash_clear, &QPushButton::clicked, [&]() {
        dict_hash->clear();
        update_table();
        });
    QObject::connect(button_hash_file, &QPushButton::clicked, [&]() {
        QString file_name = QFileDialog::getOpenFileName(this, QStringLiteral("Открыть файл"), QDir::currentPath(), "Text Files (*.txt)");
        if (!check_hash_clear->isChecked()) {
            dict_hash->clear();
        }
        dict_hash->from_string(read_from_file(file_name));
        update_table();
        });
    QObject::connect(button_hash_generate_file, &QPushButton::clicked, [&]() {
        if (QFile::exists("generated_text.txt")) {
            QFile::remove("generated_text.txt");
        }
        generate_text_to_file("generated_text", 10000, true);
        });
    QObject::connect(button_hash_update, &QPushButton::clicked, [&]() {
        if (line_size_hash->text() == "0") {
            return;
        }
        delete dict_hash;
        dict_hash = new dict_hash_table<QString, int>(line_size_hash->text().toInt());
        update_table();
        });
    update_table();

    layout_hash->addLayout(layout_hash_functions);
    layout_hash->addWidget(table_hash);
    QWidget* widget_dict_hash = new QWidget(this);
    widget_dict_hash->setLayout(layout_hash);
    tab_main->addTab(widget_dict_hash, QStringLiteral("Хэш-таблица"));







    button_tree_add = new QPushButton(QStringLiteral("Добавить"), this);
    button_tree_remove = new QPushButton(QStringLiteral("Удалить"), this);
    button_tree_find = new QPushButton(QStringLiteral("Найти"), this);
    button_tree_clear = new QPushButton(QStringLiteral("Очистить таблицу"), this);
    button_tree_file = new QPushButton(QStringLiteral("Загрузить из файла"), this);
    button_tree_generate_file = new QPushButton(QStringLiteral("Сгенерировать файл"), this);

    check_tree_clear = new QCheckBox(QStringLiteral("Дополнить текущий"), this);

    line_tree_add_key = new QLineEdit(this);
    line_tree_remove_key = new QLineEdit(this);
    line_tree_remove_result = new QLineEdit(this);
    line_tree_remove_result->setReadOnly(true);
    line_tree_find_key = new QLineEdit(this);
    line_tree_find_result = new QLineEdit(this);
    line_tree_find_result->setReadOnly(true);


    label_tree = new QLabel(QStringLiteral("Красно-чёрное дерево"), this);
    label_tree->setFont(QFont("Arial", 12, QFont::Bold));
    label_tree_add_key = new QLabel(QStringLiteral("Ключ"), this);
    label_tree_remove_key = new QLabel(QStringLiteral("Ключ"), this);
    label_tree_remove_result = new QLabel(QStringLiteral("Результат"), this);
    label_tree_find_key = new QLabel(QStringLiteral("Ключ"), this);
    label_tree_find_result = new QLabel(QStringLiteral("Результат"), this);

    
    QHBoxLayout* layout_tree = new QHBoxLayout(this);
    QGridLayout* layout_tree_functions = new QGridLayout(this);
    layout_tree_functions->addWidget(label_tree, 0, 0, 1, 3, Qt::AlignTop | Qt::AlignCenter);

    layout_tree_functions->addWidget(button_tree_generate_file, 3, 1);
    layout_tree_functions->addWidget(button_tree_clear, 3, 0, 2, 1, Qt::AlignCenter);
    layout_tree_functions->addWidget(button_tree_file, 4, 1);
    layout_tree_functions->addWidget(check_tree_clear, 4, 2);


    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_tree_functions->addWidget(horLine, 5, 0, 1, 3);

    layout_tree_functions->addWidget(label_tree_add_key, 6, 0, Qt::AlignBottom | Qt::AlignCenter);
    layout_tree_functions->addWidget(line_tree_add_key, 7, 0, Qt::AlignTop | Qt::AlignCenter);

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_tree_functions->addWidget(horLine, 8, 0, 1, 3);

    layout_tree_functions->addWidget(label_tree_remove_key, 9, 0, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_tree_functions->addWidget(line_tree_remove_key, 10, 0, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_tree_functions->addWidget(label_tree_remove_result, 9, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_tree_functions->addWidget(line_tree_remove_result, 10, 1, 1, 1, Qt::AlignTop | Qt::AlignCenter);

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_tree_functions->addWidget(horLine, 11, 0, 1, 3);

    layout_tree_functions->addWidget(label_tree_find_key, 12, 0, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_tree_functions->addWidget(line_tree_find_key, 13, 0, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_tree_functions->addWidget(label_tree_find_result, 12, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    layout_tree_functions->addWidget(line_tree_find_result, 13, 1, 1, 1, Qt::AlignTop | Qt::AlignCenter);

    layout_tree_functions->addWidget(button_tree_add, 7, 2, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_tree_functions->addWidget(button_tree_remove, 10, 2, 1, 1, Qt::AlignTop | Qt::AlignCenter);
    layout_tree_functions->addWidget(button_tree_find, 13, 2, 1, 1, Qt::AlignTop | Qt::AlignCenter);

    scene = new QGraphicsScene();

    

    

    view = new Tree_view(this);

    view->setScene(scene);

    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setRenderHint(QPainter::Antialiasing);
    view->setSceneRect(-50000, -50000, 100000, 100000);
    view->centerOn(0, 0);
    view->setWindowTitle("Graph Visualization");
    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    view->setBackgroundBrush(QBrush(QColor(192, 192, 192)));

    QObject::connect(button_tree_add, &QPushButton::clicked, [&]() {
        if (line_tree_add_key->text() == "") {
            return;
        }
        dict_tree->add(line_tree_add_key->text());
        dict_tree->draw(scene);
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);

        });
    QObject::connect(button_tree_remove, &QPushButton::clicked, [&]() {
        line_tree_remove_result->setText(dict_tree->remove(line_tree_remove_key->text()) ? QStringLiteral("Удалён") : QStringLiteral("Не удалён"));
        dict_tree->draw(scene);
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        });
    QObject::connect(button_tree_find, &QPushButton::clicked, [&]() {
        line_tree_find_result->setText(dict_tree->find(line_tree_find_key->text()) ? QStringLiteral("Найдено") : QStringLiteral("Не найдено"));
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(line_tree_find_result->text() == QStringLiteral("Найдено") ? QStringLiteral("Результат: значение найдено") : QStringLiteral("Результат: значение не найдено"));
        msgBox.exec();
        });
    QObject::connect(button_tree_clear, &QPushButton::clicked, [&]() {
        dict_tree->clear();
        dict_tree->draw(scene);
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        });
    QObject::connect(button_tree_file, &QPushButton::clicked, [&]() {
        QString file_name = QFileDialog::getOpenFileName(this, QStringLiteral("Открыть файл"), QDir::currentPath(), "Text Files (*.txt)");
        if (!check_tree_clear->isChecked()) {
            dict_tree->clear();
        }
        dict_tree->from_string(read_from_file(file_name));
        dict_tree->draw(scene);
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        });
    QObject::connect(button_tree_generate_file, &QPushButton::clicked, [&]() {
        if (QFile::exists("generated_text.txt")) {
            QFile::remove("generated_text.txt");
        }
        generate_text_to_file("generated_text", 10000, true);
        });

    dict_tree->draw(scene);

    layout_tree->addLayout(layout_tree_functions);
    layout_tree->addWidget(view);
    QWidget* widget_dict_tree = new QWidget(this);
    widget_dict_tree->setLayout(layout_tree);
    tab_main->addTab(widget_dict_tree, QStringLiteral("RB-дерево"));

    //

    button_algo_generate_file = new QPushButton(QStringLiteral("Сгенерировать файл"), this);
    button_algo_file = new QPushButton(QStringLiteral("Прочитать файл"), this);
    button_compress = new QPushButton(QStringLiteral("Закодировать"), this);
    button_decompress = new QPushButton(QStringLiteral("Декодировать"), this);
    button_compress_double = new QPushButton(QStringLiteral("Двухступ. кодирование"), this);
    button_decompress_double = new QPushButton(QStringLiteral("Двухступ. декодирование"), this);
    button_is_correct = new QPushButton(QStringLiteral("Проверить"), this);

    text_algo_file = new QTextEdit(this);
    text_algo_file->setFixedSize(450, 200);
    text_algo_decompressed = new QTextEdit(this);
    text_algo_decompressed->setReadOnly(true);
    text_algo_decompressed->setFixedSize(450, 200);
    line_algo_compressed = new QLineEdit(this);
    line_algo_compressed->setReadOnly(true);
    

    label_cost_compression = new QLabel(QStringLiteral("Цена код.:"));
    label_koef_compression = new QLabel(QStringLiteral("Коэф. сжатия:"));
    label_is_correct = new QLabel(QStringLiteral("Текста совпадают:"));
    label_LZW = new QLabel(QStringLiteral("Алгоритмы кодирования"));
    label_LZW->setFont(QFont("Arial", 12, QFont::Bold));
    label_text_algo_file = new QLabel(QStringLiteral("Изначальный текст"));
    label_text_algo_decompressed = new QLabel(QStringLiteral("Декодированный текст"));
    label_line_algo_LZW = new QLabel(QStringLiteral("Вектор"), this);

    QHBoxLayout* layout_algo = new QHBoxLayout(this);
    QGridLayout* layout_algo_functions = new QGridLayout(this);

    layout_algo_functions->addWidget(label_LZW, 0, 0, 2, 6, Qt::AlignTop | Qt::AlignCenter);
    layout_algo_functions->addWidget(button_algo_generate_file, 1, 0, 1, 3);
    layout_algo_functions->addWidget(button_algo_file, 1, 3, 1, 3);

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_algo_functions->addWidget(horLine, 2, 0, 1, 6);

    layout_algo_functions->addWidget(button_compress_double, 3, 0, 1, 2);
    layout_algo_functions->addWidget(button_decompress_double, 3, 2, 1, 2);
    layout_algo_functions->addWidget(button_compress, 4, 0, 1, 2);
    layout_algo_functions->addWidget(button_decompress, 4, 2, 1, 2);
    layout_algo_functions->addWidget(button_is_correct, 4, 4, 1, 2);

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_algo_functions->addWidget(horLine, 5, 0, 1, 6);

    layout_algo_functions->addWidget(label_cost_compression, 6, 0, 1, 2);
    layout_algo_functions->addWidget(label_koef_compression, 6, 2, 1, 2);
    layout_algo_functions->addWidget(label_is_correct, 6, 4, 1, 2);
    layout_algo_functions->addWidget(label_line_algo_LZW, 7, 0, 1, 2, Qt::AlignRight);
    layout_algo_functions->addWidget(line_algo_compressed, 7, 2, 1, 4);

    horLine = new QFrame(this);
    horLine->setFrameShape(QFrame::HLine);
    horLine->setFrameShadow(QFrame::Plain);
    layout_algo_functions->addWidget(horLine, 8, 0, 1, 6);

    layout_algo_functions->addWidget(label_text_algo_file, 1, 7, 1, 6, Qt::AlignCenter | Qt::AlignBottom);
    layout_algo_functions->addWidget(text_algo_file, 2, 7, 3, 6, Qt::AlignCenter | Qt::AlignTop);

    layout_algo_functions->addWidget(label_text_algo_decompressed, 5, 7, 1, 6, Qt::AlignCenter | Qt::AlignBottom);
    layout_algo_functions->addWidget(text_algo_decompressed, 6, 7, 3, 6, Qt::AlignCenter | Qt::AlignTop);

    
    layout_algo->addLayout(layout_algo_functions);
    //layout_algo_functions->setSizeConstraint(QLayout::setFixefSize)

    QWidget* widget_algo = new QWidget(this);
    widget_algo->setLayout(layout_algo);
    tab_main->addTab(widget_algo, QStringLiteral("Алгоритмы кодирования"));

    button_decompress->setDisabled(true);
    button_decompress_double->setDisabled(true);

    QObject::connect(button_algo_generate_file, &QPushButton::clicked, [&]() {
        if (QFile::exists("generated_text.txt")) {
            QFile::remove("generated_text.txt");
        }
        generate_text_to_file("generated_text", 10000, true);
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QStringLiteral("Информация"));
        msgBox.setText(QStringLiteral("Текст был сгенерирован и записан в файл generated_text.txt."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        });
    QObject::connect(button_algo_file, &QPushButton::clicked, [&]() {
        QString file_name = QFileDialog::getOpenFileName(this, QStringLiteral("Открыть файл"), QDir::currentPath(), "Text Files (*.txt)");
        text_algo_file->setText(read_from_file(file_name));
        });
    QObject::connect(button_compress, &QPushButton::clicked, [&]() {
        QString str;
        if (text_algo_file->toPlainText() == "") {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(QStringLiteral("Информация"));
            msgBox.setText(QStringLiteral("Текст не был введён, кодирование не возможно"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            return;
        }
        v_LZW = compressLZW(text_algo_file->toPlainText());
        for (int i = 0; i < v_LZW.size(); i++) {
            if (i != 0) {
                str += ", ";
            }
            str += QString::number(int(v_LZW[i]));
        }
        line_algo_compressed->setText(str);

        int maxCode = 0;
        for (int code : v_LZW) {
            if (code > maxCode) maxCode = code;
        }

        int bitsPerCode = static_cast<int>(std::ceil(std::log2(maxCode + 1)));
        double totalCompressedBits = v_LZW.size() * bitsPerCode;
        int originalLength = v_LZW.length();
        
        label_cost_compression->setText(QStringLiteral("Цена код.: ") + QString::number(totalCompressedBits / originalLength));

        double originalSizeBits = text_algo_file->toPlainText().length() * 16.0;

        label_koef_compression->setText(QStringLiteral("Коэф. сжатия: ") + QString::number(text_algo_file->toPlainText().length() / double(v_LZW.size())));
        button_decompress_double->setDisabled(true);
        button_decompress->setDisabled(false);
        });
    QObject::connect(button_decompress, &QPushButton::clicked, [&]() {
        text_algo_decompressed->setPlainText((decompressLZW(v_LZW)));//
        button_compress_double->setDisabled(false);
        button_decompress_double->setDisabled(false);
        });
    QObject::connect(button_compress_double, &QPushButton::clicked, [&]() {
        if (text_algo_file->toPlainText() == "") {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(QStringLiteral("Информация"));
            msgBox.setText(QStringLiteral("Текст не был введён, кодирование не возможно"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            return;
        }
        v_LZW = compressLZW(text_algo_file->toPlainText());
        QPair<QVector<bool>, HuffmanNode*> p = compressHuffman(v_LZW);
        v_Huffman = p.first;
        root = p.second;
        /*QString str;
        for (int i = 0; i < v_Huffman.size(); i++) {
            if (i != 0) {
                str += ", ";
            }
            str += v_Huffman[i] ? "1" : "0";
        }
        line_algo_compressed->setText(str);*/
        dict_hash_table<int, QString> m;
        generateCodes(root, "", m);
        QString str;
        double cost = 0;
        int count = 0;
        for (int i = 0; i < v_LZW.size(); i++) {
            if (i != 0) {
                str += ", ";
            }
            str += QString::number(v_LZW[i]) + ": " + m.find(v_LZW[i]);
            cost += m.find(v_LZW[i]).size();
            count++;
        }
        line_algo_compressed->setText(str);
        label_cost_compression->setText(QStringLiteral("Цена код.: ") + QString::number(cost / count));
        qDebug() << text_algo_file->toPlainText().length();
        label_koef_compression->setText(QStringLiteral("Коэф. сжатия: ") + QString::number(text_algo_file->toPlainText().length() * 32 / double(v_Huffman.size())));
        button_decompress->setDisabled(true);
        button_decompress_double->setDisabled(false);
        });
    QObject::connect(button_decompress_double, &QPushButton::clicked, [&]() {
        v_LZW = decompressHuffman(v_Huffman, root);
        text_algo_decompressed->setPlainText(decompressLZW(v_LZW));
        });
    QObject::connect(button_is_correct, &QPushButton::clicked, [&]() {
        if (text_algo_file->toPlainText() == text_algo_decompressed->toPlainText()) {
            label_is_correct->setText(QStringLiteral("Текста совпадают: Да"));
        }
        else {
            label_is_correct->setText(QStringLiteral("Текста совпадают: Нет"));
        }
        });
}


void MainWindow::update_table() {
    // Очищаем таблицу перед заполнением
    table_hash->clear();

    // Устанавливаем заголовки столбцов
    QStringList headers;
    headers << "Hash" << "Key" << "Value";
    table_hash->setColumnCount(3);
    table_hash->setHorizontalHeaderLabels(headers);

    int totalRows = 0;

    // Сначала вычисляем общее количество строк
    for (int hashIndex = 0; hashIndex < dict_hash->get_table().size(); ++hashIndex) {
        const QList<QPair<QString, int>>& entries = dict_hash->get_table()[hashIndex];
        totalRows += (entries.isEmpty() ? 1 : entries.size());
    }

    table_hash->setRowCount(totalRows);

    int currentRow = 0;

    for (int hashIndex = 0; hashIndex < dict_hash->get_table().size(); ++hashIndex) {
        const QList<QPair<QString, int>>& entries = dict_hash->get_table()[hashIndex];

        if (entries.isEmpty()) {
            QTableWidgetItem* hashItem = new QTableWidgetItem(QString::number(hashIndex));
            table_hash->setItem(currentRow, 0, hashItem);
            currentRow++;
        }
        else {
            for (const auto& pair : entries) {
                QTableWidgetItem* hashItem = new QTableWidgetItem(QString::number(hashIndex));
                QTableWidgetItem* keyItem = new QTableWidgetItem(pair.first);
                QTableWidgetItem* valueItem = new QTableWidgetItem(QString::number(pair.second));

                table_hash->setItem(currentRow, 0, hashItem);
                table_hash->setItem(currentRow, 1, keyItem);
                table_hash->setItem(currentRow, 2, valueItem);

                currentRow++;
            }
        }
    }

        

    table_hash->resizeColumnsToContents();
}


void generate_text_to_file(const QString filename, size_t count, bool more_spaces) {
    QFile file(filename + ".txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл для записи";
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    for (int i = 0; i < count; ++i) {
        int index = abs(int(rand_Laplace())) % symbols.length();
        out << symbols.at(index);
    }

    file.close();
    qDebug() << "file was created";
}

QString read_from_file(QString file_name) {
    QFile inFile(file_name);
    QString text;
    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&inFile);
        in.setCodec("UTF-8");
        text = in.readAll();
        inFile.close();
        qDebug() << "Исходный текст прочитан (" << text.size() << "символов)";
        return text;
    }
    else {
        qWarning() << "Ошибка чтения файла" << file_name;
        return "";
    }
}


double rand_Laplace() {
    // mu - параметр положения (-inf : +inf)
    // lambda - параметр масштаба (0 : +inf)
    double r = QRandomGenerator::global()->generateDouble();

    if (r <= 0.5) {
        return 50 + (log(2 * r) / 0.2);
    }
    return 50 - (log(2 * (1 - r)) / 0.2);
}



QVector<int> compressLZW(const QString input) {
    dict_hash_table<QString, int> dictionary(symbols.size());
    int dictSize = symbols.size();
    QVector<int> result;

    for (int i = 0; i < symbols.size(); ++i) {
        dictionary.add(QString(symbols[i]), i);
    }
    QString w;
    

    for (int i = 0; i < input.size(); i++) {
        QChar c = input[i];
        QString wc = w + c;
        if (dictionary.find(wc) != -1) {
            w = wc;
        }
        else {
            result.append(dictionary.find(w));
            dictionary.add(wc, dictSize++);
            w = QString(c);
        }
    }

    if (!w.isEmpty()) {
        result.append(dictionary.find(w));
    }

    return result;
}


QString decompressLZW(const QVector<int>& compressed) {
    QMap<int, QString> dictionary;
    dict_hash_table<int, QString> dictionary2;
    int dictSize = symbols.size();

    for (int i = 0; i < symbols.size(); ++i) {
        dictionary.insert(i, QString(symbols[i]));
        dictionary2.add(i, QString(symbols[i]));
    }

    QString w = dictionary[compressed.first()];
    QString result = w;

    for (int i = 1; i < compressed.size(); ++i) {
        int k = compressed.at(i);
        QString entry;

        if (dictionary2.contains(k)) {
            qDebug() << "k";
        }

        if (dictionary.contains(k)) {
            entry = dictionary[k];
        }
        else if (k == dictSize) {
            entry = w + w[0];
        }
        else {
            qDebug() << "Ошибка в сжатых данных";
            return "";
        }

        result += entry;

        dictionary.insert(dictSize++, w + entry[0]);
        w = entry;
    }

    return result;
}


void generateCodes(HuffmanNode* root, string code, dict_hash_table<int, QString>& huffmanCode) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCode.add(root->value, QString::fromStdString(code));
    }

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}


HuffmanNode* buildHuffmanTree(const dict_hash_table<int, int>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;


    for (QPair<int, int> pair : freqMap.get_all_pairs()) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        HuffmanNode* newNode = new HuffmanNode(-1, left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    return pq.top();
}


QPair<QVector<bool>, HuffmanNode*> compressHuffman(const QVector<int>& lzwOutput) {
    dict_hash_table<int, int> freqMap;
    for (int val : lzwOutput) {
        freqMap.add(val, freqMap.find(val) + 1);
    }

    HuffmanNode* root = buildHuffmanTree(freqMap);

    dict_hash_table<int, QString> huffmanCode;
    generateCodes(root, "", huffmanCode);

    QVector<bool> encodedData;
    for (int val : lzwOutput) {
        string code = huffmanCode.find(val).toStdString();
        for (char bit : code) {
            encodedData.push_back(bit == '1');
        }
    }

    return qMakePair<QVector<bool>, HuffmanNode*>(encodedData, root);
}


QVector<int> decompressHuffman(const QVector<bool>& encodedData, HuffmanNode* root) {
    QVector<int> decodedData;
    HuffmanNode* current = root;

    for (bool bit : encodedData) {
        if (bit) current = current->right;
        else current = current->left;

        if (!current->left && !current->right) {
            decodedData.push_back(current->value);
            current = root;
        }
    }

    return decodedData;
}



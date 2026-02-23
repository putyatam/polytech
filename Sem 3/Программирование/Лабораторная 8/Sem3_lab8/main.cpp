#include "Book.h"

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTableView>
#include <QApplication>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLabel>
#include <QStatusBar>
#include <QWidgetAction>
#include <QMainWindow>
#include <QToolBar>

QString TableName = QString(QStringLiteral("Новая таблица"));

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow* w = new QMainWindow;
    QMenuBar* Menu = new QMenuBar(w);
    QToolBar* Tools = new QToolBar(w);
    Book* book = new Book(w);
    QTableWidget* Table = book->get_Table();
    QTableWidget* search_Table = new QTableWidget();

    w->setWindowTitle(QStringLiteral("Телефонный справочник: Новая таблица"));
    
    
    w->setFixedWidth(1200);
    w->setFixedHeight(800);
    Table->setGeometry(QRect(0, 31, 1200, 770));
    
    //
    /*book->add_Page(new Page(QString("Putyata"), QString("Mihail"), QString("Alexandrovic"), QString("Communy 44, korp. 2, kv. 91"),
        QDate(2005, 9, 22), QString("mike.putyata@gmail.com"), { "7 931 260 37 46",  "7 921 428 82 48" }, { 0, 2 }));
    book->add_Page(new Page(QString("Kozlov"), QString("Alex"), QString("Sergeevich"), QString("Communy 43"), QDate(2005, 9, 3),
        QString("lelya.kozlov@mail.ru"), { "8 888 888 88 88" }, { 1 }));*/
    //
    
    QMenu* menu_file = new QMenu(QStringLiteral("Файл"));
    QMenu* menu_table = new QMenu(QStringLiteral("Таблица"));


    QAction* action_file_open_file = new QAction(QStringLiteral("Открыть"), w);
    menu_file->addAction(action_file_open_file);
    QAction* action_file_add_to_file = new QAction(QStringLiteral("Добавить к текущей таблице"), w);
    menu_file->addAction(action_file_add_to_file);
    QAction* action_file_save_file = new QAction(QStringLiteral("Сохранить"), w);
    menu_file->addAction(action_file_save_file);
    QAction* action_file_save_file_as = new QAction(QStringLiteral("Сохранить как"), w);
    menu_file->addAction(action_file_save_file_as);
    QAction* action_file_quit = new QAction(QStringLiteral("Выход"), w);
    menu_file->addAction(action_file_quit);

    QAction* action_table_new_row = new QAction(QStringLiteral("Новая строка"), w);
    menu_table->addAction(action_table_new_row);
    QAction* action_table_delete_row = new QAction(QStringLiteral("Удалить выделенную строку"), w);
    menu_table->addAction(action_table_delete_row);
    QAction* action_table_delete_all = new QAction(QStringLiteral("Очистить"), w);
    menu_table->addAction(action_table_delete_all);
    QAction* action_table_sort = new QAction(QStringLiteral("Переключить сортировку: Выключено"), w);
    menu_table->addAction(action_table_sort);
    QAction* action_table_const = new QAction(QStringLiteral("Разрешить редактирование: Включено"), w); 
    menu_table->addAction(action_table_const);

    


    QObject::connect(action_table_new_row, &QAction::triggered, [book, Table]() {
        if (Table->editTriggers() == QAbstractItemView::NoEditTriggers) {
            QMessageBox::warning(Table->parentWidget(), QStringLiteral("Ошибка редактирования"),
                QStringLiteral("Выключен режим редактирования. Для добавления новых строк включите режим редактирования."));
        }
        else {
            book->add_Row();
        }

        });
    QObject::connect(action_file_save_file, &QAction::triggered, [book, w]() {
        writeBookToJsonFile(book, ::TableName);
        });
    QObject::connect(action_file_save_file_as, &QAction::triggered, [book]() {
        writeBookToJsonFile(book, getSaveFilePath());
        });
    QObject::connect(action_file_open_file, &QAction::triggered, [book, w]() {
        QString filename = getOpenFilePath();
        book->clear();
        readBookFromJsonFile(book, filename);
        ::TableName = filename.split("/").last().split(".").first();
        w->setWindowTitle((QString(QStringLiteral("Телефонный справочник: ")) + TableName));
        });
    QObject::connect(action_file_add_to_file, &QAction::triggered, [book, w]() {
        QString filename = getOpenFilePath();
        readBookFromJsonFile(book, filename);
        });

    QObject::connect(action_table_delete_all, &QAction::triggered, book, &Book::clear);
    QObject::connect(action_table_delete_row, &QAction::triggered, book, &Book::delete_row);
    QObject::connect(action_file_quit, &QAction::triggered, [book, &app]() {
        book->confirmExit(&app, ::TableName);
        });
    QObject::connect(action_table_sort, &QAction::triggered, [Table, action_table_sort]() {
        Table->setSortingEnabled(Table->isSortingEnabled() ? false : true);
        action_table_sort->setText(Table->isSortingEnabled() ? QStringLiteral("Переключить сортировку: Выключено") : QStringLiteral("Переключить сортировку: Включено"));
        });
    QObject::connect(action_table_const, &QAction::triggered, [Table, action_table_const]() {
        Table->setEditTriggers(Table->editTriggers() == QAbstractItemView::NoEditTriggers ? QAbstractItemView::DoubleClicked : QAbstractItemView::NoEditTriggers);
        action_table_const->setText(Table->editTriggers() == QAbstractItemView::NoEditTriggers ? QStringLiteral("Разрешить редактирование: Выключено") : QStringLiteral("Разрешить редактирвоание: Включено"));
        disableWidgetsInLastColumn(Table);
        });

    QWidget* layout_widget = new QWidget();
    QHBoxLayout* h_layout = new QHBoxLayout();
    QLineEdit* line_to_search = new QLineEdit();
    QComboBox* set_column = new QComboBox();
    QPushButton* search_button = new QPushButton(QStringLiteral("Поиск"));
    QPushButton* cancel_button = new QPushButton(QStringLiteral("Отмена"));

    QObject::connect(search_button, &QPushButton::clicked, [&]() {
        search_Table = search(book, set_column->currentIndex(), line_to_search->text());
        Table->hide();
        search_Table->show();
        search_Table->setGeometry(QRect(0, 31, 1200, 770));
        });

    QObject::connect(cancel_button, &QPushButton::clicked, [&]() {
        search_Table->clear();
        search_Table->hide();
        Table->show();
        });

    set_column->addItems(Headers);

    set_column->setFixedWidth(142);
    line_to_search->setFixedWidth(160);
    search_button->setFixedWidth(80);
    cancel_button->setFixedWidth(80);


    h_layout->addWidget(set_column);
    h_layout->addWidget(line_to_search);
    h_layout->addWidget(search_button);
    h_layout->addWidget(cancel_button);
    layout_widget->setLayout(h_layout);

    h_layout->setContentsMargins(10, 0, 10, 0);
    
    
    Tools->setGeometry(680, 0, 520, 31);
    Tools->setMovable(false);
    Menu->setGeometry(0, 0, 140, 31);
    QFont f = Menu->font();
    f.setPointSize(11);
    Menu->setFont(f);




    Menu->addMenu(menu_file);
    Menu->addMenu(menu_table);
    Tools->addWidget(layout_widget);

    w->show();
    return app.exec();
}

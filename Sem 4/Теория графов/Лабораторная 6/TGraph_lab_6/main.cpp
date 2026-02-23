#include <QtWidgets/QApplication>
#include <QTextCodec>
#include "dict_hash.h"
#include <QDebug>
#include <QString>
#include "Header.h"
#include "dict_rb_tree.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::locale::global(std::locale(""));
    QApplication a(argc, argv);
    qApp->setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, true);
    qApp->setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, true);
    MainWindow* MW = new MainWindow();
    MW->show();

    return a.exec();
}

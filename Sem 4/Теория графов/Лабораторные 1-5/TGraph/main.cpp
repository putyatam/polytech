#include <QApplication>
#include <Abstract_Graph.h>
#include <Oriented_Graph.h>
#include <mainwindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* w = new MainWindow();
    w->show();

    return a.exec();
}

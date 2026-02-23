#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel label("Привет, Qt в VS Code!");
    label.show();

    return app.exec();
}
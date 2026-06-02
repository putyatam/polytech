#pragma once

#include <QGridLayout>

#include<QTextEdit>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>

#include <QPushButton>
#include <QProcess>
#include <QMessageBox>
#include <QTimer>
#include <QInputDialog>

#include "scanner.h"
#include "codegen.h"
#include "parser.h"
#include <QtWidgets/QMainWindow>




#include "ui_compile_visual.h"



class compile_visual : public QMainWindow
{
    Q_OBJECT
public:
    compile_visual(QWidget* parent = nullptr);
    ~compile_visual();

private:
    Ui::compile_visualClass ui;

    int count_inputs;

    std::wstring buffer;
    ScanResult result;
    Group currentGroup = gNone;
    std::map<int, int> lexemsRows;
    FSM<std::wstring>* fsm;

	QProcess* m_process;

    QTimer* lexerTimer; // Таймер для "ленивого" запуска лексера

    // Вспомогательная функция для применения цветов
    void applyHighlighting(QPlainTextEdit* editor, const ScanResult& result);


    void logMessage(const QString& msg);

    void openSingleFile(QTabWidget* tabs, const QString& filePath);
    void saveTabFile(QTabWidget* tabs);
    void onSaveCurrentFileTriggered();

    void runVirtualMachine(const QString& msFilePath);

    void createNewFile(const QString& extension);

    bool closeAllTabs();
    void openProjectFromPath(const QString& dirPath);
private slots:
	void onOpenMilFileTriggered();
    void onOpenMsFileTriggered();
    void onOpenProjectTriggered();
    void onOpenProjectTriggered2();
    void onSaveAllFilesTriggered();
    void saveSpecificTab(QTabWidget* tabs, int index);

    void onRunTriggered();           // Слот кнопки "Запуск"
    void sendProcessInput();         // Слот для отправки текста из QLineEdit
    void readProcessOutput();        // Слот для чтения вывода VM
    void handleProcessFinished(int exitCode); // Слот завершения

    void onTabCloseRequested(int index, QTabWidget* tabs = nullptr);
    void closeEvent(QCloseEvent* event) override;

    void runLexicalAnalysis(); // Слот для запуска разбора по таймеру

    void onCreateMilTriggered();
    void onCreateMsTriggered();
    void onCreateProjectTriggered();

    bool Build();
    void BuildAndRun();
};

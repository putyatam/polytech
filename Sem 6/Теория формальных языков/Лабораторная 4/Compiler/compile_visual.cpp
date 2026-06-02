#include "compile_visual.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QStyle>
#include <QStyleFactory>


void compile_visual::logMessage(const QString& msg) {
        ui.textInfo->appendPlainText(msg);

        ui.textInfo->ensureCursorVisible();
}


compile_visual::compile_visual(QWidget* parent)
    : QMainWindow(parent)
{
	count_inputs = 0;
    currentGroup = gNone;

    fsm = initTm(buffer, currentGroup, result.lexems, result.errors, result.lexemsRows);

    lexerTimer = new QTimer(this);
    lexerTimer->setSingleShot(true);
    lexerTimer->setInterval(1000);
    connect(lexerTimer, &QTimer::timeout, this, &compile_visual::runLexicalAnalysis);



    ui.setupUi(this);

    ui.tabs_mil->clear();
    ui.tabs_ms->clear();

    ui.textInfo->setReadOnly(true);

    ui.lineInput->setEnabled(false);

	ui.console_out->setReadOnly(true);

	connect(ui.action_open_mil, &QAction::triggered, this, &compile_visual::onOpenMilFileTriggered);
	connect(ui.action_open_ms, &QAction::triggered, this, &compile_visual::onOpenMsFileTriggered);
	connect(ui.action_open_project, &QAction::triggered, this, &compile_visual::onOpenProjectTriggered);
    connect(ui.action_save_current, &QAction::triggered, this, &compile_visual::onSaveCurrentFileTriggered);
    connect(ui.action_save_all, &QAction::triggered, this, &compile_visual::onSaveAllFilesTriggered);

	connect(ui.action_run, &QAction::triggered, this, &compile_visual::onRunTriggered);
    connect(ui.lineInput, &QLineEdit::returnPressed, this, &compile_visual::sendProcessInput);

        connect(ui.tabs_mil, &QTabWidget::tabCloseRequested, this, [this](int index) {
        onTabCloseRequested(index, ui.tabs_mil);
        });

        connect(ui.tabs_ms, &QTabWidget::tabCloseRequested, this, [this](int index) {
        onTabCloseRequested(index, ui.tabs_ms);
        });

        connect(ui.action_create_mil, &QAction::triggered, this, &compile_visual::onCreateMilTriggered);
    connect(ui.action_create_ms, &QAction::triggered, this, &compile_visual::onCreateMsTriggered);
    connect(ui.action_create_project, &QAction::triggered, this, &compile_visual::onCreateProjectTriggered);
	connect(ui.action_build, &QAction::triggered, this, &compile_visual::Build);
    connect(ui.action_build_run, &QAction::triggered, this, &compile_visual::BuildAndRun);
    connect(ui.action_autoCorrect, &QAction::triggered, this, [this](bool checked) {
        if (checked) {
            lexerTimer->blockSignals(false);
            lexerTimer->start();
        }
        else {
            lexerTimer->blockSignals(true);
            for (int i = 0; i < ui.tabs_mil->count(); ++i) {
                QWidget* currentTab = ui.tabs_mil->widget(i);

                if (currentTab) {
                    QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(currentTab);
                    editor->setExtraSelections({});
                }
            }
        }
        });
}

compile_visual::~compile_visual()
{}


void compile_visual::onOpenMilFileTriggered() {
    logMessage("\nВыбор .mil файла...");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть Mil"), "", tr("Mil Files (*.mil)"));
    openSingleFile(ui.tabs_mil, fileName);
}

void compile_visual::onOpenMsFileTriggered() {
    logMessage("\nВыбор .ms файла...");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть MS"), "", tr("MS Files (*.ms)"));
    openSingleFile(ui.tabs_ms, fileName);
}

void compile_visual::onSaveCurrentFileTriggered() {
        QWidget* focusedWidget = QApplication::focusWidget();

        QTabWidget* targetTabs = nullptr;

    QWidget* parent = focusedWidget;
    while (parent) {
        if (parent == ui.tabs_mil) {
            targetTabs = ui.tabs_mil;
            break;
        }
        if (parent == ui.tabs_ms) {
            targetTabs = ui.tabs_ms;
            break;
        }
        parent = parent->parentWidget();
    }

                if (targetTabs) {
        saveTabFile(targetTabs);
    }
    else {
                        logMessage("Ошибка: выберите окно редактора для сохранения.");
    }
}


void compile_visual::saveTabFile(QTabWidget* tabs) {
    if (!tabs || tabs->count() == 0) return;

    int currentIndex = tabs->currentIndex();     QWidget* current = tabs->widget(currentIndex);
    QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(current);
    if (!editor) return;

    QString filePath = editor->property("filePath").toString();

    if (filePath.isEmpty()) {
        QString filter = (tabs == ui.tabs_mil) ? "Mil Files (*.mil)" : "MS Files (*.ms)";
        QString title = (tabs == ui.tabs_mil) ? "Сохранить Mil" : "Сохранить MS";

        filePath = QFileDialog::getSaveFileName(this, title, "", filter);

        if (filePath.isEmpty()) return;

        editor->setProperty("filePath", filePath);
        tabs->setTabText(currentIndex, QFileInfo(filePath).fileName());
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();

                QString tabTitle = tabs->tabText(currentIndex);
        if (tabTitle.endsWith('*')) {
                        tabTitle.chop(1);
            tabs->setTabText(currentIndex, tabTitle);
        }
        
        logMessage("Файл сохранен: " + QFileInfo(filePath).fileName());
    }
    else {
        logMessage("Ошибка доступа к файлу: " + filePath);
    }
}


void compile_visual::openSingleFile(QTabWidget* tabs, const QString& filePath) {
    if (filePath.isEmpty()) {
        logMessage("Открытие файла отменено.");
        return;
    }

        for (int i = 0; i < tabs->count(); ++i) {
        if (tabs->widget(i)->property("filePath").toString() == filePath) {
            tabs->setCurrentIndex(i);             logMessage("Файл уже открыт: " + QFileInfo(filePath).fileName());
            return;
        }
    }

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QPlainTextEdit* textEdit = new QPlainTextEdit();

                textEdit->setStyleSheet(
            "QPlainTextEdit { "
            "background-color: #2B2B2B; color: #A9B7C6; "
            "selection-background-color: #214283; selection-color: #A9B7C6; "
            "font-family: 'Consolas', 'Monaco', monospace; font-size: 13px; "
            "}"
        );
        textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        textEdit->setTabStopDistance(QFontMetricsF(textEdit->font()).horizontalAdvance(' ') * 4);

        textEdit->setPlainText(in.readAll());
        file.close();

        connect(textEdit, &QPlainTextEdit::textChanged, this, [this]() {
            lexerTimer->start();
            });

        connect(textEdit, &QPlainTextEdit::textChanged, this, [this, textEdit, tabs]() {
                        lexerTimer->start();

                        int index = tabs->indexOf(textEdit);
            if (index != -1) {
                QString title = tabs->tabText(index);
                                if (!title.endsWith('*')) {
                    tabs->setTabText(index, title + "*");
                }
            }
            });

                textEdit->setProperty("filePath", filePath);

        QString baseName = QFileInfo(filePath).fileName();
        int index = tabs->addTab(textEdit, baseName);
        tabs->setCurrentIndex(index);
        logMessage("Успешно открыт: " + baseName);
        lexerTimer->start();
    }
    else {
        logMessage("Ошибка открытия: " + filePath);
    }
}


void compile_visual::onSaveAllFilesTriggered() {
    logMessage("\nСохранение всех файлов...");

        if (ui.tabs_mil->count() > 0) {
                                for (int i = 0; i < ui.tabs_mil->count(); ++i) {
            saveSpecificTab(ui.tabs_mil, i);
        }
    }

        if (ui.tabs_ms->count() > 0) {
        for (int i = 0; i < ui.tabs_ms->count(); ++i) {
            saveSpecificTab(ui.tabs_ms, i);
        }
    }

    logMessage("Все файлы сохранены\n");
}

void compile_visual::saveSpecificTab(QTabWidget* tabs, int index) {
    QWidget* widget = tabs->widget(index);
    QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(widget);
    if (!editor) return;

    QString filePath = editor->property("filePath").toString();

        if (filePath.isEmpty()) {
                tabs->setCurrentIndex(index);

        QString filter = (tabs == ui.tabs_mil) ? "Mil Files (*.mil)" : "MS Files (*.ms)";
        filePath = QFileDialog::getSaveFileName(this, tr("Сохранить новый файл"), "", filter);

        if (filePath.isEmpty()) return; 
        editor->setProperty("filePath", filePath);
        tabs->setTabText(index, QFileInfo(filePath).fileName());
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();

                QString title = tabs->tabText(index);
        if (title.endsWith("*")) {
            tabs->setTabText(index, title.left(title.length() - 1));
        }

        logMessage("Сохранен: " + QFileInfo(filePath).fileName());
    }
}


void compile_visual::runVirtualMachine(const QString& msFilePath) {
    if (m_process && m_process->state() == QProcess::Running) {
        logMessage("Программа уже запущена.");
        return;
    }

    m_process = new QProcess(this);

        connect(m_process, &QProcess::readyReadStandardOutput, this, [this]() {
                QByteArray output = m_process->readAllStandardOutput();
        logMessage(QString::fromLocal8Bit(output));
        });

        connect(m_process, &QProcess::readyReadStandardError, this, [this]() {
        QByteArray errors = m_process->readAllStandardError();
        logMessage("Ошибка: " + QString::fromLocal8Bit(errors));
        });

        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, [this](int exitCode) {
            logMessage(QString("Программа завершена (Код: %1)").arg(exitCode));
            m_process->deleteLater();
            m_process = nullptr;
        });

        logMessage("Запуск ВМ: " + QFileInfo(msFilePath).fileName());
    m_process->start("milanvm", QStringList() << msFilePath);

    if (!m_process->waitForStarted()) {
        logMessage("Ошибка: не удалось найти или запустить 'milanvm'.");
    }
}


void compile_visual::onRunTriggered() {
        QWidget* current = ui.tabs_ms->currentWidget();

    if (!current) {
        logMessage("Ошибка: Нет активного редактора MS.");
        return;
    }

    if (!current) return;
    QString filePath = current->property("filePath").toString();
    if (filePath.isEmpty()) {
        logMessage("Ошибка: Сначала сохраните файл .ms");
        return;
    }

        if (m_process && m_process->state() != QProcess::NotRunning) {
        return;
    }

        m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels); 

    connect(m_process, &QProcess::readyReadStandardOutput, this, &compile_visual::readProcessOutput);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &compile_visual::handleProcessFinished);

        ui.console_out->clear();
    logMessage("Запуск: " + QFileInfo(filePath).fileName());

    ui.lineInput->setEnabled(true);

        m_process->start("milanvm", QStringList() << filePath);

    if (!m_process->waitForStarted()) {
        logMessage("Ошибка: Не удалось запустить milanvm.exe");
    }
    else {
        ui.lineInput->setFocus();     }
}

void compile_visual::readProcessOutput() {
    if (m_process) {
                QByteArray data = m_process->readAllStandardOutput();
        ui.console_out->appendPlainText(QString::fromLocal8Bit(data));

                ui.console_out->ensureCursorVisible();
    }
}

void compile_visual::sendProcessInput() {
    if (m_process && m_process->state() == QProcess::Running) {
		count_inputs++;
        QString input = ui.lineInput->text();

                m_process->write((input + "\n").toLocal8Bit());

                QTextCursor cursor = ui.console_out->textCursor();
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(input);
        ui.console_out->setTextCursor(cursor);
        ui.console_out->ensureCursorVisible();

        ui.lineInput->clear();     }
}

void compile_visual::handleProcessFinished(int exitCode) {
	count_inputs = 0;
    ui.textInfo->appendPlainText(QString("\nПрограмма завершена (код %1)").arg(exitCode));
    m_process->deleteLater();
    m_process = nullptr;
    ui.lineInput->setEnabled(false);
}


void compile_visual::onTabCloseRequested(int index, QTabWidget* tabs) {
        if (!tabs) {
        tabs = qobject_cast<QTabWidget*>(sender());
    }

        if (!tabs) return;

    QWidget* widget = tabs->widget(index);
    if (!widget) return;

    QString title = tabs->tabText(index);

        if (title.endsWith('*')) {
                tabs->setCurrentIndex(index);

        QString fileName = title.left(title.length() - 1);

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Сохранение изменений");
        msgBox.setText(QString("Файл '%1' был изменен.").arg(fileName));
        msgBox.setInformativeText("Хотите сохранить изменения перед закрытием?");

                QPushButton* saveButton = msgBox.addButton("Сохранить", QMessageBox::ActionRole);
        QPushButton* discardButton = msgBox.addButton("Не сохранять", QMessageBox::DestructiveRole);
        QPushButton* cancelButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);

        msgBox.setDefaultButton(saveButton);
        msgBox.exec();

                if (msgBox.clickedButton() == saveButton) {
                        saveSpecificTab(tabs, index);
                                    if (tabs->tabText(index).endsWith('*')) return;
        }
        else if (msgBox.clickedButton() == cancelButton) {
                        return;
        }
            }

        tabs->removeTab(index);

        delete widget;

    logMessage("Вкладка закрыта.");
}


void compile_visual::closeEvent(QCloseEvent* event) {
        while (ui.tabs_mil->count() > 0) {
        int last = ui.tabs_mil->count() - 1;
        ui.tabs_mil->setCurrentIndex(last);

                onTabCloseRequested(last, ui.tabs_mil);

                if (ui.tabs_mil->count() > last) {
            event->ignore();             return;
        }
    }

        while (ui.tabs_ms->count() > 0) {
        int last = ui.tabs_ms->count() - 1;
        ui.tabs_ms->setCurrentIndex(last);

                onTabCloseRequested(last, ui.tabs_ms);

        if (ui.tabs_ms->count() > last) {
            event->ignore();
            return;
        }
    }

    event->accept(); }


void compile_visual::runLexicalAnalysis() {
        QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(ui.tabs_mil->currentWidget());
    if (!editor) return;

            std::wstring content = editor->toPlainText().toStdWString();

    result.clear();

    fsm->start(content + L'\n', 0);

    
    ui.errorInfo->clear();
    
    if (!result.errors.empty()) {
        
        for (Error* err : result.errors) {
            ui.errorInfo->appendPlainText(QString("Ошибка [%1:%2]: %3 (\"%4\")")
                .arg(err->getRow()).arg(err->getPos())
                .arg(QString::fromStdWString(err->getReason()))
                .arg(QString::fromStdWString(err->getText())));
        }
    }

        applyHighlighting(editor, result);
    Group currentGroup = gNone;
    buffer.clear();
}

void compile_visual::applyHighlighting(QPlainTextEdit* editor, const ScanResult& res) {
    QList<QTextEdit::ExtraSelection> selections;
    QString text = editor->toPlainText();
    int searchPos = 0;

        for (Lexem* lex : res.lexems) {
        QString target = QString::fromStdWString(lex->getStr());
                int foundIdx = text.indexOf(target, searchPos, Qt::CaseInsensitive);

        if (foundIdx != -1) {
            QTextEdit::ExtraSelection selection;

            switch (lex->getType()) {
                            case Begin:     case End:       case If:        case Then:      case Else:
            case Fi:        case While:     case Do:        case Od:        case For:
            case Break:     case Continue:  case Switch:    case Case:      case EndCase:
            case Default:
                selection.format.setForeground(QColor("#CC7832"));
                selection.format.setFontWeight(QFont::Bold);
                break;

							case Replace:   case Reverse:   case Resize:    case Shimb: 
            case Sorted:    case ToVector:  case Trans:     case Len:
            case Find:      case Write:     case Read:
                selection.format.setForeground(QColor("#DDDE5F"));
                selection.format.setFontWeight(QFont::Bold);
                break;

                            case Int:
            case Bool:
                selection.format.setForeground(QColor("#9876AA"));
                selection.format.setFontWeight(QFont::Bold);
                break;

                            case Var:
                selection.format.setForeground(QColor("#6AAB58"));
                break;

                            case True:
            case False:
                selection.format.setForeground(QColor("#3741DE"));
                selection.format.setFontWeight(QFont::Bold);
                break;

                            case Plus:         case Diff:        case Mult:        case Div:
            case Assign:       case Equal:       case NotEqual:    case Less:
            case LessEqual:    case Greater:     case GreaterEqual:case Not:
            case ShortAnd:     case FullAnd:     case ShortOr:     case FullOr:
            case LBracket:     case RBracket:    case LCurly:      case RCurly:
            case LSquare:      case RSquare:     case Semicolon:   case Comma:
            case Colon:
                selection.format.setForeground(QColor("#A9B7C6"));
                break;

				            case Comment:
            case MultiComment:
                selection.format.setForeground(QColor("#808080"));
                break;

				            case Id:
            default:
                selection.format.setForeground(QColor("#A9B7C6"));
                break;
            }

            QTextCursor cursor(editor->document());
            cursor.setPosition(foundIdx);
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, target.length());
            selection.cursor = cursor;
            selections.append(selection);

            searchPos = foundIdx + target.length();         }
    }

        for (Error* err : res.errors) {
        QTextEdit::ExtraSelection selection;
        selection.format.setUnderlineColor(Qt::red);
        selection.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);

        QTextCursor cursor(editor->document());
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, err->getRow() - 1);
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, err->getPos());
        cursor.select(QTextCursor::WordUnderCursor);

        selection.cursor = cursor;
        selections.append(selection);
    }

    editor->setExtraSelections(selections);
}


void compile_visual::onCreateMilTriggered() {
    createNewFile("mil");
}

void compile_visual::onCreateMsTriggered() {
    createNewFile("ms");
}

void compile_visual::createNewFile(const QString& extension) {
    QString filter = QString("%1 Files (*.%2)").arg(extension.toUpper(), extension);
    QString title = QString("Создать новый .%1 файл").arg(extension);

    QString filePath = QFileDialog::getSaveFileName(this, title, "", filter);

    if (filePath.isEmpty()) return;

        if (!filePath.endsWith("." + extension)) {
        filePath += "." + extension;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                if (extension == "mil") {
            QTextStream out(&file);
            out << "Begin\n\nEnd";         }
        file.close();

                openSingleFile(extension == "mil" ? ui.tabs_mil : ui.tabs_ms, filePath);
        logMessage("Файл создан: " + filePath);
    }
    else {
        logMessage("Ошибка при создании файла: " + filePath);
    }
}


void compile_visual::onCreateProjectTriggered() {
        if (!closeAllTabs()) {
        logMessage("Создание проекта прервано пользователем.");
        return;
    }

        QString parentDir = QFileDialog::getExistingDirectory(this, "Выберите местоположение для папки проекта");
    if (parentDir.isEmpty()) return;

        bool ok;
    QString projectName = QInputDialog::getText(this, "Новый проект",
        "Введите название проекта (будет создана папка):",
        QLineEdit::Normal, "MyProject", &ok);
    if (!ok || projectName.isEmpty()) return;

        QDir dir(parentDir);
    if (dir.exists(projectName)) {
        QMessageBox::warning(this, "Ошибка", "Папка с таким именем уже существует!");
        return;
    }

    if (dir.mkdir(projectName)) {
        QString projectPath = dir.absoluteFilePath(projectName);
        QString mainMilPath = projectPath + "/main.mil";

                QFile file(mainMilPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "// Project: " << projectName << "\nBegin\n\nEnd";

            logMessage("Проект успешно создан в: " + projectPath);

                                    openProjectFromPath(projectPath);
        }
    }
    else {
        logMessage("Не удалось создать папку проекта.");
    }
}


void compile_visual::onOpenProjectTriggered2() {
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Выберите папку проекта"), "");
    if (!dirPath.isEmpty()) {
        openProjectFromPath(dirPath);
    }
}

void compile_visual::openProjectFromPath(const QString& dirPath) {
    logMessage("\nЗагрузка проекта из: " + dirPath);
    QDir dir(dirPath);

        QStringList milFiles = dir.entryList(QStringList() << "*.mil", QDir::Files);
    for (const QString& f : milFiles) {
        openSingleFile(ui.tabs_mil, dir.absoluteFilePath(f));
    }

        QStringList msFiles = dir.entryList(QStringList() << "*.ms", QDir::Files);
    for (const QString& f : msFiles) {
        openSingleFile(ui.tabs_ms, dir.absoluteFilePath(f));
    }
    logMessage("Проект загружен.");
}


void compile_visual::onOpenProjectTriggered() {
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Выберите папку проекта"), "");

    if (dirPath.isEmpty()) {
        logMessage("\nОткрытие проекта отменено.");
        return;
    }

    logMessage("\nОткрытие проекта из папки: " + dirPath);

    QDir dir(dirPath);

    QStringList milFiles = dir.entryList(QStringList() << "*.mil", QDir::Files);
    for (const QString& f : milFiles) {
        openSingleFile(ui.tabs_mil, dir.absoluteFilePath(f));
    }

    QStringList msFiles = dir.entryList(QStringList() << "*.ms", QDir::Files);
    for (const QString& f : msFiles) {
        openSingleFile(ui.tabs_ms, dir.absoluteFilePath(f));
    }

    logMessage("Загрузка проекта завершена.");
}


bool compile_visual::closeAllTabs() {
    while (ui.tabs_mil->count() > 0) {
        int last = ui.tabs_mil->count() - 1;
        ui.tabs_mil->setCurrentIndex(last);
        onTabCloseRequested(last, ui.tabs_mil);

        if (ui.tabs_mil->count() > last) return false;
    }

    while (ui.tabs_ms->count() > 0) {
        int last = ui.tabs_ms->count() - 1;
        ui.tabs_ms->setCurrentIndex(last);
        onTabCloseRequested(last, ui.tabs_ms);

        if (ui.tabs_ms->count() > last) return false;
    }

    return true;
}


bool compile_visual::Build() {
    QWidget* current = ui.tabs_mil->currentWidget();
    QPlainTextEdit* milEditor = qobject_cast<QPlainTextEdit*>(current);

    if (!milEditor) {
        logMessage("Ошибка: Нет активного редактора MIL.");
        return false;
    }

    QString milPath = milEditor->property("filePath").toString();
    if (milPath.isEmpty()) {
        logMessage("Ошибка: Сначала сохраните файл .mil.");
        return false;
    }

    QFileInfo milInfo(milPath);
    QString msPath = milInfo.absolutePath() + "/" + milInfo.baseName() + ".ms";

        std::wstring content = milEditor->toPlainText().toStdWString() + L"\n";

    result.clear();

    fsm->start(content, 0);

    if (!result.errors.empty()) {
        logMessage("Сборка прервана: лексические ошибки.");
        runLexicalAnalysis();
        return false;
    }

        auto it = result.lexems.begin();
    while (it != result.lexems.end()) {
                        if ((*it)->getType() == Comment || (*it)->getType() == MultiComment) {
            delete* it;
            it = result.lexems.erase(it);
        }
        else {
            ++it;
        }
    }

        try {
        std::string finalCode;
        {
            std::stringstream codeStream;
            CodeGen codegen(codeStream);

            Parser p(result.lexems, &codegen);
            std::vector<std::pair<std::string, int>>* errors = p.parse();
            if (!errors->empty()) {
                ui.errorInfo->setPlainText("");
                int errorRow = 0;
                int errorToken = 0;
                for (const std::pair<std::string, int>& error : *errors) {
                    for(auto numFirst : result.lexemsRows) {
                        if (numFirst.second <= error.second) {
                            errorRow = numFirst.first;
                            errorToken = numFirst.second;
                        }
                        else {
                            break;
                        }
                    }
                    ui.errorInfo->appendPlainText(("Синтаксическая ошибка на строке " + std::to_string(errorRow) + ", токен " + std::to_string(error.second - errorToken + 1) + ": " + error.first).c_str());
                }
				ui.tabs_info->setCurrentIndex(1);
                return false;
            }

                                                
                        finalCode = codeStream.str();
        }

        if (finalCode.empty()) {
            logMessage("Ошибка: Генератор выдал пустой код. Проверьте логику Parser/CodeGen.");
            return false;
        }

        QString generatedCode = QString::fromStdString(finalCode);

                QFile msFile(msPath);
        if (msFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&msFile);
            out << generatedCode;
            out.flush();
            msFile.close();

            logMessage("Сборка успешна: " + milInfo.baseName() + ".ms");

                        bool found = false;
            for (int i = 0; i < ui.tabs_ms->count(); ++i) {
                if (ui.tabs_ms->widget(i)->property("filePath").toString() == msPath) {
                    QPlainTextEdit* msEditor = qobject_cast<QPlainTextEdit*>(ui.tabs_ms->widget(i));
                    if (msEditor) {
                        msEditor->blockSignals(true);
                        msEditor->setPlainText(generatedCode);
                        msEditor->blockSignals(false);
                        ui.tabs_ms->setTabText(i, milInfo.baseName() + ".ms");
                        ui.tabs_ms->setCurrentIndex(i);
                        found = true;
                    }
                    break;
                }
            }
            if (!found) openSingleFile(ui.tabs_ms, msPath);

            return true;
        }
        else {
            logMessage("Ошибка записи в файл: " + msPath);
            return false;
        }
    }
    catch (const std::exception& e) {
        logMessage(QString("Ошибка: %1").arg(e.what()));
        return false;
    }
}


void compile_visual::BuildAndRun() {
    if (Build()) {
        onRunTriggered();
    }
}
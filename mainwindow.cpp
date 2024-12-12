#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include "qfiledialog.h"
#include "syntaxhighlighter.h"
#include "QColorDialog"
#include"QTextStream"
#include"QMessageBox"
#include"QFontDialog"
#include "BookmarkManager.h"
#include<QDebug>
#include "codeeditor.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QDebug>
#include <QInputDialog>
#include <QTimer>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化 QTabWidget
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    tabWidget->setTabsClosable(true);

    // bookmarkManager = new BookmarkManager();

    // 初始化文本框状态
    textChanged = false;

    // 先调用新建文件
    on_actionNew_triggered();

    // 显示长度和行号
    statusLabel.setMaximumWidth(150);
    statusLabel.setText("Length: 0  Lines: 1");
    ui->statusbar->addPermanentWidget(&statusLabel);

    // // 设置作者
    QLabel *author = new QLabel(ui->statusbar);
    author->setText(tr("郭敏"));
    ui->statusbar->addPermanentWidget(author);

    // // 创建光标位置标签
    statusCursorLabel.setMaximumWidth(100);
    statusCursorLabel.setText("Ln: 1  Col: 1");
    ui->statusbar->addPermanentWidget(&statusCursorLabel);


    // bookmarkMenu = ui->bookmarkmenu;



    // // 创建一个 BookmarkManager 对象
    // bookmarkManager = new BookmarkManager(this);

    // 连接信号和槽
    // connect(bookmarkManager, &BookmarkManager::bookmarkChanged, [this]() {
    //     // 使用 bookmarkMenu 和当前 editor 来更新菜单
    //     bookmarkManager->updateBookmarkMenu(bookmarkMenu, codeEditor);
    // });

    // 连接信号槽：每次文本框内容更改时更新状态栏
    connect(ui->textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::on_textEdit_textChanged);

    // 连接光标位置变化信号
    connect(ui->textEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::on_textEdit_cursorPositionChanged);

    // 连接标签页关闭事件
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::on_tabWidget_tabCloseRequested);

    // 连接标签页切换事件
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    connect(ui->actionDark, &QAction::triggered, this, &MainWindow::on_actionDark_triggered);

    connect(ui->actionLight, &QAction::triggered, this, &MainWindow::on_actionLight_triggered);

    connect(ui->actionclear, &QAction::triggered, this, &MainWindow::on_actionClearHistory_triggered);

    connect(ui->actionadd, &QAction::triggered, this, &MainWindow::on_actionAddFavorite_triggered);
    connect(ui->actionopen, &QAction::triggered, this, &MainWindow::on_actionOpenFavorite_triggered);
    connect(ui->actionremove, &QAction::triggered, this, &MainWindow::on_actionRemoveFavorite_triggered);
    connect(ui->actionremoveall, &QAction::triggered, this, &MainWindow::on_actionClearFavorites_triggered);

    connect(ui->add, &QAction::triggered, this, &MainWindow::on_actionAdd_triggered);
    connect(ui->gotobookmark, &QAction::triggered, this, &MainWindow::on_actionGoTo_triggered);
    connect(ui->remove, &QAction::triggered, this, &MainWindow::on_actionRemove_triggered);
    connect(ui->removeall, &QAction::triggered, this, &MainWindow::on_actionRemove_triggered);

    autosaveTimer = new QTimer(this);
    connect(autosaveTimer, &QTimer::timeout, this, &MainWindow::autosaveFiles);
    autosaveTimer->start(300000);
}

void MainWindow::on_actionSavetemp_triggered()
{
    CodeEditor *editor = qobject_cast<CodeEditor *>(tabWidget->currentWidget());
    if (editor) {
        QString filePath = QFileDialog::getSaveFileName(this, "Save File");
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&file);
                out << editor->toPlainText();
                file.close();

                // 更新标签标题
                tabWidget->setTabText(tabWidget->currentIndex(), QFileInfo(filePath).fileName());

                // 删除对应的临时文件
                QDir autosaveDir(QDir::tempPath() + "/temp_autosave");
                QFile::remove(autosaveDir.filePath("autosave_tab_" + QString::number(tabWidget->currentIndex()) + ".txt"));
            }
        }
    }
}

void MainWindow::recoverUnsavedFiles()
{
    QDir autosaveDir(QDir::tempPath() + "/temp_autosave");
    if (!autosaveDir.exists()) {
        return;
    }

    QStringList autosaveFiles = autosaveDir.entryList(QStringList() << "*.txt", QDir::Files);
    if (autosaveFiles.isEmpty()) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Recover Files",
        "There are unsaved files from the previous session. Do you want to recover them?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        for (const QString &fileName : autosaveFiles) {
            QFile file(autosaveDir.filePath(fileName));
            if (file.open(QFile::ReadOnly | QFile::Text)) {
                QTextStream in(&file);
                QString content = in.readAll();
                file.close();

                CodeEditor *editor = new CodeEditor();
                editor->setPlainText(content);

                QString recoveredTabName = fileName;
                recoveredTabName.replace("autosave_tab_", "Recovered Tab ");
                tabWidget->addTab(editor, recoveredTabName);
                tabWidget->setCurrentWidget(editor);
            }
        }
    }

    // 清理恢复的文件
    for (const QString &fileName : autosaveFiles) {
        QFile::remove(autosaveDir.filePath(fileName));
    }
}



void MainWindow::autosaveFiles()
{
    QDir autosaveDir(QDir::tempPath() + "/temp_autosave");
    if (!autosaveDir.exists()) {
        autosaveDir.mkpath(".");
    }

    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeEditor *editor = qobject_cast<CodeEditor *>(tabWidget->widget(i));
        if (editor) {
            QString filePath = autosaveDir.filePath("autosave_tab_" + QString::number(i) + ".txt");
            QFile file(filePath);
            if (file.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&file);
                out << editor->toPlainText();
                file.close();
            }
        }
    }

    qDebug() << "Autosaved all open files.";
}


void MainWindow::on_actionAdd_triggered()
{
    // 获取当前活动的标签页和编辑器
    CodeEditor *currentEditor = qobject_cast<CodeEditor *>(tabWidget->currentWidget());
    if (!currentEditor) {
        QMessageBox::warning(this, "错误", "没有窗口或行可以添加书签");
        return;
    }

    int currentLine = currentEditor->textCursor().blockNumber();
    if (bookmarks[currentEditor].contains(currentLine)) {
        QMessageBox::information(this, "添加", "这行已经是书签了");
    } else {
        bookmarks[currentEditor].insert(currentLine);
        QMessageBox::information(this, "添加", "添加书签成功在: " + QString::number(currentLine + 1));
    }
}

void MainWindow::on_actionGoTo_triggered()
{
    // 获取当前活动的标签页和编辑器
    CodeEditor *currentEditor = qobject_cast<CodeEditor *>(tabWidget->currentWidget());
    if (!currentEditor) {
        QMessageBox::warning(this, "错误", "没有窗口或书签可以去往");
        return;
    }

    if (bookmarks[currentEditor].isEmpty()) {
        QMessageBox::information(this, "前往", "这个文件里没有书签");
        return;
    }

    // 转换书签为可选择的列表
    QStringList bookmarkList;
    for (int line : bookmarks[currentEditor]) {
        bookmarkList.append("Line " + QString::number(line + 1));
    }

    // 弹出对话框选择书签
    QString selectedBookmark = QInputDialog::getItem(this, "去到",
                                                     "选择一个书签:",
                                                     bookmarkList, 0, false);

    if (!selectedBookmark.isEmpty()) {
        int lineNumber = selectedBookmark.split(" ")[1].toInt() - 1;

        QTextCursor cursor = currentEditor->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber);
        currentEditor->setTextCursor(cursor);
    }
}

void MainWindow::on_actionRemove_triggered()
{
    // 获取当前活动的标签页和编辑器
    CodeEditor *currentEditor = qobject_cast<CodeEditor *>(tabWidget->currentWidget());
    if (!currentEditor) {
        QMessageBox::warning(this, "错误", "没有文件可以清除书签");
        return;
    }

    if (bookmarks[currentEditor].isEmpty()) {
        QMessageBox::information(this, "移除", "在这个文件里没有书签");
        return;
    }

    // 转换书签为可选择的列表
    QStringList bookmarkList;
    for (int line : bookmarks[currentEditor]) {
        bookmarkList.append("Line " + QString::number(line + 1));
    }

    // 弹出对话框选择书签
    QString selectedBookmark = QInputDialog::getItem(this, "移除书签",
                                                     "选择一个书签移除:",
                                                     bookmarkList, 0, false);

    if (!selectedBookmark.isEmpty()) {
        int lineNumber = selectedBookmark.split(" ")[1].toInt() - 1;
        bookmarks[currentEditor].remove(lineNumber);
        QMessageBox::information(this, "Info", "移除书签在: " + QString::number(lineNumber + 1));
    }
}

void MainWindow::on_actionRemoveAll_triggered()
{
    // 获取当前活动的标签页和编辑器
    CodeEditor *currentEditor = qobject_cast<CodeEditor *>(tabWidget->currentWidget());
    if (!currentEditor) {
        QMessageBox::warning(this, "错误", "没有文件可以清除书签");
        return;
    }

    if (bookmarks[currentEditor].isEmpty()) {
        QMessageBox::information(this, "移除所有", "在这个文件里没有书签");
        return;
    }

    bookmarks[currentEditor].clear();
    QMessageBox::information(this, "移除所有", "成功清除所有书签");
}


void MainWindow::on_actionAddFavorite_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择一个文件添加进收藏夹");
    if (!filePath.isEmpty()) {
        if (favorites.contains(filePath)) {
            QMessageBox::information(this, "添加", "这个文件已经在收藏夹里了");
        } else {
            favorites.insert(filePath);
            qDebug() << "Added to favorites:" << filePath;
        }
    }
}

void MainWindow::on_actionRemoveFavorite_triggered()
{
    // 将 QSet 转为 QStringList
    QStringList favoriteList = favorites.values();

    // 弹出对话框让用户选择需要移除的路径
    QString fileToRemove = QInputDialog::getItem(this, "移除收藏的文件",
                                                 "选择一个收藏去移除:",
                                                 favoriteList, 0, false);

    if (!fileToRemove.isEmpty()) {
        // 从集合中移除
        favorites.remove(fileToRemove);
        QMessageBox::information(this, "Favorite Removed", "成功移除: " + fileToRemove);
    }
}


void MainWindow::on_actionOpenFavorite_triggered()
{
    // 将 QSet 转为 QStringList
    QStringList favoriteList = favorites.values();

    // 弹出对话框让用户选择需要打开的路径
    QString fileToOpen = QInputDialog::getItem(this, "打开收藏夹",
                                               "选择文件打开:",
                                               favoriteList, 0, false);

    if (!fileToOpen.isEmpty()) {
        QFile file(fileToOpen);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "错误", "无法打开文件: " + fileToOpen);
            return;
        }

        // 打开文件内容并显示到新标签页
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        CodeEditor *editor = new CodeEditor();
        editor->setPlainText(content);

        tabWidget->addTab(editor, QFileInfo(fileToOpen).fileName());
        tabWidget->setCurrentWidget(editor);

        // 更新状态栏
        updateStatusBar(editor);
    }
}


void MainWindow::on_actionClearFavorites_triggered()
{
    favorites.clear();  // 清空收藏夹
    QMessageBox::information(this, "Info", "成功清空收藏夹");
    qDebug() << "无法清空收藏夹";
}

void MainWindow::initFavoriteMenu()
{
//     QAction *addFavoriteAction = new QAction("Add to Favorites", this);
//     connect(addFavoriteAction, &QAction::triggered, this, &MainWindow::on_actionAddFavorite_triggered);
//     ui->favorites->addAction(addFavoriteAction);

//     QAction *removeFavoriteAction = new QAction("Remove from Favorites", this);
//     connect(removeFavoriteAction, &QAction::triggered, this, &MainWindow::on_actionRemoveFavorite_triggered);
//     ui->favorites->addAction(removeFavoriteAction);

//     QAction *openFavoriteAction = new QAction("Open Favorite", this);
//     connect(openFavoriteAction, &QAction::triggered, this, &MainWindow::on_actionOpenFavorite_triggered);
//     ui->favorites->addAction(openFavoriteAction);

//     QAction *clearFavoritesAction = new QAction("Clear Favorites", this);
//     connect(clearFavoritesAction, &QAction::triggered, this, &MainWindow::on_actionClearFavorites_triggered);
//     ui->favorites->addAction(clearFavoritesAction);
}


QString darkStyle = R"(
    QPlainTextEdit {
        background-color: #2e2e2e;
        color: white;
        font-family: Consolas, monospace;
        font-size: 12px;
    }
    QPlainTextEdit:focus {
        border: 2px solid #3498db;
    }
)";

QString lightStyle = R"(
    QPlainTextEdit {
        background-color: white;
        color: black;
        font-family: Consolas, monospace;
        font-size: 12px;
    }
    QPlainTextEdit:focus {
        border: 2px solid #3498db;
    }
)";


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        SearchDialog dlg(this, editor);
        dlg.exec();
    }
}



void MainWindow::on_actionReplace_triggered()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        ReplaceDialog dlg(this, editor);
        dlg.exec();
    }
}


//新建文件
void MainWindow::on_actionNew_triggered()
{
    // 如果有未保存的编辑，提示用户保存
    if (!userEditConfirmed()) {
        return;
    }

    // 清空当前文件路径和更改状态
    filePath = "";
    textChanged = false;

    // 创建一个新的 CodeEditor 实例
    CodeEditor *editor = new CodeEditor();
    if (!editor) {
        qDebug() << "Failed to create a new editor!";
        return;
    }

    // 创建语法高亮器，并将其与编辑器关联
    SyntaxHighlighter *highlighter = new SyntaxHighlighter(editor->document());

    // 清空内容，设置文档并初始化
    editor->setPlainText("");
      // 确保文档正确初始化

    // 将新的编辑器添加到标签页中
    tabWidget->addTab(editor, "无标题");

    // 设置当前标签页为刚创建的标签页
    tabWidget->setCurrentWidget(editor);

    // 连接信号：更新状态栏
    connect(editor, &QPlainTextEdit::textChanged, this, &MainWindow::on_textEdit_textChanged);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::on_textEdit_cursorPositionChanged);

    // 更新窗口标题
    this->setWindowTitle(tr("新建文本文件 - 编辑器"));

    // 更新状态栏
    updateStatusBar(editor);  // 更新状态栏显示文件长度和行列数

}


void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open File", ".", "Text files (*.txt);;All files (*.*)");
    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "Unable to open file");
        return;
    }

    // 添加到最近文件列表
    addToRecentFiles(filename);

    // 创建一个新的文本编辑器
    CodeEditor *editor = new CodeEditor();
    QTextStream in(&file);
    QString text = in.readAll();
    editor->setPlainText(text);

    // 创建语法高亮器，并将其与编辑器关联
    SyntaxHighlighter *highlighter = new SyntaxHighlighter(editor->document());

    // 在新的标签页中添加文本编辑器
    tabWidget->addTab(editor, QFileInfo(filename).fileName());
    tabWidget->setCurrentWidget(editor);  // 切换到这个标签页

    // 保存当前文件路径
    filePath = filename;
    updateStatusBar(editor);  // 更新状态栏显示文件长度和行列数
    file.close();

    // 连接信号：更新状态栏
    connect(editor, &QPlainTextEdit::textChanged, this, &MainWindow::on_textEdit_textChanged);

    // 调用highlightBookmarks为打开的文件添加书签高亮
    highlightBookmarks(editor);
}





//保存文件
void MainWindow::on_actionSave_triggered()
{
    // 获取当前标签页的 QPlainTextEdit
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (!editor) {
        return;
    }

    // 判断文件路径是否为空，如果为空则表示是无标题的文件
    if (filePath.isEmpty()) {
        // 弹出保存对话框让用户选择保存路径
        QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files(*.txt)"));
        if (filename.isEmpty()) {
            return;  // 如果用户取消了保存，直接返回
        }
        filePath = filename;  // 更新文件路径
    }

    // 使用更新后的 filePath 保存文件
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "保存文件失败");
        return;
    }

    QTextStream out(&file);
    QString text = editor->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
    textChanged = false;
}





//另存文件
void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files(*.txt)"));
    if (filename.isEmpty()) {
        return;
    }

    // 获取当前标签页的 QPlainTextEdit
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (!editor) {
        return;
    }

    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "..", "保存文件失败");
        return;
    }

    QTextStream out(&file);
    QString text = editor->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());
    textChanged = false;
}



// 更新状态栏
void MainWindow::updateStatusBar(QPlainTextEdit *editor)
{
    // 获取当前文本编辑器的内容
    QString text = editor->toPlainText();
    int length = text.length();
    int lines = text.count("\n") + 1;

    // 更新状态栏中的文本信息
    statusLabel.setText(QString("Length: %1  Lines: %2").arg(length).arg(lines));

    // 获取光标位置
    QTextCursor cursor = editor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    statusCursorLabel.setText(QString("Ln: %1  Col: %2").arg(line).arg(col));

    // 调试输出
    qDebug() << "Status updated: Length = " << length << ", Lines = " << lines
             << ", Ln = " << line << ", Col = " << col;
}

// 光标位置变化时的槽函数
void MainWindow::on_textEdit_cursorPositionChanged()
{
    // 获取当前活动的文本编辑器
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (!editor) {
        return;  // 如果当前没有打开的编辑器，直接返回
    }

    // 更新状态栏
    updateStatusBar(editor);
}

// 文本内容更改时的槽函数
void MainWindow::on_textEdit_textChanged()
{
    // 获取当前活动的文本编辑器
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (!editor) {
        return;  // 如果当前没有打开的编辑器，直接返回
    }

    // 更新状态栏
    updateStatusBar(editor);
}


bool MainWindow::userEditConfirmed()
{



    if(textChanged){
        QString path=filePath!=""?filePath:"无标题.txt";
        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n")+"\""+path+"\"?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        //判断用户的选择
        int r=msg.exec();
        switch (r) {
        case QMessageBox::Yes:
            on_actionSave_triggered();
            break;
        case QMessageBox::No:
            textChanged=false;
            break;
        case QMessageBox::Cancel:
            break;
        }

    }

    return true;
}




void MainWindow::on_tabWidget_currentChanged(int index)
{
    // 获取当前标签页的 CodeEditor
    CodeEditor *editor = qobject_cast<CodeEditor*>(ui->tabWidget->currentWidget());

    // 确保行号区域显示或隐藏
    if (editor) {
        editor->showLineNumberArea(true); // 或根据需要调用 false
        updateBookmarkMenu(editor);
    }
}


//恢复操作
void MainWindow::on_actionUndo_triggered()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        editor->undo();
    }
}


//剪切操作
void MainWindow::on_actionCut_triggered()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        editor->cut();
        ui->actionPaste->setEnabled(true);
    }
}


//复制操作
void MainWindow::on_actionCopy_triggered()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        editor->copy();
        ui->actionPaste->setEnabled(true);
    }
}


//粘贴操作
void MainWindow::on_actionPaste_triggered()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        editor->paste();
    }
}


//撤销操作
void MainWindow::on_actionRedo_triggered()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        editor->redo();
    }
}

//判断是否恢复
void MainWindow::on_textEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}
//判断是否执行复制和剪切
void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);

}

//判断是否执行撤销
void MainWindow::on_textEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}





void MainWindow::on_actionFontBackgroundColor_triggered()
{
    // 打开颜色选择对话框
    QColor color = QColorDialog::getColor(Qt::white, this, "选择背景颜色");

    // 如果选择的颜色有效
    if (color.isValid()) {
        // 获取当前活动的文本编辑器
        QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
        if (editor) {
            QTextCursor cursor = editor->textCursor();

            // 如果没有选中任何文本，选择整个文本
            if (cursor.selectedText().isEmpty()) {
                cursor.select(QTextCursor::Document);
            }

            // 设置背景颜色
            QTextCharFormat format;
            format.setBackground(color);
            cursor.mergeCharFormat(format);

            // 更新文本编辑器的内容
            editor->setTextCursor(cursor);
        }
    }
}



void MainWindow::on_actionBackgroundColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if (color.isValid()) {
        // 获取当前活动的文本编辑器
        QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
        if (editor) {
            editor->setStyleSheet(QString("QPlainTextEdit {background-color:%1}").arg(color.name()));
        }
    }
}



void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if (color.isValid()) {
        // 获取当前活动的文本编辑器
        QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
        if (editor) {
            editor->setStyleSheet(QString("QPlainTextEdit {color:%1}").arg(color.name()));
        }
    }
}





void MainWindow::on_actionWrap_triggered()
{
    // 获取当前活动的文本编辑器
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        QPlainTextEdit::LineWrapMode mode = editor->lineWrapMode();
        if (mode == QTextEdit::NoWrap) {
            editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
            ui->actionWrap->setChecked(true);
        } else {
            editor->setLineWrapMode(QPlainTextEdit::NoWrap);
            ui->actionWrap->setChecked(false);
        }
    }
}





void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        // 获取当前活动的文本编辑器
        QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
        if (editor) {
            editor->setFont(font);
        }
    }
}



void MainWindow::on_actionToolbar_triggered()
{
    bool visible = ui->toolBar->isVisible();
    ui->toolBar->setVisible(!visible);
    ui->actionToolbar->setChecked(!visible);
}

void MainWindow::on_actionStatusBar_triggered()
{
    bool visible = ui->statusbar->isVisible();
    ui->statusbar->setVisible(!visible);
    ui->actionStatusBar->setChecked(!visible);
}

void MainWindow::on_actionSelectAll_triggered()
{
    // 获取当前活动的文本编辑器
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        editor->selectAll();
    }
}


void MainWindow::on_action_Exit_triggered()
{
    if(userEditConfirmed()){
        exit(0);
    }
}




void MainWindow::on_tabWidget_tabCloseRequested(int index)
{

        // 获取当前标签页的文本编辑器
        QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->widget(index));

        if (editor && textChanged) {
            // 提示用户保存文件
            QString path = filePath.isEmpty() ? "无标题.txt" : filePath;
            QMessageBox msg(this);
            msg.setIcon(QMessageBox::Question);
            msg.setText(QString("是否将更改保存到\n")+"\""+path+"\"?");
            msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            int r = msg.exec();
            if (r == QMessageBox::Yes) {
                on_actionSave_triggered(); // 保存文件
            } else if (r == QMessageBox::Cancel) {
                return; // 取消删除
            }
        }

        // 删除 tab
        tabWidget->removeTab(index);


}

void MainWindow::on_actionThemeToggle_triggered() {
    static bool isDarkMode = false;  // 当前模式

    // 切换模式
    isDarkMode = !isDarkMode;

    // 获取当前编辑器
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());
    if (editor) {
        // 根据当前模式设置样式
        editor->setStyleSheet(isDarkMode ? darkStyle : lightStyle);

        // 获取当前的高亮器并更新关键词颜色
        SyntaxHighlighter *highlighter = editor->findChild<SyntaxHighlighter*>();
        if (highlighter) {
            highlighter->setKeywordColor(isDarkMode ? Qt::cyan : Qt::blue); // 关键词颜色
        }
    }

    // 更新所有标签页的样式
    onTabChanged(tabWidget->currentIndex());
}


void MainWindow::onTabChanged(int index)
{
    // 获取当前标签页中的 QPlainTextEdit
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(tabWidget->currentWidget());

    if (editor) {
        // 根据当前模式设置样式
        if (isDarkMode) {
            editor->setStyleSheet(darkStyle);
        } else {
            editor->setStyleSheet(lightStyle);
        }

        // 更新书签高亮（如果需要）
        highlightBookmarks(editor);
    }
}


void MainWindow::on_actionDark_triggered() {
    isDarkMode = true;  // 更新当前模式

    // 设置 MainWindow 和标签页的样式
    setStyleSheet(darkStyle);
    onTabChanged(tabWidget->currentIndex());  // 更新当前标签页样式
}

// 切换到浅色模式
void MainWindow::on_actionLight_triggered() {
    isDarkMode = false;  // 更新当前模式

    // 设置 MainWindow 和标签页的样式
    setStyleSheet(lightStyle);
    onTabChanged(tabWidget->currentIndex());  // 更新当前标签页样式
}

void MainWindow::addToRecentFiles(const QString &filePath)
{
    if (!recentFiles.contains(filePath)) {
        recentFiles.prepend(filePath);  // 将文件添加到列表前面
        if (recentFiles.size() > maxRecentFiles) {
            recentFiles.removeLast();  // 如果文件数超过最大限制，移除最后一个
        }
    }
    updateRecentFilesMenu();  // 更新最近文件菜单
}


void MainWindow::updateRecentFilesMenu()
{
    // 获取所有的菜单项
    QList<QAction *> actions = ui->recentFilesMenu->actions();

    // 保留“清除历史”按钮，移除其他项
    QAction *clearHistoryAction = nullptr;
    for (QAction *action : actions) {
        if (action->text() == "清除历史") {
            clearHistoryAction = action;
            break;
        }
    }

    // 如果没有找到"清除历史"按钮，则创建并添加
    if (!clearHistoryAction) {
        clearHistoryAction = new QAction("清除历史", this);
        connect(clearHistoryAction, &QAction::triggered, this, &MainWindow::on_actionClearHistory_triggered);
        ui->recentFilesMenu->addAction(clearHistoryAction);
    }

    // 清除除了“清除历史”按钮之外的其他菜单项
    for (QAction *action : actions) {
        if (action != clearHistoryAction) {
            ui->recentFilesMenu->removeAction(action);
        }
    }

    // 添加新的最近文件菜单项
    for (const QString &file : recentFiles) {
        QAction *action = new QAction(file, this);
        connect(action, &QAction::triggered, this, &MainWindow::onRecentFileTriggered);
        ui->recentFilesMenu->addAction(action);
    }
}


void MainWindow::onRecentFileTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString filename = action->text();
        QFile file(filename);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "Error", "Unable to open file");
            return;
        }

        // 打开文件并显示
        CodeEditor *editor = new CodeEditor();
        QTextStream in(&file);
        QString text = in.readAll();
        editor->setPlainText(text);

        SyntaxHighlighter *highlighter = new SyntaxHighlighter(editor->document());
        editor->setDocument(new QTextDocument(this));

        // 在新的标签页中添加文本编辑器
        tabWidget->addTab(editor, QFileInfo(filename).fileName());
        tabWidget->setCurrentWidget(editor);
        filePath = filename;
        updateStatusBar(editor);
        file.close();

        // 连接信号：更新状态栏
        connect(editor, &QPlainTextEdit::textChanged, this, &MainWindow::on_textEdit_textChanged);
    }
}

void MainWindow::on_actionClearHistory_triggered()
{
    recentFiles.clear();
    updateRecentFilesMenu();  // 清空历史文件菜单
}

void MainWindow::setupMenus()
{

}


void MainWindow::highlightBookmarks(QPlainTextEdit *editor)
{
    // QTextDocument *document = editor->document();
    // QList<QTextEdit::ExtraSelection> extraSelections;

    // // 遍历所有行，并检查是否有书签
    // for (int i = 0; i < document->blockCount(); ++i) {
    //     if (bookmarkManager->getBookmarks().contains(i)) {
    //         QTextBlock block = document->findBlockByNumber(i);  // 获取行的QTextBlock

    //         QTextEdit::ExtraSelection selection;
    //         selection.format.setBackground(QBrush(QColor(139, 69, 19)));  // 设置背景色为棕色
    //         selection.cursor = QTextCursor(block);  // 设置光标为该行的开始

    //         // 将这个选项添加到列表中
    //         extraSelections.append(selection);
    //     }
    // }

    // // 设置额外的选项（背景色）
    // editor->setExtraSelections(extraSelections);  // 更新高亮显示
}

void MainWindow::addBookmark()
{
    QList<CodeEditor*> editors = findChildren<CodeEditor*>();
    for (CodeEditor *editor : editors) {
        if (editor) {
            QTextCursor cursor = editor->textCursor();
            int line = cursor.blockNumber();  // 获取光标所在行号，注意这里是从0开始的
            editor->onBookmarkAdded(line);  // 调用 CodeEditor 方法添加书签
        }
    }
}

void MainWindow::updateBookmarkMenu(QPlainTextEdit *editor)
{
    // if (!editor) {
    //     qDebug() << "Editor is null!";
    //     return;
    // }

    // // 清空现有菜单项
    // bookmarkMenu->clear();

    // // 获取当前编辑器的书签列表
    // QSet<int> currentBookmarks = bookmarkManager->getBookmarks(editor);
    // qDebug() << "Current bookmarks for this editor: " << currentBookmarks;

    // // 为每个书签添加菜单项
    // foreach (int line, currentBookmarks) {
    //     QAction *bookmarkAction = new QAction(QString("Bookmark Line %1").arg(line + 1), bookmarkMenu);
    //     connect(bookmarkAction, &QAction::triggered, [this, line, editor]() {
    //         bookmarkManager->goToBookmark(line, editor);  // 跳转到书签行
    //     });
    //     bookmarkMenu->addAction(bookmarkAction);  // 将书签添加到菜单
    // }
}




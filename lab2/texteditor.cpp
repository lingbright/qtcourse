#include "TextEditor.h"
#include "ui_TextEditor.h"
#include "AboutDialog.h"
#include "SearchDialog.h"
#include "ReplaceDialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QTimer>
#include <QRegularExpression>
#include <QTextBlock>

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::TextEditor),
    textEdit(new QTextEdit(this)),
    isModified(false),
    searchDirectionDown(true) // 默认向下查找
{
    ui->setupUi(this);
    setCentralWidget(textEdit);

    // 信号和槽连接
    connect(ui->actionNew, &QAction::triggered, this, &TextEditor::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &TextEditor::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &TextEditor::saveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &TextEditor::saveFileAs);
    connect(ui->actionCopy, &QAction::triggered, textEdit, &QTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, textEdit, &QTextEdit::paste);
    connect(ui->actionCut, &QAction::triggered, textEdit, &QTextEdit::cut);
    connect(ui->actionUndo, &QAction::triggered, this, &TextEditor::undoAction);
    connect(ui->actionRedo, &QAction::triggered, this, &TextEditor::redoAction);
    connect(ui->actionFind, &QAction::triggered, this, &TextEditor::findText);
    connect(ui->actionReplace, &QAction::triggered, this, &TextEditor::replaceText);
    connect(ui->actionAbout, &QAction::triggered, this, &TextEditor::showAbout);
    connect(textEdit, &QTextEdit::textChanged, this, &TextEditor::updateWindowTitle);
    connect(ui->actionFontSize, &QAction::triggered, this, &TextEditor::changeFont);
    connect(ui->actionFontColor, &QAction::triggered, this, &TextEditor::changeFontColor);
    connect(ui->actionBackgroundColor, &QAction::triggered, this, &TextEditor::changeBackgroundColor);
    connect(ui->actionToolBar, &QAction::triggered, this, &TextEditor::toggleToolBar);
    connect(ui->actionStatusBar, &QAction::triggered, this, &TextEditor::toggleStatusBar);
    connect(ui->actionCut, &QAction::triggered, this, &TextEditor::cutText);
    connect(ui->actionCopy, &QAction::triggered, this, &TextEditor::copyText);
    connect(ui->actionPaste, &QAction::triggered, this, &TextEditor::pasteText);
    connect(ui->actionWordWrap, &QAction::triggered, this, &TextEditor::toggleLineWrap);
    connect(ui->actioneditcolor, &QAction::triggered, this, &TextEditor::changeeditcolor);
    connect(ui->actionExit, &QAction::triggered, this, &TextEditor::exitApp);

    // 自动保存定时器
    QTimer *autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &TextEditor::autoSave);
    autoSaveTimer->start(60000); // 每60秒自动保存一次

    // 连接光标位置更新
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &TextEditor::updateCursorPosition);
}

void TextEditor::changeeditcolor() {
    QColor color = QColorDialog::getColor(textEdit->palette().color(QPalette::Base), this, "选择编辑器背景色");
    if (color.isValid()) {
        // 修改编辑器背景色
        textEdit->setStyleSheet("background-color: " + color.name() + ";");
    }
}

TextEditor::~TextEditor() {
    delete ui;
    delete textEdit;
}

void TextEditor::exitApp() {
    // 如果文件有修改，提示用户是否保存
    if (isModified && !confirmDiscardChanges()) {
        return; // 用户选择不退出
    }

    // 关闭应用程序
    QApplication::quit();
}


void TextEditor::newFile()
{
    if (isModified && !confirmDiscardChanges()) return;

    textEdit->clear();
    currentFile.clear();
    setWindowTitle("未命名 - 文本编辑器");
    isModified = false;
}

void TextEditor::openFile()
{
    if (isModified && !confirmDiscardChanges()) return;

    QString fileName = QFileDialog::getOpenFileName(this, "打开文件");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        currentFile = fileName;
        textEdit->setPlainText(file.readAll());
        file.close();
        setWindowTitle(fileName + " - 文本编辑器");
        isModified = false;
    }
}

void TextEditor::saveFile()
{
    if (currentFile.isEmpty()) {
        saveFileAs();
    } else {
        saveToFile(currentFile);
    }
}

void TextEditor::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "另存为");
    if (!fileName.isEmpty()) {
        saveToFile(fileName);
    }
}

void TextEditor::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        currentFile = fileName;
        file.write(textEdit->toPlainText().toUtf8());
        file.close();
        isModified = false;
        setWindowTitle(currentFile + " - 文本编辑器");
    }
}

void TextEditor::clearHighlights() {
    QTextDocument *doc = textEdit->document();
    QTextCursor cursor(doc);

    // 设置清除高亮的格式（透明背景）
    QTextCharFormat clearFormat;
    clearFormat.setBackground(Qt::transparent);

    // 遍历文档中的每个文本块（block），并清除高亮
    cursor.beginEditBlock();  // 开始一个编辑块，以便批量修改
    QTextBlock block = doc->begin();
    while (block.isValid()) {
        cursor.setPosition(block.position());
        cursor.movePosition(QTextCursor::NextBlock);  // 移动到下一块文本

        cursor.select(QTextCursor::BlockUnderCursor);  // 选择当前文本块
        cursor.mergeCharFormat(clearFormat);  // 清除高亮
        block = block.next();  // 获取下一个文本块
    }
    cursor.endEditBlock();  // 结束编辑块
}



void TextEditor::findText() {
    clearHighlights(); // 清除高亮
    SearchDialog dialog(this);

    connect(&dialog, &SearchDialog::findText, this, [this](const QString &searchText, bool caseSensitive, bool searchDown) {
        QTextDocument::FindFlags flags;
        if (caseSensitive) {
            flags |= QTextDocument::FindCaseSensitively;
        }

        QTextCursor cursor = textEdit->textCursor();

        if (searchDown) {
            // 从当前光标位置查找下一个
            if (textEdit->find(searchText, flags)) {
                cursor = textEdit->textCursor();
                QTextCharFormat highlightFormat;
                highlightFormat.setBackground(Qt::yellow);
                cursor.mergeCharFormat(highlightFormat);
                textEdit->setTextCursor(cursor);
            } else {
                QMessageBox::information(this, "查找结果", "没有找到匹配项。");
            }
        } else {
            // 从当前光标位置向上查找
            cursor.movePosition(QTextCursor::Start);
            if (textEdit->find(searchText, flags | QTextDocument::FindBackward)) {
                cursor = textEdit->textCursor();
                QTextCharFormat highlightFormat;
                highlightFormat.setBackground(Qt::yellow);
                cursor.mergeCharFormat(highlightFormat);
                textEdit->setTextCursor(cursor);
            } else {
                QMessageBox::information(this, "查找结果", "没有找到匹配项。");
            }
        }
    });

    dialog.exec();
}

void TextEditor::replaceText() {
    clearHighlights(); // 清除高亮
    ReplaceDialog dialog(this);

    connect(&dialog, &ReplaceDialog::rreplaceText, this, [this](const QString &searchText, const QString &replaceText, bool caseSensitive, bool replaceAll, bool searchDown) {
        QTextDocument::FindFlags flags;
        if (caseSensitive) {
            flags |= QTextDocument::FindCaseSensitively;
        }

        QTextCursor cursor = textEdit->textCursor();

        if (replaceAll) {
            cursor.movePosition(QTextCursor::Start); // 从文档开始查找
            bool found = textEdit->find(searchText, flags); // 查找第一个匹配项
            while (found) {
                cursor = textEdit->textCursor(); // 获取当前光标
                cursor.insertText(replaceText); // 替换找到的文本
                clearHighlights(); // 清除高亮

                // 每次替换后，将光标移回文档开头，继续查找下一个匹配项
                textEdit->moveCursor(QTextCursor::Start);  // 确保光标回到开头
                found = textEdit->find(searchText, flags); // 查找下一个匹配项
            }
            // 替换完毕后退出
            QMessageBox::information(this, "替换完毕", "已替换所有匹配项。");
            return;
        }

        // 以下是处理查找下一个和向上查找的逻辑
        if (searchDown) {
            // 查找下一个匹配项
            if (textEdit->find(searchText, flags)) {
                cursor = textEdit->textCursor();
                if (!replaceText.isEmpty()) {
                    cursor.insertText(replaceText); // 替换当前匹配项
                    clearHighlights(); // 清除高亮
                    cursor.movePosition(QTextCursor::NextCharacter);
                    textEdit->setTextCursor(cursor); // 更新光标
                }
            } else {
                QMessageBox::information(this, "查找结果", "没有找到匹配项。");
            }
        } else {
            // 向上查找
            cursor.movePosition(QTextCursor::Start);
            if (textEdit->find(searchText, flags | QTextDocument::FindBackward)) {
                cursor = textEdit->textCursor();
                if (!replaceText.isEmpty()) {
                    cursor.insertText(replaceText); // 替换当前匹配项
                    clearHighlights(); // 清除高亮
                    cursor.movePosition(QTextCursor::NextCharacter); // 移动光标到下一个位置
                    textEdit->setTextCursor(cursor); // 更新光标
                }
            } else {
                QMessageBox::information(this, "查找结果", "没有找到匹配项。");
            }
        }
    });

    dialog.exec();
}





void TextEditor::changeFont() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
    if (ok) {
        textEdit->setFont(font);
    }
}

void TextEditor::changeFontColor() {
    QColor color = QColorDialog::getColor(textEdit->textColor(), this);
    if (color.isValid()) {
        QTextCharFormat format;
        format.setForeground(color);
        QTextCursor cursor = textEdit->textCursor();
        cursor.mergeCharFormat(format);
        textEdit->setTextCursor(cursor);  // 更新光标，应用颜色
    }
}

void TextEditor::changeBackgroundColor() {
    QColor color = QColorDialog::getColor(textEdit->textBackgroundColor(), this);
    if (color.isValid()) {
        QTextCharFormat format;
        format.setBackground(color);
        QTextCursor cursor = textEdit->textCursor();
        cursor.mergeCharFormat(format);
        textEdit->setTextCursor(cursor);  // 更新光标，应用背景色
    }
}

void TextEditor::toggleToolBar() {
    ui->toolBar->setVisible(!ui->toolBar->isVisible());
}

void TextEditor::toggleStatusBar() {
    statusBar()->setVisible(!statusBar()->isVisible());
}

void TextEditor::autoSave() {
    if (!currentFile.isEmpty() && isModified) {
        saveToFile(currentFile);
    }
}

void TextEditor::updateWindowTitle() {
    setWindowTitle(currentFile + " - 文本编辑器" + (isModified ? "*" : ""));
}

void TextEditor::showAbout() {
    AboutDialog dialog(this);
    dialog.exec();
}

void TextEditor::updateCursorPosition() {
    QTextCursor cursor = textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int column = cursor.columnNumber() + 1;
    statusBar()->showMessage(QString("行: %1, 列: %2").arg(line).arg(column));
}

void TextEditor::undoAction() {
    textEdit->undo();
}

void TextEditor::redoAction() {
    textEdit->redo();
}

void TextEditor::cutText() {
    textEdit->cut();
}

void TextEditor::copyText() {
    textEdit->copy();
}

void TextEditor::pasteText() {
    textEdit->paste();
}

void TextEditor::toggleLineWrap() {
    textEdit->setLineWrapMode(textEdit->lineWrapMode() == QTextEdit::WidgetWidth ? QTextEdit::NoWrap : QTextEdit::WidgetWidth);
}

bool TextEditor::confirmDiscardChanges() {
    auto reply = QMessageBox::warning(this, tr("确认"), tr("文件尚未保存，确定要丢弃更改吗？"), QMessageBox::Yes | QMessageBox::No);
    return reply == QMessageBox::Yes;
}

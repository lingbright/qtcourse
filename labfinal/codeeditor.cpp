
#include "codeeditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QPlainTextEdit>


CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    // 创建行号区域
    lineNumberArea = new LineNumberArea(this);
    highlighter = new SyntaxHighlighter(this->document());

    // 连接信号与槽，更新行号区域
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // 确保在构造函数中初始化
    // bookmarkManager = new BookmarkManager(this);  // 初始化 bookmarkManager
    // bookmarkMenu = new QMenu(this);  // 初始化 bookmarkMenu

    // CodeEditor 类中的连接操作
    // connect(bookmarkManager, &BookmarkManager::bookmarkChanged, this, &CodeEditor::updateBookmarkMenu);
}


// 计算行号区域的宽度
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

// 更新行号区域的宽度
void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

// 更新行号区域
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

// 切换行号区域的显示状态
void CodeEditor::showLineNumberArea(bool flag)
{
    if (flag) {
        // 显示行号区域
        lineNumberArea->show();
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    } else {
        // 隐藏行号区域
        lineNumberArea->hide();
        setViewportMargins(0, 0, 0, 0);
    }
}


// 重写 resizeEvent，确保行号区域跟随文本区域调整
void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

// 高亮当前行
void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    // 高亮当前行
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);

}




// 绘制行号区域
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::mousePressEvent(QMouseEvent *event)
{
    QTextCursor cursor = cursorForPosition(event->pos());
    QTextDocument *document = this->document();

    // 获取鼠标点击位置的文本
    QString text = cursor.block().text();

    // 使用正则表达式匹配是否为超链接
    QRegularExpression linkRegex(R"((https?|ftp)://[^\s/$.?#].[^\s]*|mailto:[^\s@]+@[^\s@]+\.[^\s@]+)");
    QRegularExpressionMatch match = linkRegex.match(text);

    if (match.hasMatch()) {
        QString link = match.captured(0);  // 获取捕获的链接
        QDesktopServices::openUrl(QUrl(link));  // 用默认浏览器打开链接
    } /* else {
        int line = cursor.blockNumber();  // 获取鼠标点击位置的行号

        // 检查是否点击了书签
        if (bookmarkManager->getBookmarks().contains(line)) {
            // 如果已经是书签，则移除
            qDebug() << "Removing bookmark at line:" << line;
            bookmarkManager->removeBookmark(line);
        } else {
            // 否则添加书签
            qDebug() << "Adding bookmark at line:" << line;
            bookmarkManager->addBookmark(line);
        }
    } */

    highlightBookmarks();

    // 调用父类的 mousePressEvent 保持默认行为
    QPlainTextEdit::mousePressEvent(event);
}



void CodeEditor::highlightBookmarks()
{
    // // 获取文档
    // QTextDocument *document = this->document();

    // // 清空现有的高亮，避免重复添加
    // QList<QTextEdit::ExtraSelection> extraSelections;

    // // 高亮书签所在的行
    // for (int i = 0; i < document->blockCount(); ++i) {
    //     if (bookmarkManager->getBookmarks().contains(i)) {
    //         QTextBlock block = document->findBlockByNumber(i);  // 获取书签所在的行

    //         QTextEdit::ExtraSelection bookmarkSelection;
    //         bookmarkSelection.format.setBackground(QBrush(QColor(139, 69, 19)));  // 设置书签行的背景色为棕色

    //         // 获取光标并移动到行的起始位置
    //         QTextCursor cursor(block);
    //         cursor.movePosition(QTextCursor::StartOfBlock);  // 移动光标到该行开始
    //         int start = cursor.position();  // 获取行起始位置

    //         cursor.movePosition(QTextCursor::EndOfBlock);  // 移动光标到该行末尾
    //         int end = cursor.position();    // 获取行结束位置

    //         // 设置选区（确保结束位置不超出文档内容）
    //         bookmarkSelection.cursor.setPosition(start);
    //         bookmarkSelection.cursor.setPosition(end, QTextCursor::KeepAnchor);  // 保持选择状态

    //         // 打印调试信息：书签的起始和结束位置
    //         qDebug() << "Bookmark range: " << start << " to " << end;

    //         extraSelections.append(bookmarkSelection);
    //     }
    // }

    // // 如果有高亮选区，更新编辑器中的所有高亮
    // if (!extraSelections.isEmpty()) {
    //     setExtraSelections(extraSelections);
    //     qDebug() << "Extra selections updated";
    // }
    // else {
    //     qDebug() << "No bookmarks to highlight";
    // }
}

void CodeEditor::onBookmarkAdded(int line)
{
    // bookmarkManager->addBookmark(this, line);  // 使用当前编辑器（标签页）添加书签
    // updateBookmarkMenu();  // 更新菜单
}


void CodeEditor::onBookmarkRemoved(int line)
{
    // bookmarkManager->removeBookmark(this, line);  // 使用当前编辑器（标签页）移除书签
    // updateBookmarkMenu();  // 更新菜单
}

void CodeEditor::updateBookmarkMenu()
{
    // bookmarkManager->updateBookmarkMenu(bookmarkMenu, this);  // 更新当前标签页的书签菜单
}





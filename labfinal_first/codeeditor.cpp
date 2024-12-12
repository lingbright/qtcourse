
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


#include "bookmarkmanager.h"
#include <QTextCursor>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QMenu>
#include <QAction>

BookmarkManager::BookmarkManager(QObject *parent) : QObject(parent) {}

void BookmarkManager::clearBookmarks(QPlainTextEdit *editor)
{
    if (editor) {
        bookmarks.remove(editor);  // 清除指定编辑器的书签
        emit bookmarkChanged(editor);  // 通知书签发生了变化
    }
}

void BookmarkManager::addBookmark(QPlainTextEdit *editor, int line)
{
    if (editor) {
        bookmarks[editor].insert(line);  // 为指定编辑器添加书签
        qDebug() << "Adding bookmark at line:" << line << "for editor:" << editor;
        emit bookmarkChanged(editor);  // 通知书签发生变化
    }
}


void BookmarkManager::removeBookmark(QPlainTextEdit *editor, int line)
{
    if (editor) {
        bookmarks[editor].remove(line);  // 从指定编辑器中移除书签
        emit bookmarkChanged(editor);  // 通知书签发生了变化
    }
}

QSet<int> BookmarkManager::getBookmarks(QPlainTextEdit *editor) const
{
    return editor ? bookmarks.value(editor) : QSet<int>();  // 返回指定编辑器的书签
}

void BookmarkManager::updateBookmarkMenu(QMenu *bookmarkMenu, QPlainTextEdit *editor)
{
    if (!bookmarkMenu || !editor) {
        qDebug() << "bookmarkMenu or editor is null!";
        return;
    }

    // 清空现有菜单项
    bookmarkMenu->clear();

    // 获取当前编辑器的书签列表
    QSet<int> currentBookmarks = getBookmarks(editor);
    qDebug() << "Updating menu for editor:" << editor << "with bookmarks:" << currentBookmarks;

    foreach (int line, currentBookmarks) {
        qDebug() << "Adding bookmark for line:" << line + 1;  // 输出正确的行号

        QAction *bookmarkAction = new QAction(QString("Bookmark Line %1").arg(line + 1), bookmarkMenu);
        connect(bookmarkAction, &QAction::triggered, [this, line, editor]() {
            goToBookmark(line, editor);  // 跳转到对应书签
        });
        bookmarkMenu->addAction(bookmarkAction);  // 添加书签到菜单
    }
    qDebug() << "Menu updated. Number of actions:" << bookmarkMenu->actions().size();
}



void BookmarkManager::goToBookmark(int line, QPlainTextEdit *editor)
{
    qDebug() << "Jumping to bookmark at line:" << line;

    if (!editor) {
        qDebug() << "Editor is null!";
        return;
    }

    // 确保行号在有效范围内
    if (line < 0 || line >= editor->document()->blockCount()) {
        qDebug() << "Invalid line number for jump!";
        return;
    }

    if (getBookmarks(editor).contains(line)) {
        qDebug() << "Bookmark found for line:" << line;

        QTextBlock block = editor->document()->findBlockByNumber(line);
        if (block.isValid()) {
            QTextCursor cursor(block);
            editor->setTextCursor(cursor);  // 设置光标到目标位置
            editor->ensureCursorVisible();  // 确保光标可见
            qDebug() << "Successfully jumped to bookmark at line: " << line;
        } else {
            qDebug() << "Invalid block for line: " << line;
        }
    } else {
        qDebug() << "Bookmark not found for line: " << line;
    }
}

#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <QObject>
#include <QSet>
#include <QMenu>
#include <QAction>
#include <QPlainTextEdit>

class BookmarkManager : public QObject
{
    Q_OBJECT

public:
    explicit BookmarkManager(QObject *parent = nullptr);
    void updateBookmarkMenu(QMenu *bookmarkMenu, QPlainTextEdit *editor); // 用于更新菜单

     QMap<QPlainTextEdit*, QSet<int>> bookmarks;

public slots:
    void clearBookmarks(QPlainTextEdit *editor);
    void addBookmark(QPlainTextEdit *editor, int line);
    void removeBookmark(QPlainTextEdit *editor, int line);
    QSet<int> getBookmarks(QPlainTextEdit *editor) const;
    void goToBookmark(int line, QPlainTextEdit *editor);
signals:

    void bookmarkChanged(QPlainTextEdit* editor);

private:

};

#endif // BOOKMARKMANAGER_H

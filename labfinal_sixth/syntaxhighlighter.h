#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QColor>
#include <QString>
#include <QTextCharFormat>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *document);

    void setKeywordColor(const QColor &color);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat linkFormat;  // 用于超链接的格式化

    QStringList keywordPatterns;  // 用于存储语言的关键字模式

    void initKeywordPatterns();  // 初始化关键字模式
};

#endif // SYNTAXHIGHLIGHTER_H

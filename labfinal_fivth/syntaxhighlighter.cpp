#include "syntaxhighlighter.h"

// 构造函数
SyntaxHighlighter::SyntaxHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{
    initKeywordPatterns();  // 初始化关键字模式

    // 设置关键字的格式化
    keywordFormat.setForeground(Qt::blue);
    commentFormat.setForeground(Qt::green);
    stringFormat.setForeground(Qt::darkRed);
    numberFormat.setForeground(Qt::darkMagenta);
    linkFormat.setForeground(Qt::darkCyan);
}

// 初始化关键字模式
void SyntaxHighlighter::initKeywordPatterns()
{
    keywordPatterns << "\\bif\\b" << "\\belse\\b" << "\\bwhile\\b"
                    << "\\bfor\\b" << "\\breturn\\b" << "\\bint\\b"
                    << "\\bfloat\\b" << "\\bdouble\\b" << "\\bchar\\b";
}

// 设置关键字颜色
void SyntaxHighlighter::setKeywordColor(const QColor &color)
{
    keywordFormat.setForeground(color);
}

// 处理语法高亮
void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // 高亮关键字
    foreach (const QString &pattern, keywordPatterns) {
        QRegularExpression regex(pattern);
        QRegularExpressionMatchIterator matchIterator = regex.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
        }
    }

    // 高亮注释（示例：以 "//" 开头的注释）
    QRegularExpression commentRegex("//[^\n]*");
    QRegularExpressionMatchIterator commentIterator = commentRegex.globalMatch(text);
    while (commentIterator.hasNext()) {
        QRegularExpressionMatch match = commentIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }

    // 高亮字符串（示例：用双引号括起来的字符串）
    QRegularExpression stringRegex("\".*\"");  // 匹配字符串
    QRegularExpressionMatchIterator stringIterator = stringRegex.globalMatch(text);
    while (stringIterator.hasNext()) {
        QRegularExpressionMatch match = stringIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), stringFormat);
    }

    // 高亮数字（示例：整数和浮点数）
    QRegularExpression numberRegex("\\b[0-9]+\\b");
    QRegularExpressionMatchIterator numberIterator = numberRegex.globalMatch(text);
    while (numberIterator.hasNext()) {
        QRegularExpressionMatch match = numberIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), numberFormat);
    }
    // 高亮超链接
    QRegularExpression linkRegex(R"((https?|ftp)://[\S]+)");
    QRegularExpressionMatchIterator linkIterator = linkRegex.globalMatch(text);
    QTextCharFormat linkFormat;
    linkFormat.setForeground(Qt::blue);        // 设置字体颜色为蓝色
    linkFormat.setFontUnderline(true);        // 添加下划线
    linkFormat.setUnderlineColor(Qt::blue);  // 设置下划线颜色为蓝色

    while (linkIterator.hasNext()) {
        QRegularExpressionMatch match = linkIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), linkFormat);
    }
}

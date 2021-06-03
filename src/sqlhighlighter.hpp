#ifndef SQLHIGHLIGHTER_HPP_INCLUDED
#define SQLHIGHLIGHTER_HPP_INCLUDED

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QVector>

class QString;
class QTextDocument;

/* Simple SQL highlighter. Highlights only keywords,
 * numbers, strings and comments. */
class SQLHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<Rule> rules;

public:
    SQLHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text);
};

#endif

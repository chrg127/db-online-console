#include <QSyntaxHighlighter>
#include <QString>
#include <QTextDocument>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QVector>

/* this thing only highlights keywords */

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


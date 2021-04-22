#include "sqlhighlighter.hpp"

#include <QString>

#define raw(word) QStringLiteral("\\b" word "\\b")

static const QString keywords[] = {
    raw("access"), raw("add"), raw("as"), raw("asc"), raw("begin"), raw("by"), raw("case"),
    raw("check"), raw("cluster"), raw("column"), raw("cache"), raw("compress"), raw("connect"), raw("current"),
    raw("cursor"), raw("decimal"), raw("default"), raw("desc"), raw("else"), raw("elsif"), raw("end"),
    raw("exception"), raw("exclusive"), raw("file"), raw("for"), raw("from"), raw("function"), raw("group"),
    raw("having"), raw("identified"), raw("if"), raw("immediate"), raw("increment"), raw("index"), raw("initial"),
    raw("initrans"), raw("into"), raw("is"), raw("level"), raw("link"), raw("logging"), raw("loop"),
    raw("maxextents"), raw("maxtrans"), raw("mode"), raw("modify"), raw("monitoring"), raw("nocache"), raw("nocompress"),
    raw("nologging"), raw("noparallel"), raw("nowait"), raw("of"), raw("offline"), raw("on"), raw("online"),
    raw("start"), raw("parallel"), raw("successful"), raw("synonym"), raw("table"), raw("tablespace"), raw("then"),
    raw("to"), raw("trigger"), raw("uid"), raw("unique"), raw("user"), raw("validate"), raw("values"),
    raw("view"), raw("when"), raw("whenever"), raw("where"), raw("with"), raw("option"), raw("order"),
    raw("pctfree"), raw("pctused"), raw("privileges"), raw("procedure"), raw("public"), raw("resource"), raw("return"),
    raw("row"), raw("rowlabel"), raw("rownum"), raw("rows"), raw("session"), raw("share"), raw("size"),
    raw("smallint"), raw("type"), raw("using"), raw("join"), raw("cross"), raw("inner"), raw("outer"),
    raw("left"), raw("right"), raw("analyze"), raw("audit"), raw("comment"), raw("commit"), raw("delete"), raw("drop"),
    raw("execute"), raw("explain"), raw("grant"), raw("lock"), raw("noaudit"), raw("rename"), raw("revoke"), raw("rollback"),
    raw("savepoint"), raw("set"), raw("truncate"), raw("create"), raw("update"), raw("alter"), raw("select"),
    raw("insert"), raw("contained"),
};

static const QString operators[] = {
    raw("\\="), raw("true"), raw("false"), raw("\\."), raw("null"),
    raw("not"), raw("and"), raw("or"), raw("in"), raw("any"), raw("some"),
    raw("all"), raw("between"), raw("exists"), raw("like"), raw("escape"),
    raw("union"), raw("intersect"), raw("minus"), raw("prior"), raw("distinct"),
    raw("sysdate"), raw("out")
};

static const QString types[] = {
    raw("bfile"), raw("blob"), raw("boolean"), raw("char"), raw("character"), raw("clob"), raw("date"), raw("datetime"),
    raw("dec"), raw("decimal"), raw("float"), raw("int"), raw("integer"), raw("long"), raw("mlslabel"), raw("nchar"),
    raw("nclob"), raw("number"), raw("numeric"), raw("nvarchar2"), raw("precision"), raw("raw"), raw("rowid"), raw("smallint"),
    raw("real"), raw("timestamp"), raw("urowid"), raw("varchar"), raw("varchar2"), raw("varray"),
};
#undef raw

SQLHighlighter::SQLHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // create a format in a one-liner
    const auto create_format = [](const QBrush &color, bool is_bold)
    {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        if (is_bold)
            fmt.setFontWeight(QFont::Bold);
        return fmt;
    };

    // add multiple rules in a one-liner
    const auto add_multi = [&, this](const auto &arr, const QBrush &color, bool is_bold)
    {
        const auto fmt = create_format(color, is_bold);
        for (const auto &elem : arr) {
            rules.append({
                .pattern = QRegularExpression(elem),
                .format = fmt
            });
        }
    };

    add_multi(keywords,  Qt::darkBlue, true);
    add_multi(operators, Qt::darkRed,  true);
    add_multi(types,     Qt::darkCyan, true);

    // from top to bottom: numbers, strings, comments
    // for string regex: https://stackoverflow.com/questions/171480/regex-grabbing-values-between-quotation-marks
#define regex(word) QRegularExpression(QStringLiteral(word))
    rules.append({ .pattern = regex("[0-9]"), .format  = create_format(Qt::darkMagenta, true) });
    rules.append({ .pattern = regex(R"((["'])(?:(?=(\\?))\2.)*?\1)"), .format = create_format(Qt::darkGreen, false) });
    rules.append({ .pattern = regex("--[^\n]*"), .format  = create_format(Qt::darkGray, false)   });
#undef regex
}

void SQLHighlighter::highlightBlock(const QString &text)
{
    for (const auto &rule : rules) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}


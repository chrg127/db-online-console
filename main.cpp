// java fa schifo!!

#include <string>
#include <QWidget>
#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSqlError>
#include <fmt/core.h>
#include "window.hpp"
#include "debug.hpp"

void test_query()
{
    QSqlQuery query;
    query.exec("select * from games");
    while (query.next()) {
        std::string name = query.value(0).toString().toStdString();
        fmt::print("model: {}\n", name);
    }
}

void test_query_model()
{
    QSqlQueryModel qmodel;
    qmodel.setQuery("select * from games");
    for (int i = 0; i < qmodel.rowCount(); i++) {
        QString name = qmodel.record(i).value("name").toString();
        fmt::print("model: {}\n", name.toStdString());
    }
}

void test_table()
{
    QSqlTableModel tab;
    tab.setTable("users");
    tab.setFilter("id >= 2");
    tab.setSort(2, Qt::DescendingOrder);
    tab.select();
    fmt::print("{}\n", tab.rowCount());
    for (int i = 0; i < tab.rowCount(); i++) {
        QString name = tab.record(i).value("name").toString();
        fmt::print("tab: {}\n", name.toStdString());
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Window window;
    window.adjustSize();
    window.move(QGuiApplication::primaryScreen()->availableGeometry().center() - window.rect().center());
    window.show();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("");
    db.setDatabaseName("test");
    db.setUserName("");
    db.setPassword("");
    bool ok = db.open();
    if (!ok) {
        error("{}\n", db.lastError().text().toStdString());
        return 1;
    }

    return app.exec();
}


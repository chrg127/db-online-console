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

/*
void run_query(mysqlpp::Connection &conn, const char *qstr)
{
    fmt::print("running: {}\n", qstr);
    mysqlpp::Query query = conn.query(qstr);
    mysqlpp::StoreQueryResult res = query.store();
    if (res) {
        for (const auto &row : res)
            fmt::print("\t{} {} {}\n", row[0], row[1], row[2]);
    } else
        fmt::print("query failed\n");
}

void run_query_games(mysqlpp::Connection &conn, const char *qstr)
{
    mysqlpp::Query query = conn.query(qstr);
    mysqlpp::StoreQueryResult res = query.store();
    if (!res) {
        fmt::print(stderr, "query failed\n");
        return;
    }
    fmt::print("{:30} {:30} {:30}\n", "name", "genre", "console");
    for (size_t i = 0; i < res.num_rows(); i++) {
        fmt::print("{:30} {:30} {:30}\n",
                res[i]["name"],
                res[i]["genre"],
                res[i]["console"]);
    }
}
*/

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

void db_test()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("");
    db.setDatabaseName("test");
    db.setUserName("");
    db.setPassword("");
    bool ok = db.open();
    if (!ok)
        error("{}\n", db.lastError().text().toStdString());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Window window;
    window.adjustSize();
    window.move(QGuiApplication::primaryScreen()->availableGeometry().center() - window.rect().center());
    window.show();

    return app.exec();
}


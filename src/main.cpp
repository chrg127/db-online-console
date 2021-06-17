// java fa schifo!!

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include "window.hpp"
#include "debug.hpp"

bool connect_db(const QString &table)
{
    auto s = [](const auto &p) { return p.toStdString(); };

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("");
    db.setDatabaseName(table);
    db.setUserName("chris");
    db.setPassword("mypass");
    bool ok = db.open();
    if (!ok)
        error("{}\n", s(db.lastError().text()));
    fmt::print("connection name: {}\n", s(db.connectionName()));
    fmt::print("connection opts: {}\n", s(db.connectOptions()));
    fmt::print("hostname: {}\n", s(db.hostName()));
    fmt::print("username: {}\n", s(db.userName()));
    fmt::print("password: {}\n", s(db.password()));
    return ok;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Window window;
    window.adjustSize();
    window.move(QGuiApplication::primaryScreen()->availableGeometry().center() - window.rect().center());
    window.show();

    connect_db("test");

    return app.exec();
}

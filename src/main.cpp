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
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("");
    db.setDatabaseName(table);
    db.setUserName("");
    db.setPassword("");
    bool ok = db.open();
    if (!ok)
        error("{}\n", db.lastError().text().toStdString());
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

// java fa schifo!!

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSurfaceFormat>
#include <QMessageBox>
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
    else {
        fmt::print("connection name: {}\n", s(db.connectionName()));
        fmt::print("connection opts: {}\n", s(db.connectOptions()));
        fmt::print("hostname: {}\n", s(db.hostName()));
        fmt::print("username: {}\n", s(db.userName()));
        fmt::print("password: {}\n", s(db.password()));
    }
    return ok;
}

void set_surface_format()
{
    QSurfaceFormat format;
    format.setSamples(16);
    format.setDepthBufferSize(16);
    QSurfaceFormat::setDefaultFormat(format);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    set_surface_format();

    Window window;
    window.adjustSize();
    window.move(QGuiApplication::primaryScreen()->availableGeometry().center() - window.rect().center());
    window.show();

    if (!connect_db("test")) {
        QMessageBox box;
        box.setText("Couldn't connect to the database. Quitting.");
        box.exec();
        return 1;
    }

    return app.exec();
}


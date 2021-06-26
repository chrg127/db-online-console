#include "database.hpp"

#include <QSqlDatabase>
#include <QSqlError>
#include <fmt/core.h>

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
        fmt::print(stderr, "{}\n", s(db.lastError().text()));
    return ok;
}

bool validate_user(const QString &user, const QString &password)
{
    return user == "user";
}

bool validate_admin(const QString &admin, const QString &password)
{
    return admin == "admin" && password == "password";
}

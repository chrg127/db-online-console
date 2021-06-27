#include "database.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

static const QString login_query = R"(
select *
from Utente u
where u.nome = '%1'
and u.cognome = '%2'
and u.password = '%3';
)";

static const QString search_game_query = R"(
select id, titolo as Titolo, genere as Genere, azienda as Azienda
from Videogioco vg
where vg.%1 like '%%%2%%';
)";

static const QString search_user_query = R"(
select *
from User u
where u.nome like '%%%1%%'
or u.cognome like '%%%1%%'
)";

static const QString game_id_query = R"(
select * from Videogioco where id = %1
)";

namespace db {

bool connect(const QString &table)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("");
    db.setDatabaseName(table);
    db.setUserName("chris");
    db.setPassword("mypass");
    bool ok = db.open();
    if (!ok)
        qDebug() << db.lastError().text();
    return ok;
}

bool validate_user(const QString &name, const QString &surname, const QString &password)
{
    // QSqlQuery query(login_query.arg(name).arg(surname).arg(password));
    // return (query.size() != 0);
    return true;
}

bool validate_admin(const QString &name, const QString &surname, const QString &password)
{
    return name == QString("admin") && surname == QString("admin") && password == QString("admin");
}

std::optional<QString> run_query(QSqlQueryModel &model, const QString &query)
{
    model.setQuery(query);
    QSqlError error = model.lastError();
    if (!error.isValid())
        return std::nullopt;
    return QString("Error found while executing the query:\n%1\n%2")
            .arg(error.driverText())
            .arg(error.databaseText());
}

void search_games(QSqlQueryModel &tofill, const QString &search_text, const QString &category)
{
    auto err = run_query(tofill, search_game_query.arg(category).arg(search_text));
    if (err)
        qDebug() << err.value();
}

void search_users(QSqlQueryModel &tofill, const QString &search_text)
{
    qDebug() << search_text;
}

GameInfo get_game_info(int id)
{
    GameInfo info;
    QSqlQuery query(game_id_query.arg(id));
    while (query.next()) {
#define USE(name, col) info.name = query.value(col).toString()
        USE(title, 1); USE(genre, 2); USE(year, 3);
        USE(company, 4); USE(director, 5);
        info.price = query.value(6).toInt();
#undef USE
    }
    return info;
}

UserInfo get_user_info(int id)
{
    qDebug() << id;
    return {};
}

void best_games(QSqlQueryModel &tofill)
{

}

void most_played_games(QSqlQueryModel &tofill)
{

}

} // namespace db

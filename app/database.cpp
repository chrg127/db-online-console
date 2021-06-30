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
select id, nome as Nome, cognome as Cognome
from Utente u
where u.nome like '%%%1%%'
or u.cognome like '%%%1%%'
)";

static const QString game_id_query = R"(
select * from Videogioco where id = %1
)";

static const QString user_id_query = R"(
select * from Utente where id = %1
)";

static const QString best_games_query = R"(
select id, vg.titolo as Titolo, vg.genere as Genere, vg.azienda as Azienda, count(id_usr) as Preferenza
from Preferenza p, Videogioco vg
where p.id_vg = vg.id
group by id_vg
order by Preferenza desc
limit 5;
)";

static const QString most_played_query = R"(
select id, vg.titolo as Titolo, vg.genere as Genere, vg.azienda as Azienda, count(p.ore_gioco) as Ore
from Partita p, Videogioco vg
where vg.id = p.id_vg
group by id_vg
order by Ore desc
limit 5;
)";

static const QString find_copy_query = R"(
select min(cv.id) as id, count(*) as count
from CopiaVideogioco cv
where cv.id_vg = %1
and cv.id not in (select id_copia from Acquisto);
)";

static const QString buy_copy_query = R"(
insert into Acquisto values(%1, %2, now());
)";

static const QString get_plans_query = "select * from Piano p where p.id_usr = %1;";
static const QString get_canc_query = R"(
select *
from Piano p, PianoCancellato pc
where p.id_usr = %1
and p.id = pc.id;
)";

static const QString curr_plan_query = R"(
select *
from Piano p
where p.id_usr = %1
where datediff(now(), p.data_fine) < 0
and p.id not in (select pc.id
                 from PianoCancellato pc);
)";

static const QString get_fav_query = R"(
select vg.id, vg.titolo as Titolo
from Videogioco vg, Preferenza p
where vg.id = p.id_vg and p.id_usr = %1
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

#define RUNQUERY(m, q) do { if (auto err = run_query((m), (q)); err) { qDebug() << *err; } } while (0)

void search_games(QSqlQueryModel &tofill, const QString &search_text, const QString &category)
{
    RUNQUERY(tofill, search_game_query.arg(category).arg(search_text));
}

void search_users(QSqlQueryModel &tofill, const QString &search_text)
{
    RUNQUERY(tofill, search_user_query.arg(search_text));
}

void best_games(QSqlQueryModel &tofill) { RUNQUERY(tofill, best_games_query); }
void most_played_games(QSqlQueryModel &tofill) { RUNQUERY(tofill, most_played_query); }
void get_favorites(QSqlQueryModel &tofill, int uid) { RUNQUERY(tofill, get_fav_query.arg(uid)); }

int validate_user(const QString &name, const QString &surname, const QString &password)
{
    return 2;
    // QSqlQuery query(login_query.arg(name).arg(surname).arg(password));
    // if (query.size() == 0)
    //     return -1;
    // query.first();
    // return query.value(0).toInt();
}

int validate_admin(const QString &name, const QString &surname, const QString &password)
{
    return name == QString("admin") && surname == QString("admin") && password == QString("admin") ? 0 : -1;
}

GameInfo get_game_info(int id)
{
    QSqlQuery query(game_id_query.arg(id));
    query.first();
#define USE(name, col) .name = query.value(col).toString()
    return {
        USE(title, 1), USE(genre, 2), USE(year, 3),
        USE(company, 4), USE(director, 5),
        .price = query.value(6).toInt(),
        .ncopies = get_copy_info(id).second,
    };
#undef USE
}

UserInfo get_user_info(int id)
{
    QSqlQuery query(user_id_query.arg(id));
    query.first();
    return {
        .name    = query.value(1).toString(),
        .surname = query.value(2).toString(),
        .daily_hours = 0,
        .total_hours = 0,
        .session_part = 0,
        .session_create = 0,
    };
}

PlanInfo get_curr_plan_info(int uid)
{
    return { .has_plan = false, .type = PlanType::YEAR, .start = QDate::currentDate(), .end = QDate::currentDate() };
}

std::pair<int, int> get_copy_info(int id)
{
    QSqlQuery query(find_copy_query.arg(id));
    query.first();
    return std::make_pair(query.value(0).toInt(), query.value(1).toInt());
}

bool buy_game(int id, int uid)
{
    auto [id_copy, count] = get_copy_info(id);
    if (count == 0)
        return false;
    QString s = buy_copy_query.arg(id_copy).arg(uid);
    qDebug() << s;
    // QSqlQuery buy_query(s);
    return true;
}

bool create_plan(int uid, PlanType type)
{
    qDebug() << uid;
    return true;
}

bool cancel_plan(int uid)
{
    qDebug() << uid;
    return true;
}

bool add_favorite(int uid, int vid)
{
    return false;
}

std::optional<QString> create_session(int vid, int uid, const std::vector<int> &uids, QDate date, int time)
{
    qDebug() << vid;
    qDebug() << "creatore:" << uid;
    for (auto x : uids)
        qDebug() << "partecipatore:" << x;
    qDebug() << date;
    qDebug() << time;
    return std::nullopt;
}

} // namespace db

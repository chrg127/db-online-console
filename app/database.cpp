#include "database.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTime>

static const QString login_query = R"(
select *
from Utente u
where u.nome = '%1'
and u.cognome = '%2'
and u.password = '%3';
)";

static const QString curr_plan_query = R"(
select id, id_usr, tipologia, data_acquisto, ora_acquisto, data_fine
from Piano p
where p.id_usr = %1
and datediff(now(), p.data_fine) < 0
and p.id not in (
    select pc.id
    from PianoCancellato pc
);
)";

static const QString create_plan_query = R"(
insert into Piano(id_usr, tipologia, data_acquisto, ora_acquisto, data_fine, ora_fine) values(%1, '%2', now(), now(), '%3', now());
)";

static const QString cancel_plan_query = "insert into PianoCancellato(id, data_canc, ora_canc) values(%1, now(), now());";

static const QString search_game_query = R"(
select id, titolo as Titolo, genere as Genere, azienda as Azienda
from Videogioco vg
where vg.%1 like '%%%2%%';
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

static const QString game_id_query = "select id, titolo, genere, anno, azienda, produttore, prezzo from Videogioco where id = %1";

static const QString find_copy_query = R"(
select min(cv.id) as id, count(*) as count
from CopiaVideogioco cv
where cv.id_vg = %1
and cv.id not in (select id_copia from Acquisto);
)";

static const QString buy_copy_query = "insert into Acquisto(id_copia, id_usr, data_acquisto, ora_acquisto) values(%1, %2, now(), now());";

static const QString search_user_query = R"(
select id, nome as Nome, cognome as Cognome
from Utente u
where u.nome like '%%%1%%'
or u.cognome like '%%%1%%'
)";

static const QString user_id_query = "select id, nome, cognome, password, email, telefono from Utente where id = %1";

static const QString daily_hours_query = R"(
select avg(p.ore_gioco) as ore_giornaliere
from Partita p
where p.id_usr = %1
group by p.data;
)";

static const QString total_hours_query = "select count(ore_gioco) as 'Ore totali' from Partita where id_usr = %1";

static const QString get_fav_query = R"(
select vg.id, vg.titolo as Titolo
from Videogioco vg, Preferenza p
where vg.id = p.id_vg and p.id_usr = %1
)";

static const QString check_fav_query = "select * from Preferenza p where p.id_usr = %1 and p.id_vg = %2;";
static const QString insert_fav_query = "insert into Preferenza(id_usr, id_vg) values(%1, %2);";

static const QString check_session_query = "select * from VideogiocoMultiplayer where id_vg = %1 and min_giocatori <= %2 and max_giocatori >= %2;";
static const QString create_session_query = "insert into Sessione(id_vg, id_creatore, data, ora, tempo_trascorso) values(%1, %2, now(), now(), %3);";
static const QString insert_partecipation_query = "insert into Partecipazione(id_usr, id_session) values(%1, last_insert_id());";

static const QString session_statistic_query = R"(
    select tb2.vg2, tb2.Titolo, ifnull(tb1.creat, 0) as Creazioni, tb2.part as Partecipazioni from (
        select vg.id as vg1, vg.titolo, count(s.id) as creat
        from Videogioco vg, Sessione s
        where vg.id = s.id_vg
        and s.id_creatore = %1
        group by vg.id
    ) as tb1 right join (
        select vg.id as vg2, vg.titolo as titolo, count(s.id) as part
        from Partecipazione p, Sessione s, Videogioco vg
        where p.id_session = s.id
        and s.id_vg = vg.id
        and p.id_usr = %1
        group by vg.id
    ) as tb2 on vg1 = vg2
)";

static const QString monthly_profit_query = R"(
select sum(profits) from (
    select sum(vg.prezzo) as profits
    from CopiaVideogioco cv, Acquisto a, Videogioco vg
    where a.id_copia = cv.id and vg.id = cv.id_vg
    and extract(year_month from a.data_acquisto) = '%1'

    union

    select sum(costo)
    from Piano p, TipologiaPiano tp
    where p.tipologia = tp.tipologia
    and extract(year_month from p.data_acquisto) = '%1'
) as tb;
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



/* actual operations ahead */

int validate_user(const QString &name, const QString &surname, const QString &password)
{
    return 10;
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

std::optional<PlanInfo> get_curr_plan_info(int uid)
{
    QSqlQuery query(curr_plan_query.arg(uid));
    if (query.size() == 0)
        return std::nullopt;
    query.first();
    return PlanInfo{
        .id = query.value(0).toInt(),
        .type = string_to_plan(query.value(2).toString()),
        .start = query.value(3).toDate(),
        .end = query.value(5).toDate(),
        .start_time = query.value(4).toDateTime(),
    };
}

bool create_plan(int uid, PlanType type)
{
    if (get_curr_plan_info(uid))
        return false;
    QDate end = plan_date_end(QDate::currentDate(), type);
    auto q = create_plan_query
                    .arg(uid)
                    .arg(plan_to_string(type))
                    .arg(end.toString(Qt::ISODate));
    // qDebug() << q;
    QSqlQuery query(q);
    return true;
}

bool cancel_plan(int planid)
{
    auto q = cancel_plan_query.arg(planid);
    // qDebug() << q;
    QSqlQuery query(q);
    return true;
}

void search_games(QSqlQueryModel &tofill, const QString &search_text, const QString &category)
{
    RUNQUERY(tofill, search_game_query.arg(category).arg(search_text));
}

void best_games(QSqlQueryModel &tofill)
{
    RUNQUERY(tofill, best_games_query);
}

void most_played_games(QSqlQueryModel &tofill)
{
    RUNQUERY(tofill, most_played_query);
}

std::pair<int, int> get_copy_info(int id)
{
    QSqlQuery query(find_copy_query.arg(id));
    query.first();
    return std::make_pair(query.value(0).toInt(), query.value(1).toInt());
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

bool buy_game(int id, int uid)
{
    auto [id_copy, count] = get_copy_info(id);
    if (count == 0)
        return false;
    QString s = buy_copy_query.arg(id_copy).arg(uid);
    // qDebug() << s;
    QSqlQuery buy_query(s);
    return true;
}

void search_users(QSqlQueryModel &tofill, const QString &search_text)
{
    RUNQUERY(tofill, search_user_query.arg(search_text));
}

UserInfo get_user_info(int uid)
{
    QSqlQuery info(user_id_query.arg(uid));
    QSqlQuery daily(daily_hours_query.arg(uid));
    QSqlQuery total(total_hours_query.arg(uid));
    info.first();
    daily.first();
    total.first();
    return {
        .name    = info.value(1).toString(),
        .surname = info.value(2).toString(),
        .daily_hours = daily.value(0).toInt(),
        .total_hours = total.value(0).toInt(),
        .session_part = 0,
        .session_create = 0,
    };
}

void get_favorites(QSqlQueryModel &tofill, int uid)
{
    RUNQUERY(tofill, get_fav_query.arg(uid));
}

bool add_favorite(int uid, int vid)
{
    QSqlQuery check(check_fav_query.arg(uid).arg(vid));
    if (check.size() == 0) {
        auto s = insert_fav_query.arg(uid).arg(vid);
        // qDebug() << s;
        QSqlQuery query(s);
        return true;
    }
    return false;
}

std::optional<QString> create_session(int vid, int uid, const std::vector<int> &uids, int time)
{
    QSqlQuery check(check_session_query.arg(vid).arg(uids.size()));
    if (check.size() == 0)
        return "il gioco selezionato non è un gioco multiplayer, oppure non supporta il numero di giocatori";
    auto s = create_session_query.arg(vid).arg(uid).arg(time);
    qDebug() << s;
    QSqlQuery create(s);
    for (auto id : uids) {
        auto t = insert_partecipation_query.arg(id);
        qDebug() << t;
        QSqlQuery part(t);
    }
    return std::nullopt;
}

void get_session_statistics(QSqlQueryModel &tofill, int uid)
{
    RUNQUERY(tofill, session_statistic_query.arg(uid));
}

int get_monthly_profit(int year, int month)
{
    QString yearstr  = QString::number(year);
    QString monthstr = QString("%1").arg((int)month, 2, 10, QChar('0'));
    auto s = monthly_profit_query.arg(yearstr+monthstr);
    // qDebug() << s;
    QSqlQuery query(s);
    query.first();
    return query.value(0).toInt();
}

} // namespace db


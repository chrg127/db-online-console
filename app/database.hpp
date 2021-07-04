#ifndef DATABASE_HPP_INCLUDED
#define DATABASE_HPP_INCLUDED

#include <vector>
#include <optional>
#include <utility>
#include <QDate>
#include <QDateTime>
#include <QString>
#include <QSqlError>
#include <QSqlQueryModel>

namespace db {

struct GameInfo {
    QString title, genre, year, company, director;
    int price, ncopies;
};

struct UserInfo {
    QString name, surname;
    int daily_hours, total_hours, session_part, session_create;
};

enum class PlanType {
    GRATIS, MONTH, YEAR,
};

struct PlanInfo {
    int id;
    PlanType type;
    QDate start, end;
    QDateTime start_time;
};

inline PlanType int_to_plan(int x)
{
    switch (x) {
    case 0: return PlanType::GRATIS;
    case 1: return PlanType::MONTH;
    case 2: return PlanType::YEAR;
    default: return PlanType::GRATIS;
    }
}

inline QString plan_to_string(PlanType type)
{
    switch (type) {
    case PlanType::GRATIS: return "Gratuito";
    case PlanType::MONTH:  return "Mensile";
    case PlanType::YEAR:   return "Annuale";
    default:               return "error";
    }
}

inline PlanType string_to_plan(const QString &str)
{
    return str == "Gratuito" ? PlanType::GRATIS
         : str == "Mensile"  ? PlanType::MONTH
         : str == "Annuale"  ? PlanType::YEAR
         : PlanType::GRATIS;
}

inline QDate plan_date_end(QDate date_start, PlanType type)
{
    switch (type) {
    case PlanType::GRATIS: return date_start.addMonths(1);
    case PlanType::MONTH:  return date_start.addMonths(1);
    case PlanType::YEAR:   return date_start.addYears(1);
    default:               return date_start;
    }
}

bool connect(const QString &table, const QString &name, const QString &password);
std::optional<QString> run_query(QSqlQueryModel &model, const QString &query);

int validate_user(const QString &name, const QString &surname, const QString &password);
int validate_admin(const QString &name, const QString &surname, const QString &password);

std::optional<PlanInfo> get_curr_plan_info(int uid);
bool create_plan(int uid, PlanType type);
bool cancel_plan(int planid);

void search_games(QSqlQueryModel &tofill, const QString &search_text, const QString &category);
GameInfo get_game_info(int vid);
std::pair<int, int> get_copy_info(int vid);
bool buy_game(int id, int uid);
void best_games(QSqlQueryModel &tofill);
void most_played_games(QSqlQueryModel &tofill);

void search_users(QSqlQueryModel &tofill, const QString &search_text);
UserInfo get_user_info(int id);
void get_favorites(QSqlQueryModel &tofill, int uid);
bool add_favorite(int uid, int vid);

std::optional<QString> create_session(int vid, int uid, const std::vector<int> &uids, int time);
void get_session_statistics(QSqlQueryModel &tofill, int uid);

int get_monthly_profit(int year, int month);

} // namespace db

#endif

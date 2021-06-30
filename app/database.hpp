#ifndef DATABASE_HPP_INCLUDED
#define DATABASE_HPP_INCLUDED

#include <vector>
#include <optional>
#include <utility>
#include <QDate>
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
    bool has_plan;
    PlanType type;
    QDate start, end;
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
    case PlanType::GRATIS: return "Gratis";
    case PlanType::MONTH:  return "Mensile";
    case PlanType::YEAR:   return "Annuale";
    default:               return "error";
    }
}

bool connect(const QString &table);

std::optional<QString> run_query(QSqlQueryModel &model, const QString &query);
void search_games(QSqlQueryModel &tofill, const QString &search_text, const QString &category);
void search_users(QSqlQueryModel &tofill, const QString &search_text);
void best_games(QSqlQueryModel &tofill);
void most_played_games(QSqlQueryModel &tofill);
void get_favorites(QSqlQueryModel &tofill, int uid);

int validate_user(const QString &name, const QString &surname, const QString &password);
int validate_admin(const QString &name, const QString &surname, const QString &password);
GameInfo get_game_info(int id);
UserInfo get_user_info(int id);
PlanInfo get_curr_plan_info(int uid);
std::pair<int, int> get_copy_info(int id);
bool buy_game(int id, int uid);
bool create_plan(int uid, PlanType type);
bool cancel_plan(int uid);
bool add_favorite(int uid, int vid);
std::optional<QString> create_session(int vid, int uid, const std::vector<int> &uids, QDate date, int time);

} // namespace db

#endif

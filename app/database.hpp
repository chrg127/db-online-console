#ifndef DATABASE_HPP_INCLUDED
#define DATABASE_HPP_INCLUDED

#include <optional>
#include <utility>
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
};

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
std::pair<int, int> get_copy_info(int id);
bool buy_game(int id, int uid);
void cancel_plan(int id);

} // namespace db

#endif

#ifndef DATABASE_HPP_INCLUDED
#define DATABASE_HPP_INCLUDED

#include <optional>
#include <QString>
#include <QSqlError>
#include <QSqlQueryModel>

namespace db {

struct GameInfo {
    QString title, genre, year, company, director;
    int price;
};

struct UserInfo {
    QString name, surname;
};

bool connect(const QString &table);
bool validate_user(const QString &name, const QString &surname, const QString &password);
bool validate_admin(const QString &name, const QString &surname, const QString &password);
std::optional<QString> run_query(QSqlQueryModel &model, const QString &query);
void search_games(QSqlQueryModel &tofill, const QString &search_text, const QString &category);
void search_users(QSqlQueryModel &tofill, const QString &search_text);
GameInfo get_game_info(int id);
UserInfo get_user_info(int id);
void best_games(QSqlQueryModel &tofill);
void most_played_games(QSqlQueryModel &tofill);

} // namespace db

#endif

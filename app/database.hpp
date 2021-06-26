#ifndef DATABASE_HPP_INCLUDED
#define DATABASE_HPP_INCLUDED

#include <QString>

bool connect_db(const QString &table);
bool validate_user(const QString &user, const QString &password);
bool validate_admin(const QString &admin, const QString &password);

#endif

#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QMainWindow>
#include <QWidget>
#include <QSqlQueryModel>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QBoxLayout>
#include <QDialog>
#include <QGroupBox>

class QPushButton;
class QStackedWidget;
class QLabel;
class QLineEdit;
class QTableView;
class QTextEdit;
class QBoxLayout;
class QListWidget;
class SQLHighlighter;
class GLWidget;
namespace db {
class GameInfo;
class UserInfo;
class PlanInfo;
}

class Window : public QMainWindow {
    Q_OBJECT
    QStackedWidget *stack_widget;
public:
    enum class Screen {
        LOGIN, ADMIN, USER,
#ifdef _CATPRISM
        CATPRISM,
#endif
    };
    explicit Window(QWidget *parent = nullptr);
    void show_screen(Screen screen);
};

class LoginScreen : public QWidget {
    Q_OBJECT
    QLineEdit *name_box, *pass_box;
public:
    explicit LoginScreen(Window *mainwnd, QWidget *parent = nullptr);
signals:
    void logged(int id);
};

class VideogameProfile;
class UserProfile;
class PlanProfile;

class UserScreen : public QWidget {
    Q_OBJECT
    int uid, vid;
    UserProfile *user_profile;
    PlanProfile *plan_profile;
    QTableView *plans, *favorites;
    QSqlQueryModel plans_model, fav_model;
    QWidget *make_game_tab();
    QWidget *make_user_tab();
    QWidget *make_profile_tab();
public:
    UserScreen(Window *wnd, QWidget *parent = nullptr);
    int getuid() const { return uid; }
    int getvid() const { return vid; }
    void update_favorites();
public slots:
    void on_login(int id);
};

class VideogameProfile : public QGroupBox {
    Q_OBJECT
    QLabel *title, *genre, *year, *company, *director, *price, *ncopies;
    int id;
public:
    VideogameProfile(UserScreen *parent = nullptr);
    void set_info(int id, const db::GameInfo &info);
};

class UserProfile : public QGroupBox {
    Q_OBJECT
    QLabel *name, *surname;
public:
    UserProfile(QWidget *parent = nullptr);
    void set_info(const db::UserInfo &info);
};

class PlanProfile : public QWidget {
    Q_OBJECT
    QLabel *type, *start, *end, *noplan;
    QWidget *plan_form;
public:
    PlanProfile(QWidget *parent = nullptr);
    void set_info(const db::PlanInfo &info);
};

class Searcher : public QGroupBox {
    Q_OBJECT
public:
    QLineEdit *bar;
    QPushButton *bt;
    QSqlQueryModel model;
    QTableView *table;
    QBoxLayout *lt;

    Searcher(const QString &title, QWidget *parent = nullptr);
    void insert(int i, QLayout *lt) { this->lt->insertLayout(i, lt); }
    void after_search() { table->hideColumn(0); table->resizeColumnsToContents(); }
    void on_search(auto &&fn)
    {
        connect(bt, &QPushButton::released, fn);
        connect(bar, &QLineEdit::returnPressed, fn);
    }
    void on_tab_click(auto &&fn)
    {
        connect(table, &QTableView::clicked, fn);
    }
};

class AdminScreen : public QWidget {
    Q_OBJECT
    QSqlQueryModel result_model;
public:
    explicit AdminScreen(Window *wnd, QWidget *parent = nullptr);
};

#ifdef _CATPRISM
class CatPrismScreen : public QWidget {
    Q_OBJECT
    GLWidget *glwidget;
public:
    explicit CatPrismScreen(QWidget *parent = nullptr);
};
#endif

#endif

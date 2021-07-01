#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <optional>
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
#include <QHeaderView>

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

class DBTable : public QTableView {
    Q_OBJECT
    QSqlQueryModel m_model;
public:
    DBTable(QWidget *parent = nullptr)
        : QTableView(parent)
    {
        setModel(&m_model);
    }

    void on_click(auto &&fn) { connect(this, &QTableView::clicked, fn); }
    // QSqlQueryModel & model() const { return m_model; }
    void fill(auto &&fn, auto&&... args)
    {
        std::invoke(fn, m_model, std::forward<decltype(args)>(args)...);
        hideColumn(0);
        resizeColumnsToContents();
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
};

class VideogameProfile;
class UserProfile;
class PlanProfile;

class UserScreen : public QWidget {
    Q_OBJECT
    int uid, vid, svid;
    UserProfile *user_profile;
    PlanProfile *plan_profile;
    DBTable *favorites;
    QTabWidget *tabs;
    QGroupBox *create_plan_group, *curr_plan_group;
    QListWidget *session_users;
    std::vector<int> session_users_ids;

    QWidget *make_profile_tab();
    QWidget *make_game_tab();
    QWidget *make_users_tab();
    QWidget *make_session_tab();

public:
    UserScreen(Window *wnd, QWidget *parent = nullptr);
    int getuid() const { return uid; }
    int getvid() const { return vid; }
    void on_plan_changed(bool created);
public slots:
    void on_login(int id);
};

class VideogameProfile : public QGroupBox {
    Q_OBJECT
    QLabel *title, *genre, *year, *company, *director, *price, *ncopies;
    QBoxLayout *lt;
    int id;
public:
    VideogameProfile(UserScreen *parent = nullptr);
    void set_info(const db::GameInfo &info);
    void update_copies(int id);
    void insert(int i, QLayout *lt) { this->lt->insertLayout(i, lt); }
};

class UserProfile : public QGroupBox {
    Q_OBJECT
    QLabel *name, *surname, *daily_hours, *total_hours, *session_part, *session_create;
public:
    UserProfile(QWidget *parent = nullptr);
    void set_info(const db::UserInfo &info);
};

class PlanProfile : public QWidget {
    Q_OBJECT
    int id = -1;
    QLabel *type, *start, *end, *noplan;
    QWidget *plan_form;
public:
    PlanProfile(QWidget *parent = nullptr);
    void set_info(const std::optional<db::PlanInfo> &info);
    int planid() const { return id; }
};

class Searcher : public QGroupBox {
    Q_OBJECT
public:
    QLineEdit *bar;
    QPushButton *bt;
    DBTable *table;
    QBoxLayout *lt;

    Searcher(const QString &title, QWidget *parent = nullptr);
    void insert(int i, QLayout *lt) { this->lt->insertLayout(i, lt); }
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

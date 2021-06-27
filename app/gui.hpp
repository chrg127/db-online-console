#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QMainWindow>
#include <QWidget>
#include <QSqlQueryModel>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>

class QPushButton;
class QStackedWidget;
class QLabel;
class QLineEdit;
class QTableView;
class QTextEdit;
class SQLHighlighter;
class GLWidget;
namespace db {
class GameInfo;
class UserInfo;
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
private:
    void create_menu();
    void create_statusbar();
    void create_widgets();
};

class LoginScreen : public QWidget {
    Q_OBJECT
    QLineEdit *name_box, *pass_box;
public:
    explicit LoginScreen(Window *mainwnd, QWidget *parent = nullptr);
};

class UserScreen : public QWidget {
    Q_OBJECT
    QSqlQueryModel gametabmodel, usertabmodel;
    QWidget *make_game_tab();
    QWidget *make_user_tab();
public:
    UserScreen(Window *wnd, QWidget *parent = nullptr);
};

class VideogameProfile : public QWidget {
    Q_OBJECT
    QLabel *title, *genre, *year, *company, *director, *price;
public:
    VideogameProfile(QWidget *parent = nullptr);
    void set_info(const db::GameInfo &info);
};

class UserProfile : public QWidget {
    Q_OBJECT
    QLabel *name, *surname;
public:
    UserProfile(QWidget *parent = nullptr);
    void set_info(const db::UserInfo &info);
};

class AdminScreen : public QWidget {
    Q_OBJECT
    // QPushButton *query_button;
    // QPushButton *exit_button;
    // QTableView *result_tab;
    QSqlQueryModel result_model;
    // QTextEdit *query_editor;
    // SQLHighlighter *highlighter;
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

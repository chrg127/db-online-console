#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QMainWindow>
#include <QWidget>
#include <QSqlQueryModel>

class QPushButton;
class QStackedWidget;
class QLabel;
class QLineEdit;
class QTableView;
class QTextEdit;
class SQLHighlighter;
class GLWidget;

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
    QPushButton *exit_button;
    QLabel *label;
public:
    UserScreen(Window *wnd, QWidget *parent = nullptr);
};

class AdminScreen : public QWidget {
    Q_OBJECT
    QPushButton *query_button;
    QPushButton *exit_button;
    QTableView *result_tab;
    QSqlQueryModel result_model;
    QTextEdit *query_editor;
    SQLHighlighter *highlighter;
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

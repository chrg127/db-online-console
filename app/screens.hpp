#ifndef SCREENS_HPP_INCLUDED
#define SCREENS_HPP_INCLUDED

#include <QWidget>
#include <QSqlQueryModel>

class QLabel;
class QLineEdit;
class QPushButton;
class QLabel;
class QTableView;
class QTextEdit;
class QGroupBox;
class SQLHighlighter;
class Window;
class GLWidget;

class LoginScreen : public QWidget {
    Q_OBJECT
private:
    QLabel *image;
    QGroupBox *login_box;
    QLineEdit *name_box, *pass_box;
    QPushButton *admin_button, *user_button, *prism_button;
public:
    explicit LoginScreen(Window *mainwnd, QWidget *parent = nullptr);
};

class UserScreen : public QWidget {
    Q_OBJECT
private:
    QPushButton *exit_button;
    QLabel *label;
public:
    UserScreen(Window *wnd, QWidget *parent = nullptr);
};

class AdminScreen : public QWidget {
    Q_OBJECT
private:
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
private:
    GLWidget *glwidget;
public:
    explicit CatPrismScreen(QWidget *parent = nullptr);
};
#endif

#endif

#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QMainWindow>

class QLabel;
class QPushButton;
class QStackedWidget;
class LoginScreen;
class AdminScreen;
class UserScreen;

class Window : public QMainWindow {
    Q_OBJECT

private:
    QLabel *java_label;
    QStackedWidget *stack_widget;
    LoginScreen *login_screen;
    AdminScreen *admin_screen;
    UserScreen  *user_screen;

public:
    enum class Screen {
        LOGIN, ADMIN, USER,
    };

    explicit Window(QWidget *parent = nullptr);
    void show_screen(Screen screen);

private:
    void create_menu();
    void create_statusbar();
    void create_widgets();
};

#endif

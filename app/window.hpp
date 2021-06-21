#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QMainWindow>

class QLabel;
class QPushButton;
class QStackedWidget;
class LoginScreen;
class AdminScreen;
class UserScreen;
#ifdef _CATPRISM
class CatPrismScreen;
#endif

class Window : public QMainWindow {
    Q_OBJECT

private:
    QLabel *java_label;
    QStackedWidget *stack_widget;
    LoginScreen *login_screen;
    AdminScreen *admin_screen;
    UserScreen  *user_screen;
#ifdef _CATPRISM
    CatPrismScreen *prism_screen;
#endif

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

#endif

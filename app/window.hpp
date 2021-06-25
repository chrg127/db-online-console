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

#endif

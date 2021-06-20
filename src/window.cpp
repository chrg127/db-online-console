#include "window.hpp"

#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <fmt/core.h>
#include "screens.hpp"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Database project"));
    create_menu();
    create_statusbar();
    create_widgets();
}

void Window::create_menu()
{
    enum MenuIndex {
        MENU_FILE,
        MENU_COUNT,
    };
    QMenu *menus[MENU_COUNT];

    const auto add_item = [&, this](MenuIndex idx, const QString &text, bool enable, auto &&func)
    {
        auto *act = new QAction(text, this);
        connect(act, &QAction::triggered, this, func);
        menus[idx]->addAction(act);
        act->setEnabled(enable);
    };

    menus[MENU_FILE] = menuBar()->addMenu("&File");
    add_item(MENU_FILE, "&Open", true, []() { fmt::print("dinosauri\n"); });
}

void Window::create_statusbar()
{
    auto *status_label = new QLabel(this);
    statusBar()->addPermanentWidget(status_label);
    statusBar()->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
}

void Window::create_widgets()
{
    auto *center = new QWidget(this);
    center->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    setCentralWidget(center);

    java_label = new QLabel("java fa schifo!", center);

    stack_widget = new QStackedWidget;
    login_screen = new LoginScreen(this, center);
    stack_widget->addWidget(login_screen);

    admin_screen = new AdminScreen(this, center);
    stack_widget->addWidget(admin_screen);

    user_screen  = new UserScreen(this, center);
    stack_widget->addWidget(user_screen);

#ifdef _CATPRISM
    prism_screen = new CatPrismScreen(center);
    stack_widget->addWidget(prism_screen);
#endif

    auto *lt = new QVBoxLayout(center);
    lt->addWidget(java_label);
    lt->addWidget(stack_widget);
}

void Window::show_screen(Screen screen)
{
    switch (screen) {
    case Screen::LOGIN: stack_widget->setCurrentIndex(0); break;
    case Screen::ADMIN: stack_widget->setCurrentIndex(1); break;
    case Screen::USER:  stack_widget->setCurrentIndex(2); break;
#ifdef _CATPRISM
    case Screen::CATPRISM: stack_widget->setCurrentIndex(3); break;
#endif
    }
}


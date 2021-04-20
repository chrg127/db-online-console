#include "window.hpp"

#include <QAction>
#include <QMenuBar>
#include <fmt/core.h>

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumWidth(400);
    setMinimumHeight(400);
    setWindowTitle(QStringLiteral("Database project"));

    QWidget *center = new QWidget(this);
    create_menu();

    /*
    button = new QPushButton("execute ur shtty query", this);
    javafaschifo_label = new QLabel("java fa schifo!", this);
    button->move(100, 0);
    button->resize(200, 50);
    tabview = new QTableView(this);
    tabview->move(25, 200);
    tabview->setModel(&tabmodel);
    tabview->resize(400, 150);
    connect(button, &QPushButton::clicked, this, &Window::filltab);
    */
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
        QAction *act = new QAction(text, this);
        connect(act, &QAction::triggered, this, func);
        menus[idx]->addAction(act);
        act->setEnabled(enable);
    };

    menus[MENU_FILE] = menuBar()->addMenu("&File");
    add_item(MENU_FILE, "&Open", true, []() { fmt::print("dinosauri\n"); });
}

void Window::filltab(bool b)
{
    tabmodel.setQuery("select * from games");
}


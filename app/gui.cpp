#include "window.hpp"

#include <optional>
#include <QAction>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPixmap>
#include <QStackedWidget>
#include <QStatusBar>
#include <QSqlError>
#include <QSizePolicy>
#include <QTextEdit>
#include <QTableView>
#include <fmt/core.h>
#include "qthelpers.hpp"
#include "sqlhighlighter.hpp"
#include "database.hpp"
#ifdef _CATPRISM
#include "opengl.hpp"
#endif

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
    // enum MenuIndex { MENU_FILE, MENU_COUNT, };
    // QMenu *menus[MENU_COUNT];

    // const auto add_item = [&, this](MenuIndex idx, const QString &text, bool enable, auto &&func)
    // {
    //     auto *act = new QAction(text, this);
    //     connect(act, &QAction::triggered, this, func);
    //     menus[idx]->addAction(act);
    //     act->setEnabled(enable);
    // };

    // menus[MENU_FILE] = menuBar()->addMenu("&File");
    // add_item(MENU_FILE, "&Open", true, []() { fmt::print("dinosauri\n"); });
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

    stack_widget = make_layout<QStackedWidget>(
        new LoginScreen(this, center),
        new AdminScreen(this, center),
        new UserScreen(this, center)
#ifdef _CATPRISM
        new CatPrismScreen(center);
#endif
    );
    auto lt = make_layout<QVBoxLayout>(
        new QLabel("java fa schifo!", center),
        stack_widget
    );
    center->setLayout(lt);
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

LoginScreen::LoginScreen(Window *mainwnd, QWidget *parent)
    : QWidget(parent)
{
    QLabel *image = new QLabel;
    image->setPixmap(QPixmap("logo.png"));
    image->setAlignment(Qt::AlignCenter);

    name_box = new QLineEdit;
    pass_box = new QLineEdit;
    pass_box->setEchoMode(QLineEdit::EchoMode::Password);
    auto *login_box = new QGroupBox("Login");
    login_box->setLayout(make_grid_layout(
        std::tuple{ new QLabel("Username: "), 0, 0 },
        std::tuple{ name_box,   0, 1 },
        std::tuple{ new QLabel("Password: "), 1, 0 },
        std::tuple{ pass_box,   1, 1 }
    ));
    login_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    auto *admin_button = new QPushButton("Login as admin");
    auto *user_button = new QPushButton("Login as user");
    connect(admin_button, &QPushButton::released, [mainwnd]() { mainwnd->show_screen(Window::Screen::ADMIN); });
    connect(user_button, &QPushButton::released,  [this, mainwnd]()
    {
        if (validate_user(name_box->text(), pass_box->text()))
            mainwnd->show_screen(Window::Screen::USER);
        else {
            msgbox(QString("Couldn't find validate user %1 %2 Make sure you've typed the right username and password.")
                   .arg(name_box->text())
                   .arg(pass_box->text()));
        }
    });

#ifdef _CATPRISM
    auto *prism_button = new QPushButton("View the cat prism");
    connect(prism_button, &QPushButton::released, [mainwnd]() { mainwnd->show_screen(Window::Screen::CATPRISM); });
#endif

    auto *mainlt = make_layout<QVBoxLayout>(
        image, login_box,
        make_layout<QHBoxLayout>(admin_button, user_button)
    );
    mainlt->setContentsMargins(100, 10, 100, 10);
    this->setLayout(mainlt);
}

UserScreen::UserScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    exit_button = new QPushButton("Exit");
    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });
    setLayout(make_layout<QVBoxLayout>(new QLabel("duke"), exit_button));
}

template <typename T>
std::optional<QString> run_query(T &model, const QString &query)
{
    model.setQuery(query);
    QSqlError error = model.lastError();
    if (!error.isValid())
        return std::nullopt;
    return QString("Error found while executing the query:\n%1\n%2")
            .arg(error.driverText())
            .arg(error.databaseText());
}

AdminScreen::AdminScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    result_tab = new QTableView(this);
    result_tab->setModel(&result_model);

    query_editor = new QTextEdit(this);
    highlighter  = new SQLHighlighter(query_editor->document());
    query_editor->setFont(make_font("Monospace", QFont::TypeWriter));

    query_button = new QPushButton("Execute a query", this);
    connect(query_button, &QPushButton::released, this, [this]()
    {
        auto errmsg = run_query(result_model, query_editor->toPlainText());
        if (errmsg)
            msgbox(errmsg.value());
    });

    exit_button = new QPushButton("Exit", this);
    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });

    setLayout(
        make_layout<QVBoxLayout>(
            make_layout<QHBoxLayout>(query_editor, result_tab),
            query_button,
            exit_button
        )
    );
}

#ifdef _CATPRISM
CatPrismScreen::CatPrismScreen(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *lt = new QVBoxLayout(this);
    glwidget = new GLWidget(this);
    lt->addWidget(glwidget);
}
#endif


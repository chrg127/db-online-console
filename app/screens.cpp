#include "screens.hpp"

#include <optional>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QTextEdit>
#include <QGroupBox>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QPixmap>
#include <QSqlError>
#include <QMessageBox>
#include <QSizePolicy>
#include <fmt/core.h>
#include "qthelpers.hpp"
#include "window.hpp"
#include "sqlhighlighter.hpp"
#ifdef _CATPRISM
#include "opengl.hpp"
#endif

LoginScreen::LoginScreen(Window *mainwnd, QWidget *parent)
    : QWidget(parent)
{
    image = new QLabel;
    image->setPixmap(QPixmap("logo.png"));
    image->setAlignment(Qt::AlignCenter);

    name_box = new QLineEdit;
    pass_box = new QLineEdit;
    pass_box->setEchoMode(QLineEdit::EchoMode::Password);
    login_box = new QGroupBox("Login");
    login_box->setLayout(make_grid_layout(
        std::tuple{ new QLabel("Username: "), 0, 0 },
        std::tuple{ name_box,   0, 1 },
        std::tuple{ new QLabel("Password: "), 1, 0 },
        std::tuple{ pass_box,   1, 1 }
    ));
    login_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    admin_button = new QPushButton("Login as admin");
    user_button = new QPushButton("Login as user");
    connect(admin_button, &QPushButton::released, [mainwnd]() { mainwnd->show_screen(Window::Screen::ADMIN); });
    connect(user_button, &QPushButton::released,  [mainwnd]() { mainwnd->show_screen(Window::Screen::USER); });

#ifdef _CATPRISM
    prism_button = new QPushButton("View the cat prism");
    connect(prism_button, &QPushButton::released, [mainwnd]() { mainwnd->show_screen(Window::Screen::CATPRISM); });
#endif

    auto *buttonlt = make_layout<QHBoxLayout>(admin_button, user_button);
    auto *mainlt = make_layout<QVBoxLayout>(image, login_box, buttonlt);
    this->setLayout(mainlt);
    mainlt->setContentsMargins(100, 10, 100, 10);
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
        if (errmsg) {
            QMessageBox box;
            box.setText(*errmsg);
            box.exec();
        }
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


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
#include "window.hpp"
#include "sqlhighlighter.hpp"
#include "opengl.hpp"

LoginScreen::LoginScreen(Window *mainwnd, QWidget *parent)
    : QWidget(parent)
{
    image = new QLabel;
    image->setPixmap(QPixmap("logo.png"));
    image->setAlignment(Qt::AlignCenter);

    name_label = new QLabel("Username: ");

    pass_label = new QLabel("Password: ");

    name_box = new QLineEdit;

    pass_box = new QLineEdit;
    pass_box->setEchoMode(QLineEdit::EchoMode::Password);

    auto *grid = new QGridLayout;
    grid->addWidget(name_label, 0, 0);
    grid->addWidget(name_box, 0, 1);
    grid->addWidget(pass_label, 1, 0);
    grid->addWidget(pass_box, 1, 1);

    login_box = new QGroupBox("Login");
    login_box->setLayout(grid);
    login_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    admin_button = new QPushButton("Login as admin");
    connect(admin_button, &QPushButton::released,
            [mainwnd]() { mainwnd->show_screen(Window::Screen::ADMIN); });

    user_button = new QPushButton("Login as user");
    connect(user_button, &QPushButton::released,
            [mainwnd]() { mainwnd->show_screen(Window::Screen::USER); });

    prism_button = new QPushButton("View the cat prism");
    connect(prism_button, &QPushButton::released,
            [mainwnd]() { mainwnd->show_screen(Window::Screen::CATPRISM); });

    auto *buttonlt = new QHBoxLayout;
    buttonlt->addWidget(admin_button);
    buttonlt->addWidget(user_button);
    buttonlt->addWidget(prism_button);

    auto *mainlt = new QVBoxLayout(this);
    mainlt->addWidget(image);
    mainlt->addWidget(login_box);
    mainlt->addLayout(buttonlt);
    mainlt->setContentsMargins(100, 10, 100, 10);
}

UserScreen::UserScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    label = new QLabel("duke");
    exit_button = new QPushButton("Exit");
    connect(exit_button, &QPushButton::released,
            [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });
    auto *mainlt = new QVBoxLayout(this);
    mainlt->addWidget(label);
    mainlt->addWidget(exit_button);
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
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    query_editor->setFont(font);

    auto *hlt = new QHBoxLayout;
    hlt->addWidget(query_editor);
    hlt->addWidget(result_tab);

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
    connect(exit_button, &QPushButton::released,
            [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });

    auto *vlt = new QVBoxLayout(this);
    vlt->addLayout(hlt);
    vlt->addWidget(query_button);
    vlt->addWidget(exit_button);
}

CatPrismScreen::CatPrismScreen(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *lt = new QVBoxLayout(this);
    glwidget = new GLWidget(this);
    lt->addWidget(glwidget);
}


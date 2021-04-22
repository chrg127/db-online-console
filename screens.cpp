#include "screens.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QTextEdit>
#include <QGroupBox>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QFontDatabase>
#include <QPixmap>
#include <QSqlError>
#include <QMessageBox>
#include <fmt/core.h>
#include "window.hpp"
#include "sqlhighlighter.hpp"

LoginScreen::LoginScreen(Window *mainwnd, QWidget *parent)
    : QWidget(parent)
{
    image      = new QLabel;
    image->setPixmap(QPixmap("face.png"));
    image->setAlignment(Qt::AlignCenter);

    name_label = new QLabel("Username: ");
    pass_label = new QLabel("Password: ");
    name_box   = new QLineEdit;
    pass_box   = new QLineEdit;
    pass_box->setEchoMode(QLineEdit::EchoMode::Password);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(name_label, 0, 0);
    grid->addWidget(name_box, 0, 1);
    grid->addWidget(pass_label, 1, 0);
    grid->addWidget(pass_box, 1, 1);

    login_box = new QGroupBox("Login");
    login_box->setLayout(grid);

    admin_button = new QPushButton("Login as admin");
    connect(admin_button, &QPushButton::released,
            [mainwnd]() { mainwnd->show_screen(Window::Screen::ADMIN); });

    user_button = new QPushButton("Login as user");
    connect(user_button, &QPushButton::released,
            [mainwnd]() { mainwnd->show_screen(Window::Screen::USER); });

    QHBoxLayout *buttonlt = new QHBoxLayout;
    buttonlt->addWidget(admin_button);
    buttonlt->addWidget(user_button);

    QVBoxLayout *mainlt = new QVBoxLayout(this);
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
    QVBoxLayout *mainlt = new QVBoxLayout(this);
    mainlt->addWidget(label);
    mainlt->addWidget(exit_button);
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

    QHBoxLayout *hlt = new QHBoxLayout;
    hlt->addWidget(query_editor);
    hlt->addWidget(result_tab);

    query_button = new QPushButton("Execute a query", this);
    connect(query_button, &QPushButton::released, this, &AdminScreen::run_query);

    exit_button = new QPushButton("Exit", this);
    connect(exit_button, &QPushButton::released,
            [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });

    QVBoxLayout *vlt = new QVBoxLayout(this);
    vlt->addLayout(hlt);
    vlt->addWidget(query_button);
    vlt->addWidget(exit_button);
}

void AdminScreen::run_query()
{
    result_model.setQuery(query_editor->toPlainText());
    QSqlError error = result_model.lastError();
    if (!error.isValid())
        return;
    QMessageBox msg;
    msg.setText(
        QString("Error found while executing the query:\n%1\n%2")
        .arg(error.driverText())
        .arg(error.databaseText())
    );
    msg.exec();
}


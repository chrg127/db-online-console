#include "gui.hpp"

#include <QDebug>
#include <QAction>
#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QStatusBar>
#include <QSqlError>
#include <QSizePolicy>
#include <QTextEdit>
#include <QTableView>
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

    auto *name_box = new QLineEdit;
    auto *surname_box = new QLineEdit;
    auto *pass_box = new QLineEdit;
    pass_box->setEchoMode(QLineEdit::EchoMode::Password);

    auto *admin_button = new QPushButton("Login as admin");
    auto *user_button = new QPushButton("Login as user");

    auto validate = [=](auto &&validate_fn, Window::Screen screen)
    {
        auto name = name_box->text(), surname = surname_box->text(), password = pass_box->text();
        if (validate_fn(name, surname, password))
            mainwnd->show_screen(screen);
        else {
            msgbox(QString("Couldn't validate user %1 %2. Make sure you've typed the right username and password.")
                   .arg(name).arg(surname));
        }
    };

    connect(admin_button, &QPushButton::released, [=](){ validate(db::validate_admin, Window::Screen::ADMIN); });
    connect(user_button,  &QPushButton::released, [=](){ validate(db::validate_user,  Window::Screen::USER); });

#ifdef _CATPRISM
    auto *prism_button = new QPushButton("View the cat prism");
    connect(prism_button, &QPushButton::released, [mainwnd]() { mainwnd->show_screen(Window::Screen::CATPRISM); });
#endif

    auto *login_box = new QGroupBox("Login");
    login_box->setLayout(make_form_layout(
        std::tuple{ "Nome:",     name_box },
        std::tuple{ "Cognome:",  surname_box },
        std::tuple{ "Password:", pass_box }
    ));
    login_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
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
    auto *exit_button = new QPushButton("Esci");
    auto *tabs = new QTabWidget;

    tabs->insertTab(0, make_game_tab(), "Videogiochi");
    tabs->insertTab(1, make_user_tab(), "Utenti");

    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });

    setLayout(make_layout<QVBoxLayout>(tabs, exit_button));
}

QWidget *UserScreen::make_game_tab()
{
    auto *searchbar = new QLineEdit;
    auto *category = make_comboxbox(
        std::tuple{"Titolo", QVariant("titolo")},
        std::tuple{"Genere", QVariant("genere")},
        std::tuple{"Azienda", QVariant("azienda")}
    );
    auto *table = make_table(&gametabmodel);
    auto *searchbt = new QPushButton("Cerca");
    auto *profile = new VideogameProfile;
    auto *bestbt = new QPushButton("Giochi più popolari");
    auto *most_player_bt = new QPushButton("Giochi più giocati");

    table->verticalHeader()->hide();
    profile->hide();

    auto search_games = [=]()
    {
        db::search_games(
            gametabmodel,
            searchbar->text(),
            category->currentData().toString()
        );
        table->hideColumn(0);
        table->resizeColumnsToContents();
    };

    connect(searchbt, &QPushButton::released, search_games);
    connect(searchbar, &QLineEdit::returnPressed, search_games);
    connect(table, &QTableView::clicked, [=](const auto &i)
    {
        profile->set_info(db::get_game_info(i.siblingAtColumn(0).data().toInt()));
        profile->show();
    });
    connect(bestbt,         &QPushButton::released, [=]() { db::best_games(gametabmodel); });
    connect(most_player_bt, &QPushButton::released, [=]() { db::most_played_games(gametabmodel); });

    auto *container = layout_widget(make_layout<QVBoxLayout>(
        make_layout<QHBoxLayout>(searchbar, searchbt),
        make_layout<QHBoxLayout>(new QLabel("Filtra per:"), category),
        make_layout<QHBoxLayout>(bestbt, most_player_bt),
        table
    ));
    container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    container->setMinimumWidth(400);
    auto *lt = make_layout<QHBoxLayout>(container, profile);
    lt->setAlignment(container, Qt::AlignLeft);
    return layout_widget(lt);
}

QWidget *UserScreen::make_user_tab()
{
    auto *searchbar = new QLineEdit;
    auto *table = make_table(&usertabmodel);
    auto *searchbt = new QPushButton("Cerca");
    auto *profile = new UserProfile;

    table->verticalHeader()->hide();
    profile->hide();

    auto search = [=]()
    {
        db::search_users(usertabmodel, searchbar->text());
        table->hideColumn(0);
        table->resizeColumnsToContents();
    };
    connect(searchbt, &QPushButton::released, search);
    connect(searchbar, &QLineEdit::returnPressed, search);
    connect(table, &QTableView::clicked, [=](const auto &i)
    {
        profile->set_info(db::get_user_info(i.row()));
        profile->show();
    });

    auto *userlt = make_layout<QHBoxLayout>(
        make_layout<QVBoxLayout>(
            make_layout<QHBoxLayout>(searchbar, searchbt),
            table
        ),
        profile
    );
    return layout_widget(userlt);
}


VideogameProfile::VideogameProfile(QWidget *parent)
    : QWidget(parent)
{
    title = new QLabel;
    genre = new QLabel;
    year = new QLabel;
    company = new QLabel;
    director = new QLabel;
    price = new QLabel;
    auto *playbt = new QPushButton("Gioca");
    auto *buybt = new QPushButton("Compra copia fisica");

    setLayout(make_layout<QVBoxLayout>(
        make_form_layout(
            std::tuple{ "Titolo: ",                 title },
            std::tuple{ "Genere: ",                 genre },
            std::tuple{ "Anno: ",                   year },
            std::tuple{ "Azienda: ",                company },
            std::tuple{ "Direttore principale: ",   director },
            std::tuple{ "Prezzo copia fisica: ",    price }
        ),
        make_layout<QHBoxLayout>(playbt, buybt)
    ));
}

void VideogameProfile::set_info(const db::GameInfo &info)
{
#define SET(name) name->setText(info.name)
    SET(title), SET(genre), SET(year), SET(company), SET(director);
#undef SET
    price->setText(QString::number(info.price));
}

UserProfile::UserProfile(QWidget *parent)
    : QWidget(parent)
{
    name = new QLabel;
    surname = new QLabel;
    setLayout(make_grid_layout(
        std::tuple{ new QLabel("Nome: "),       0, 0 },
        std::tuple{ new QLabel("Cognome: "),    1, 0 },
        std::tuple{ name,       0, 1 },
        std::tuple{ surname,    1, 0 }
    ));
}

void UserProfile::set_info(const db::UserInfo &info)
{
#define SET(name) name->setText(info.name)
    SET(name); SET(surname);
#undef SET
}

AdminScreen::AdminScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    auto *result_tab   = make_table(&result_model);
    auto *query_editor = new QTextEdit;
    auto *highlighter  = new SQLHighlighter(query_editor->document());
    auto *query_button = new QPushButton("Execute a query");
    auto *exit_button  = new QPushButton("Exit");

    // connect(result_tab, &QTableView::clicked, [](const auto &i){ qDebug() << i.siblingAtColumn(2).data(); });
    // result_tab->verticalHeader()->hide();
    query_editor->setFont(make_font("Monospace", QFont::TypeWriter));

    connect(query_button, &QPushButton::released, this, [=]()
    {
        auto errmsg = db::run_query(result_model, query_editor->toPlainText());
        if (errmsg)
            msgbox(errmsg.value());
    });
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


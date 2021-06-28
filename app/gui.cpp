#include "gui.hpp"

#include <QDebug>
#include <QAction>
#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
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
    auto *status_label = new QLabel(this);
    statusBar()->addPermanentWidget(status_label);
    statusBar()->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    auto *center = new QWidget(this);
    center->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    setCentralWidget(center);

    auto *loginscreen = new LoginScreen(this, center);
    auto *userscreen = new UserScreen(this, center);
    connect(loginscreen, &LoginScreen::logged, userscreen, &UserScreen::on_login);

    stack_widget = make_layout<QStackedWidget>(
        loginscreen,
        new AdminScreen(this, center),
        userscreen
#ifdef _CATPRISM
        , new CatPrismScreen(center);
#endif
    );
    auto lt = make_layout<QVBoxLayout>(
        new QLabel("java fa schifo!", center),
        stack_widget
    );
    center->setLayout(lt);
}

// void Window::create_menu()
// {
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
// }

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
        if (int id = validate_fn(name, surname, password); id != -1) {
            // set_user_id(id);
            mainwnd->show_screen(screen);
            emit logged(id);
        } else {
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
    tabs->insertTab(1, make_profile_tab(), "Il mio profilo");

    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });

    setLayout(make_layout<QVBoxLayout>(tabs, exit_button));
}

QWidget *UserScreen::make_game_tab()
{
    auto *searcher = new Searcher;
    auto *category = make_comboxbox(
        std::tuple{"Titolo", QVariant("titolo")},
        std::tuple{"Genere", QVariant("genere")},
        std::tuple{"Azienda", QVariant("azienda")}
    );
    auto *profile = new VideogameProfile(this);
    auto *bestbt = new QPushButton("Giochi più popolari");
    auto *most_player_bt = new QPushButton("Giochi più giocati");

    profile->hide();
    searcher->setMinimumWidth(450);
    searcher->insert(1, make_form_layout(std::tuple{"Filtra per:", category}));
    searcher->insert(2, make_layout<QHBoxLayout>(bestbt, most_player_bt));

    searcher->on_search([=]()
    {
        db::search_games(
            searcher->model,
            searcher->bar->text(),
            category->currentData().toString()
        );
        searcher->after_search();
    });
    searcher->on_tab_click([=](const auto &i)
    {
        int id = i.siblingAtColumn(0).data().toInt();
        profile->set_info(id, db::get_game_info(id));
        profile->show();
    });
    connect(bestbt,         &QPushButton::released, [=]() { db::best_games(searcher->model); searcher->after_search(); });
    connect(most_player_bt, &QPushButton::released, [=]() { db::most_played_games(searcher->model); searcher->after_search(); });

    auto *lt = make_layout<QHBoxLayout>(searcher, profile);
    lt->setAlignment(searcher, Qt::AlignLeft);
    return layout_widget(lt);
}

QWidget *UserScreen::make_user_tab()
{
    auto *searcher = new Searcher;
    auto *profile = new UserProfile;

    profile->hide();
    searcher->setMinimumWidth(200);

    searcher->on_search([=]()
    {
        db::search_users(searcher->model, searcher->bar->text());
        searcher->after_search();
    });
    searcher->on_tab_click([=](const auto &i)
    {
        profile->set_info(db::get_user_info(i.siblingAtColumn(0).data().toInt()));
        profile->show();
    });

    auto *lt = make_layout<QHBoxLayout>(searcher, profile);
    lt->setAlignment(searcher, Qt::AlignLeft);
    return layout_widget(lt);
}

QWidget *UserScreen::make_profile_tab()
{
    user_profile = new UserProfile;
    favorites = make_table(&fav_model);

    auto *plan_group = new QGroupBox("Piani");
    plan_group->setLayout(make_layout<QVBoxLayout>(
        new QLabel("Il tuo piano scade il:"),
        new QLabel("2021-06-5"),
        make_layout<QHBoxLayout>(
            new QPushButton("Cancella piano corrente"),
            new QPushButton("Crea nuovo piano")
        )
    ));
    auto *lt = make_layout<QHBoxLayout>(
        user_profile,
        plan_group,
        make_layout<QVBoxLayout>(new QLabel("Giochi preferiti"), favorites)
    );
    return layout_widget(lt);
}

void UserScreen::on_login(int id)
{
    if (id == 0) // admin
        return;
    uid = id;
    user_profile->set_info(db::get_user_info(uid));
    db::get_favorites(fav_model, uid);
    favorites->resizeColumnsToContents();
}

VideogameProfile::VideogameProfile(UserScreen *parent)
    : QWidget(parent)
{
    title = new QLabel;
    genre = new QLabel;
    year = new QLabel;
    company = new QLabel;
    director = new QLabel;
    price = new QLabel;
    ncopies = new QLabel;
    auto *playbt = new QPushButton("Gioca");
    auto *buybt = new QPushButton("Compra copia fisica");

    connect(buybt, &QPushButton::released, [this, parent]()
    {
        if (db::buy_game(parent->getvid(), parent->getuid())) {
            msgbox("Il gioco è stato comprato.");
            ncopies->setText(QString::number(db::get_copy_info(id).second));
        } else
            msgbox("Questo gioco non ha più copie fisiche disponibili.");
    });

    setLayout(make_layout<QVBoxLayout>(
        make_form_layout(
            std::tuple{ "Titolo: ",                 title },
            std::tuple{ "Genere: ",                 genre },
            std::tuple{ "Anno: ",                   year },
            std::tuple{ "Azienda: ",                company },
            std::tuple{ "Direttore principale: ",   director },
            std::tuple{ "Prezzo copia fisica: ",    price },
            std::tuple{ "Numero copie disponibili:", ncopies }
        ),
        make_layout<QHBoxLayout>(playbt, buybt)
    ));
}

void VideogameProfile::set_info(int id, const db::GameInfo &info)
{
    this->id = id;
#define SET(name) name->setText(info.name)
    SET(title), SET(genre), SET(year), SET(company), SET(director);
#undef SET
    price->setText(QString::number(info.price));
    ncopies->setText(QString::number(info.ncopies));
}

UserProfile::UserProfile(QWidget *parent)
    : QWidget(parent)
{
    name = new QLabel;
    surname = new QLabel;
    setLayout(make_form_layout(
        std::tuple{ new QLabel("Nome: "),    name },
        std::tuple{ new QLabel("Cognome: "), surname }
    ));
}

void UserProfile::set_info(const db::UserInfo &info)
{
#define SET(name) name->setText(info.name)
    SET(name);
    SET(surname);
#undef SET
}

Searcher::Searcher(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    bar = new QLineEdit;
    bt = new QPushButton("Cerca");
    table = make_table(&model);

    table->verticalHeader()->hide();

    lt = make_layout<QVBoxLayout>(
        make_layout<QHBoxLayout>(bar, bt),
        table
    );
    setLayout(lt);
}

AdminScreen::AdminScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    auto *result_tab   = make_table(&result_model);
    auto *query_editor = new QTextEdit;
    auto *highlighter  = new SQLHighlighter(query_editor->document());
    auto *query_button = new QPushButton("Execute a query");
    auto *exit_button  = new QPushButton("Exit");

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

